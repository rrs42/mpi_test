#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <magick/api.h>
#include <mpi.h>

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

void worker(Local_MPI_Types* types, int rank)
{
    printf("Worker\n");

    WorkUnit work;
    MPI_Status status;

    MPI_Recv(&work, 1, types->workunit_type, 0, 0, MPI_COMM_WORLD, &status);

    printf("Recieved work unit (%d, %d)\n", work.bound.width, work.bound.height);
    printf("Region (%f,%f), (%f,%f)\n", work.region.ul.x, work.region.ul.y,
        work.region.lr.x, work.region.lr.y);

    return;
}

void master(Local_MPI_Types* types)
{
    int width = WIDTH;
    int height = HEIGHT;

    int zones = 4;

    Bound img_size = { width, height };

    printf("Allocating %zu\n", bound_length(img_size) * sizeof(Pixel));
    Pixel* pixels = malloc(bound_length(img_size) * sizeof(Pixel));
    Rect r;

    Point origin = { 0.0, 0.0 };
    RectSize rsize = { 2.0, 2.0 };

    make_rect(&r, origin, rsize);

    WorkUnit* bands = malloc(sizeof(WorkUnit) * zones);
    for (int zone = 0; zone < zones; zone++) {
        bands[zone].bound.width = width;
        bands[zone].bound.height = (height / 4);
        bands[zone].region.ul.x = r.ul.x;
        bands[zone].region.ul.y = r.ul.y - ((rsize.height / 4.0) * zone);
        bands[zone].region.lr.x = r.lr.x;
        bands[zone].region.lr.y = r.ul.y - ((rsize.height / 4.0) * ((zone + 1)));
    }

    for (int i = 0; i < zones; i++) {
        printf_region(bands[i].region);
    }

    // for (int y = 0; y < height; y++) {
    //     for (int x = 0; x < width; x++) {
    //         int i = map_index(x, y, img_size);
    //         pixels[i].red = 0x55;
    //         pixels[i].green = 0x00;
    //         pixels[i].blue = 0x55;
    //     }
    // }

    // MPI_Send(&work, 1, types->workunit_type, 1, 0, MPI_COMM_WORLD);

    // printf("size Pixel : %ld\n", sizeof(Pixel));
    // printf("addr pixels[0] : %p\n", &pixels[0]);
    // printf("addr pixels[1] : %p\n", &pixels[1]);
    // int a, b, c, d, e;
    // a = map_index(0, 0, img_size);
    // b = map_index(width - 1, 0, img_size);
    // c = map_index(0, height - 1, img_size);
    // d = map_index(width - 1, height - 1, img_size);
    // e = map_index(0, 256, img_size);
    // printf("a,b,c,d,e = %d, %d, %d, %d, %d\n", a, b, c, d, e);

    // printf("Rect : upper left (%f,%f) lower right (%f,%f)\n", r.ul.x, r.ul.y, r.lr.x, r.lr.y);

    if (pixels) {
        free(pixels);
    }

    if (bands) {
        free(bands);
    }
}

int main(int argc, char** argv)
{
    int rank, size;

    if (MPI_Init(NULL, NULL) != MPI_SUCCESS) {
        printf("Unable to init MPI\n");
        return -1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // if (size < 2) {
    //     printf("Must have 2 or more procesess\n");
    //     MPI_Abort(MPI_COMM_WORLD, -1);
    //     return -1;
    // }

    Local_MPI_Types types;

    /* Create Point MPI type */
    int blocklens[10] = { 2 };
    MPI_Aint displacements[10] = { offsetof(Point, x), offsetof(Point, y) };
    MPI_Datatype datatypes[10] = { MPI_DOUBLE };

    MPI_Type_create_struct(1, blocklens, displacements, datatypes, &types.point_type);
    MPI_Type_commit(&types.point_type);

    /* Create Rect MPI type */
    blocklens[0] = 2;
    displacements[0] = offsetof(Rect, ul);
    displacements[1] = offsetof(Rect, lr);
    datatypes[0] = types.point_type;

    MPI_Type_create_struct(1, blocklens, displacements, datatypes, &types.rect_type);
    MPI_Type_commit(&types.rect_type);

    /* Create RectSize type */
    blocklens[1] = 2;
    displacements[0] = offsetof(RectSize, width);
    displacements[1] = offsetof(RectSize, height);
    datatypes[0] = MPI_DOUBLE;

    MPI_Type_create_struct(1, blocklens, displacements, datatypes, &types.rectsize_type);
    MPI_Type_commit(&types.rectsize_type);

    /* Create Bound type */
    blocklens[0] = 2;
    displacements[0] = offsetof(Bound, width);
    displacements[1] = offsetof(Bound, height);
    datatypes[0] = MPI_INT;

    MPI_Type_create_struct(1, blocklens, displacements, datatypes, &types.bound_type);

    /* Create WorkUnit type */
    blocklens[0] = 1;
    blocklens[1] = 1;
    displacements[0] = offsetof(WorkUnit, bound);
    displacements[2] = offsetof(WorkUnit, region);

    datatypes[0] = types.bound_type;
    datatypes[1] = types.rect_type;

    MPI_Type_create_struct(2, blocklens, displacements, datatypes, &types.workunit_type);
    MPI_Type_commit(&types.workunit_type);

    /* Create Pixel type */
    blocklens[0] = 3;
    displacements[0] = offsetof(Pixel, red);
    displacements[1] = offsetof(Pixel, green);
    displacements[2] = offsetof(Pixel, blue);

    datatypes[0] = MPI_UINT8_T;

    MPI_Type_create_struct(1, blocklens, displacements, datatypes, &types.pixel_type);
    MPI_Type_commit(&types.pixel_type);

    if (rank != 0) {
        worker(&types, rank);
        MPI_Finalize();
    } else {
        // master
        master(&types);
        MPI_Finalize();
    }
    return 0;
}
