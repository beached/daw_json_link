#!/bin/bash

wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
apt-add-repository 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal-12 main'
apt-add-repository 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal-13 main'


