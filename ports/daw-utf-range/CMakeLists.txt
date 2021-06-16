cmake_minimum_required(VERSION 3.14)

project(utf_range VERSION 1.0.1 LANGUAGES NONE)

add_library(utf_range INTERFACE)
target_compile_features(utf_range INTERFACE cxx_std_17)

find_package(daw_header_libraries REQUIRED)
target_link_libraries(utf_range INTERFACE daw::header_libraries)

set(CMAKE_INSTALL_INCLUDEDIR include/utf_range CACHE PATH "")
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(
    TARGETS utf_range
    EXPORT daw_utf_rangeTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    daw_utf_rangeConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

set(install_dir "${CMAKE_INSTALL_DATAROOTDIR}/daw_utf_range")

file(WRITE "${PROJECT_BINARY_DIR}/daw_utf_rangeConfig.cmake" [[
include(CMakeFindDependencyMacro)
find_dependency(daw_header_libraries)

include("${CMAKE_CURRENT_LIST_DIR}/daw_utf_rangeTargets.cmake")
]])

install(
    FILES
    "${PROJECT_BINARY_DIR}/daw_utf_rangeConfig.cmake"
    "${PROJECT_BINARY_DIR}/daw_utf_rangeConfigVersion.cmake"
    DESTINATION "${install_dir}"
)

install(
    EXPORT daw_utf_rangeTargets
    NAMESPACE daw::
    DESTINATION "${install_dir}"
)
