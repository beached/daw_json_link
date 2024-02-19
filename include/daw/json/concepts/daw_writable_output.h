// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_writable_output_basics.h"

#if not defined( DAW_JSON_NO_DEFAULT_OSTREAM_WRITER )
#include "daw_writable_output_ostream.h"
#endif

#if not defined( DAW_JSON_NO_DEFAULT_CSTDIO_WRITER )
#include "daw_writable_output_cstdio.h"
#endif
