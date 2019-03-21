#ifndef _MPI_TEST_H_
#define _MPI_TEST_H_

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <mpi.h>

typedef struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;

void make_mpi_type_Pixel(MPI_Datatype* type);

typedef struct Bound {
    uint32_t width;
    uint32_t height;
} Bound;

void make_mpi_type_Bound(MPI_Datatype* type);

typedef struct Point {
    double_t x;
    double_t y;
} Point;

void make_mpi_type_Point(MPI_Datatype* type);

typedef struct RectSize {
    double_t width;
    double_t height;
} RectSize;

void make_mpi_type_RectSize(MPI_Datatype* type);

typedef struct Rect {
    Point ul;
    Point lr;
} Rect;

void make_mpi_type_Rect(MPI_Datatype* type, MPI_Datatype point_type);

typedef struct WorkUnit {
    Bound bound;
    Rect region;
} WorkUnit;

void make_mpi_type_WorkUnit(MPI_Datatype* type, MPI_Datatype bound_type, MPI_Datatype rect_type);

int bound_index(int x, int y, Bound size);
int bound_length(Bound size);
void make_bound(Bound* bound, int width, int height);
void make_rect(Rect* rect, Point center, RectSize size);

Point map_coord_to_point(int x, int y, WorkUnit w);

#define printf_point(p) printf("(%f,%f)", p.x, p.y);
#define printf_region(r) \
    printf("Region :");  \
    printf_point(r.ul);  \
    printf(" x ");       \
    printf_point(r.lr);  \
    printf("\n");

#define printf_bound(b) printf("%d x %d", b.width, b.height);
#define printf_workunit(w) \
    printf("WorkUnit ");   \
    printf_bound(w.bound); \
    printf("\n");          \
    printf_region(w.region);

#endif
