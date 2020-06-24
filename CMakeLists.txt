cmake_minimum_required(VERSION 3.10)

project(daw_json_link_prj)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")

message("CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}")
enable_testing()

include("${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt")

include_directories("./include")
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(DAW_JSON_USE_SANITIZERS OFF CACHE STRING "Enable address and undefined behaviour sanitizers")

set(DAW_JSON_NO_CONST_EXPR OFF CACHE STRING "Ensure guaranteed copy elision in array/class types")
if (DAW_JSON_NO_CONST_EXPR)
    message("DAW_JSON_NO_CONST_EXPR=ON: Guaranteed copy elision in class/array types is ON")
    add_definitions(-DDAW_JSON_NO_CONST_EXPR)
else ()
    message("DAW_JSON_NO_CONST_EXPR=OFF: Guaranteed copy elision in class/array types is OFF")
endif ()

set(DAW_JSON_DONT_USE_EXCEPTIONS OFF CACHE STRING "Don't throw exceptions when json errors occur")
if (DAW_JSON_DONT_USE_EXCEPTIONS)
    if (DAW_JSON_DONT_USE_EXCEPTIONS=OFF)
        message("DAW_USE_JSON_EXCEPTIONS=ON: Errors throw json_exception")
        add_definitions(-DDAW_USE_JSON_EXCEPTIONS=true)
    else ()
        message("DAW_USE_JSON_EXCEPTIONSON_DONT_USE_EXCEPTIONS=ON: Errors abort program")
    endif ()
else ()
    message("DAW_JSON_DONT_USE_EXCEPTIONS=OFF: Errors throw json_exception")
    add_definitions(-DDAW_USE_JSON_EXCEPTIONS=true)
endif ()

set(DAW_JSON_CHECK_DEBUG_ONLY OFF CACHE STRING "Only check for json errors in debug")
if (DAW_JSON_CHECK_DEBUG_ONLY)
    if (DAW_JSON_CHECK_DEBUG_ONLY=OFF)
        message("DAW_JSON_CHECK_DEBUG_ONLY=OFF: Always checking for valid json")
    else ()
        message("DAW_JSON_CHECK_DEBUG_ONLY=ON: Only checking for valid json in debug")
        add_definitions(-DDAW_JSON_CHECK_DEBUG_ONLY=true)
    endif ()
else ()
    message("DAW_JSON_CHECK_DEBUG_ONLY=OFF: Always checking for valid json")
    add_definitions(-DDAW_JSON_CHECK_ALWAYS=true)
endif ()

#if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
#	set(DAW_ALLOW_SSE3 ON CACHE STRING "Allow SSE3 Modes")
#else( )
	set(DAW_ALLOW_SSE3 OFF CACHE STRING "Allow SSE3 Modes")
#endif( )
if (DAW_ALLOW_SSE3)
    message("DAW_ALLOW_SSE3=ON: Allow SSE3 Modes")
    add_definitions(-DDAW_ALLOW_SSE3=true)
