// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_arrow_proxy.h"
#include "daw_json_assert.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_value_fwd.h"
#include "daw_json_traits.h"

#include <daw/daw_move.h>

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
#if defined( DAW_JSON_HAS_CPP23_RANGE_CTOR )
				using iterator_category = std::input_iterator_tag;
#else
				// We have to lie so that std::distance uses O(1) instead of O(N)
				using iterator_category = std::random_access_iterator_tag;
#endif
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
			  : json_parse_kv_class_iterator_base<ParseState,
			                                      can_be_random_iterator_v<IsKnown>> {

				using base =
				  json_parse_kv_class_iterator_base<ParseState,
				                                    can_be_random_iterator_v<IsKnown>>;
				using iterator_category = typename base::iterator_category;
				using element_t = typename JsonMember::json_element_t;
				using member_container_type = json_base_type_t<JsonMember>;
				using value_type =
				  kv_class_iter_impl::container_value_type_or<JsonMember,
				                                              member_container_type>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using iterator_range_t = ParseState;
				using difference_type = typename base::difference_type;

				using key_t = typename JsonMember::json_key_t;
				using value_t = typename JsonMember::json_element_t;

				json_parse_kv_class_iterator( ) = default;

				constexpr explicit json_parse_kv_class_iterator( iterator_range_t &r )
				  : base{ &r } {
					daw_json_ensure( not IsKnown and not base::parse_state->empty( ),
					                 ErrorReason::UnexpectedEndOfData );
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

				DAW_ATTRIB_NOINLINE value_type operator*( ) const {
					// This is hear to satisfy indirectly_readable
					daw_json_error( ErrorReason::UnexpectedEndOfData );
				}

				constexpr value_type operator*( ) {
					daw_json_assert_weak(
					  base::parse_state and base::parse_state->has_more( ),
					  ErrorReason::UnexpectedEndOfData, *base::parse_state );
					auto key = parse_value<key_t, false, key_t::expected_type>(
					  *base::parse_state );
					name::name_parser::trim_end_of_name( *base::parse_state );

					return json_class_constructor<value_type,
					                              default_constructor<value_type>>(
					  std::move( key ),
					  parse_value<value_t, false, value_t::expected_type>(
					    *base::parse_state ) );
				}

				constexpr json_parse_kv_class_iterator &operator++( ) {
					daw_json_assert_weak( base::parse_state,
					                      ErrorReason::AttemptToAccessPastEndOfValue,
					                      *base::parse_state );
					base::parse_state->move_next_member_or_end( );
					daw_json_assert_weak( base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					if( base::parse_state->front( ) == '}' ) {
#if not defined( NDEBUG )
						if constexpr( IsKnown ) {
							if( base::parse_state ) {
								daw_json_ensure( base::parse_state->counter == 0,
								                 ErrorReason::AttemptToAccessPastEndOfValue,
								                 *base::parse_state );
								base::parse_state->counter--;
							}
						}
#endif
						// Cleanup at end of value
						base::parse_state->remove_prefix( );
						base::parse_state->trim_left_checked( );
						// Ensure we are equal to default
						base::parse_state = nullptr;
					}
#if not defined( NDEBUG )
					if constexpr( IsKnown ) {
						if( base::parse_state ) {
							daw_json_ensure( base::parse_state->counter > 0,
							                 ErrorReason::AttemptToAccessPastEndOfValue,
							                 *base::parse_state );
							base::parse_state->counter--;
						}
					}
#endif
					return *this;
				}

				DAW_ATTRIB_INLINE constexpr void operator++( int ) {
					(void)operator++( );
				}

				friend constexpr bool
				operator==( json_parse_kv_class_iterator const &lhs,
				            json_parse_kv_class_iterator const &rhs ) {
					// using identity as equality
					return lhs.parse_state == rhs.base::parse_state;
				}

				friend constexpr bool
				operator!=( json_parse_kv_class_iterator const &lhs,
				            json_parse_kv_class_iterator const &rhs ) {
					return not( lhs == rhs );
				}
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
