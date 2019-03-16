#ifndef _MPI_TEST_H_
#define _MPI_TEST_H_

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;

typedef struct Bound {
    uint32_t width;
    uint32_t height;
} Bound;

typedef struct Point {
    double_t x;
    double_t y;
} Point;

typedef struct RectSize {
    double_t width;
    double_t height;
} RectSize;

typedef struct Rect {
    Point ul;
    Point lr;
} Rect;

typedef struct WorkUnit {
    Bound bound;
    Rect region;
} WorkUnit;

int bound_index(int x, int y, Bound size);
int bound_length(Bound size);
void make_bound(Bound* bound, int width, int height);
size_t map_size(int width, int height);
void make_rect(Rect* rect, Point center, RectSize size);

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
