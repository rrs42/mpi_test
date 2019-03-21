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

typedef struct Local_MPI_Types {
    MPI_Datatype pixel_type;
    MPI_Datatype bound_type;
    MPI_Datatype point_type;
    MPI_Datatype rect_type;
    MPI_Datatype rectsize_type;
    MPI_Datatype workunit_type;
} Local_MPI_Types;

void make_mpi_types(Local_MPI_Types* types)
{
    make_mpi_type_Pixel(&types->pixel_type);
    make_mpi_type_Bound(&types->bound_type);
    make_mpi_type_Point(&types->point_type);
    make_mpi_type_Rect(&types->rect_type, types->point_type);
    make_mpi_type_RectSize(&types->rectsize_type);
    make_mpi_type_WorkUnit(&types->workunit_type, types->bound_type, types->rect_type);
}

void worker(Local_MPI_Types* types, int rank)
{
    WorkUnit work;
    MPI_Status status;

    MPI_Scatter(NULL, 1, types->workunit_type, &work, 1, types->workunit_type, 0, MPI_COMM_WORLD);

    printf("Worker %d Recieved work unit:", rank);
    printf_workunit(work);

    return;
}

void master(Local_MPI_Types* types, int world_size, const Bound img_geometry)
{
    int zones = world_size;

    printf("Allocating %zu for pixel array\n", bound_length(img_geometry) * sizeof(Pixel));
    Pixel* pixels = malloc(bound_length(img_geometry) * sizeof(Pixel));
    Rect r;

    Point origin = { 0.0, 0.0 };
    RectSize rsize = { 2.0, 2.0 };

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

    if (pixels) {
        free(pixels);
    }

    if (bands) {
        free(bands);
    }
}

static const char* usage[] = {
    "mpi_test [-w <width>] [-t <height>]",
    NULL
};

int main(int argc, const char** argv)
{
    int rank, size;

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
        MPI_Finalize();
    } else {
        // master
        Bound img_geometry;

        struct argparse_option options[] = {
            OPT_HELP(),
            OPT_INTEGER('w', "width", &img_geometry.width, "image width"),
            OPT_INTEGER('t', "height", &img_geometry.height, "image height"),
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
        MPI_Finalize();
    }

    return 0;
}
