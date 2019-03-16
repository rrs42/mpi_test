cmake_minimum_required (VERSION 2.6)

project (mpi_test)

include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
# pkg_check_modules(GraphicsMagick REQUIRED)
pkg_search_module(GM REQUIRED GraphicsMagick)

add_library(argparse argparse.c)

add_executable(mpi_test mpi_test.c mpi_test_lib.c)
target_link_libraries(mpi_test LINK_PUBLIC argparse)
target_include_directories(mpi_test PUBLIC ${GM_INCLUDE_DIRS})
target_link_libraries(mpi_test ${GM_LIBRARIES})
target_link_directories(mpi_test PUBLIC ${GM_LIBRARY_DIRS})
