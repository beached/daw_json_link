#!/bin/bash

cd build

echo "Starting full build"
cmake --build . --config $1 --target full -j 2

