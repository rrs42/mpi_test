#!/bin/bash -x

clean_only=0
build_type="Release"

while getopts "cdr" OPTION; do
	case "$OPTION" in
	c)
		echo "Cleanup..."
		clean_only=1
		;;
	d)
		build_type="Debug"
		;;
	r)
		build_type="Release"
		;;
	esac
done

if [ -d ./build ]; then
	rm -rf ./build
fi

if [ $clean_only == 1 ]; then
	exit
fi

mkdir build

CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=${build_type}"
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
