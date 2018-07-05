cmake_minimum_required( VERSION 2.8.12 )

project( daw_json_link_prj )

find_package( Boost 1.58.0 COMPONENTS system filesystem regex iostreams unit_test_framework REQUIRED )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest )
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -march=native -pthread -Weverything -Wno-c++98-compat -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -march=native -pthread -Wall -Wno-deprecated-declarations)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	endif( )
endif( )

include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/include" )
link_directories( "${CMAKE_SOURCE_DIR}/dependent_projects/build/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/json/daw_json_link.h
	${HEADER_FOLDER}/daw/json/daw_json_link_v3.h
	${HEADER_FOLDER}/daw/json/daw_json_link_datetime.h
	${HEADER_FOLDER}/daw/json/daw_json_link_streams.h
	${HEADER_FOLDER}/daw/json/daw_json_parsers.h
)

set( SOURCE_FILES
)

#set( CMAKE_CTEST_COMMAND ctest -V )
add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

install( DIRECTORY ${HEADER_FOLDER}/daw/json/ DESTINATION include/daw/json/ )

add_executable( daw_json_link_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/daw_json_link_test.cpp )
target_link_libraries( daw_json_link_test_bin char_range ${Boost_LIBRARIES} )
add_test( daw_json_link_test daw_json_link_test_bin )
add_dependencies( check daw_json_link_test_bin )

add_executable( daw_json_link_v3_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/daw_json_link_v3_test.cpp )
target_link_libraries( daw_json_link_v3_test_bin char_range ${Boost_LIBRARIES} )
add_test( daw_json_link_v3_test daw_json_link_v3_test_bin )
add_dependencies( check daw_json_link_v3_test_bin )

