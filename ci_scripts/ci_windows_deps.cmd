@echo off

echo "Making build directory"
md build
cd build

echo "Configuring project"
cmake -DCMAKE_BUILD_TYPE=Debug ..

echo "Starting dep build"
cmake --build . --config Debug --target -j 2
