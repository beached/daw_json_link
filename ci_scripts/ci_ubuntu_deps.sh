#!/bin/bash

echo "Making build directory"
mkdir build
cd build

echo "Configuring project"
CC=$3 CXX=$4 CXXFLAGS=$5 cmake -DCMAKE_BUILD_TYPE=$1 -DDAW_NUM_RUNS=1 -DDAW_ENABLE_TESTING=ON -DDAW_WERROR=ON -DDAW_JSON_USE_SANITIZERS=ON -DCMAKE_CXX_STANDARD=$2 ..

echo "Starting dep build"
cmake --build . --config $1 -j 2

