#include "mpi_test.h"
#include <assert.h>

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

Point map_coord_to_point(int x, int y, WorkUnit w)
{
    Point p;

    RectSize rs;

    rs.width = w.region.lr.x - w.region.ul.x;
    rs.height = w.region.ul.y - w.region.lr.y;

    p.x = rs.width / w.bound.width * x - w.region.ul.y;
    p.y = rs.height / w.bound.height * y - w.region.ul.x;

    return p;
}
