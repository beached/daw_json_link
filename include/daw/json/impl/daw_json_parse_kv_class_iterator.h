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
#include "daw_json_parse_value_fwd.h"
#include "version.h"

#include <daw/daw_move.h>

#include <ciso646>
#include <cstddef>
#include <iterator>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename ParseState, bool>
			struct json_parse_kv_class_iterator_base {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				ParseState *parse_state = nullptr;
			};

			template<typename ParseState>
			struct json_parse_kv_class_iterator_base<ParseState, true> {
				// We have to lie so that std::distance uses O(1) instead of O(N)
				using iterator_category = std::random_access_iterator_tag;
				using difference_type = std::ptrdiff_t;
				ParseState *parse_state = nullptr;

				constexpr difference_type
				operator-( json_parse_kv_class_iterator_base const &rhs ) const {
					if( rhs.parse_state ) {
						return static_cast<difference_type>( rhs.parse_state->counter );
					}
					return 0;
				}
			};

			namespace kv_class_iter_impl {
				template<typename T>
				using container_value_t = typename T::value_type;

				template<typename JsonMember>
				using default_value_type =
				  std::pair<typename JsonMember::json_key_t,
				            typename JsonMember::json_element_t>;

				template<typename JsonMember, typename T>
				using container_value_type_or =
				  daw::detected_or_t<default_value_type<JsonMember>, container_value_t,
				                     T>;
			} // namespace kv_class_iter_impl

			template<typename JsonMember, typename ParseState, bool IsKnown>
			struct json_parse_kv_class_iterator
			  : json_parse_kv_class_iterator_base<ParseState, can_random_v<IsKnown>> {

				using base =
				  json_parse_kv_class_iterator_base<ParseState, can_random_v<IsKnown>>;
				using iterator_category = typename base::iterator_category;
				using element_t = typename JsonMember::json_element_t;
				using member_container_type = typename JsonMember::base_type;
				using value_type =
				  kv_class_iter_impl::container_value_type_or<JsonMember,
				                                              member_container_type>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using iterator_range_t = ParseState;
				using difference_type = typename base::difference_type;

				using key_t = typename JsonMember::json_key_t;
				using value_t = typename JsonMember::json_element_t;

				inline constexpr json_parse_kv_class_iterator( ) = default;

				inline constexpr explicit json_parse_kv_class_iterator(
				  iterator_range_t &r )
				  : base{ &r } {
					if( base::parse_state->front( ) == '}' ) {
						// Cleanup at end of value
						if( not IsKnown ) {
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					}
				}

				inline constexpr value_type operator*( ) {
					daw_json_assert_weak(
					  base::parse_state and base::parse_state->has_more( ),
					  ErrorReason::UnexpectedEndOfData, *base::parse_state );
					auto key = parse_value<key_t>( *base::parse_state,
					                               ParseTag<key_t::expected_type>{ } );
					name::name_parser::trim_end_of_name( *base::parse_state );

					return json_class_constructor<value_type,
					                              default_constructor<value_type>>(
					  DAW_MOVE( key ),
					  parse_value<value_t>( *base::parse_state,
					                        ParseTag<value_t::expected_type>{ } ) );
				}

				inline constexpr json_parse_kv_class_iterator &operator++( ) {
					daw_json_assert_weak( base::parse_state,
					                      ErrorReason::AttemptToAccessPastEndOfValue,
					                      *base::parse_state );
					base::parse_state->move_next_member_or_end( );
					daw_json_assert_weak( base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					if( base::parse_state->front( ) == '}' ) {
#ifndef NDEBUG
						if constexpr( IsKnown ) {
							if( base::parse_state ) {
								daw_json_assert( base::parse_state->counter > 0,
								                 ErrorReason::AttemptToAccessPastEndOfValue,
								                 *base::parse_state );
								base::parse_state->counter--;
							}
						}
#endif
						if constexpr( not IsKnown ) {
							// Cleanup at end of value
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					}
#ifndef NDEBUG
					if constexpr( IsKnown ) {
						if( base::parse_state ) {
							daw_json_assert( base::parse_state->counter > 0,
							                 ErrorReason::AttemptToAccessPastEndOfValue,
							                 *base::parse_state );
							base::parse_state->counter--;
						}
					}
#endif
					return *this;
				}

				friend inline constexpr bool
				operator==( json_parse_kv_class_iterator const &lhs,
				            json_parse_kv_class_iterator const &rhs ) {
					// using identity as equality
					return lhs.parse_state == rhs.base::parse_state;
				}

				friend inline constexpr bool
				operator!=( json_parse_kv_class_iterator const &lhs,
				            json_parse_kv_class_iterator const &rhs ) {
					return not( lhs == rhs );
				}
			};
		} // namespace json_details
	}   // namespace DAW_ATTRIB_HIDDEN
} // namespace daw::json
