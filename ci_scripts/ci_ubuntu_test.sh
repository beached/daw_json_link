#!/bin/bash

export NANOBENCH_SUPPRESS_WARNINGS=1
cd build
ctest -C $1 -VV -j1 --timeout 500 --output-on-failure


