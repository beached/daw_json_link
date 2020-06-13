// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_value_fwd.h"

namespace daw::json::json_details {

	template<typename JsonMember, typename Range>
	struct json_parse_value_array_iterator {
		using iterator_category = std::input_iterator_tag;
		using element_t = typename JsonMember::json_element_t;
		using value_type = typename element_t::parse_to_t;
		using reference = void;
		using pointer = void;
		using difference_type = std::ptrdiff_t;
		using iterator_range_t = Range;

		iterator_range_t *rng = nullptr;

		inline constexpr json_parse_value_array_iterator( ) = default;

		inline constexpr explicit json_parse_value_array_iterator(
		  iterator_range_t &r )
		  : rng( &r ) {
			if( rng->front( ) == ']' ) {
				// Cleanup at end of value
				rng->remove_prefix( );
				rng->trim_left_checked( );
				// Ensure we are equal to default
				rng = nullptr;
			}
		}

		inline constexpr value_type operator*( ) {
			daw_json_assert_weak( rng and rng->can_parse_more( ),
			                      "Expected data to parse" );
			return parse_value<element_t>( ParseTag<element_t::expected_type>{ },
			                               *rng );
		}

		inline constexpr json_parse_value_array_iterator &operator++( ) {
			daw_json_assert_weak( rng, "Unexpected increment" );
			rng->clean_tail( );
			daw_json_assert_weak( rng->has_more( ), "Unexpected end of data" );
			if( rng->front( ) == ']' ) {
				// Cleanup at end of value
				rng->remove_prefix( );
				rng->trim_left_checked( );
				// Ensure we are equal to default
				rng = nullptr;
			}
			return *this;
		}

		inline constexpr json_parse_value_array_iterator operator++( int ) {
			auto result = *this;
			(void)this->operator++( );
			return result;
		}

		inline constexpr bool
		operator==( json_parse_value_array_iterator const &rhs ) const {
			// using identity as equality
			return rng == rhs.rng;
		}

		inline constexpr bool
		operator!=( json_parse_value_array_iterator const &rhs ) const {
			// using identity as equality
			return rng != rhs.rng;
		}
	};
} // namespace daw::json::json_details
