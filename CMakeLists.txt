cmake_minimum_required(VERSION 3.14)

include(cmake/in-source-guard.cmake)

project(
    daw_json_link
    VERSION 3.0.0
    DESCRIPTION "Fast JSON serialization and parsing in C++"
    HOMEPAGE_URL "https://github.com/beached/daw_json_link"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(daw_json_link INTERFACE)
add_library(daw::json_link ALIAS daw_json_link)

set_property(TARGET daw_json_link PROPERTY EXPORT_NAME json_link)

target_include_directories(
    daw_json_link ${daw_json_link_warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(daw_json_link INTERFACE cxx_std_17)

find_package(daw_header_libraries REQUIRED)
find_package(daw_utf_range REQUIRED)

target_link_libraries(
    daw_json_link INTERFACE
    daw::header_libraries daw::utf_range
)

# ---- Options ----

include(cmake/options.cmake)

# ---- Install rules ----

include(cmake/install-rules.cmake)

# ---- Developer mode ----

if(NOT daw_json_link_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of daw_json_link"
  )
endif()

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

option(ENABLE_COVERAGE "Enable coverage support separate from CTest's" OFF)
if(ENABLE_COVERAGE)
  include(cmake/coverage.cmake)
endif()

include(cmake/lint-targets.cmake)
include(cmake/spell-targets.cmake)
