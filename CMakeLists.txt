# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link
#

cmake_minimum_required(VERSION 3.10)

project("json_link"
        VERSION "2.0.2"
        DESCRIPTION "Static JSON parsing in C++"
        HOMEPAGE_URL "https://github.com/beached/daw_json_link"
        LANGUAGES CXX)

# ---- Includes ----

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# ---- Definitions ----

set(DAW_JSON_LINK_DEFINITIONS "")

option(DAW_JSON_NO_CONST_EXPR "Ensure guaranteed copy elision in array/class types" OFF)
if (DAW_JSON_NO_CONST_EXPR)
    list(APPEND DAW_JSON_LINK_DEFINITIONS "DAW_JSON_NO_CONST_EXPR=true")
endif ()

option(DAW_USE_JSON_EXCEPTIONS "Throw exceptions when json errors occur" ON)
if (DAW_USE_JSON_EXCEPTIONS)
    list(APPEND DAW_JSON_LINK_DEFINITIONS "DAW_USE_JSON_EXCEPTIONS=true")
else ()
    list(APPEND DAW_JSON_LINK_DEFINITIONS "DAW_DONT_USE_JSON_EXCEPTIONS=true")
endif ()

set(allow_sse42 OFF)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    set(allow_sse42 ON)
endif ()

option(DAW_ALLOW_SSE42 "Allow SSE42 Modes" ${allow_sse42})
if (DAW_ALLOW_SSE42)
    list(APPEND DAW_JSON_LINK_DEFINITIONS "DAW_ALLOW_SSE42=true")
endif ()

# ---- Declare library ----

add_library(${PROJECT_NAME} INTERFACE)
add_library(daw::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} INTERFACE ${DAW_JSON_LINK_DEFINITIONS})

# ---- Add dependencies ----

add_subdirectory(extern)
target_link_libraries(${PROJECT_NAME} INTERFACE daw::header_libraries daw::utf_range)

# ---- Install ----

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(json_link_INSTALL_CMAKEDIR
        "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake"
        CACHE PATH
        "The install directory to use for CMake package config files")

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/daw_${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/daw_${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${json_link_INSTALL_CMAKEDIR})

write_basic_package_version_file("daw_${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE daw_${PROJECT_NAME}Targets.cmake
        NAMESPACE daw::
        DESTINATION ${json_link_INSTALL_CMAKEDIR})

install(FILES "${PROJECT_BINARY_DIR}/daw_${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/daw_${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}$)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/daw DESTINATION include)
