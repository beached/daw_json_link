# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link
#

cmake_minimum_required( VERSION 3.14 )

project( "daw-json-link"
         VERSION "3.17.0"
         DESCRIPTION "Static JSON parsing in C++"
         HOMEPAGE_URL "https://github.com/beached/daw_json_link"
         LANGUAGES C CXX )

set( CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested." )

# Options used. Those tagged with Define are also the name of the source definition
set( DAW_JSON_VER_OVERRIDE OFF CACHE STRING "Override the default inline namespace name used for API versioning" )
option( DAW_USE_PACKAGE_MANAGEMENT "Do not use FetchContent and assume dependencies are installed" OFF )
option( DAW_ENABLE_TESTING "Build unit tests and examples" OFF )
option( DAW_JSON_PARSER_DIAGNOSTICS "Define: Output debug info while parsing" OFF )

option( DAW_USE_CPP17_NAMES "Define: Use the C++17 names instead of CNTTP/Static Strings" )
if( DAW_USE_CPP17_NAMES )
    add_compile_definitions( DAW_USE_CPP17_ABI )
endif()

option( DAW_JSON_ENABLE_FULL_RVO "Define: Enables RVO in places so that non-copy/moveable types can be returned.  This can cause perf issues as it forces a call to std::uncaught_exceptions( )" OFF )
if( DAW_JSON_ENABLE_FULL_RVO )
	add_compile_definitions( DAW_JSON_ENABLE_FULL_RVO )
endif( )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
    option( DAW_NO_FLATTEN "Define: Disable function flattening optimization" ON )
elseif( DEFINED ( CMAKE_BUILD_TYPE ))
    if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        option( DAW_NO_FLATTEN "Define: Disable function flattening optimization" ON )
    else()
        option( DAW_NO_FLATTEN "Define: Disable function flattening optimization" OFF )
    endif()
else()
    option( DAW_NO_FLATTEN "Define: Disable function flattening optimization" OFF )
endif()

option( DAW_JSON_FORCE_INT128 "Define: Force support for 128bit int" OFF )
if( CMAKE_CXX_FLAGS MATCHES "-fno-exceptions" )
    option( DAW_USE_EXCEPTIONS "Define: Throw exceptions when json errors occur or terminate" OFF )
    if( DAW_USE_EXCEPTIONS )
        message( FATAL_ERROR "CXX_FLAGS contains -fno-exceptions and -DDAW_USE_EXCEPTIONS=ON has been requested.  Cannot do both" )
    endif()
else()
    option( DAW_USE_EXCEPTIONS "Define: Throw exceptions when json errors occur or terminate" ON )
endif()
include( CMakeDependentOption )

option( DAW_JSON_SHOW_ERROR_BEFORE_TERMINATE "Define: Output error reason before calling terminate in non-exception builds" OFF )
if( DAW_JSON_SHOW_ERROR_BEFORE_TERMINATE )
    message( STATUS "DAW_JSON_SHOW_ERROR_BEFORE_TERMINATE is set.  Prior to calling terminate on error, reason will be output to stderr" )
    add_compile_definitions( DAW_JSON_SHOW_ERROR_BEFORE_TERMINATE=1 )
endif()

if( DAW_JSON_VER_OVERRIDE )
    message( STATUS "DAW_JSON_VER_OVERRIDE is set to '${DAW_JSON_VER_OVERRIDE}', overriding inline namespace for API version" )
    add_compile_definitions( DAW_JSON_VER_OVERRIDE=${DAW_JSON_VER_OVERRIDE} )
endif()

include( GNUInstallDirs )

set( PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
set( json_link_INSTALL_CMAKEDIR
     "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake"
     CACHE STRING
     "The install directory to use for CMake package config files" )


add_compile_definitions( DAW_CXX_STANDARD=${CMAKE_CXX_STANDARD} )

if( NOT DAW_USE_PACKAGE_MANAGEMENT )
    add_subdirectory( extern )
else()
    find_package( daw-header-libraries REQUIRED )
    find_package( daw-utf-range REQUIRED )
endif()

if( DAW_JSON_PARSER_DIAGNOSTICS )
    message( STATUS "Building with parser diagnostics enabled" )
    add_compile_definitions( DAW_JSON_PARSER_DIAGNOSTICS )
endif()

if( DAW_JSON_FORCE_INT128 )
    if( DAW_JSON_NO_INT128 )
    else()
        message( STATUS "Forcing support of int128" )
        add_definitions( -DDAW_HAS_INT128 )
    endif()
endif()

if( DAW_USE_EXCEPTIONS )
    message( STATUS "DAW_USE_EXCEPTIONS=ON: Errors throw json_exception" )
    add_definitions( -DDAW_USE_EXCEPTIONS=true )
else()
    message( STATUS "DAW_USE_EXCEPTIONS=OFF: Errors call std::terminate" )
    add_definitions( -DDAW_DONT_USE_EXCEPTIONS=true )
endif()

if( DAW_NO_FLATTEN )
    message( STATUS "DAW_NO_FLATTEN=ON: Disabling function flattening and forced inline optimization" )
    add_definitions( -DDAW_NO_FLATTEN -DDAW_NO_FORCED_INLINE )
endif()

if( DAW_JSON_NO_FAIL_ON_NO_NAME_NAME )
    message( STATUS "DAW_JSON_NO_FAIL_ON_NO_NAME_NAME=ON: Disabling check for no_name named members" )
    add_definitions( -DDAW_JSON_NO_FAIL_ON_NO_NAME_NAME )
endif()

option( DAW_JSON_CNTTP_JSON_NAME "Force CNTTP member names to allow literals on compilers that don't set feature flags bug support it" OFF )
if( DAW_JSON_CNTTP_JSON_NAME )
    message( STATUS "DAW_JSON_CNTTP_JSON_NAME=ON: Forcing CNTTP Json Member Name support" )
    add_definitions( -DDAW_JSON_CNTTP_JSON_NAME )
endif()

add_library( ${PROJECT_NAME} INTERFACE )
add_library( daw::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )
add_library( daw::json_link ALIAS ${PROJECT_NAME} )
if( NOT DAW_USE_PACKAGE_MANAGEMENT )
    target_link_libraries( ${PROJECT_NAME} INTERFACE daw::daw-header-libraries daw::daw-utf-range )
endif()

target_compile_features( ${PROJECT_NAME} INTERFACE cxx_std_17 )
target_include_directories( ${PROJECT_NAME}
                            INTERFACE
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                            )

install( TARGETS ${PROJECT_NAME}
         EXPORT ${PROJECT_NAME}_Targets
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

include( CMakePackageConfigHelpers )

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${json_link_INSTALL_CMAKEDIR} )

write_basic_package_version_file( "${PROJECT_NAME}ConfigVersion.cmake"
                                  VERSION ${PROJECT_VERSION}
                                  COMPATIBILITY SameMajorVersion )

install( EXPORT ${PROJECT_NAME}_Targets
         FILE ${PROJECT_NAME}Targets.cmake
         NAMESPACE daw::
         DESTINATION ${json_link_INSTALL_CMAKEDIR} )

install( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
         "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION ${json_link_INSTALL_CMAKEDIR} )

install( DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/ )

if( DAW_ENABLE_TESTING )
    enable_testing()
    add_subdirectory( tests )
endif()

