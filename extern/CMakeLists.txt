# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link
#

# This prevents tests running on dependencies
option(DAW_ENABLE_TESTING "Build unit tests" OFF)
set(DAW_ENABLE_TESTING OFF)

include(FetchContent)

if (NOT TARGET daw::daw-header-libraries)
    FetchContent_Declare(
            daw_header_libraries
            GIT_REPOSITORY https://github.com/beached/header_libraries.git
            GIT_TAG v2.53.2
    )
    FetchContent_MakeAvailable(daw_header_libraries)
endif ()

if (NOT TARGET daw::daw_utf_range)
    FetchContent_Declare(
            daw_utf_range
            GIT_REPOSITORY https://github.com/beached/utf_range.git
            GIT_TAG v2.2.2
    )
    FetchContent_MakeAvailable(daw_utf_range)
endif ()

