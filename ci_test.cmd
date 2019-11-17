
md build
cd build
pwd
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --target full -j 2
ctest -C Debug
