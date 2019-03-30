#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <magick/api.h>
#include <mpi.h>

#include "argparse.h"
#include "mpi_test.h"

const int WIDTH = 1024;
const int HEIGHT = 768;

void write_image(Pixel* pixels, int width, int height, char* filename)
{
    ExceptionInfo exception;
    char geometry[MaxTextExtent];

    InitializeMagick(NULL);

    ImageInfo* image_info = CloneImageInfo(0);
    GetExceptionInfo(&exception);
    Image* image = ConstituteImage(width, height, "RGB", CharPixel, pixels, &exception);
    if (image == NULL) {
        CatchException(&exception);
        DestroyMagick();
        return;
    }

    snprintf(geometry, MaxTextExtent, "%dx%d", width, height);

    image_info->size = geometry;
    strncpy(image->filename, filename, MaxTextExtent);
    if (!WriteImage(image_info, image)) {
        CatchException(&exception);
        DestroyImage(image);
        DestroyMagick();
        return;
    }
}

int escapes(Point p)
{
    complex double z0, z;

    z0 = p.x + p.y * I;
    z = z0;

    int i;
    for (i = 0; i < 255; i++) {
        z = z * z + z0;
        if (cabs(z) >= 2.0) {
            break;
        }
    }

    return i;
}

Pixel* generate_band(WorkUnit band, int rank)
{
    Pixel* pixels = malloc(bound_length(band.bound) * sizeof(Pixel));
    printf("Worker %d: allocated %zu bytes\n", rank, bound_length(band.bound) * sizeof(Pixel));

    for (int y = 0; y < band.bound.height; y++) {
        for (int x = 0; x < band.bound.width; x++) {
            Point p = map_coord_to_point(x, y, band);

            int c = escapes(p);
            int i = bound_index(x, y, band.bound);

            pixels[i].red = c;
            pixels[i].green = c;
            pixels[i].blue = c;
        }
    }

    return pixels;
}

void worker(Local_MPI_Types* types, int rank)
{
    WorkUnit work;
    MPI_Status status;

    MPI_Scatter(NULL, 1, types->workunit_type, &work, 1, types->workunit_type, 0, MPI_COMM_WORLD);

    printf("Worker %d Recieved work unit:", rank);
    printf_workunit(work);

    Pixel* pixels = generate_band(work, rank);
    printf("Worker %d:Done generating band\n", rank);

    MPI_Gather(pixels, bound_length(work.bound), types->pixel_type, NULL, bound_length(work.bound), types->pixel_type, 0, MPI_COMM_WORLD);

    printf("Worker %d: results sent\n", rank);

    free(pixels);
    return;
}

void master(Local_MPI_Types* types, int world_size, const Bound img_geometry)
{
    int zones = world_size;

    printf("Allocating %zu for pixel array\n", bound_length(img_geometry) * sizeof(Pixel));
    Pixel* pixels = malloc(bound_length(img_geometry) * sizeof(Pixel));
    Rect r;

    Point origin = { -0.5, 0.0 };
    RectSize rsize = { 2.5, 2.5 };

    make_rect(&r, origin, rsize);

    WorkUnit* bands = malloc(sizeof(WorkUnit) * zones);
    for (int zone = 0; zone < zones; zone++) {
        bands[zone].bound.width = img_geometry.width;
        bands[zone].bound.height = (img_geometry.height / zones);
        bands[zone].region.ul.x = r.ul.x;
        bands[zone].region.ul.y = r.ul.y - ((rsize.height / zones) * zone);
        bands[zone].region.lr.x = r.lr.x;
        bands[zone].region.lr.y = r.ul.y - ((rsize.height / zones) * ((zone + 1)));
    }

    for (int i = 0; i < zones; i++) {
        printf_workunit(bands[i]);
    }

    WorkUnit work;
    MPI_Scatter(bands, 1, types->workunit_type, &work, 1, types->workunit_type, 0, MPI_COMM_WORLD);

    printf("Root node:\n");
    printf_workunit(work);

    Pixel* band_pixels = generate_band(work, 0);
    printf("Worker %d:Done generating band\n", 0);

    MPI_Gather(band_pixels, bound_length(work.bound), types->pixel_type,
        pixels, bound_length(work.bound), types->pixel_type, 0, MPI_COMM_WORLD);

    printf("Worker %d: results sent\n", 0);

    write_image(pixels, img_geometry.width, img_geometry.height, "test.png");

    if (band_pixels) {
        free(band_pixels);
    }

    if (pixels) {
        free(pixels);
    }

    if (bands) {
        free(bands);
    }
}

static const char* usage[] = {
    "mpi_test [-x <width>] [-y <height>]",
    NULL
};

int main(int argc, const char** argv)
{
    int rank, size;

    Pixel p = { 0x1a, 0x2b, 0x3c };
    Pixel_HSV p_1 = rgb2hsv(p);
    Pixel p_2 = hsv2rgb(p_1);

    printf("(RGB) %X, %X, %X\n", p.red, p.green, p.blue);
    printf("(HSV) %f, %f, %f\n", p_1.h, p_1.s, p_1.v);
    printf("(RGB) %X, %X, %X\n", p_2.red, p_2.green, p_2.blue);

    if (MPI_Init(NULL, NULL) != MPI_SUCCESS) {
        printf("Unable to init MPI\n");
        return -1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Local_MPI_Types types;
    make_mpi_types(&types);

    if (rank != 0) {
        worker(&types, rank);
    } else {
        // master
        Bound img_geometry;

        struct argparse_option options[] = {
            OPT_HELP(),
            OPT_INTEGER('x', "width", &img_geometry.width, "image width"),
            OPT_INTEGER('y', "height", &img_geometry.height, "image height"),
            OPT_END()
        };

        if (img_geometry.width == 0)
            img_geometry.width = WIDTH;
        if (img_geometry.height == 0)
            img_geometry.height = HEIGHT;

        struct argparse argparse;
        argparse_init(&argparse, options, usage, 0);
        argc = argparse_parse(&argparse, argc, argv);
        printf("selected width, height = %d, %d\n", img_geometry.width, img_geometry.height);

        master(&types, size, img_geometry);
    }

    MPI_Finalize();
    return 0;
}
