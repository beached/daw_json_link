// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_exec_modes.h"

#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <cstring>

#if defined( __GNUC__ )
#define DAW_CAN_CONSTANT_EVAL( ... ) __builtin_constant_p( __VA_ARGS__ )
#else
#define DAW_CAN_CONSTANT_EVAL( ... ) true
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {

			template<char c, typename ExecTag, bool expect_long, typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			memchr_unchecked( CharT *first, CharT *last ) {
#if DAW_HAS_BUILTIN( __builtin_char_memchr )
				if constexpr( expect_long ) {
					return __builtin_char_memchr(
					  first, '"', static_cast<std::size_t>( last - first ) );
				} else
#else
				if constexpr( expect_long ) {
					if( ( not( DAW_IS_CONSTANT_EVALUATED( ) |
					           DAW_CAN_CONSTANT_EVAL( first ) ) ) |
					    daw::traits::not_same_v<ExecTag, constexpr_exec_tag> ) {
						return static_cast<CharT *>(
						  std::memchr( static_cast<void const *>( first ), '"',
						               static_cast<std::size_t>( last - first ) ) );
					}
				} else
#endif
				{
					while( *first != c ) {
						++first;
					}
					return first;
				}
			}

			template<char c, typename ExecTag, bool expect_long, typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			memchr_checked( CharT *first, CharT *last ) {
#if DAW_HAS_BUILTIN( __builtin_char_memchr )
				if constexpr( expect_long ) {
					return __builtin_char_memchr(
					  first, '"', static_cast<std::size_t>( last - first ) );
				} else
#else
				if constexpr( expect_long ) {
					if( ( not( DAW_IS_CONSTANT_EVALUATED( ) |
					           DAW_CAN_CONSTANT_EVAL( first ) ) ) |
					    daw::traits::not_same_v<ExecTag, constexpr_exec_tag> ) {
						return static_cast<CharT *>(
						  std::memchr( static_cast<void const *>( first ), '"',
						               static_cast<std::size_t>( last - first ) ) );
					}
				} else
#endif
				{
					while( DAW_LIKELY( first < last ) and *first != c ) {
						++first;
					}
					return first;
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json