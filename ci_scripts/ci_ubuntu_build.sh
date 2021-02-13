#!/bin/bash

cd build

echo "Starting full build"
cmake --build . --config $1 -j 2

