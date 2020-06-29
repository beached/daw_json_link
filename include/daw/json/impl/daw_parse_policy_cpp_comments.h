// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_parse_policy_policy_details.h"

#include <daw/daw_hide.h>

namespace daw::json {
	class CppCommentSkippingPolicy final {
		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments_unchecked( Range &rng ) {
			if( rng.front( ) == '/' ) {
				rng.remove_prefix( );
				switch( rng.front( ) ) {
				case '/':
					move_to_next_of_unchecked( rng, '\n' );
					rng.remove_prefix( );
					break;
				case '*':
					rng.remove_prefix( );
					while( true ) {
						move_to_next_of_unchecked( rng, '*' );
						rng.remove_prefix( );
						if( rng.front( ) == '/' ) {
							break;
						}
						rng.remove_prefix( );
					}
					break;
				}
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		skip_comments_checked( Range &rng ) {
			if( rng.has_more( ) and rng.front( ) == '/' ) {
				rng.remove_prefix( );
				if( not rng.has_more( ) ) {
					return;
				}
				switch( rng.front( ) ) {
				case '/':
					move_to_next_of_checked( rng, '\n' );
					if( rng.has_more( ) ) {
						rng.remove_prefix( );
					}
					break;
				case '*':
					rng.remove_prefix( );
					while( rng.has_more( ) ) {
						move_to_next_of_checked( rng, '*' );
						if( rng.has_more( ) ) {
							rng.remove_prefix( );
						}
						if( not rng.has_more( ) or rng.front( ) == '/' ) {
							break;
						}
						rng.remove_prefix( );
					}
					break;
				}
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void skip_comments( Range &rng ) {
			if constexpr( Range::is_unchecked_input ) {
				skip_comments_unchecked( rng );
			} else {
				skip_comments_checked( rng );
			}
		}

	public:
		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		move_to_next_of_unchecked( Range &rng, char c ) {
			while( rng.front( ) != c ) {
				rng.remove_prefix( );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		move_to_next_of_checked( Range &rng, char c ) {
			while( rng.has_more( ) and rng.front( ) != c ) {
				rng.remove_prefix( );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_checked( Range &rng ) {
			skip_comments_checked( rng );
			while( rng.has_more( ) and rng.is_space_unchecked( ) ) {
				rng.remove_prefix( );
				skip_comments_checked( rng );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		trim_left_unchecked( Range &rng ) {
			skip_comments_unchecked( rng );
			while( rng.is_space_unchecked( ) ) {
				rng.remove_prefix( );
			}
		}

		template<typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr void move_to_next_of( Range &rng,
		                                                             char c ) {
			skip_comments( rng );
			daw_json_assert_weak( rng.has_more( ), "Unexpected end of data" );
			while( rng.front( ) != c ) {
				daw_json_assert_weak( rng.has_more( ), "Unexpected end of data" );
				rng.remove_prefix( );
				skip_comments( rng );
			}
		}

		template<typename Range, std::size_t N>
		DAW_ATTRIBUTE_FLATTEN static constexpr void
		move_to_next_of( Range &rng, char const ( &str )[N] ) {
			skip_comments( rng );
			daw_json_assert_weak( rng.has_more( ), "Unexpected end of data" );
			while( not parse_policy_details::in( rng.front( ), str ) ) {
				daw_json_assert_weak( rng.has_more( ), "Unexpected end of data" );
				rng.remove_prefix( );
				skip_comments( rng );
			}
		}

		DAW_ATTRIBUTE_FLATTEN static constexpr bool at_literal_end( char c ) {
			return c == '\0' or c == ',' or c == ']' or c == '}' or c == '#';
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
		         typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr Range
		skip_bracketed_item_checked( Range &rng ) {
			// Not checking for Left as it is required to be skipped already
			auto result = rng;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = rng.first;
			char const *const ptr_last = rng.last;
			if( ptr_first < ptr_last and rng.front( ) == PrimLeft ) {
				++ptr_first;
			}
			while( ptr_first < ptr_last and prime_bracket_count > 0 ) {
				switch( *ptr_first ) {
				case '\\':
					++ptr_first;
					break;
				case '"':
					++ptr_first;
					while( ptr_first < ptr_last and *ptr_first != '"' ) {
						if( *ptr_first == '\\' ) {
							++ptr_first;
							if( ptr_first >= ptr_last ) {
								break;
							}
						}
						++ptr_first;
					}
					daw_json_assert( ptr_first < ptr_last, "Unexpected end of stream" );
					break;
				case ',':
					if( prime_bracket_count == 1 and second_bracket_count == 0 ) {
						++cnt;
					}
					break;
				case PrimLeft:
					++prime_bracket_count;
					break;
				case PrimRight:
					--prime_bracket_count;
					break;
				case SecLeft:
					++second_bracket_count;
					break;
				case SecRight:
					--second_bracket_count;
					break;
				case '/':
					++ptr_first;
					daw_json_assert( ptr_first < ptr_last, "Unexpected end of stream" );
					switch( *ptr_first ) {
					case '/':
						++ptr_first;
						while( ( ptr_last - ptr_first ) > 1 and *ptr_first != '\n' ) {
							++ptr_first;
						}
						break;
					case '*':
						++ptr_first;
						while( ( ptr_last - ptr_first ) >= 3 and *ptr_first != '*' and
						       *std::next( ptr_first ) != '/' ) {
							++ptr_first;
						}
						break;
					default:
						daw_json_error( "Unexpected character in stream" );
					}
					break;
				}
				++ptr_first;
			}
			daw_json_assert_weak( prime_bracket_count == 0 and
			                        second_bracket_count == 0,
			                      "Unexpected bracketing" );
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = ptr_first;
			result.counter = cnt;
			rng.first = ptr_first;
			return result;
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight,
		         typename Range>
		DAW_ATTRIBUTE_FLATTEN static constexpr Range
		skip_bracketed_item_unchecked( Range &rng ) {
			// Not checking for Left as it is required to be skipped already
			auto result = rng;
			std::size_t cnt = 0;
			std::uint32_t prime_bracket_count = 1;
			std::uint32_t second_bracket_count = 0;
			char const *ptr_first = rng.first;
			if( *ptr_first == PrimLeft ) {
				++ptr_first;
			}
			while( prime_bracket_count > 0 ) {
				switch( *ptr_first ) {
				case '\\':
					++ptr_first;
					break;
				case '"':
					++ptr_first;
					while( *ptr_first != '"' ) {
						if( *ptr_first == '\\' ) {
							++ptr_first;
						}
						++ptr_first;
					}
					break;
				case ',':
					if( prime_bracket_count == 1 and second_bracket_count == 0 ) {
						++cnt;
					}
					break;
				case PrimLeft:
					++prime_bracket_count;
					break;
				case PrimRight:
					--prime_bracket_count;
					break;
				case SecLeft:
					++second_bracket_count;
					break;
				case SecRight:
					--second_bracket_count;
					break;
				case '/':
					++ptr_first;
					switch( *ptr_first ) {
					case '/':
						++ptr_first;
						while( *ptr_first != '\n' ) {
							++ptr_first;
						}
						break;
					case '*':
						++ptr_first;
						while( *ptr_first != '*' and *std::next( ptr_first ) != '/' ) {
							++ptr_first;
						}
						break;
					default:
						daw_json_error( "Unexpected character in stream" );
					}
					break;
				}
				++ptr_first;
			}
			// We include the close primary bracket in the range so that subsequent
			// parsers have a terminator inside their range
			result.last = ptr_first;
			result.counter = cnt;
			rng.first = ptr_first;
			return result;
		}
	}; // namespace daw::json
} // namespace daw::json
