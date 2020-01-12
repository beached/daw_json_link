#!/bin/bash

if [ ! -f /usr/local/bin/cmake ];then
	find ${HOME}/cmake_bin
	echo "###################################"
	echo "cmake build: starting, not in cache"
  mkdir cmake_build
  cd cmake_build
	echo "###################################"
	echo "cmake build: download"
  wget --quiet https://github.com/Kitware/CMake/releases/download/v3.16.2/cmake-3.16.2.tar.gz
  tar xzf cmake-3.16.2.tar.gz
  cd cmake-3.16.2
	echo "###################################"
	echo "cmake build: bootstrap"
  ./bootstrap --prefix=${HOME}/cmake_bin
	echo "###################################"
	echo "cmake build: make"
  make
	echo "###################################"
	echo "cmake build: installing cmake"
  sudo make install
  cd ..
  cd ..
	echo "###################################"
	echo "cmake build: complete"
fi
