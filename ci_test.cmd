copy glean.cmake.renamewin glean.cmake
md build
cd build
pwd
echo "Configuring project"
cmake -DCMAKE_BUILD_TYPE=Debug ..
echo "Starting dep build"
cmake --build . --config Debug --target -j 2
echo "Starting full build"
cmake --build . --config Debug --target full -j 2
ctest -C Debug
