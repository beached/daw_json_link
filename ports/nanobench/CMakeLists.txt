cmake_minimum_required(VERSION 3.14)

project(nanobench VERSION 4.3.5 LANGUAGES CXX)

add_library(nanobench STATIC src/test/app/nanobench.cpp)
target_compile_features(nanobench PUBLIC cxx_std_11)
target_include_directories(nanobench PRIVATE src/include)

set(CMAKE_INSTALL_INCLUDEDIR include/nanobench CACHE PATH "")
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
    FILES src/include/nanobench.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    TARGETS nanobench
    EXPORT nanobenchTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    nanobenchConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

set(install_dir "${CMAKE_INSTALL_DATAROOTDIR}/nanobench")

file(WRITE "${PROJECT_BINARY_DIR}/nanobenchConfig.cmake" [[
include("${CMAKE_CURRENT_LIST_DIR}/nanobenchTargets.cmake")
]])

install(
    FILES
    "${PROJECT_BINARY_DIR}/nanobenchConfig.cmake"
    "${PROJECT_BINARY_DIR}/nanobenchConfigVersion.cmake"
    DESTINATION "${install_dir}"
)

install(
    EXPORT nanobenchTargets
    NAMESPACE nanobench::
    DESTINATION "${install_dir}"
)
