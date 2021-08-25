# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/header_libraries
#

cmake_minimum_required(VERSION 3.14)

project("install-test"
        VERSION "1.0.0"
        DESCRIPTION "Tests that find_package works"
        HOMEPAGE_URL "https://github.com/beached/header_libraries"
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package( daw-header-libraries REQUIRED )
find_package( daw-utf-range REQUIRED )
find_package( daw-json-link REQUIRED )

enable_testing()

add_executable( ${PROJECT_NAME} test.cpp )
target_compile_options( ${PROJECT_NAME} INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/permissive->)
target_link_libraries( ${PROJECT_NAME} PRIVATE daw::daw-header-libraries daw::daw-utf-range daw::daw-json-link )
add_test( ${PROJECT_NAME}_test ${PROJECT_NAME} )
