#!/bin/bash

echo "Making build directory"
mkdir build
cd build

echo "Configuring project"
cmake -DCMAKE_BUILD_TYPE=Debug ..

echo "Starting dep build"
cmake --build . --config Debug --target -j 2

