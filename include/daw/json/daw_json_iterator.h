// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>

#include "daw_json_link.h"
#include "impl/daw_json_link_impl.h"

namespace daw::json {
	/// allow iteration over an array of json
	template<typename JsonElement, bool TrustedInput = false,
	         char separator = ','>
	class json_array_iterator {
		impl::IteratorRange<char const *, char const *, TrustedInput> m_state{
		  nullptr, nullptr};
		// This lets us fastpath and just skip n characters
		mutable intmax_t m_can_skip = -1;

	public:
		using value_type = typename JsonElement::parse_to_t;
		using reference = value_type;
		using pointer = value_type;
		using difference_type = ptrdiff_t;
		// Can do forward iteration and be stored
		using iterator_category = std::input_iterator_tag;

		constexpr json_array_iterator( ) noexcept = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_iterator, daw::remove_cvref_t<String>>> = nullptr>
		constexpr json_array_iterator( String &&jd,
		                               std::string_view start_path = "" )
		  : m_state(
		      impl::find_range<TrustedInput>( std::forward<String>( jd ),
		                        {start_path.data( ), start_path.size( )} ) ) {

			static_assert(
			  daw::traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
			  "String must be like a string_view" );

			if constexpr( not TrustedInput ) {
				json_assert( m_state.front( ) == '[',
				             "Arrays are expected to start with a [" );
			}

			m_state.remove_prefix( );
			m_state.trim_left( );
		}

		[[nodiscard]] constexpr value_type operator*( ) const noexcept {
			if constexpr( not TrustedInput ) {
				json_assert( not m_state.empty( ) and not m_state.in( ']' ),
				             "Unexpected end of stream" );
			}

			auto tmp = m_state;
			return impl::parse_value<JsonElement>(
			  ParseTag<JsonElement::expected_type>{}, tmp );
			m_can_skip = std::distance( m_state.begin( ), tmp.begin( ) );
		}

		constexpr json_array_iterator &operator++( ) noexcept {
			if constexpr( not TrustedInput ) {
				json_assert( m_state.empty( ) and not m_state.in( ']' ),
				             "Unexpected end of stream" );
			}
			if( m_can_skip >= 0 ) {
				m_state.first = std::next( m_state.first, m_can_skip );
				m_can_skip = -1;
			} else {
				(void)impl::skip_known_value<JsonElement>( m_state );
			}
			m_state.trim_left( );
			if( m_state.in( separator ) ) {
				m_state.remove_prefix( );
				m_state.trim_left( );
			}
			return *this;
		}

		constexpr json_array_iterator operator++( int ) noexcept {
			auto tmp = *this;
			(void)operator++( );
			return tmp;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const noexcept {
			return not m_state.is_null( ) and not m_state.front( ']' );
		}

		[[nodiscard]] constexpr bool
		operator==( json_array_iterator const &rhs ) const noexcept {
			return ( m_state.begin( ) == rhs.m_state.begin( ) ) or
			       ( not( *this ) and not rhs );
		}

		[[nodiscard]] constexpr bool
		operator!=( json_array_iterator const &rhs ) const noexcept {
			return not( *this == rhs );
		}
	};

	template<typename JsonElement, bool TrustedInput = false,
	         char separator = ','>
	struct json_array_range {
		using value_type =
		  json_array_iterator<JsonElement, TrustedInput, separator>;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		value_type m_first{};
		value_type m_last{};

	public:
		constexpr json_array_range( ) noexcept = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_range, daw::remove_cvref_t<String>>> = nullptr>
		constexpr json_array_range( String &&jd, std::string_view start_path = "" )
		  : m_first( std::forward<String>( jd ), start_path ) {}

		[[nodiscard]] constexpr reference begin( ) noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_reference begin( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr const_reference cbegin( ) const noexcept {
			return m_first;
		}

		[[nodiscard]] constexpr reference end( ) noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_reference end( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr const_reference cend( ) const noexcept {
			return m_last;
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_first == m_last;
		}
	};
} // namespace daw::json
