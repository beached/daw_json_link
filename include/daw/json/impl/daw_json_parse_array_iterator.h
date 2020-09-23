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
#include "daw_json_iterator_range.h"
#include "daw_json_parse_value_fwd.h"

#include <ciso646>

namespace daw::json::json_details {
	template<typename Range, bool>
	struct json_parse_array_iterator_base {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		static constexpr bool has_counter = false;
		Range *rng = nullptr;
	};

	template<typename Range>
	struct json_parse_array_iterator_base<Range, true> {
		// We have to lie so that std::distance uses O(1) instead of O(N)
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		static constexpr bool has_counter = true;
		Range *rng = nullptr;

		constexpr difference_type
		operator-( json_parse_array_iterator_base const &rhs ) const {
			if( rhs.rng ) {
				return static_cast<difference_type>( rhs.rng->counter ) + 1;
			}
			return 0;
		}
	};

	template<typename JsonMember, typename Range, bool IsKnown>
	struct json_parse_array_iterator
	  : json_parse_array_iterator_base<Range, can_random_v<IsKnown>> {

		using base = json_parse_array_iterator_base<Range, can_random_v<IsKnown>>;
		using iterator_category = typename base::iterator_category;
		using element_t = typename JsonMember::json_element_t;
		using value_type = typename element_t::parse_to_t;
		using reference = value_type;
		using pointer = arrow_proxy<value_type>;
		using iterator_range_t = Range;
		using difference_type = typename base::difference_type;

		inline constexpr json_parse_array_iterator( ) = default;

		inline constexpr explicit json_parse_array_iterator( iterator_range_t &r )
		  : base{ &r } {
			if( base::rng->front( ) == ']' ) {
				if constexpr( not IsKnown ) {
					// Cleanup at end of value
					base::rng->remove_prefix( );
					base::rng->trim_left_checked( );
					// Ensure we are equal to default
				}
				base::rng = nullptr;
			}
		}

		inline constexpr value_type operator*( ) {
			daw_json_assert_weak( base::rng and base::rng->has_more( ),
			                      "Expected data to parse", *base::rng );
			return parse_value<element_t>( ParseTag<element_t::expected_type>{ },
			                               *base::rng );
		}

		inline constexpr json_parse_array_iterator &operator++( ) {
			//daw_json_assert_weak( base::rng, "Unexpected increment", *base::rng );
			base::rng->clean_tail( );
			daw_json_assert_weak( base::rng->has_more( ), "Unexpected end of data", *base::rng );
			if( base::rng->front( ) == ']' ) {
#ifndef NDEBUG
				if constexpr( base::has_counter ) {
					daw_json_assert_weak( base::rng->counter == 0,
					                      "Unexpected item count", *base::rng );
				}
#endif
				if constexpr( not IsKnown ) {
					// Cleanup at end of value
					base::rng->remove_prefix( );
					base::rng->trim_left_checked( );
					// Ensure we are equal to default
				}
				base::rng = nullptr;
			}
#ifndef NDEBUG
			if constexpr( base::has_counter ) {
				if( base::rng ) {
					daw_json_assert_weak( base::rng->counter > 0,
					                      "Unexpected item count", *base::rng );
					base::rng->counter--;
				}
			}
#endif
			return *this;
		}

		inline constexpr bool
		operator==( json_parse_array_iterator const &rhs ) const {
			return base::rng == rhs.base::rng;
		}

		inline constexpr bool
		operator!=( json_parse_array_iterator const &rhs ) const {
			return base::rng != rhs.base::rng;
		}
	};
} // namespace daw::json::json_details
