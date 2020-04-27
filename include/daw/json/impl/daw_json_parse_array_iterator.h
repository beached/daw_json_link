// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_value_fwd.h"

namespace daw::json::json_details {

	template<typename JsonMember, typename First, typename Last,
	         bool IsUnCheckedInput>
	struct json_parse_value_array_iterator {
		using iterator_category = std::input_iterator_tag;
		using element_t = typename JsonMember::json_element_t;
		using value_type = element_t;
		using reference = void;
		using pointer = void;
		using difference_type = std::ptrdiff_t;
		using iterator_range_t = IteratorRange<First, Last, IsUnCheckedInput>;

		iterator_range_t *rng = nullptr;

		constexpr json_parse_value_array_iterator( ) = default;

		constexpr json_parse_value_array_iterator( iterator_range_t &r )
		  : rng( &r ) {}

		constexpr decltype(auto) operator*( ) {
			return parse_value<element_t>( ParseTag<element_t::expected_type>{ },
			                               *rng );
		}

		constexpr json_parse_value_array_iterator &operator++( ) {
			rng->clean_tail( );
			daw_json_assert_weak( rng->has_more( ), "Unexpected end of data" );
			if( rng->front( ) == ']' ) {
				// Cleanup at end of value
				rng->remove_prefix( );
				rng->trim_left( );
				// Ensure we are equal to default
				rng = nullptr;
			}
			return *this;
		}

		constexpr json_parse_value_array_iterator operator++( int ) {
			auto result = *this;
			(void)this->operator++( );
			return result;
		}

		constexpr bool
		operator==( json_parse_value_array_iterator const &rhs ) const {
			// using identity as equality
			return rng == rhs.rng;
		}

		constexpr bool
		operator!=( json_parse_value_array_iterator const &rhs ) const {
			// using identity as equality
			return rng != rhs.rng;
		}
	};

} // namespace daw::json::json_details
