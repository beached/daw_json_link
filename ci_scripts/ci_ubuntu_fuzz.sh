#!/bin/bash

cd build
cmake --build . --target daw_json_fuzzing --config $1 -j 2
UBSAN_OPTIONS=halt_on_errors=1 ./tests/daw_json_fuzzing -max_len=4000 -max_total_time=120


