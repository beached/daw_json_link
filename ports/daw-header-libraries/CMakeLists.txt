cmake_minimum_required(VERSION 3.14)

project(header_libraries VERSION 1.9.0 LANGUAGES NONE)

add_library(header_libraries INTERFACE)
target_compile_features(header_libraries INTERFACE cxx_std_17)

set(CMAKE_INSTALL_INCLUDEDIR include/header_libraries CACHE PATH "")
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(
    TARGETS header_libraries
    EXPORT daw_header_librariesTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    daw_header_librariesConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

set(install_dir "${CMAKE_INSTALL_DATAROOTDIR}/daw_header_libraries")

file(WRITE "${PROJECT_BINARY_DIR}/daw_header_librariesConfig.cmake" [[
include("${CMAKE_CURRENT_LIST_DIR}/daw_header_librariesTargets.cmake")
]])

install(
    FILES
    "${PROJECT_BINARY_DIR}/daw_header_librariesConfig.cmake"
    "${PROJECT_BINARY_DIR}/daw_header_librariesConfigVersion.cmake"
    DESTINATION "${install_dir}"
)

install(
    EXPORT daw_header_librariesTargets
    NAMESPACE daw::
    DESTINATION "${install_dir}"
)
