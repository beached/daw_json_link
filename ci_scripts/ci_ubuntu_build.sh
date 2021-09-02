#!/bin/bash

cd build

echo "Starting full build"
cmake --build . --target full --config $1 -j 2 -v

