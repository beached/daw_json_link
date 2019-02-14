cmake_minimum_required( VERSION 2.8.12 )

project( daw_json_link_prj )
#set( CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
find_package( Boost 1.58.0 COMPONENTS system iostreams filesystem regex unit_test_framework REQUIRED )
enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

include_directories( "./include" )

set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
if( EXISTS "${DEP_ROOT_DIR}" )
	message( "${DEP_ROOT_DIR}" )
	message( "using dependency folder" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${DEP_ROOT_DIR}/install/include" )
	link_directories( "${DEP_ROOT_DIR}/install/lib" )
else( )
	message( "downloading and building deps" )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
endif( )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/json/daw_json_link.h
	${HEADER_FOLDER}/daw/json/daw_json_link_v3.h
	${HEADER_FOLDER}/daw/json/daw_json_link_datetime.h
	${HEADER_FOLDER}/daw/json/daw_json_link_streams.h
	${HEADER_FOLDER}/daw/json/daw_json_parsers.h
)

add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

add_executable( daw_json_link_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/daw_json_link_test.cpp )
target_link_libraries( daw_json_link_test_bin utf_range ${Boost_LIBRARIES} )
add_dependencies( daw_json_link_test_bin dependency_stub )
add_test( daw_json_link_test daw_json_link_test_bin )
add_dependencies( check daw_json_link_test_bin )

add_executable( daw_json_link_v3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/daw_json_link_v3_test.cpp )
target_link_libraries( daw_json_link_v3_test_bin utf_range ${Boost_LIBRARIES} )
add_dependencies( daw_json_link_v3_test_bin dependency_stub )
add_test( daw_json_link_v3_test daw_json_link_v3_test_bin )
add_dependencies( check daw_json_link_v3_test_bin )

install( DIRECTORY ${HEADER_FOLDER}/daw/json/ DESTINATION include/daw/json/ )

