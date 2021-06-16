if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/daw_json_link CACHE PATH "")
endif()

# Project is configured with no languages, so tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Object code libraries (lib)")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT daw_json_link_Development
)

install(
    TARGETS daw_json_link
    EXPORT daw_json_linkTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    daw_json_linkConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Allow package maintainers to freely override the path for the configs
set(
    daw_json_link_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/daw_json_link"
    CACHE STRING "CMake package config location relative to the install prefix"
)
mark_as_advanced(daw_json_link_INSTALL_CMAKEDIR)

install(
    FILES
    cmake/daw_json_linkConfig.cmake
    "${PROJECT_BINARY_DIR}/daw_json_linkConfigVersion.cmake"
    DESTINATION "${daw_json_link_INSTALL_CMAKEDIR}"
    COMPONENT daw_json_link_Development
)

install(
    EXPORT daw_json_linkTargets
    NAMESPACE daw::
    DESTINATION "${daw_json_link_INSTALL_CMAKEDIR}"
    COMPONENT daw_json_link_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