else ()
    message("DAW_ALLOW_SSE3=OFF: Do not allow SSE3 Modes")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/debug")
    set(GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/release")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release")
    set(GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/debug")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release")
    set(GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/debug")
endif ()

message("Building for: ${CMAKE_BUILD_TYPE}")
message("Checking for glean cache: '${GLEAN_CACHE}'")

option(GLEAN_INSTALL_ROOT "")
if (EXISTS "${GLEAN_INSTALL_ROOT}")
    message("using glean dependencies @ ${GLEAN_INSTALL_ROOT}")
    message("installing to ${GLEAN_INSTALL_ROOT}")
    add_custom_target(dependency_stub)
    include_directories(SYSTEM "${GLEAN_INSTALL_ROOT}/include")
    link_directories("${GLEAN_INSTALL_ROOT}/lib")
elseif (EXISTS "${GLEAN_CACHE}")
    message("using glean dependencies at ${GLEAN_CACHE}")
    add_custom_target(dependency_stub)
    include_directories(SYSTEM "${GLEAN_CACHE}/include")
    link_directories("${GLEAN_CACHE}/lib")
elseif (EXISTS "${GLEAN_CACHE2}")
    message("looks like glean is in use but missing ${GLEAN_CACHE}")
elseif (EXISTS "${CMAKE_SOURCE_DIR}/glean.cmake")
    message("glean.cmake detected")
    include("${CMAKE_SOURCE_DIR}/glean.cmake")
    add_library(dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp")
    add_dependencies(dependency_stub ${DEP_PROJECT_DEPS})
    include_directories(SYSTEM "${CMAKE_BINARY_DIR}/install/include")
    link_directories("${CMAKE_BINARY_DIR}/install/lib")
endif ()

set(HEADER_FOLDER "include")
set(TEST_FOLDER "tests")
set(SOURCE_FOLDER "src")

include_directories(${HEADER_FOLDER})

set(HEADER_FILES
        ${HEADER_FOLDER}/daw/json/daw_json_link.h
        ${HEADER_FOLDER}/daw/json/daw_json_iterator.h
        ${HEADER_FOLDER}/daw/json/impl/daw_iterator_range.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_assert.h
        ${HEADER_FOLDER}/daw/json/impl/daw_iso8601_utils.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_link_types_fwd.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_array_iterator.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_common.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_name.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_real.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_string_quote.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_unsigned_int.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_value.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_parse_value_fwd.h
        ${HEADER_FOLDER}/daw/json/impl/daw_json_to_string.h
        )

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_custom_target(full)
add_dependencies(check full)

add_executable(daw_json_link_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/daw_json_link_test.cpp)
add_dependencies(daw_json_link_test_bin dependency_stub)
add_test(NAME daw_json_link_test COMMAND daw_json_link_test_bin)
add_dependencies(full daw_json_link_test_bin)

add_executable(city_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/city_test.cpp)
add_dependencies(city_test_bin dependency_stub)
#add_test( NAME city_test COMMAND city_test_bin)
add_dependencies(full city_test_bin)

add_executable(int_array_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/int_array_test.cpp)
add_dependencies(int_array_test_bin dependency_stub)
add_test(NAME int_array_test COMMAND int_array_test_bin)
add_dependencies(full int_array_test_bin)

add_executable(int_array_basic_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/int_array_basic_test.cpp)
add_dependencies(int_array_basic_test_bin dependency_stub)
add_test(NAME int_array_basic_test COMMAND int_array_basic_test_bin)
add_dependencies(full int_array_basic_test_bin)

add_executable(float_array_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/float_array_test.cpp)
add_dependencies(float_array_test_bin dependency_stub)
add_test(NAME float_array_test COMMAND float_array_test_bin)
add_dependencies(full float_array_test_bin)

add_executable(float_array_basic_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/float_array_basic_test.cpp)
add_dependencies(float_array_basic_test_bin dependency_stub)
add_test(NAME float_array_basic_test COMMAND float_array_basic_test_bin)
add_dependencies(full float_array_basic_test_bin)

add_executable(simple_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/simple_test.cpp)
add_dependencies(simple_test_bin dependency_stub)
#add_test( NAME simple_test COMMAND simple_test_bin)
add_dependencies(full simple_test_bin)

add_executable(coords_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/coords_test.cpp)
add_dependencies(coords_test_bin dependency_stub)
target_link_libraries(coords_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
#add_test( NAME coords_test COMMAND coords_test_bin)
add_dependencies(full coords_test_bin)

if (NOT DEFINED MSVC)
    add_executable(kostya_bench_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/kostya_bench.cpp)
    add_dependencies(kostya_bench_bin dependency_stub)
    target_link_libraries(kostya_bench_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
    #add_test( NAME kostya_bench COMMAND kostya_bench_bin)
    add_dependencies(full kostya_bench_bin)

    add_executable(kostya_bench2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/kostya_bench2.cpp)
    add_dependencies(kostya_bench2_bin dependency_stub)
    target_link_libraries(kostya_bench2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
    #add_test( NAME kostya_bench2 COMMAND kostya_bench2_bin)
    add_dependencies(full kostya_bench2_bin)
endif ()

add_executable(coords_test2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/coords_test2.cpp)
add_dependencies(coords_test2_bin dependency_stub)
target_link_libraries(coords_test2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
#add_test( NAME coords_test2 COMMAND coords_test2_bin)
add_dependencies(full coords_test2_bin)

add_executable(nativejson_bench_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/nativejson_bench.cpp)
add_dependencies(nativejson_bench_bin dependency_stub)
target_link_libraries(nativejson_bench_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME nativejson_bench COMMAND nativejson_bench_bin ./twitter.json ./citm_catalog.json ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full nativejson_bench_bin)

add_executable(nativejson_bench2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test2.h ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/nativejson_bench2.cpp)
add_dependencies(nativejson_bench2_bin dependency_stub)
target_link_libraries(nativejson_bench2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME nativejson_bench2 COMMAND nativejson_bench2_bin ./twitter.json ./citm_catalog.json ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full nativejson_bench2_bin)

add_executable(nativejson_bench_basic_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/nativejson_bench_basic.cpp)
add_dependencies(nativejson_bench_basic_bin dependency_stub)
target_link_libraries(nativejson_bench_basic_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME nativejson_bench_basic COMMAND nativejson_bench_basic_bin ./twitter.json ./citm_catalog.json ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full nativejson_bench_basic_bin)

add_executable(nativejson_bench_basic2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test2.h ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/nativejson_bench_basic2.cpp)
add_dependencies(nativejson_bench_basic2_bin dependency_stub)
target_link_libraries(nativejson_bench_basic2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME nativejson_bench_basic2 COMMAND nativejson_bench_basic2_bin ./twitter.json ./citm_catalog.json ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full nativejson_bench_basic2_bin)

add_executable(citm_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/citm_test.cpp)
add_dependencies(citm_test_bin dependency_stub)
target_link_libraries(citm_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME citm_test COMMAND citm_test_bin ./citm_catalog.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full citm_test_bin)

add_executable(citm_test_basic_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/citm_test_basic.cpp)
add_dependencies(citm_test_basic_bin dependency_stub)
target_link_libraries(citm_test_basic_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME citm_test_basic COMMAND citm_test_basic_bin ./citm_catalog.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full citm_test_basic_bin)

add_executable(twitter_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h ${TEST_FOLDER}/twitter_test.cpp)
add_dependencies(twitter_test_bin dependency_stub)
target_link_libraries(twitter_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME twitter_test COMMAND twitter_test_bin ./twitter.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full twitter_test_bin)

add_executable(twitter_test2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test2.h ${TEST_FOLDER}/twitter_test2.cpp)
add_dependencies(twitter_test2_bin dependency_stub)
target_link_libraries(twitter_test2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME twitter_test2 COMMAND twitter_test2_bin ./twitter.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full twitter_test2_bin)

add_executable(twitter_test_basic_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h ${TEST_FOLDER}/twitter_test_basic.cpp)
add_dependencies(twitter_test_basic_bin dependency_stub)
target_link_libraries(twitter_test_basic_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME twitter_test_basic COMMAND twitter_test_basic_bin ./twitter.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full twitter_test_basic_bin)

add_executable(twitter_test_basic2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h ${TEST_FOLDER}/twitter_test_basic2.cpp)
add_dependencies(twitter_test_basic2_bin dependency_stub)
target_link_libraries(twitter_test_basic2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME twitter_test_basic2 COMMAND twitter_test_basic2_bin ./twitter.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full twitter_test_basic2_bin)

add_executable(canada_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/canada_test.cpp)
add_dependencies(canada_test_bin dependency_stub)
target_link_libraries(canada_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME canada_test COMMAND canada_test_bin ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full canada_test_bin)

add_executable(canada_test_basic_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/canada_test_basic.cpp)
add_dependencies(canada_test_basic_bin dependency_stub)
target_link_libraries(canada_test_basic_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME canada_test_basic COMMAND canada_test_basic_bin ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full canada_test_basic_bin)

add_executable(kv_map_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/kv_map_test.cpp)
add_dependencies(kv_map_test_bin dependency_stub)
add_test(NAME kv_map_test COMMAND kv_map_test_bin)
add_dependencies(full kv_map_test_bin)

add_executable(cookbook_kv1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_kv1_test.cpp)
add_dependencies(cookbook_kv1_test_bin dependency_stub)
add_test(NAME cookbook_kv1_test COMMAND cookbook_kv1_test_bin ./cookbook_kv1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_kv1_test_bin)

add_executable(cookbook_kv2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_kv2_test.cpp)
add_dependencies(cookbook_kv2_test_bin dependency_stub)
add_test(NAME cookbook_kv2_test COMMAND cookbook_kv2_test_bin ./cookbook_kv2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_kv2_test_bin)

add_executable(cookbook_class1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_class1_test.cpp)
add_dependencies(cookbook_class1_test_bin dependency_stub)
add_test(NAME cookbook_class1_test COMMAND cookbook_class1_test_bin ./cookbook_class1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_class1_test_bin)

add_executable(cookbook_class2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_class2_test.cpp)
add_dependencies(cookbook_class2_test_bin dependency_stub)
add_test(NAME cookbook_class2_test COMMAND cookbook_class2_test_bin ./cookbook_class2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_class2_test_bin)

add_executable(cookbook_class3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_class3_test.cpp)
add_dependencies(cookbook_class3_test_bin dependency_stub)
add_test(NAME cookbook_class3_test COMMAND cookbook_class3_test_bin ./cookbook_class3.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_class3_test_bin)

add_executable(cookbook_array1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_array1_test.cpp)
add_dependencies(cookbook_array1_test_bin dependency_stub)
add_test(NAME cookbook_array1_test COMMAND cookbook_array1_test_bin ./cookbook_array1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_array1_test_bin)

add_executable(cookbook_array2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_array2_test.cpp)
add_dependencies(cookbook_array2_test_bin dependency_stub)
add_test(NAME cookbook_array2_test COMMAND cookbook_array2_test_bin ./cookbook_array2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_array2_test_bin)

add_executable(cookbook_array3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_array3_test.cpp)
add_dependencies(cookbook_array3_test_bin dependency_stub)
add_test(NAME cookbook_array3_test COMMAND cookbook_array3_test_bin ./cookbook_array3.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_array3_test_bin)

add_executable(cookbook_escaped_strings1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_escaped_strings1_test.cpp)
add_dependencies(cookbook_escaped_strings1_test_bin dependency_stub)
add_test(NAME cookbook_escaped_strings1_test COMMAND cookbook_escaped_strings1_test_bin ./cookbook_escaped_strings1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_escaped_strings1_test_bin)

add_executable(full_unicode_roundtrip_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/full_unicode_roundtrip_test.cpp)
add_dependencies(full_unicode_roundtrip_test_bin dependency_stub)
add_test(NAME full_unicode_roundtrip_test COMMAND full_unicode_roundtrip_test_bin ./full_unicode.json ./full_unicode_escaped.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full full_unicode_roundtrip_test_bin)

add_executable(make_full24_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/make_full24.cpp)
add_dependencies(make_full24_bin dependency_stub)
add_dependencies(full make_full24_bin)

add_executable(cookbook_enums1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_enums1_test.cpp)
add_dependencies(cookbook_enums1_test_bin dependency_stub)
add_test(NAME cookbook_enums1_test COMMAND cookbook_enums1_test_bin ./cookbook_enums1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_enums1_test_bin)

add_executable(cookbook_enums2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_enums2_test.cpp)
add_dependencies(cookbook_enums2_test_bin dependency_stub)
add_test(NAME cookbook_enums2_test COMMAND cookbook_enums2_test_bin ./cookbook_enums2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_enums2_test_bin)

add_executable(cookbook_graphs1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_graphs1_test.cpp)
add_dependencies(cookbook_graphs1_test_bin dependency_stub)
add_test(NAME cookbook_graphs1_test COMMAND cookbook_graphs1_test_bin ./cookbook_graphs1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_graphs1_test_bin)

add_executable(cookbook_optional_values1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_optional_values1_test.cpp)
add_dependencies(cookbook_optional_values1_test_bin dependency_stub)
add_test(NAME cookbook_optional_values1_test COMMAND cookbook_optional_values1_test_bin ./cookbook_optional_values1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_optional_values1_test_bin)

add_executable(cookbook_variant1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_variant1_test.cpp)
add_dependencies(cookbook_variant1_test_bin dependency_stub)
add_test(NAME cookbook_variant1_test COMMAND cookbook_variant1_test_bin ./cookbook_variant1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_variant1_test_bin)

add_executable(small_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/small_test.cpp)
add_dependencies(small_test_bin dependency_stub)
add_test(NAME small_test COMMAND small_test_bin ./small_test.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full small_test_bin)

add_executable(should_fail_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/should_fail_001.cpp)
add_dependencies(should_fail_bin dependency_stub)
add_test(should_fail_test should_fail_bin)
add_dependencies(full should_fail_bin)

add_executable(trailing_commas_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/trailing_commas.cpp)
add_dependencies(trailing_commas_bin dependency_stub)
add_test(trailing_commas_test trailing_commas_bin)
add_dependencies(full trailing_commas_bin)

add_executable(cookbook_parsing_individual_members1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_parsing_individual_members1_test.cpp)
add_dependencies(cookbook_parsing_individual_members1_test_bin dependency_stub)
add_test(NAME cookbook_parsing_individual_members1_test COMMAND cookbook_parsing_individual_members1_test_bin ./cookbook_parsing_individual_members1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_parsing_individual_members1_test_bin)

add_executable(cookbook_parsing_individual_members2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_parsing_individual_members2_test.cpp)
add_dependencies(cookbook_parsing_individual_members2_test_bin dependency_stub)
add_test(NAME cookbook_parsing_individual_members2_test COMMAND cookbook_parsing_individual_members2_test_bin ./cookbook_parsing_individual_members2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_parsing_individual_members2_test_bin)

add_executable(cookbook_parsing_individual_members3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_parsing_individual_members3_test.cpp)
add_dependencies(cookbook_parsing_individual_members3_test_bin dependency_stub)
add_test(NAME cookbook_parsing_individual_members3_test COMMAND cookbook_parsing_individual_members3_test_bin ./cookbook_parsing_individual_members3.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_parsing_individual_members3_test_bin)

add_executable(cookbook_variant2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_variant2_test.cpp)
add_dependencies(cookbook_variant2_test_bin dependency_stub)
add_test(NAME cookbook_variant2_test COMMAND cookbook_variant2_test_bin ./cookbook_variant2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_variant2_test_bin)

add_executable(optional_tagged_variant_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/optional_tagged_variant_test.cpp)
add_dependencies(optional_tagged_variant_test_bin dependency_stub)
add_test(NAME optional_tagged_variant_test COMMAND optional_tagged_variant_test_bin ./optional_tagged_variant.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full optional_tagged_variant_test_bin)

add_executable(optional_variant_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/optional_variant_test.cpp)
add_dependencies(optional_variant_test_bin dependency_stub)
add_test(NAME optional_variant_test COMMAND optional_variant_test_bin ./optional_variant.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full optional_variant_test_bin)

add_executable(daw_murmur3_test_bin EXCLUDE_FROM_ALL ${TEST_FOLDER}/daw_murmur3_test.cpp)
add_dependencies(daw_murmur3_test_bin dependency_stub)
add_test(NAME daw_murmur3_test COMMAND daw_murmur3_test_bin)
add_dependencies(full daw_murmur3_test_bin)

add_executable(apache_builds_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/apache_builds.h ${TEST_FOLDER}/apache_builds_test.cpp)
add_dependencies(apache_builds_test_bin dependency_stub)
target_link_libraries(apache_builds_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME apache_builds_test COMMAND apache_builds_test_bin ./apache_builds.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full apache_builds_test_bin)

add_executable(cookbook_dates1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_dates1_test.cpp)
add_dependencies(cookbook_dates1_test_bin dependency_stub)
add_test(NAME cookbook_dates1_test COMMAND cookbook_dates1_test_bin ./cookbook_dates1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_dates1_test_bin)

add_executable(cookbook_dates2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_dates2_test.cpp)
add_dependencies(cookbook_dates2_test_bin dependency_stub)
add_test(NAME cookbook_dates2_test COMMAND cookbook_dates2_test_bin ./cookbook_dates2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_dates2_test_bin)

add_executable(cookbook_dates3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_dates3_test.cpp)
add_dependencies(cookbook_dates3_test_bin dependency_stub)
add_test(NAME cookbook_dates3_test COMMAND cookbook_dates3_test_bin ./cookbook_dates3.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_dates3_test_bin)

add_executable(no_move_or_copy_cls_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/no_move_or_copy_cls_test.cpp)
add_dependencies(no_move_or_copy_cls_test_bin dependency_stub)
add_test(NAME no_move_or_copy_cls_test COMMAND no_move_or_copy_cls_test_bin)
add_dependencies(full no_move_or_copy_cls_test_bin)

add_executable(cookbook_class_from_array1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_class_from_array1_test.cpp)
add_dependencies(cookbook_class_from_array1_test_bin dependency_stub)
add_test(NAME cookbook_class_from_array1_test COMMAND cookbook_class_from_array1_test_bin ./cookbook_class_from_array1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_class_from_array1_test_bin)

add_executable(cookbook_class_from_array2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_class_from_array2_test.cpp)
add_dependencies(cookbook_class_from_array2_test_bin dependency_stub)
add_test(NAME cookbook_class_from_array2_test COMMAND cookbook_class_from_array2_test_bin ./cookbook_class_from_array2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_class_from_array2_test_bin)

add_executable(cookbook_numbers1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_numbers1_test.cpp)
add_dependencies(cookbook_numbers1_test_bin dependency_stub)
add_test(NAME cookbook_numbers1_test COMMAND cookbook_numbers1_test_bin ./cookbook_numbers1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_numbers1_test_bin)

add_executable(cookbook_numbers2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_numbers2_test.cpp)
add_dependencies(cookbook_numbers2_test_bin dependency_stub)
add_test(NAME cookbook_numbers2_test COMMAND cookbook_numbers2_test_bin ./cookbook_numbers2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_numbers2_test_bin)

add_executable(cookbook_unknown_types_and_delayed_parsing1_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_unknown_types_and_delayed_parsing1_test.cpp)
add_dependencies(cookbook_unknown_types_and_delayed_parsing1_test_bin dependency_stub)
add_test(NAME cookbook_unknown_types_and_delayed_parsing1_test COMMAND cookbook_unknown_types_and_delayed_parsing1_test_bin ./cookbook_unknown_types_and_delayed_parsing1.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_unknown_types_and_delayed_parsing1_test_bin)

add_executable(cookbook_unknown_types_and_delayed_parsing2_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/cookbook_unknown_types_and_delayed_parsing2_test.cpp)
add_dependencies(cookbook_unknown_types_and_delayed_parsing2_test_bin dependency_stub)
add_test(NAME cookbook_unknown_types_and_delayed_parsing2_test COMMAND cookbook_unknown_types_and_delayed_parsing2_test_bin ./cookbook_unknown_types_and_delayed_parsing2.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full cookbook_unknown_types_and_delayed_parsing2_test_bin)

add_executable(stream_output_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/stream_output_test.cpp)
add_dependencies(stream_output_test_bin dependency_stub)
add_test(NAME stream_output_test COMMAND stream_output_test_bin)
add_dependencies(full stream_output_test_bin)

add_executable(int_sanity_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/int_sanity_test.cpp)
add_dependencies(int_sanity_test_bin dependency_stub)
add_test(NAME int_sanity_test COMMAND int_sanity_test_bin)
add_dependencies(full int_sanity_test_bin)

add_executable(nativejson_roundtrip_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/geojson.h ${TEST_FOLDER}/nativejson_roundtrip.cpp)
add_dependencies(nativejson_roundtrip_bin dependency_stub)
target_link_libraries(nativejson_roundtrip_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME nativejson_roundtrip COMMAND nativejson_roundtrip_bin ./twitter.json ./citm_catalog.json ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full nativejson_roundtrip_bin)

add_executable(test_details_skip_string_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_skip_string.cpp)
add_dependencies(test_details_skip_string_bin dependency_stub)
add_test(test_details_skip_string_test test_details_skip_string_bin)
add_dependencies(full test_details_skip_string_bin)

add_executable(test_details_parse_value_class_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_class.cpp)
add_dependencies(test_details_parse_value_class_bin dependency_stub)
add_test(test_details_parse_value_class_test test_details_parse_value_class_bin)
add_dependencies(full test_details_parse_value_class_bin)

add_executable(test_details_parse_value_array_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_array.cpp)
add_dependencies(test_details_parse_value_array_bin dependency_stub)
add_test(test_details_parse_value_array_test test_details_parse_value_array_bin)
add_dependencies(full test_details_parse_value_array_bin)

add_executable(test_details_parse_value_custom_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_custom.cpp)
add_dependencies(test_details_parse_value_custom_bin dependency_stub)
add_test(test_details_parse_value_custom_test test_details_parse_value_custom_bin)
add_dependencies(full test_details_parse_value_custom_bin)

add_executable(test_details_skip_array_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_skip_array.cpp)
add_dependencies(test_details_skip_array_bin dependency_stub)
add_test(test_details_skip_array_test test_details_skip_array_bin)
add_dependencies(full test_details_skip_array_bin)

add_executable(test_details_skip_class_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_skip_class.cpp)
add_dependencies(test_details_skip_class_bin dependency_stub)
add_test(test_details_skip_class_test test_details_skip_class_bin)
add_dependencies(full test_details_skip_class_bin)

add_executable(test_details_parse_value_null_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_null.cpp)
add_dependencies(test_details_parse_value_null_bin dependency_stub)
add_test(test_details_parse_value_null_test test_details_parse_value_null_bin)
add_dependencies(full test_details_parse_value_null_bin)

add_executable(test_details_parse_value_unsigned_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_unsigned.cpp)
add_dependencies(test_details_parse_value_unsigned_bin dependency_stub)
add_test(test_details_parse_value_unsigned_test test_details_parse_value_unsigned_bin)
add_dependencies(full test_details_parse_value_unsigned_bin)

add_executable(test_details_parse_value_signed_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_signed.cpp)
add_dependencies(test_details_parse_value_signed_bin dependency_stub)
add_test(test_details_parse_value_signed_test test_details_parse_value_signed_bin)
add_dependencies(full test_details_parse_value_signed_bin)

add_executable(test_details_parse_value_real_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_parse_value_real.cpp)
add_dependencies(test_details_parse_value_real_bin dependency_stub)
add_test(test_details_parse_value_real_test test_details_parse_value_real_bin)
add_dependencies(full test_details_parse_value_real_bin)

add_executable(test_details_skip_number_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_details_skip_number.cpp)
add_dependencies(test_details_skip_number_bin dependency_stub)
add_test(test_details_skip_number_test test_details_skip_number_bin)
add_dependencies(full test_details_skip_number_bin)

add_executable(json_benchmark_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/json_benchmark.cpp)
add_dependencies(json_benchmark_bin dependency_stub)
add_test(NAME json_benchmark_test COMMAND json_benchmark_bin ./apache_builds.json ./twitter.json ./citm_catalog.json ./canada.json WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test_data/")
add_dependencies(full json_benchmark_bin)

cmake_host_system_information(RESULT _proc_desc QUERY PROCESSOR_DESCRIPTION)
string(REPLACE "#" "" _proc_desc ${_proc_desc})
cmake_host_system_information(RESULT _os_name QUERY OS_NAME)
string(REPLACE "#" "" _os_name ${_os_name})
cmake_host_system_information(RESULT _os_rel QUERY OS_RELEASE)
string(REPLACE "#" "" _os_rel ${_os_rel})
cmake_host_system_information(RESULT _os_ver QUERY OS_VERSION)
string(REPLACE "#" "" _os_ver ${_os_ver})
cmake_host_system_information(RESULT _os_plat QUERY OS_PLATFORM)
string(REPLACE "#" "" _os_plat ${_os_plat})
target_compile_definitions(json_benchmark_bin PRIVATE -DPROCESSOR_DESCRIPTION="${_proc_desc}")
target_compile_definitions(json_benchmark_bin PRIVATE -DOS_NAME="${_os_name}")
target_compile_definitions(json_benchmark_bin PRIVATE -DOS_RELEASE="${_os_rel}")
target_compile_definitions(json_benchmark_bin PRIVATE -DOS_VERSION="${_os_ver}")
target_compile_definitions(json_benchmark_bin PRIVATE -DOS_PLATFORM="${_os_plat}")
target_compile_definitions(json_benchmark_bin PRIVATE -DBUILD_TYPE="${CMAKE_BUILD_TYPE}")

add_executable(test_stateful_json_value_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/test_stateful_json_value.cpp)
add_dependencies(test_stateful_json_value_bin dependency_stub)
add_test(test_stateful_json_value_test test_stateful_json_value_bin)
add_dependencies(full test_stateful_json_value_bin)




install(DIRECTORY ${HEADER_FOLDER}/daw/json/ DESTINATION include/daw/json/)
install(DIRECTORY ${HEADER_FOLDER}/third_party/ DESTINATION include/third_party/)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")

