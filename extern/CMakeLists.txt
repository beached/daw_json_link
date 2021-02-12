# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link
#

# This prevents tests running on dependencies
option( DAW_ENABLE_TESTING "Build unit tests" OFF )
if( DAW_ENABLE_TESTING )
	set( DAW_ENABLE_TESTING OFF )
endif( )

include( FetchContent )
set( FETCHCONTENT_UPDATES_DISCONNECTED ON )

FetchContent_Declare(
	daw_header_libraries
	GIT_REPOSITORY https://github.com/beached/header_libraries.git
)

FetchContent_Declare(
	daw_utf_range
	GIT_REPOSITORY https://github.com/beached/utf_range.git
)

FetchContent_MakeAvailable(daw_header_libraries daw_utf_range)

