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
#include "daw_vec_intel.h"
#include "version.h"

#include <daw/daw_attributes.h>
#include <daw/daw_not_null.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		namespace json_details {

			inline constexpr std::uint32_t too_small = std::numeric_limits<std::uint32_t>::max( ) << 16U;
			inline constexpr std::uint32_t array_end_found = 1U << 16U;

			[[nodiscard]] inline std::pair<std::uint64_t, std::uint32_t>
			init_unsigned_mask( daw::not_null<char const *> first, daw::not_null<char const *> last ) {
				std::size_t offset = 0;
				while( last - first >= 16 ) {
					auto const data = vec::vec128( first );
					auto const ascii_zeros = vec::vec128( vec::set_char, '0' );
					auto const ascii_nines = vec::vec128( vec::set_char, '9' );
					auto const ascii_rbracket = vec::vec128( vec::set_char, ']' );
					auto const is_ge_zero = data >= ascii_zeros;
					auto const is_le_nine = data <= ascii_nines;
					auto const is_num = static_cast<unsigned>( ( is_ge_zero & is_le_nine ).to_mask( ) );
					if( is_num != 0 ) {
						auto const is_rbracket = static_cast<unsigned>( ( data == ascii_rbracket ).to_mask( ) );
						if( is_rbracket == 0 ) {
							return { offset, is_num };
						}
						auto const p =
						  ( ( 1U << ( static_cast<unsigned>( __builtin_ctz( is_rbracket ) + 1 ) ) ) - 1U );
						return { ( array_end_found | offset ), is_num & p };
					}
					first += 16;
					offset += 16;
				}
				// Not enough room to vectorize 0xFFFF'FFFF means no data
				return { too_small | offset, 0 };
			}

			template<typename ParseState, bool>
			struct json_parse_unsigned_array_iterator_base {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = false;

				ParseState *parse_state = nullptr;

				inline json_parse_unsigned_array_iterator_base( ParseState *ps )
				  : parse_state( ps ) {}
			};

			template<typename JsonMember, typename ParseState, bool KnownBounds>
			struct json_parse_unsigned_array_iterator :
			  json_parse_unsigned_array_iterator_base<ParseState, can_random_v<KnownBounds>> {

				using base = json_parse_unsigned_array_iterator_base<ParseState, can_random_v<KnownBounds>>;

				using iterator_category = typename base::iterator_category;
				using element_t = typename JsonMember::json_element_t;
				using value_type = typename element_t::parse_to_t;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using parse_state_t = ParseState;
				using difference_type = typename base::difference_type;
				using size_type = std::size_t;

				inline constexpr json_parse_unsigned_array_iterator( ) = default;

				inline constexpr explicit json_parse_unsigned_array_iterator( parse_state_t &r )
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

				DAW_ATTRIB_INLINE
				constexpr value_type operator*( ) {
					daw_json_assert_weak( base::parse_state and base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );

					return parse_value<element_t>( *base::parse_state,
					                               ParseTag<element_t::expected_type>{ } );
				}

				DAW_ATTRIB_INLINE constexpr json_parse_unsigned_array_iterator &operator++( ) {
					daw_json_assert_weak( base::parse_state,
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					base::parse_state->trim_left( );

					daw_json_assert_weak( base::parse_state->has_more( ) and
					                        base::parse_state->is_at_next_array_element( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );

					base::parse_state->move_next_member_or_end( );
					daw_json_assert_weak( base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					if( base::parse_state->front( ) == ']' ) {
#ifndef NDEBUG
						if constexpr( base::has_counter ) {
							daw_json_assert_weak( base::counter == 0,
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
					} else {
#ifndef NDEBUG
						if constexpr( base::has_counter ) {
							daw_json_assert_weak( base::counter > 0,
							                      ErrorReason::AttemptToAccessPastEndOfValue,
							                      *base::parse_state );
							--base::counter;
						}
#endif
					}
					return *this;
				}

				friend inline constexpr bool operator==( json_parse_unsigned_array_iterator const &lhs,
				                                         json_parse_unsigned_array_iterator const &rhs ) {
					return lhs.parse_state == rhs.parse_state;
				}

				friend inline constexpr bool operator!=( json_parse_unsigned_array_iterator const &lhs,
				                                         json_parse_unsigned_array_iterator const &rhs ) {
					return not( lhs == rhs );
				}

				constexpr json_parse_unsigned_array_iterator &begin( ) {
					return *this;
				}

				constexpr json_parse_unsigned_array_iterator end( ) const {
					return { };
				}
			};
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
