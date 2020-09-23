cmake_minimum_required(VERSION 3.10)

project("json_link_tests" LANGUAGES CXX)

# ---- Import root project ----

include(FetchContent)
FetchContent_Declare(json_link SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
FetchContent_MakeAvailable(json_link)

# ---- Definitions and options ----

# for DAW_TEST_DEFINITIONS and DAW_COMPILER_OPTIONS
include(cmake/test_compiler_options.cmake)

# ---- Tests ----

enable_testing()

add_library(json_test INTERFACE)
target_link_libraries(json_test INTERFACE daw::json_link daw::header_libraries daw::utf_range)
target_include_directories(json_test INTERFACE include)

add_custom_target(full)

set(DAW_TEST_DATA_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../test_data")

# daw_add_test(NAME <name>
#         [SOURCE <source_path>]
#         [ARGS <argN>...]
#         [NAME_SUFFIX]
#         [DONT_ADD_TEST]
#         [WITH_DATA])
#
# NAME_SUFFIX: suffix the test's name used for `add_test` with `_test`
#
# SOURCE: path to the source file for the test, defaults to src/<name>.cpp
#
# WITH_DATA: defines the WORKING_DIRECTORY argument for `add_test` to be in the test_data directory
#
# ARGS: additional arguments in the `add_test` COMMAND values
#
# DONT_ADD_TEST: `add_test` won't be called if defined
#
function(daw_add_test)
  cmake_parse_arguments(DAW_TEST "DONT_ADD_TEST;WITH_DATA;NAME_SUFFIX" "NAME;SOURCE" "ARGS" ${ARGN})

  set(test_name "${DAW_TEST_NAME}")
  if (DAW_TEST_NAME_SUFFIX)
    string(APPEND test_name _test)
  endif ()

  if (NOT DAW_TEST_SOURCE)
    set(DAW_TEST_SOURCE src/${DAW_TEST_NAME}.cpp)
  endif ()

  add_executable(${DAW_TEST_NAME} ${DAW_TEST_SOURCE})
  target_link_libraries(${DAW_TEST_NAME} PRIVATE json_test)
  target_compile_options(${DAW_TEST_NAME} PRIVATE ${DAW_COMPILER_OPTIONS})
  target_compile_features(${DAW_TEST_NAME} PRIVATE cxx_std_17)
  target_compile_definitions(${DAW_TEST_NAME} PRIVATE ${DAW_TEST_DEFINITIONS})
  add_dependencies(full ${DAW_TEST_NAME})

  if (NOT DAW_TEST_DONT_ADD_TEST)
    set(working_dir)
    if (DAW_TEST_WITH_DATA)
      set(working_dir "WORKING_DIRECTORY;${DAW_TEST_DATA_DIRECTORY}")
    endif ()

    add_test(NAME ${test_name} COMMAND ${DAW_TEST_NAME} ${DAW_TEST_ARGS} ${working_dir})
  endif ()
endfunction()

daw_add_test(NAME daw_json_link_test)

daw_add_test(NAME city_test DONT_ADD_TEST)

daw_add_test(NAME int_array_test)

daw_add_test(NAME int_array_basic_test)

daw_add_test(NAME float_array_test)

daw_add_test(NAME float_array_basic_test)

daw_add_test(NAME simple_test DONT_ADD_TEST)

daw_add_test(NAME coords_test DONT_ADD_TEST)

if (NOT DEFINED MSVC)
  daw_add_test(NAME kostya_bench DONT_ADD_TEST)

  daw_add_test(NAME kostya_bench2 DONT_ADD_TEST)

  daw_add_test(NAME kostya_bench3 DONT_ADD_TEST)
endif ()

daw_add_test(NAME coords_test2 DONT_ADD_TEST)

daw_add_test(NAME nativejson_bench ARGS ./twitter.json ./citm_catalog.json ./canada.json WITH_DATA)

daw_add_test(NAME nativejson_bench2 ARGS ./twitter.json ./citm_catalog.json ./canada.json WITH_DATA)

daw_add_test(NAME nativejson_bench_basic ARGS ./twitter.json ./citm_catalog.json ./canada.json WITH_DATA)

daw_add_test(NAME nativejson_bench_basic2 ARGS ./twitter.json ./citm_catalog.json ./canada.json WITH_DATA)

daw_add_test(NAME citm_test ARGS ./citm_catalog.json WITH_DATA)

daw_add_test(NAME citm_test_basic ARGS ./citm_catalog.json WITH_DATA)

daw_add_test(NAME twitter_test ARGS ./twitter.json WITH_DATA)

daw_add_test(NAME twitter_test2 ARGS ./twitter.json WITH_DATA)

daw_add_test(NAME twitter_test_basic ARGS ./twitter.json WITH_DATA)

daw_add_test(NAME twitter_test_basic2 ARGS ./twitter.json WITH_DATA)

daw_add_test(NAME twitter_timeline_test ARGS ./twitter_timeline.json WITH_DATA)

daw_add_test(NAME canada_test ARGS ./canada.json WITH_DATA)

daw_add_test(NAME canada_test_basic ARGS ./canada.json WITH_DATA)

daw_add_test(NAME kv_map_test)

daw_add_test(NAME cookbook_kv1_test ARGS ./cookbook_kv1.json WITH_DATA)

daw_add_test(NAME cookbook_kv2_test ARGS ./cookbook_kv2.json WITH_DATA)

daw_add_test(NAME cookbook_kv3_test ARGS ./cookbook_kv3.json WITH_DATA)

daw_add_test(NAME cookbook_kv4_test ARGS ./cookbook_kv4.json WITH_DATA)

daw_add_test(NAME cookbook_class1_test ARGS ./cookbook_class1.json WITH_DATA)

daw_add_test(NAME cookbook_class2_test ARGS ./cookbook_class2.json WITH_DATA)

daw_add_test(NAME cookbook_class3_test ARGS ./cookbook_class3.json WITH_DATA)

daw_add_test(NAME cookbook_array1_test ARGS ./cookbook_array1.json WITH_DATA)

daw_add_test(NAME cookbook_array2_test ARGS ./cookbook_array2.json WITH_DATA)

daw_add_test(NAME cookbook_array3_test ARGS ./cookbook_array3.json WITH_DATA)

daw_add_test(NAME cookbook_escaped_strings1_test ARGS ./cookbook_escaped_strings1.json WITH_DATA)

daw_add_test(NAME full_unicode_roundtrip_test ARGS ./full_unicode.json ./full_unicode_escaped.json WITH_DATA)

daw_add_test(NAME make_full24 DONT_ADD_TEST)

daw_add_test(NAME cookbook_enums1_test ARGS ./cookbook_enums1.json WITH_DATA)

daw_add_test(NAME cookbook_enums2_test ARGS ./cookbook_enums2.json WITH_DATA)

daw_add_test(NAME cookbook_graphs1_test ARGS ./cookbook_graphs1.json WITH_DATA)

daw_add_test(NAME cookbook_optional_values1_test ARGS ./cookbook_optional_values1.json WITH_DATA)

daw_add_test(NAME cookbook_variant1_test ARGS ./cookbook_variant1.json WITH_DATA)

daw_add_test(NAME small_test ARGS ./small_test.json WITH_DATA)

daw_add_test(NAME should_fail SOURCE src/should_fail_001.cpp NAME_SUFFIX)

daw_add_test(NAME trailing_commas NAME_SUFFIX)

daw_add_test(NAME cookbook_parsing_individual_members1_test ARGS ./cookbook_parsing_individual_members1.json WITH_DATA)

daw_add_test(NAME cookbook_parsing_individual_members2_test ARGS ./cookbook_parsing_individual_members2.json WITH_DATA)

daw_add_test(NAME cookbook_parsing_individual_members3_test ARGS ./cookbook_parsing_individual_members3.json WITH_DATA)

daw_add_test(NAME cookbook_variant2_test ARGS ./cookbook_variant2.json WITH_DATA)

daw_add_test(NAME cookbook_variant3_test ARGS ./cookbook_variant3.json WITH_DATA)

daw_add_test(NAME optional_tagged_variant_test ARGS ./optional_tagged_variant.json WITH_DATA)

daw_add_test(NAME optional_variant_test ARGS ./optional_variant.json WITH_DATA)

daw_add_test(NAME daw_murmur3_test)

daw_add_test(NAME apache_builds_test ARGS ./apache_builds.json WITH_DATA)

daw_add_test(NAME apache_builds_cpp_comments_test ARGS ./apache_builds_cpp_comments.json WITH_DATA)

daw_add_test(NAME apache_builds_hash_comments_test ARGS ./apache_builds_hash_comments.json WITH_DATA)

daw_add_test(NAME cookbook_dates1_test ARGS ./cookbook_dates1.json WITH_DATA)

daw_add_test(NAME cookbook_dates2_test ARGS ./cookbook_dates2.json WITH_DATA)

daw_add_test(NAME cookbook_dates3_test ARGS ./cookbook_dates3.json WITH_DATA)

daw_add_test(NAME cookbook_dates4_test ARGS ./cookbook_dates4.json WITH_DATA)

daw_add_test(NAME no_move_or_copy_cls_test)

daw_add_test(NAME cookbook_class_from_array1_test ARGS ./cookbook_class_from_array1.json WITH_DATA)

daw_add_test(NAME cookbook_class_from_array2_test ARGS ./cookbook_class_from_array2.json WITH_DATA)

daw_add_test(NAME cookbook_numbers1_test ARGS ./cookbook_numbers1.json WITH_DATA)

daw_add_test(NAME cookbook_numbers2_test ARGS ./cookbook_numbers2.json WITH_DATA)

daw_add_test(NAME cookbook_unknown_types_and_delayed_parsing1_test ARGS ./cookbook_unknown_types_and_delayed_parsing1.json WITH_DATA)

daw_add_test(NAME cookbook_unknown_types_and_delayed_parsing2_test ARGS ./cookbook_unknown_types_and_delayed_parsing2.json WITH_DATA)

daw_add_test(NAME cookbook_unknown_types_and_delayed_parsing3_test)

daw_add_test(NAME stream_output_test)

daw_add_test(NAME int_sanity_test)

daw_add_test(NAME nativejson_roundtrip ARGS ./twitter.json ./citm_catalog.json ./canada.json WITH_DATA)

daw_add_test(NAME test_stateful_json_value ARGS ./test_stateful_json_value.json WITH_DATA NAME_SUFFIX)

daw_add_test(NAME test_details_skip_string NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_class NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_array NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_custom NAME_SUFFIX)

daw_add_test(NAME test_details_skip_array NAME_SUFFIX)

daw_add_test(NAME test_details_skip_class NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_null NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_unsigned NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_signed NAME_SUFFIX)

daw_add_test(NAME test_details_parse_value_real NAME_SUFFIX)

daw_add_test(NAME test_details_skip_number NAME_SUFFIX)

daw_add_test(NAME strings_test ARGS ./strings.json WITH_DATA)

daw_add_test(NAME strings_escaped_test ARGS ./strings_escaped.json WITH_DATA)

daw_add_test(NAME numbers_test ARGS ./numbers.json WITH_DATA)

daw_add_test(NAME daw_json_iostream_test)

daw_add_test(NAME daw_json_minify DONT_ADD_TEST)

daw_add_test(NAME daw_json_minify_full DONT_ADD_TEST)

# ---- JSON Benchmark ----

add_executable(json_benchmark src/json_benchmark.cpp)
target_link_libraries(json_benchmark PRIVATE json_test)
if (NOT DEFINED MSVC)
  add_test(NAME json_benchmark_test
          COMMAND json_benchmark ./apache_builds.json ./twitter.json ./citm_catalog.json ./canada.json
          WORKING_DIRECTORY "${DAW_TEST_DATA_DIRECTORY}"
          )
endif ()
add_dependencies(full json_benchmark)

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

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/../.git)
	find_package(Git)

	if (GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            OUTPUT_VARIABLE BUILD_VERSION
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
	else ()
    set(BUILD_VERSION "unknown")
	endif ()
else ()
  message(FATAL_ERROR "The benchmark target requires the project to be tracked by git")
endif()

target_compile_definitions(json_benchmark PRIVATE
        SOURCE_CONTROL_REVISION="${BUILD_VERSION}"
        PROCESSOR_DESCRIPTION="${_proc_desc}"
        OS_NAME="${_os_name}"
        OS_RELEASE="${_os_rel}"
        OS_VERSION="${_os_ver}"
        OS_PLATFORM="${_os_plat}"
        BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        ${DAW_TEST_DEFINITIONS}
        )

target_compile_options(json_benchmark PRIVATE ${DAW_COMPILER_OPTIONS})

target_compile_features(json_benchmark PRIVATE cxx_std_17)
