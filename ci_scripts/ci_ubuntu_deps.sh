#!/bin/bash

echo "Making build directory"
mkdir build
cd build

echo "Configuring project"
CC=clang-9 CXX=clang++-9 CXXFLAGS=-stdlib=libc++ cmake -DCMAKE_BUILD_TYPE=Debug  -DDAW_JSON_USE_SANITIZERS=On ..

echo "Starting dep build"
cmake --build . --config Debug --target -j 2

