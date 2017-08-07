cmake_minimum_required( VERSION 2.8.12 )

project( daw_json_link_prj )

include( ExternalProject )

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

externalproject_add( 
	header_libraries_prj
	GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
	GIT_TAG "master"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/deps/headlibs"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add( 
	char_range_prj
	GIT_REPOSITORY "https://github.com/beached/char_range.git"
	GIT_TAG "master"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/deps/chrrng"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
	date_prj
	GIT_REPOSITORY "https://github.com/beached/date.git"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/date"
	GIT_TAG "master"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw_json_link.h
	${HEADER_FOLDER}/daw_json_link_datetime.h
	${HEADER_FOLDER}/daw_json_link_streams.h
	${HEADER_FOLDER}/daw_json_parsers.h
)

set( SOURCE_FILES
)

#install( TARGETS daw_json_link DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/json )

add_executable( daw_json_link_test_bin ${HEADER_FILES} ${SOURCE_FILES} ${TEST_FOLDER}/daw_json_link_test.cpp )
add_dependencies( daw_json_link_test_bin header_libraries_prj char_range_prj date_prj )
target_link_libraries( daw_json_link_test_bin char_range ${Boost_LIBRARIES} )
add_test( daw_json_link_test daw_json_link_test_bin )

#add_executable( gh_events_test_bin ${HEADER_FILES} ${TEST_FOLDER}/gh_events.cpp )
#add_dependencies( gh_events_test_bin header_libraries_prj char_range_prj date_prj )
#target_link_libraries( gh_events_test_bin char_range ${Boost_LIBRARIES} )
#add_test( gh_events_test gh_events_test_bin )

