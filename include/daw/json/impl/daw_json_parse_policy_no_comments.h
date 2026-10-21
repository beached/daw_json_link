// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_parse_common.h"
#include "daw/json/impl/daw_json_parse_policy_policy_details.h"
#include "daw/json/impl/daw_json_string_util.h"
#include "daw/json/impl/daw_not_const_ex_functions.h"

#include <daw/daw_attributes.h>
#include <daw/daw_constant.h>
#include <daw/daw_function_table.h>
#include <daw/daw_likely.h>
#include <daw/daw_not_null.h>
#include <daw/daw_traits.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// @brief For documents without comments
			struct no_comment_skipper {
				/// The character a comment starts with, none
				static constexpr char comment_start = '\0';

				/// The end of the comment at first, or first when there is none
				template<typename ParseState>
				[[nodiscard]] static constexpr char const *
				skip( char const *first, char const *, ParseState const & ) {
					return first;
				}
			};

			/// Skip the JSON class or array at parse_state.first and check that it
			/// is structurally valid: brackets close in order, members are a name, a
			/// colon and a value, and values are separated by commas.  Trailing
			/// commas are allowed.  Literals and numbers are checked by
			/// skip_scalar_token_checked.  Comments are skipped like whitespace by
			/// CommentSkipper.  Without comments this must accept and reject the
			/// same documents as skip_bracketed_item_simd_validated
			template<SkipBracketedType BracketedType, typename CommentSkipper,
			         typename ParseState>
			DAW_ATTRIB_FLATTEN constexpr ParseState
			skip_bracketed_item_validated( ParseState &parse_state ) {
				DAW_CPP23_STATIC_LOCAL constexpr bool is_class =
				  BracketedType == json_details::SkipBracketedType::Class;
				// What may come next at the current depth
				enum : unsigned char {
					name_or_end,
					colon,
					value,
					value_or_end,
					comma_or_end,
					// Started part way through the item, after a value or a comma
					resume
				};
				constexpr auto is_whitespace = []( char c ) {
					return ( static_cast<unsigned>( static_cast<unsigned char>( c ) ) -
					         1U ) <= 0x1FU;
				};

				char const *ptr_first = parse_state.first;
				char const *const ptr_last = parse_state.last;
				// A null range is a logic error, not bad input
				daw_json_dbg_ensure( ptr_first != nullptr and ptr_last != nullptr,
				                     ErrorReason::InvalidNull );
				if( DAW_UNLIKELY( ptr_first >= ptr_last ) ) {
					return parse_state;
				}
				auto result = parse_state;
				std::size_t cnt = 0;
				// Bit n is set when depth n + 1 is a class
				std::uint64_t kinds = is_class ? 1U : 0U;
				std::size_t depth = 1;
				unsigned char state = resume;

				if( *ptr_first == ( is_class ? '{' : '[' ) ) {
					++ptr_first;
					state = is_class ? name_or_end : value_or_end;
				}
				while( DAW_LIKELY( ptr_first < ptr_last ) ) {
					char const c = *ptr_first;
					bool const at_class = ( ( kinds >> ( depth - 1U ) ) & 1U ) != 0;
					// Where a value may start: after a colon, at the start of an array
					// or after its commas, or when resuming in an array
					bool const value_allowed = ( state == value ) |
					                           ( state == value_or_end ) |
					                           ( ( state == resume ) & not at_class );
					switch( c ) {
					case '"':
						if( ( state == name_or_end ) |
						    ( ( state == resume ) & at_class ) ) {
							state = colon;
						} else {
							daw_json_ensure( value_allowed,
							                 ErrorReason::ExpectedTokenNotFound,
							                 parse_state );
							state = comma_or_end;
						}
						++ptr_first;
						ptr_first = json_details::mem_skip_until_end_of_string<
						  ParseState::is_unchecked_input,
						  typename ParseState::exec_tag_t>( ptr_first, ptr_last );
						daw_json_ensure( ptr_first < ptr_last and *ptr_first == '"',
						                 ErrorReason::UnexpectedEndOfData,
						                 parse_state );
						break;
					case ':':
						daw_json_ensure(
						  state == colon, ErrorReason::ExpectedTokenNotFound, parse_state );
						state = value;
						break;
					case ',':
						daw_json_ensure( ( state == comma_or_end ) | ( state == resume ),
						                 ErrorReason::InvalidStartOfValue,
						                 parse_state );
						if( depth == 1 ) {
							++cnt;
						}
						state = at_class ? name_or_end : value_or_end;
						break;
					case '{':
					case '[':
						daw_json_ensure( value_allowed,
						                 state == colon ? ErrorReason::ExpectedTokenNotFound
						                                : ErrorReason::MissingMemberName,
						                 parse_state );
						daw_json_ensure(
						  depth < 64, ErrorReason::MaxDepthExceeded, parse_state );
						if( c == '{' ) {
							kinds |= std::uint64_t{ 1 } << depth;
							state = name_or_end;
						} else {
							kinds &= ~( std::uint64_t{ 1 } << depth );
							state = value_or_end;
						}
						++depth;
						break;
					case '}':
					case ']':
						daw_json_ensure( at_class == ( c == '}' ),
						                 ErrorReason::InvalidBracketing,
						                 parse_state );
						daw_json_ensure(
						  ( state == comma_or_end ) | ( state == resume ) |
						    ( state == ( at_class ? name_or_end : value_or_end ) ),
						  state == colon ? ErrorReason::ExpectedTokenNotFound
						                 : ErrorReason::InvalidStartOfValue,
						  parse_state );
						--depth;
						if( depth == 0 ) {
							++ptr_first;
							// We include the close primary bracket in the range so that
							// subsequent parsers have a terminator inside their range
							result.last = json_details::input_pointer( result.first, ptr_first );
							result.counter = cnt;
							parse_state.first =
							  json_details::input_pointer( parse_state.first, ptr_first );
							return result;
						}
						state = comma_or_end;
						break;
					default: {
						if( is_whitespace( c ) ) {
							break;
						}
						if constexpr( CommentSkipper::comment_start != '\0' ) {
							auto const after_comment =
							  CommentSkipper::skip( ptr_first, ptr_last, parse_state );
							if( after_comment != ptr_first ) {
								ptr_first = after_comment;
								continue;
							}
						}
						// The start of a literal or number
						daw_json_ensure( value_allowed,
						                 state == name_or_end
						                   ? ErrorReason::MissingMemberName
						                   : ErrorReason::ExpectedTokenNotFound,
						                 parse_state );
						state = comma_or_end;
						ptr_first = parse_policy_details::skip_scalar_token_checked<
						  CommentSkipper::comment_start>(
						  ptr_first, ptr_last, parse_state );
						continue;
					}
					}
					++ptr_first;
				}
				// Data ended before the item was closed
				daw_json_error( true, ErrorReason::InvalidBracketing, parse_state );
			}

		} // namespace json_details

		struct NoCommentSkippingPolicy {
			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			trim_left_checked( ParseState &parse_state ) {
				if constexpr( not ParseState::minified_document ) {
					// SIMD here was much slower, most JSON has very minimal whitespace
					auto first = parse_state.first;
					auto const last = parse_state.last;
					// A null range is a logic error, not bad input
					daw_json_dbg_ensure( first != nullptr and last != nullptr,
					                     ErrorReason::InvalidNull );

					// only used when not zero terminated string and gcc9 warns
					(void)last;

					if constexpr( ParseState::is_zero_terminated_string ) {
						// Ensure that zero terminator isn't included in skipable value
						DAW_ASSUME( first );
						while( DAW_UNLIKELY(
						  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
						    1U ) <= 0x1FU ) ) {

							++first;
						}
					} else {
						while(
						  DAW_LIKELY( first < last ) and
						  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
						    1U ) <= 0x1FU ) {
							++first;
						}
					}
					parse_state.first = first;
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			trim_left_unchecked( ParseState &parse_state ) {
				if constexpr( not ParseState::minified_document ) {
					auto first = parse_state.first;
					// A null range is a logic error, not bad input
					daw_json_dbg_ensure( first != nullptr, ErrorReason::InvalidNull );
					while( DAW_UNLIKELY(
					  ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
					    1U ) <= 0x1F ) ) {

						++first;
					}
					parse_state.first = first;
				}
			}

			template<typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			move_next_member_unchecked( ParseState &parse_state ) {
				auto pf =
				  daw::not_null<char const *>( daw::never_null, parse_state.first );
				auto pl =
				  daw::not_null<char const *>( daw::never_null, parse_state.last );
				auto const position =
				  json_details::memchr_unchecked<'"',
				                                 typename ParseState::exec_tag_t,
				                                 ParseState::expect_long_strings>( pf,
				                                                                   pl );
				parse_state.first =
				  json_details::input_pointer( parse_state.first, position.get( ) );
			}

			template<char... keys, typename ParseState>
			DAW_ATTRIB_FLATINLINE static constexpr void
			move_to_next_of( ParseState &parse_state ) {
				static_assert( sizeof...( keys ) > 0 );
				static_assert( sizeof...( keys ) <= 16 );

				if( not json_details::use_constexpr_exec_mode<
				      typename ParseState::exec_tag_t>( ) ) {
					auto pf = daw::not_null<char const *>{ parse_state.first };
					auto pl = daw::not_null<char const *>{ parse_state.last };
					auto const position =
					  json_details::mempbrk<ParseState::is_unchecked_input,
					                        typename ParseState::exec_tag_t,
					                        ParseState::expect_long_strings,
					                        keys...>( pf, pl );
					parse_state.first =
					  json_details::input_pointer( parse_state.first, position.get( ) );
				} else {
					auto first = daw::not_null<char const *>( parse_state.first );
					auto const last = daw::not_null<char const *>( parse_state.last );

					// silencing gcc9 unused warning.  last is used inside if constexpr
					// blocks
					(void)last;

					if( ParseState::is_zero_terminated_string ) {
						daw_json_assert_weak( first < last and *first != '\0',
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
						while( not parse_policy_details::in<keys...>( *first ) ) {
							++first;
						}
						daw_json_assert_weak(
						  *first != '\0', ErrorReason::UnexpectedEndOfData, parse_state );
					} else {
						daw_json_assert_weak(
						  first < last, ErrorReason::UnexpectedEndOfData, parse_state );
						while( not parse_policy_details::in<keys...>( *first ) ) {
							++first;
							daw_json_assert_weak(
							  first < last, ErrorReason::UnexpectedEndOfData, parse_state );
						}
					}
					parse_state.first =
					  json_details::input_pointer( parse_state.first, first.get( ) );
				}
			}

			/// Skip the JSON class or array at parse_state.first and check that it
			/// is structurally valid, see json_details::skip_bracketed_item_validated
			template<json_details::SkipBracketedType BracketedType,
			         typename ParseState>
			DAW_ATTRIB_INLINE static constexpr ParseState
			skip_bracketed_item_checked( ParseState &parse_state ) {
				return json_details::skip_bracketed_item_validated<
				  BracketedType,
				  json_details::no_comment_skipper>( parse_state );
			}

			template<json_details::SkipBracketedType BracketedType,
			         typename ParseState>
			DAW_ATTRIB_NOINLINE static constexpr ParseState
			skip_bracketed_item_unchecked( ParseState &parse_state ) {
				DAW_CPP23_STATIC_LOCAL constexpr char PrimLeft =
				  BracketedType == json_details::SkipBracketedType::Class ? '{' : '[';
				// Not checking for Left as it is required to be skipped already
				using PrimRight = daw::constant<PrimLeft == '{' ? '}' : ']'>;
				using SecLeft = daw::constant<PrimLeft == '{' ? '[' : '{'>;
				using SecRight = daw::constant<SecLeft::value == '{' ? '}' : ']'>;

				auto result = parse_state;
				std::size_t cnt = 0;
				std::uint32_t prime_bracket_count = 1;
				std::uint32_t second_bracket_count = 0;
				char const *ptr_first = parse_state.first;
				char const *const ptr_last = parse_state.last;
				// A null range is a logic error, not bad input
				daw_json_dbg_ensure( ptr_first != nullptr and ptr_last != nullptr,
				                     ErrorReason::InvalidNull );

				if( *ptr_first == PrimLeft ) {
					++ptr_first;
				}
				while( true ) {
					switch( *ptr_first ) {
					case '\\':
						++ptr_first;
						break;
					case '"':
						++ptr_first;
						ptr_first = json_details::mem_skip_until_end_of_string<
						  ParseState::is_unchecked_input,
						  typename ParseState::exec_tag_t>( ptr_first, ptr_last );
						break;
					case ',':
						if( DAW_UNLIKELY( ( prime_bracket_count == 1 ) &
						                  ( second_bracket_count == 0 ) ) ) {
							++cnt;
						}
						break;
					case PrimLeft:
						++prime_bracket_count;
						break;
					case PrimRight::value:
						--prime_bracket_count;
						if( prime_bracket_count == 0 ) {
							++ptr_first;
							// We include the close primary bracket in the range so that
							// subsequent parsers have a terminator inside their range
							result.last = json_details::input_pointer( result.first, ptr_first );
							result.counter = cnt;
							parse_state.first = json_details::input_pointer( parse_state.first, ptr_first );
							return result;
						}
						break;
					case SecLeft::value:
						++second_bracket_count;
						break;
					case SecRight::value:
						--second_bracket_count;
						break;
					}
					++ptr_first;
				}
				// Should never get here, only loop exit is when PrimaryRight is found
				// and count == 0
				DAW_UNREACHABLE( );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
