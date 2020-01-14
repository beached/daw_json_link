#!/bin/bash

cd build

echo "Starting full build"
cmake --build . --config Debug --target full -j 2 -- -k 1000

