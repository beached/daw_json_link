# The vcpkg provided port for nanobench does not install a CMake package, so it
# has to be provided with an overlay port instead

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO martinus/nanobench
    REF v4.3.5
    SHA512 d24bf484d81f8fc19f43881d9efa804691f53eca182ba936fab12cf398165a8e803e2560f409e5a0daaa16ebd26a5a4018cbf53f97dae355196b9c1a93efcb7d
    HEAD_REF master
)

file(
    COPY "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt"
    DESTINATION "${SOURCE_PATH}"
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(
    INSTALL "${SOURCE_PATH}/LICENSE"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright
)
