#!/bin/bash

cp glean.cmake.renamewin glean.cmake
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --target full -j 2
ctest -C Debug

