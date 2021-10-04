cd build

set NANOBENCH_SUPPRESS_WARNINGS=1
ctest -C %2 -j5 -VV --timeout 1200

