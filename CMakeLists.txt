cmake_minimum_required( VERSION 2.8.12 )

project( daw_json_link_prj )
#set( CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
enable_testing( )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

include_directories( "./include" )

set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
endif( )
message( "Building for: ${CMAKE_BUILD_TYPE}" )

option( GLEAN_INSTALL_ROOT "" )
if( EXISTS "${GLEAN_INSTALL_ROOT}" ) 
	message( "using glean dependencies" )
	message( "installing to ${GLEAN_INSTALL_ROOT}" )

	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${GLEAN_INSTALL_ROOT}/include" )
	link_directories( "${GLEAN_INSTALL_ROOT}/lib" )
elseif( EXISTS "${CMAKE_SOURCE_DIR}/glean.cmake" )
	message( "glean.cmake detected" )
	include( "${CMAKE_SOURCE_DIR}/glean.cmake" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
	include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
	link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
elseif( EXISTS "${DEP_ROOT_DIR}" )
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

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )
set( SOURCE_FOLDER "src" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/json/daw_json_link.h
	${HEADER_FOLDER}/daw/json/daw_json_iterator.h
	${HEADER_FOLDER}/daw/json/impl/daw_json_link_impl.h
	${HEADER_FOLDER}/daw/json/impl/daw_iterator_range.h
)

add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )
add_custom_target( full )

add_executable( daw_json_link_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/daw_json_link_test.cpp )
add_dependencies( daw_json_link_test_bin dependency_stub )
add_test( daw_json_link_test daw_json_link_test_bin )
add_dependencies( check daw_json_link_test_bin )
add_dependencies( full daw_json_link_test_bin )

add_executable( city_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/city_test.cpp )
add_dependencies( city_test_bin dependency_stub )
add_test( city_test city_test_bin )
add_dependencies( check city_test_bin )
add_dependencies( full city_test_bin )

add_executable( int_array_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/int_array_test.cpp )
add_dependencies( int_array_test_bin dependency_stub )
add_test( int_array_test int_array_test_bin )
add_dependencies( check int_array_test_bin )
add_dependencies( full int_array_test_bin )

add_executable( simple_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/simple_test.cpp )
add_dependencies( simple_test_bin dependency_stub )
add_test( simple_test simple_test_bin )
add_dependencies( check simple_test_bin )
add_dependencies( full simple_test_bin )

add_executable( coords_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/coords_test.cpp )
add_dependencies( coords_test_bin dependency_stub )
target_link_libraries( coords_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
add_test( coords_test coords_test_bin )
add_dependencies( check coords_test_bin )
add_dependencies( full coords_test_bin )

add_executable( coords_test2_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/coords_test2.cpp )
add_dependencies( coords_test2_bin dependency_stub )
target_link_libraries( coords_test2_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
add_test( coords_test2 coords_test2_bin )
add_dependencies( check coords_test2_bin )
add_dependencies( full coords_test2_bin )

add_executable( nativejson_bench_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/twitter_test.h  ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/canada_test.h ${TEST_FOLDER}/nativejson_bench.cpp )
add_dependencies( nativejson_bench_bin dependency_stub )
target_link_libraries( nativejson_bench_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
add_test( nativejson_bench nativejson_bench_bin )
add_dependencies( check nativejson_bench_bin )
add_dependencies( full nativejson_bench_bin )

add_executable( citm_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/citm_test.h ${TEST_FOLDER}/citm_test.cpp )
add_dependencies( citm_test_bin dependency_stub )
target_link_libraries( citm_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
add_test( citm_test citm_test_bin )
add_dependencies( check citm_test_bin )
add_dependencies( full citm_test_bin )

add_executable( kv_map_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/kv_map_test.cpp )
add_dependencies( kv_map_test_bin dependency_stub )
add_test( kv_map_test kv_map_test_bin )
add_dependencies( check kv_map_test_bin )
add_dependencies( full kv_map_test_bin )

install( DIRECTORY ${HEADER_FOLDER}/daw/json/ DESTINATION include/daw/json/ )

