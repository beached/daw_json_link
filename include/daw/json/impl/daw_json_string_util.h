// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_exec_modes.h"
#include "daw/json/impl/daw_json_parse_policy_policy_details.h"
#include "daw/json/impl/daw_not_const_ex_functions.h"

#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <cstring>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<char c, typename ExecTag>
			DAW_ATTRIB_FLATINLINE static constexpr daw::not_null<char const *>
			memchr_unchecked_long( daw::not_null<char const *> first,
			                       daw::not_null<char const *> last ) {
#if not defined( NDEBUG )
				daw_json_ensure( first <= last, ErrorReason::Unknown );
#endif
#if DAW_HAS_BUILTIN( __builtin_char_memchr )
				return __builtin_char_memchr(
				  first, '"', static_cast<std::size_t>( last - first ) );
#else
				if( not json_details::use_constexpr_exec_mode<ExecTag>( ) ) {
					return static_cast<char const *>(
					  std::memchr( static_cast<void const *>( first ),
					               '"',
					               static_cast<std::size_t>( last - first ) ) );
				}
				(void)last;
				while( *first != c ) {
					++first;
				}
				return first;
#endif
			}

			template<char c>
			DAW_ATTRIB_FLATINLINE static constexpr daw::not_null<char const *>
			memchr_unchecked_short( daw::not_null<char const *> first ) {
				while( *first != c ) {
					++first;
				}
				return first;
			}

			/// Search for a character in a string.
			/// \tparam c character to find in string
			/// \tparam ExecTag The current execution policy
			/// \tparam expect_long Use methods optimized for longer strings
			/// \param first Iterator to the start of string
			/// \param last Iterator at one past end of string
			/// \return char const * with position of first after search
			/// \pre first can be read from(not null)
			template<char c, typename ExecTag, bool expect_long>
			DAW_ATTRIB_FLATINLINE static constexpr daw::not_null<char const *>
			memchr_unchecked( daw::not_null<char const *> first,
			                  daw::not_null<char const *> last ) {
#if not defined( NDEBUG )
				daw_json_ensure( first <= last, ErrorReason::Unknown );
#endif
				if constexpr( expect_long ) {
					return memchr_unchecked_long<c, ExecTag>( first, last );
				} else {
					return memchr_unchecked_short<c>( first );
				}
			}

			template<char c, typename ExecTag>
			DAW_ATTRIB_FLATINLINE static constexpr daw::not_null<char const *>
			memchr_checked_long( daw::not_null<char const *> first,
			                     daw::not_null<char const *> last ) {
#if not defined( NDEBUG )
				daw_json_ensure( first <= last, ErrorReason::Unknown );
#endif
#if DAW_HAS_BUILTIN( __builtin_char_memchr )
				return __builtin_char_memchr(
				  first, '"', static_cast<std::size_t>( last - first ) );
#elif DAW_HAS_BUILTIN( __builtin_memchr )
				return static_cast<char const *>( __builtin_memchr(
				  first, '"', static_cast<std::size_t>( last - first ) ) );
#else
				if( not json_details::use_constexpr_exec_mode<ExecTag>( ) ) {
					return static_cast<char const *>(
					  std::memchr( static_cast<void const *>( first ),
					               '"',
					               static_cast<std::size_t>( last - first ) ) );
				}
				while( DAW_LIKELY( first < last ) and *first != c ) {
					++first;
				}
				return first;
#endif
			}

			template<char c>
			DAW_ATTRIB_INLINE static constexpr daw::not_null<char const *>
			memchr_checked_short( daw::not_null<char const *> first,
			                      daw::not_null<char const *> const last ) {
#if not defined( NDEBUG )
				daw_json_ensure( first <= last, ErrorReason::Unknown );
#endif
				while( DAW_LIKELY( first < last ) and *first != c ) {
					++first;
				}
				return first;
			}

			/// Search for a character in a string.
			/// \tparam c character to find in string
			/// \tparam ExecTag The current execution policy
			/// \tparam expect_long Use methods optimized for longer strings
			/// \param first Iterator to the start of string
			/// \param last Iterator at one past end of string
			/// \return char const * with position of first after search
			/// \pre first can be read from(not null)
			template<char c, typename ExecTag, bool expect_long>
			DAW_ATTRIB_FLATINLINE static constexpr daw::not_null<char const *>
			memchr_checked( daw::not_null<char const *> first,
			                daw::not_null<char const *> last ) {
				if constexpr( expect_long ) {
					return memchr_checked_long<c, ExecTag>( first, last );
				} else {
					return memchr_checked_short<c>( first, last );
				}
			}

			template<typename ExecTag, char... chars>
			DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			mempbrk_unchecked_long( daw::not_null<char const *> first ) {
#if DAW_HAS_BUILTIN( __builtin_strpbrk )
				constexpr char const needles[]{ chars..., '\0' };
				daw::not_null<char const *> res = __builtin_strpbrk( first, needles );
				return res;
#else
				if( not json_details::use_constexpr_exec_mode<ExecTag>( ) ) {
					constexpr char const needles[]{ chars..., '\0' };
					char const *res = std::strpbrk( first, needles );
#if not defined( NDEBUG )
					daw_json_ensure( res != nullptr, ErrorReason::UnexpectedEndOfData );
#endif
					return res;
				}
				while( not parse_policy_details::in<chars...>( *first ) ) {
					++first;
				}
				return first;
#endif
			}

			template<char... chars>
			DAW_ATTRIB_INLINE constexpr daw::not_null<char const *>
			mempbrk_unchecked_short( daw::not_null<char const *> first ) {
				while( not parse_policy_details::in<chars...>( *first ) ) {
					++first;
				}
				return first;
			}

			template<typename ExecTag, bool expect_long, char... chars>
			DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			mempbrk_unchecked( daw::not_null<char const *> first ) {
				if constexpr( expect_long ) {
					return mempbrk_unchecked_long<ExecTag, chars...>( first );
				} else {
					return mempbrk_unchecked_short<chars...>( first );
				}
			}

			template<typename ExecTag, char... chars>
			DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			mempbrk_checked_long( daw::not_null<char const *> first,
			                      daw::not_null<char const *> last ) {
#if not defined( NDEBUG )
				daw_json_ensure( first <= last, ErrorReason::Unknown );
#endif
				if( not json_details::use_constexpr_exec_mode<ExecTag>( ) ) {
					return mem_move_to_next_of<false, ExecTag, chars...>( first, last );
				}
				while( DAW_LIKELY( first < last ) and
				       not parse_policy_details::in<chars...>( *first ) ) {
					++first;
				}
				return first;
			}

			template<char... chars>
			DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			mempbrk_checked_short( daw::not_null<char const *> first,
			                       daw::not_null<char const *> last ) {
#if not defined( NDEBUG )
				daw_json_ensure( first <= last, ErrorReason::Unknown );
#endif
				while( DAW_LIKELY( first < last ) and
				       not parse_policy_details::in<chars...>( *first ) ) {
					++first;
				}
				return first;
			}

			template<typename ExecTag, bool expect_long, char... chars>
			DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			mempbrk_checked( daw::not_null<char const *> first,
			                 daw::not_null<char const *> last ) {
				if constexpr( expect_long ) {
					return mempbrk_checked_long<ExecTag, chars...>( first, last );
				} else {
					return mempbrk_checked_short<chars...>( first, last );
				}
			}

			template<bool is_unchecked_input, typename ExecTag, bool expect_long,
			         char... chars>
			DAW_ATTRIB_FLATINLINE constexpr daw::not_null<char const *>
			mempbrk( daw::not_null<char const *> first,
			         daw::not_null<char const *> last ) {

				if constexpr( is_unchecked_input ) {
					return mempbrk_unchecked<ExecTag, expect_long, chars...>( first );
				} else {
					return mempbrk_checked<ExecTag, expect_long, chars...>( first, last );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json