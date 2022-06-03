// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_exec_modes.h"
#include "daw_json_parse_policy_policy_details.h"
#include "daw_not_const_ex_functions.h"

#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <cstring>

#if defined( __GNUC__ )
#define DAW_CAN_CONSTANT_EVAL( ... ) \
	( __builtin_constant_p( __VA_ARGS__ ) == 1 )
#else
#define DAW_CAN_CONSTANT_EVAL( ... ) true
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// Search for a character in a string.
			/// \tparam c character to find in string
			/// \tparam ExecTag The current execution policy
			/// \tparam expect_long Use methods optimized for longer strings
			/// \tparam CharT character type in string
			/// \param first Iterator to the start of string
			/// \param last Iterator at one past end of string
			/// \return CharT * with position of first after search
			/// \pre first can be read from(not null)
			template<char c, typename ExecTag, bool expect_long, typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			memchr_unchecked( CharT *first, CharT *last ) {
#if DAW_HAS_BUILTIN( __builtin_char_memchr )
				if constexpr( expect_long ) {
					return __builtin_char_memchr(
					  first, '"', static_cast<std::size_t>( last - first ) );
				} else
#else
#if defined( DAW_IS_CONSTANT_EVALUATED )
				bool is_cxeval =
				  DAW_IS_CONSTANT_EVALUATED( ) | DAW_CAN_CONSTANT_EVAL( first );
#else
				bool is_cxeval = DAW_CAN_CONSTANT_EVAL( first );
#endif
				if constexpr( expect_long ) {
					if( ( not is_cxeval ) |
					    daw::traits::not_same_v<ExecTag, constexpr_exec_tag> ) {
						return static_cast<CharT *>(
						  std::memchr( static_cast<void const *>( first ), '"',
						               static_cast<std::size_t>( last - first ) ) );
					}
					(void)last;
					while( *first != c ) {
						++first;
					}
					return first;
				} else
#endif
				{
					(void)last;
					while( *first != c ) {
						++first;
					}
					return first;
				}
			}

			/// Search for a character in a string.
			/// \tparam c character to find in string
			/// \tparam ExecTag The current execution policy
			/// \tparam expect_long Use methods optimized for longer strings
			/// \tparam CharT character type in string
			/// \param first Iterator to the start of string
			/// \param last Iterator at one past end of string
			/// \return CharT * with position of first after search
			/// \pre first can be read from(not null)
			template<char c, typename ExecTag, bool expect_long, typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			memchr_checked( CharT *first, CharT *last ) {
#if DAW_HAS_BUILTIN( __builtin_char_memchr )
				if constexpr( expect_long ) {
					return __builtin_char_memchr(
					  first, '"', static_cast<std::size_t>( last - first ) );
				} else
#elif DAW_HAS_BUILTIN( __builtin_memchr )
				if constexpr( expect_long ) {
					return __builtin_memchr( first, '"',
					                         static_cast<std::size_t>( last - first ) );
				} else
#else
#if defined( DAW_IS_CONSTANT_EVALUATED )
				bool is_cxeval =
				  DAW_IS_CONSTANT_EVALUATED( ) | DAW_CAN_CONSTANT_EVAL( first );
#else
				bool is_cxeval = DAW_CAN_CONSTANT_EVAL( first );
#endif
				if constexpr( expect_long ) {
					if( ( not is_cxeval ) |
					    daw::traits::not_same_v<ExecTag, constexpr_exec_tag> ) {
						return static_cast<CharT *>(
						  std::memchr( static_cast<void const *>( first ), '"',
						               static_cast<std::size_t>( last - first ) ) );
					}
					while( DAW_LIKELY( first < last ) and *first != c ) {
						++first;
					}
					return first;
				} else
#endif
				{
					while( DAW_LIKELY( first < last ) and *first != c ) {
						++first;
					}
					return first;
				}
			}

			template<typename ExecTag, bool expect_long, char... chars,
			         typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			mempbrk_unchecked( CharT *first, CharT * /*last*/ ) {
#if DAW_HAS_BUILTIN( __builtin_strpbrk )
				if constexpr( expect_long ) {
					constexpr char const needles[]{ chars..., '\0' };
					CharT *res = __builtin_strpbrk( first, needles );
#if not defined( NDEBUG )
					daw_json_ensure( res != nullptr, ErrorReason::UnexpectedEndOfData );
#endif
					return res;
				} else
#else
#if defined( DAW_IS_CONSTANT_EVALUATED )
				bool is_cxeval =
				  DAW_IS_CONSTANT_EVALUATED( ) | DAW_CAN_CONSTANT_EVAL( first );
#else
				bool is_cxeval = DAW_CAN_CONSTANT_EVAL( first );
#endif
				if constexpr( expect_long ) {
					if( ( not is_cxeval ) |
					    daw::traits::not_same_v<ExecTag, constexpr_exec_tag> ) {
						constexpr char const needles[]{ chars..., '\0' };
						CharT *res = std::strpbrk( first, needles );
#if not defined( NDEBUG )
						daw_json_ensure( res != nullptr, ErrorReason::UnexpectedEndOfData );
#endif
						return res;
					}
					while( not parse_policy_details::in<chars...>( *first ) ) {
						++first;
					}
					return first;
				} else
#endif
				{
					while( not parse_policy_details::in<chars...>( *first ) ) {
						++first;
					}
					return first;
				}
			}

			template<typename ExecTag, bool expect_long, char... chars,
			         typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *
			mempbrk_checked( CharT *first, CharT *last ) {
				if constexpr( expect_long ) {
#if defined( DAW_IS_CONSTANT_EVALUATED )
					bool is_cxeval =
					  DAW_IS_CONSTANT_EVALUATED( ) | DAW_CAN_CONSTANT_EVAL( first );
#else
					bool is_cxeval = DAW_CAN_CONSTANT_EVAL( first );
#endif
					if( ( not is_cxeval ) |
					    daw::traits::not_same_v<ExecTag, constexpr_exec_tag> ) {

						return mem_move_to_next_of<false, chars...>( first, last );
					}
					while( DAW_LIKELY( first < last ) and
					       not parse_policy_details::in<chars...>( *first ) ) {
						++first;
					}
					return first;
				} else {
					while( DAW_LIKELY( first < last ) and
					       not parse_policy_details::in<chars...>( *first ) ) {
						++first;
					}
					return first;
				}
			}

			template<bool is_unchecked_input, typename ExecTag, bool expect_long,
			         char... chars, typename CharT>
			DAW_ATTRIB_FLATINLINE inline constexpr CharT *mempbrk( CharT *first,
			                                                       CharT *last ) {

				if constexpr( is_unchecked_input ) {
					return mempbrk_unchecked<ExecTag, expect_long, chars...>( first,
					                                                          last );
				} else {
					return mempbrk_unchecked<ExecTag, expect_long, chars...>( first,
					                                                          last );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json