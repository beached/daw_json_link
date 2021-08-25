#!/bin/bash

set -e

cd build

echo "#################################"
echo "Starting Install Tests"
sudo cmake --install .
cd ../cmake/test_project/
mkdir build
cd build
cmake ..
cmake --build .
ctest -C Debug -VV
