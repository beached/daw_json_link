#!/bin/bash
#version 2018_11_17 

[[ -d ./dependent_projects/build ]] || mkdir ./dependent_projects/build
[[ -d ./dependent_projects/build/release ]] || mkdir ./dependent_projects/build/release
[[ -d ./dependent_projects/build/debug ]] || mkdir ./dependent_projects/build/debug

cd ./dependent_projects/build/release
cmake ../.. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make clean
make -j2
cd ../debug
cmake ../.. -DCMAKE_BUILD_TYPE=debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make clean
make -j2
