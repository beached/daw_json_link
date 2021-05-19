// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_arrow_proxy.h"
#include "daw_json_assert.h"
#include "daw_json_parse_policy.h"
#include "daw_json_parse_value_fwd.h"
#include "version.h"

#include <ciso646>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename ParseState, bool>
			struct json_parse_array_iterator_base {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = false;

				ParseState *parse_state = nullptr;
			};

			template<typename ParseState>
			struct json_parse_array_iterator_base<ParseState, true> {
				// We have to lie so that std::distance uses O(1) instead of O(N)
				using iterator_category = std::random_access_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = true;

				ParseState *parse_state = nullptr;

				inline constexpr difference_type
				operator-( json_parse_array_iterator_base const &rhs ) const {
					// rhs is the iterator with the parser in it.  We should know how many
					// items are in play because we already counted them in the skip_array
					// call. If it is null, that means it has hit the end of the array
					if( DAW_LIKELY( rhs.parse_state ) ) {
						return static_cast<difference_type>( rhs.parse_state->counter ) + 1;
					}
					return 0;
				}
			};

			template<typename JsonMember, typename ParseState, bool KnownBounds>
			struct json_parse_array_iterator
			  : json_parse_array_iterator_base<ParseState,
			                                   can_random_v<KnownBounds>> {

				using base =
				  json_parse_array_iterator_base<ParseState, can_random_v<KnownBounds>>;
				using iterator_category = typename base::iterator_category;
				using element_t = typename JsonMember::json_element_t;
				using value_type = typename element_t::parse_to_t;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using parse_state_t = ParseState;
				using difference_type = typename base::difference_type;
				bool at_first = true;
				inline constexpr json_parse_array_iterator( ) = default;

				inline constexpr explicit json_parse_array_iterator( parse_state_t &r )
				  : base{ &r } {
					if( DAW_UNLIKELY( base::parse_state->front( ) == ']' ) ) {
						if constexpr( not KnownBounds ) {
							// Cleanup at end of value
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					}
				}

				DAW_ONLY_INLINE inline constexpr value_type operator*( ) {
					daw_json_assert_weak(
					  base::parse_state and base::parse_state->has_more( ),
					  ErrorReason::UnexpectedEndOfData, *base::parse_state );

					at_first = false;
					if constexpr( KnownBounds ) {
						if constexpr( is_guaranteed_rvo_v<ParseState> ) {
							struct cleanup_t {
								ParseState &p;
								std::size_t counter;
								CPP20CONSTEXPR inline ~cleanup_t( ) noexcept( false ) {
#ifdef HAS_CPP20CONSTEXPR
									if( not std::is_constant_evaluated( ) ) {
#endif
										if( DAW_LIKELY( std::uncaught_exceptions( ) == 0 ) ) {
											p.counter = counter;
										}
#ifdef HAS_CPP20CONSTEXPR
									} else {
										ptr.counter = counter;
									}
#endif
								}
							} const run_after_parse{ *base::parse_state,
							                         base::parse_state->counter };
							(void)run_after_parse;
							return parse_value<element_t>(
							  ParseTag<element_t::expected_type>{ }, *base::parse_state );
						} else {
							auto const cnt = base::parse_state->counter;
							auto result = parse_value<element_t>(
							  ParseTag<element_t::expected_type>{ }, *base::parse_state );
							base::parse_state->counter = cnt;
							return result;
						}
					} else {
						return parse_value<element_t>(
						  ParseTag<element_t::expected_type>{ }, *base::parse_state );
					}
				}

				DAW_ONLY_INLINE inline constexpr json_parse_array_iterator &
				operator++( ) {
					// daw_json_assert_weak( base::parse_state, "Unexpected increment",
					// *base::parse_state );
					base::parse_state->template clean_end_of_value<']'>( at_first );
					daw_json_assert_weak( base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					if( base::parse_state->front( ) == ']' ) {
#ifndef NDEBUG
						if constexpr( base::has_counter ) {
							daw_json_assert_weak( base::parse_state->counter == 0,
							                      ErrorReason::AttemptToAccessPastEndOfValue,
							                      *base::parse_state );
						}
#endif
						if constexpr( not KnownBounds ) {
							// Cleanup at end of value
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					}
#ifndef NDEBUG
					if constexpr( base::has_counter ) {
						if( DAW_LIKELY( base::parse_state ) ) {
							daw_json_assert_weak( base::parse_state->counter > 0,
							                      ErrorReason::AttemptToAccessPastEndOfValue,
							                      *base::parse_state );
							base::parse_state->counter--;
						}
					}
#endif
					return *this;
				}

				friend inline constexpr bool
				operator==( json_parse_array_iterator const &lhs,
				            json_parse_array_iterator const &rhs ) {
					return lhs.parse_state == rhs.parse_state;
				}

				friend inline constexpr bool
				operator!=( json_parse_array_iterator const &lhs,
				            json_parse_array_iterator const &rhs ) {
					return lhs.parse_state != rhs.parse_state;
				}
			};
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
