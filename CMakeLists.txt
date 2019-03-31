cmake_minimum_required (VERSION 3.10)

project (mpi_test C)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")

set (CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR})

find_package(MPI REQUIRED)
find_package(GraphicsMagick REQUIRED)

add_library(argparse argparse.c)

add_executable(mpi_test mpi_test.c mpi_test_lib.c)
target_include_directories(mpi_test PUBLIC ${MAGICK_INCLUDE_DIR})
target_include_directories(mpi_test PUBLIC ${MPI_C_HEADER_DIR})
target_link_libraries(mpi_test LINK_PUBLIC argparse)
target_link_libraries(mpi_test LINK_PUBLIC ${MPI_mpi_LIBRARY})
target_link_libraries(mpi_test LINK_PUBLIC ${MAGICK_LIBRARIES})
target_link_libraries(mpi_test LINK_PUBLIC m)
