cmake_minimum_required (VERSION 2.6)

project (mpi_test)

include_directories(${PROJECT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)

pkg_search_module(OMPI REQUIRED ompi-c)
include_directories(${OMPI_INCLUDE_DIRS})
find_library(MPI_LIBS ${OMPI_LIBRARIES} PATHS ${OMPI_LIBRARY_DIRS})

pkg_search_module(GM REQUIRED GraphicsMagick)
include_directories(mpi_test PUBLIC ${GM_INCLUDE_DIRS})
find_library(GRMG_LIBS ${GM_LIBRARIES} PATHS ${GM_LIBRARY_DIRS})

add_library(argparse argparse.c)

add_executable(mpi_test mpi_test.c mpi_test_lib.c)
target_link_libraries(mpi_test LINK_PUBLIC argparse)
target_link_libraries(mpi_test LINK_PUBLIC ${MPI_LIBS})
target_link_libraries(mpi_test LINK_PUBLIC ${GRMG_LIBS})
target_link_libraries(mpi_test LINK_PUBLIC m)

#target_link_directories(mpi_test PUBLIC ${GM_LIBRARY_DIRS})
