#include "mpi_test.h"
#include <assert.h>
#include <float.h>

void make_mpi_type_Pixel(MPI_Datatype* type)
{
    int blocklengths[] = { 3 };
    MPI_Aint displacements[] = {
        offsetof(Pixel, red),
        offsetof(Pixel, green),
        offsetof(Pixel, blue),
    };
    MPI_Datatype datatypes[] = {
        MPI_UINT8_T,
    };

    MPI_Type_create_struct(1, blocklengths, displacements, datatypes, type);
    MPI_Type_commit(type);
}

Pixel_HSV rgb2hsv(Pixel pixel)
{
    double_t h, s, v;
    double_t r_prime, g_prime, b_prime, c_max, c_min, delta;
    r_prime = (double_t)pixel.red / UINT8_MAX;
    g_prime = (double_t)pixel.green / UINT8_MAX;
    b_prime = (double_t)pixel.blue / UINT8_MAX;

    c_max = r_prime > g_prime ? r_prime : g_prime;
    c_max = c_max > b_prime ? c_max : b_prime;
    c_min = r_prime < g_prime ? r_prime : g_prime;
    c_min = c_min < b_prime ? c_min : b_prime;

    delta = c_max - c_min;

    if (delta == 0.0) {
        h = 0.0;
    } else if (r_prime >= c_max) {
        h = (g_prime - b_prime) / delta;
    } else if (g_prime >= c_max) {
        h = 2 + (b_prime - r_prime) / delta;
    } else {
        h = 4 + (r_prime - g_prime) / delta;
    }
    h *= 60.0;
    if (h < 0.0) {
        h += 360.0;
    }

    if (c_max <= 0.0) {
        s = 0.0;
    } else {
        s = delta / c_max;
    }

    v = c_max;

    Pixel_HSV result = {
        h,
        s,
        v
    };

    return result;
}

Pixel hsv2rgb(Pixel_HSV pixel)
{
    double_t C = pixel.v * pixel.s;
    double_t H_prime = pixel.h;
    if (H_prime >= 360.0)
        H_prime = 0.0;
    H_prime /= 60.0;
    int i = (int)trunc(H_prime);

    double_t X = C * (1.0 - fabs(fmod(H_prime, 2) - 1.0));
    double_t m = pixel.v - C;

    double r_prime, g_prime, b_prime;
    switch (i) {
    case 0:
        r_prime = C;
        g_prime = X;
        b_prime = 0;
        break;
    case 1:
        r_prime = X;
        g_prime = C;
        b_prime = 0;
        break;
    case 2:
        r_prime = 0;
        g_prime = C;
        b_prime = X;
        break;
    case 3:
        r_prime = 0;
        g_prime = X;
        b_prime = C;
        break;
    case 4:
        r_prime = X;
        g_prime = 0;
        b_prime = C;
        break;
    case 5:
        r_prime = C;
        g_prime = 0;
        b_prime = X;
        break;
    }

    Pixel result = {
        round((r_prime + m) * UINT8_MAX),
        round((g_prime + m) * UINT8_MAX),
        round((b_prime + m) * UINT8_MAX)
    };

    return result;
}

void make_mpi_type_Bound(MPI_Datatype* type)
{
    int blocklengths[] = { 2 };
    MPI_Aint displacements[] = {
        offsetof(Bound, width),
        offsetof(Bound, height),
    };
    MPI_Datatype datatypes[] = {
        MPI_UINT32_T,
    };

    MPI_Type_create_struct(1, blocklengths, displacements, datatypes, type);
    MPI_Type_commit(type);
}

void make_mpi_type_Point(MPI_Datatype* type)
{
    int blocklengths[] = { 2 };
    MPI_Aint displacements[] = {
        offsetof(Point, x),
        offsetof(Point, y),
    };
    MPI_Datatype datatypes[] = {
        MPI_DOUBLE,
    };

    MPI_Type_create_struct(1, blocklengths, displacements, datatypes, type);
    MPI_Type_commit(type);
}

void make_mpi_type_RectSize(MPI_Datatype* type)
{
    int blocklengths[] = { 2 };
    MPI_Aint displacements[] = {
        offsetof(RectSize, width),
        offsetof(RectSize, height),
    };
    MPI_Datatype datatypes[] = {
        MPI_DOUBLE,
    };

    MPI_Type_create_struct(1, blocklengths, displacements, datatypes, type);
    MPI_Type_commit(type);
}

void make_mpi_type_Rect(MPI_Datatype* type, MPI_Datatype point_type)
{
    int blocklengths[] = { 2 };
    MPI_Aint displacements[] = {
        offsetof(Rect, ul),
        offsetof(Rect, lr),
    };
    MPI_Datatype datatypes[] = {
        point_type,
    };

    MPI_Type_create_struct(1, blocklengths, displacements, datatypes, type);
    MPI_Type_commit(type);
}

void make_mpi_type_WorkUnit(MPI_Datatype* type, MPI_Datatype bound_type, MPI_Datatype rect_type)
{
    int blocklengths[] = { 1, 1 };
    MPI_Aint displacements[] = {
        offsetof(WorkUnit, bound),
        offsetof(WorkUnit, region),
    };
    MPI_Datatype datatypes[] = {
        bound_type,
        rect_type,
    };

    MPI_Type_create_struct(2, blocklengths, displacements, datatypes, type);
    MPI_Type_commit(type);
}

void make_mpi_types(Local_MPI_Types* types)
{
    make_mpi_type_Pixel(&types->pixel_type);
    make_mpi_type_Bound(&types->bound_type);
    make_mpi_type_Point(&types->point_type);
    make_mpi_type_Rect(&types->rect_type, types->point_type);
    make_mpi_type_RectSize(&types->rectsize_type);
    make_mpi_type_WorkUnit(&types->workunit_type, types->bound_type, types->rect_type);
}

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

void make_rect(Rect* rect, Point center, RectSize size)
{
    rect->ul.x = center.x - (size.width / 2.0);
    rect->ul.y = center.y + (size.height / 2.0);

    rect->lr.x = center.x + (size.width / 2.0);
    rect->lr.y = center.y - (size.height / 2.0);
}

double_t rect_width(Rect r)
{
    return r.lr.x - r.ul.x;
}

double_t rect_height(Rect r)
{
    return r.ul.y - r.lr.y;
}


Point map_coord_to_point(int x, int y, WorkUnit w)
{
    Point p;

    RectSize rs;

    double_t x_offset = (rect_width(w.region) / w.bound.width) * x;
    double_t y_offset = (rect_height(w.region) / w.bound.height) * y;;

    p.x = w.region.ul.x + x_offset;
    p.y = w.region.ul.y - y_offset;

    return p;
}
