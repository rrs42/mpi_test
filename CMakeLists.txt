cmake_minimum_required (VERSION 2.6)

project (mpi_test)

include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
pkg_search_module(GM REQUIRED GraphicsMagick)

find_library(GRMG_LIBS ${GM_LIBRARIES} PATHS ${GM_LIBRARY_DIRS})

add_library(argparse argparse.c)

add_executable(mpi_test mpi_test.c mpi_test_lib.c)
target_link_libraries(mpi_test LINK_PUBLIC argparse)

#if(NOT APPLE)
    target_link_libraries(mpi_test LINK_PUBLIC m)
    #endif()

target_include_directories(mpi_test PUBLIC ${GM_INCLUDE_DIRS})
target_link_libraries(mpi_test LINK_PUBLIC ${GRMG_LIBS})
#target_link_directories(mpi_test PUBLIC ${GM_LIBRARY_DIRS})
