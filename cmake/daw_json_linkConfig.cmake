include(CMakeFindDependencyMacro)
find_dependency(daw_header_libraries)
find_dependency(daw_utf_range)

include("${CMAKE_CURRENT_LIST_DIR}/daw_json_linkTargets.cmake")
