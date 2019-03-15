#!/bin/bash -x

if [ -d ./build ] ; then
	rm -rf ./build
fi

export OMPI_CC=gcc-8
export OMPI_CXX=c++-8
export CC=$(which mpicc)
export CXX=$(which mpicxx)
CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Debug"

cmake ${CMAKE_OPTIONS} . -B build
( cd build ; cmake --build . )
