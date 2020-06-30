#!/bin/bash

cd build
ctest -C $1 -VV -j5 --timeout 300


