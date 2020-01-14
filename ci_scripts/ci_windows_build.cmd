cd build

REM ##############################
REM Building
cmake --build . --config Debug --target -j 2 -- -k 1000
