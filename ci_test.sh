#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug -j 2 -target full
ctest

