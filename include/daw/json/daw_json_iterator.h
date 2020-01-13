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

#include "daw_json_link.h"
#include "impl/daw_json_link_impl.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iterator/daw_back_inserter.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>

namespace daw::json {
	/// allow iteration over an array of json
	template<typename JsonElement, bool IsUnCheckedInput = false,
	         char separator = ','>
	class json_array_iterator {
		template<typename String>
		static constexpr json_details::IteratorRange<char const *, char const *,
		                                             IsUnCheckedInput>
		get_range( String &&data, std::string_view member_path ) {
			auto [is_found, result] = json_details::find_range<IsUnCheckedInput>(
			  std::forward<String>( data ),
			  {member_path.data( ), member_path.size( )} );
			daw_json_assert( is_found, "Could not find path to member" );
			daw_json_assert( result.front( ) == '[', "Member is not an array" );
			return result;
		}

	public:
		using element_type =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<element_type, void>,
		               "Unknown JsonElement type." );
		using value_type = typename element_type::parse_to_t;
		using reference = value_type;
		using pointer = value_type;
		using difference_type = ptrdiff_t;
		// Can do forward iteration and be stored
		using iterator_category = std::input_iterator_tag;

	private:
		// This lets us fastpath and just skip n characters
		json_details::IteratorRange<char const *, char const *, IsUnCheckedInput>
		  m_state{nullptr, nullptr};
		mutable difference_type m_can_skip = -1;

	public:
		constexpr json_array_iterator( ) = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_iterator, daw::remove_cvref_t<String>>> = nullptr>
		constexpr json_array_iterator( String &&jd,
		                               std::string_view start_path = "" )
		  : m_state( get_range( std::forward<String>( jd ), start_path ) ) {

			static_assert(
			  daw::traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
			  "StringRaw must be like a string_view" );
			m_state.trim_left( );
			daw_json_assert_weak( m_state.front( '[' ),
			                      "Arrays are expected to start with a [" );

			m_state.remove_prefix( );
			m_state.trim_left( );
		}

		[[nodiscard]] constexpr value_type operator*( ) const {
			daw_json_assert_weak( m_state.has_more( ) and not m_state.in( ']' ),
			                      "Unexpected end of stream" );

			auto tmp = m_state;
			auto result = json_details::parse_value<element_type>(
			  ParseTag<element_type::expected_type>{}, tmp );

			m_can_skip = std::distance( m_state.begin( ), tmp.begin( ) );
			return result;
		}

		constexpr json_array_iterator &operator++( ) {
			daw_json_assert_weak( m_state.has_more( ) and m_state.front( ) != ']',
			                      "Unexpected end of stream" );
			if( m_can_skip >= 0 ) {
				m_state.first = std::next( m_state.first, m_can_skip );
				m_can_skip = -1;
			} else {
				(void)json_details::skip_known_value<element_type>( m_state );
			}
			m_state.trim_left( );
			if( m_state.in( separator ) ) {
				m_state.remove_prefix( );
				m_state.trim_left( );
			}
			return *this;
		}

		constexpr json_array_iterator operator++( int ) {
			auto tmp = *this;
			(void)operator++( );
			return tmp;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const {
			return not m_state.is_null( ) and m_state.has_more( ) and
			       m_state.front( ) != ']';
		}

		[[nodiscard]] constexpr bool
		operator==( json_array_iterator const &rhs ) const {
			return ( m_state.begin( ) == rhs.m_state.begin( ) ) or
			       ( not( *this ) and not rhs );
		}

		[[nodiscard]] constexpr bool
		operator!=( json_array_iterator const &rhs ) const {
			return not( *this == rhs );
		}
	};

	template<typename JsonElement, bool IsUnCheckedInput = false,
	         char separator = ','>
	struct json_array_range {
		using iterator =
		  json_array_iterator<JsonElement, IsUnCheckedInput, separator>;

	private:
		iterator m_first{};
		iterator m_last{};

	public:
		constexpr json_array_range( ) = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_range, daw::remove_cvref_t<String>>> = nullptr>
		constexpr json_array_range( String &&jd, std::string_view start_path = "" )
		  : m_first( std::forward<String>( jd ), start_path ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return m_first;
		}

		[[nodiscard]] constexpr iterator end( ) {
			return m_last;
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_first == m_last;
		}
	};

	template<typename JsonElement, char separator = ','>
	using json_array_range_unchecked =
	  json_array_range<JsonElement, true, separator>;

	template<typename JsonElement, char separator = ','>
	using json_array_iterator_unchecked =
	  json_array_iterator<JsonElement, true, separator>;
} // namespace daw::json
