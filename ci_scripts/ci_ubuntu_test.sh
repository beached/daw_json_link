#!/bin/bash

cd build
ctest -C Debug -VV -j5 --timeout 300


