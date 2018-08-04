cmake_minimum_required( VERSION 2.8.12 )

project( daw_json_link_prj )

find_package( Boost 1.58.0 COMPONENTS system iostreams filesystem regex unit_test_framework REQUIRED )
enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK )

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 )
ELSE( )
	find_package( Threads REQUIRED )
	if( THREADS_HAVE_PTHREAD_ARG )
		set( COMPILER_SPECIFIC_LIBS, "-lpthread" )
	endif()

	IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options(-std=c++14 -Weverything -Wno-c++98-compat -g -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
	ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -g -Wall -Wno-deprecated-declarations)
	ENDIF()
ENDIF()

if( EXISTS "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/include" )
	link_directories( "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/lib" )
else( )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )

	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub header_libraries_prj char_range_prj date_prj )
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
target_link_libraries( daw_json_link_test_bin char_range ${Boost_LIBRARIES} )
add_dependencies( daw_json_link_test_bin dependency_stub )
add_test( daw_json_link_test daw_json_link_test_bin )
add_dependencies( check daw_json_link_test_bin )

add_executable( daw_json_link_v3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/daw_json_link_v3_test.cpp )
target_link_libraries( daw_json_link_v3_test_bin char_range ${Boost_LIBRARIES} )
add_dependencies( daw_json_link_v3_test_bin dependency_stub )
add_test( daw_json_link_v3_test daw_json_link_v3_test_bin )
add_dependencies( check daw_json_link_v3_test_bin )

install( DIRECTORY ${HEADER_FOLDER}/daw/json/ DESTINATION include/daw/json/ )

