// Copyright( c ) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_arrow_proxy.h"
#include "daw_json_assert.h"
#include "daw_json_parse_value_fwd.h"
#include "version.h"

#include <daw/daw_attributes.h>
#include <daw/daw_move.h>

#include <ciso646>

namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		namespace json_details DAW_ATTRIB_HIDDEN {
			template<typename ParseState, bool>
			struct json_parse_kv_array_iterator_base {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = false;
				ParseState *parse_state = nullptr;
			};

			template<typename ParseState>
			struct json_parse_kv_array_iterator_base<ParseState, true> {
				// We have to lie so that std::distance uses O(1) instead of O(N)
				using iterator_category = std::random_access_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = true;
				ParseState *parse_state = nullptr;
				difference_type counter = 0;

				constexpr json_parse_kv_array_iterator_base( ) = default;

				explicit inline constexpr json_parse_kv_array_iterator_base(
				  ParseState *pd ) noexcept
				  : parse_state( pd )
				  , counter( static_cast<difference_type>( pd->counter ) ) {}

				inline constexpr difference_type
				operator-( json_parse_kv_array_iterator_base const &rhs ) const {
					// rhs is the iterator with the parser in it.  We should know how many
					// items are in play because we already counted them in the skip_array
					// call.
					return rhs.counter;
				}
			};

			template<typename JsonMember, typename ParseState, bool KnownBounds>
			struct json_parse_kv_array_iterator
			  : json_parse_kv_array_iterator_base<ParseState,
			                                      can_random_v<KnownBounds>> {

				using base =
				  json_parse_kv_array_iterator_base<ParseState,
				                                    can_random_v<KnownBounds>>;
				using iterator_category = typename base::iterator_category;
				using json_key_t = typename JsonMember::json_key_t;
				using json_element_t = typename JsonMember::json_value_t;
				using value_type = std::pair<typename json_key_t::parse_to_t const,
				                             typename json_element_t::parse_to_t>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using parse_state_t = ParseState;
				using difference_type = typename base::difference_type;

				using json_class_type = typename JsonMember::json_class_t;
				inline constexpr json_parse_kv_array_iterator( ) = default;

				inline constexpr explicit json_parse_kv_array_iterator(
				  parse_state_t &r )
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

				static inline constexpr value_type
				get_pair( typename json_class_type::parse_to_t &&v ) {
					return value_type( std::get<0>( DAW_MOVE( v.members ) ),
					                   std::get<1>( DAW_MOVE( v.members ) ) );
				}

				DAW_ATTRIB_INLINE inline constexpr value_type operator*( ) {
					daw_json_assert_weak(
					  base::parse_state and base::parse_state->has_more( ),
					  ErrorReason::UnexpectedEndOfData, *base::parse_state );

					return get_pair( parse_value<json_class_type>(
					  *base::parse_state, ParseTag<JsonParseTypes::Class>{ } ) );
				}

				DAW_ATTRIB_INLINE inline constexpr json_parse_kv_array_iterator &
				operator++( ) {
					daw_json_assert_weak( base::parse_state,
					                      ErrorReason::UnexpectedEndOfData );
					base::parse_state->trim_left( );

					daw_json_assert_weak(
					  base::parse_state->has_more( ) and
					    base::parse_state->is_at_next_array_element( ),
					  ErrorReason::UnexpectedEndOfData, *base::parse_state );

					base::parse_state->move_next_member_or_end( );
					daw_json_assert_weak( base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData );
					if( DAW_UNLIKELY( base::parse_state->front( ) == ']' ) ) {
#ifndef NDEBUG
						if constexpr( base::has_counter ) {
							daw_json_assert_weak( base::counter == 0,
							                      ErrorReason::UnexpectedEndOfData );
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
						daw_json_assert_weak( base::counter > 0,
						                      ErrorReason::UnexpectedEndOfData );
						--base::counter;
					}
#endif
					return *this;
				}

				friend inline constexpr bool
				operator==( json_parse_kv_array_iterator const &lhs,
				            json_parse_kv_array_iterator const &rhs ) {
					return lhs.parse_state == rhs.parse_state;
				}

				friend inline constexpr bool
				operator!=( json_parse_kv_array_iterator const &lhs,
				            json_parse_kv_array_iterator const &rhs ) {
					return not( lhs == rhs );
				}
			};
		} // namespace DAW_ATTRIB_HIDDEN
	}   // namespace DAW_JSON_VER
} // namespace daw::json
