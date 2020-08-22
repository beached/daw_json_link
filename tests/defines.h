// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#if defined( DAW_JSON_NO_CONST_EXPR )
#define DAW_CONSTEXPR
#else
#define DAW_CONSTEXPR constexpr
#endif
