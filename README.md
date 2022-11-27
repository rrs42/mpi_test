MPI test program
================

Generates a (currently monochrome) Mandelbrot set image using MPI to
distribute the work.

Tested with:
- clang, OpenMPI4 on Mac OS
- gcc, OpenMPI4 on Linux (RHEL 8)
- Intel oneAPI, Intel MPI (2022) on Linux (RHEL 8)

When run without mpirun it will generate the image with a single thread.

Requires:
- CMake
- An MPI Library (e.g. OpenMPI or IntelMPI)
- GraphicsMagick library (The code should work with ImageMagick but would require a bit of CMake tweaking)

Work is distributed by banding the image horizontally based on the number
of workers. Will probably fail if the number of mpithreads is greater than
the number of rows in the output image.

Program arguments:

 -o [file]       Output file name

 -x [width]      Image width (default 1024)

 -y [height]     Image height (default 768)

