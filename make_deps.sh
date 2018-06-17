#!/bin/bash

[[ -d ./dependent_projects/build ]] || mkdir ./dependent_projects/build

cd ./dependent_projects/build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make clean
make -j2
