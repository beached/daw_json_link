#!/bin/bash

echo "Making build directory"
mkdir build
cd build

echo "Configuring project"
CC=$3 CXX=$4 CXXFLAGS=$5 cmake -G Ninja -DCMAKE_BUILD_TYPE=$1 -DDAW_NUM_RUNS=1 -DDAW_ENABLE_TESTING=ON -DDAW_WERROR=ON -DDAW_ALLOW_SSE42=ON -DCMAKE_CXX_STANDARD=$2 ..

