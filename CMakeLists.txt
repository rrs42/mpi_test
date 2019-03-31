cmake_minimum_required (VERSION 3.1)

project (mpi_test)

set (CMAKE_CXX_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)

pkg_search_module(OMPI REQUIRED ompi-c)
include_directories(${OMPI_INCLUDE_DIRS})

pkg_search_module(GM REQUIRED GraphicsMagick)
include_directories(mpi_test PUBLIC ${GM_INCLUDE_DIRS})

add_library(argparse argparse.c)

add_executable(mpi_test mpi_test.c mpi_test_lib.c)
target_link_libraries(mpi_test LINK_PUBLIC argparse)
target_link_libraries(mpi_test LINK_PUBLIC ${OMPI_LINK_LIBRARIES})
target_link_libraries(mpi_test LINK_PUBLIC ${GM_LINK_LIBRARIES})
target_link_libraries(mpi_test LINK_PUBLIC m)
