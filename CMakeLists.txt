# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link
#

cmake_minimum_required(VERSION 3.14)

project("json_link"
        VERSION "3.0.0"
        DESCRIPTION "Static JSON parsing in C++"
        HOMEPAGE_URL "https://github.com/beached/daw_json_link"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions( DAW_CXX_STANDARD=${CMAKE_CXX_STANDARD} )

add_subdirectory(extern)
option(DAW_ENABLE_TESTING "Build unit tests" OFF)
option(DAW_JSON_PARSER_DIAGNOSTICS "Output debug info while parsing" OFF)
if( DAW_JSON_PARSER_DIAGNOSTICS )
    message( "Building with parser diagnostics enabled" )
    add_compile_definitions( DAW_JSON_PARSER_DIAGNOSTICS )
endif( )

include(GNUInstallDirs)
set(json_link_INSTALL_CMAKEDIR
        "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake"
        CACHE STRING
        "The install directory to use for CMake package config files")

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(DAW_JSON_FORCE_INT128 OFF CACHE STRING "Force support for 128bit int")
if (DAW_JSON_FORCE_INT128)
    if (DAW_JSON_NO_INT128)
    else ()
        message("Forcing support of int128")
        add_definitions(-DDAW_HAS_INT128)
    endif ()
endif ()

set(DAW_USE_EXCEPTIONS ON CACHE STRING "Throw exceptions when json errors occur")
if (DAW_USE_EXCEPTIONS)
    message("DAW_USE_EXCEPTIONS=ON: Errors throw json_exception")
    add_definitions(-DDAW_USE_EXCEPTIONS=true)
else ()
    message("DAW_USE_EXCEPTIONS=OFF: Errors call std::terminate")
    add_definitions(-DDAW_DONT_USE_EXCEPTIONS=true)
endif ()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # G++ has difficulty and takes too long to compile
    option(DAW_NO_FLATTEN "Disable function flattening optimization" ON )
else( )
    option(DAW_NO_FLATTEN "Disable function flattening optimization" OFF )
endif()

if (DAW_NO_FLATTEN)
    message("DAW_NO_FLATTEN=ON: Disabling function flattening optimization")
    add_definitions(-DDAW_NO_FLATTEN)
endif ()

option(DAW_JSON_NO_FAIL_ON_NO_NAME_NAME "Ensure that no named mapping uses no_name as it's name.  Using it can slow down compilation" ON )
if (DAW_JSON_NO_FAIL_ON_NO_NAME_NAME)
    message("DAW_JSON_NO_FAIL_ON_NO_NAME_NAME=ON: Disabling check for no_name named members")
    add_definitions(-DDAW_JSON_NO_FAIL_ON_NO_NAME_NAME)
endif ()

add_library(${PROJECT_NAME} INTERFACE)
add_library(daw::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE daw::header_libraries daw::utf_range)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME}
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/daw_${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${json_link_INSTALL_CMAKEDIR})

write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE daw::
        DESTINATION ${json_link_INSTALL_CMAKEDIR})

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${json_link_INSTALL_CMAKEDIR}$)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/)

if (DAW_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()

