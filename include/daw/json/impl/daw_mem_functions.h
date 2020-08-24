// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_exec_modes.h"
#include "daw_mem_functions_common.h"
#include "daw_mem_functions_sse42.h"

#include <daw/daw_hide.h>

#include <cstring>

namespace daw::json::json_details {
	DAW_ATTRIBUTE_FLATTEN static inline constexpr bool
	is_escaped( char const *ptr, char const *min_ptr ) {
		if( *( ptr - 1 ) != '\\' ) {
			return false;
		}
		if( ( ptr - min_ptr ) < 2 ) {
			return false;
		}
		return *( ptr - 2 ) != '\\';
	}

	template<bool is_unchecked_input, typename Tag>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr char const *
	mem_skip_string( Tag const &tag, char const *first, char const *const last ) {
		return mem_move_to_next_of<is_unchecked_input, '"', '\\'>( tag, first,
		                                                           last );
	}

	template<bool is_unchecked_input, typename Tag>
	DAW_ATTRIBUTE_FLATTEN static inline char const *
	mem_skip_until_end_of_string( Tag const &tag, char const *first,
	                              char const *const last ) {
		while( first < last ) {
			auto ptr =
			  mem_move_to_next_of<is_unchecked_input, '"'>( tag, first, last );
			if( DAW_JSON_LIKELY( not is_escaped( ptr, first ) ) ) {
				return ptr;
			} else {
				first = ptr + 1;
				continue;
			}
		}
		return first;
	}
} // namespace daw::json::json_details
