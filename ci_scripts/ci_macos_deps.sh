#!/bin/bash

echo "Making build directory"
mkdir build
cd build

echo "Configuring project"
cmake -DCMAKE_BUILD_TYPE=$1 -DDAW_NUM_RUNS=1 -DDAW_ENABLE_TESTING=ON -DDAW_WERROR=ON ..

echo "Starting dep build"
cmake --build . --config $1 -j 2

