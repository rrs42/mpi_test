#!/bin/bash -x

if [ -d ./build ]; then
	rm -rf ./build
fi

mkdir build

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Debug"
#CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release"

cmake ${CMAKE_OPTIONS} . -B build
(
	cd build
	cmake ${CMAKE_OPTIONS} ..
)
(
	cd build
	cmake --build .
)
