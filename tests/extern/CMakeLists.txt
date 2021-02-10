# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link
#

# This prevents tests running on dependencies
option( DAW_ENABLE_TESTING "Build unit tests" OFF )
if( DAW_ENABLE_TESTING )
	set( DAW_ENABLE_TESTING OFF )
endif( )

include( FetchContent )
set( FETCHCONTENT_UPDATES_DISCONNECTED ON )

option( BENCHMARK_ENABLE_TESTING "Enable testing in Google Benchmark" OFF )
FetchContent_Declare(google_test
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master)

FetchContent_Declare(google_benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master) 

FetchContent_MakeAvailable(google_test google_benchmark)

FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG v4.1.0
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable( nanobench )

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)

FetchContent_MakeAvailable(fmt)
get_target_property(FMTLIB_IID fmt INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(fmt PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${FMTLIB_IID}")


