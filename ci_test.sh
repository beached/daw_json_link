#!/bin/bash

mkdir build
cd build
CC=clang-9 CXX=clang++-9 CXXFLAGS=-stdlib=libc++ cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --target full -j 2
ctest -C Debug

