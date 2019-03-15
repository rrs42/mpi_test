#include "mpi_test.h"
#include <assert.h>

int bound_index(int x, int y, Bound size)
{
    assert(x < size.width);
    assert(y < size.height);

    return (y * size.width) + x;
}

int bound_length(Bound size)
{
    return size.height * size.width;
}

void make_bound(Bound* bound, int width, int height)
{
    bound->width = width;
    bound->height = height;
}

size_t map_size(int width, int height)
{
    return width * height * sizeof(Pixel);
}

void make_rect(Rect* rect, Point center, RectSize size)
{
    rect->ul.x = center.x - (size.width / 2.0);
    rect->ul.y = center.y + (size.height / 2.0);

    rect->lr.x = center.x + (size.width / 2.0);
    rect->lr.y = center.y - (size.height / 2.0);
}
