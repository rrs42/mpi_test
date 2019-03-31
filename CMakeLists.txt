cmake_minimum_required (VERSION 3.1)

project (mpi_test)
set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR})

find_package(MPI)

find_package(PkgConfig REQUIRED)


pkg_search_module(GM REQUIRED GraphicsMagick)
include_directories(mpi_test PUBLIC ${GM_INCLUDE_DIRS})

add_library(argparse argparse.c)

add_executable(mpi_test mpi_test.c mpi_test_lib.c)
target_include_directories(mpi_test PUBLIC ${MPI_C_HEADER_DIR})
target_link_libraries(mpi_test LINK_PUBLIC argparse)
target_link_libraries(mpi_test LINK_PUBLIC ${MPI_mpi_LIBRARY})
target_link_libraries(mpi_test LINK_PUBLIC ${GM_LINK_LIBRARIES})
target_link_libraries(mpi_test LINK_PUBLIC m)
