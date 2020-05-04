// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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
#include <daw/daw_scope_guard.h>
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
	namespace json_details {
		template<typename T>
		struct arrow_proxy {
			T value;

			[[nodiscard]] constexpr T *operator->( ) {
				return &value;
			}
		};
	} // namespace json_details

	/// allow iteration over an array of json
	template<typename JsonElement, bool IsUnCheckedInput = false>
	class json_array_iterator {

		template<typename String>
		static constexpr json_details::IteratorRange<char const *, IsUnCheckedInput>
		get_range( String &&data, std::string_view member_path ) {
			auto [is_found, result] = json_details::find_range<IsUnCheckedInput>(
			  std::forward<String>( data ),
			  { member_path.data( ), member_path.size( ) } );
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
		using pointer = json_details::arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		// Can do forward iteration and be stored
		using iterator_category = std::input_iterator_tag;

	private:
		// This lets us fastpath and just skip n characters
		using Range = json_details::IteratorRange<char const *, IsUnCheckedInput>;
		Range m_state{ nullptr, nullptr };
		mutable difference_type m_can_skip = -1;

	public:
		constexpr json_array_iterator( ) = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_iterator, daw::remove_cvref_t<String>>> = nullptr>
		constexpr explicit json_array_iterator( String &&jd,
		                                        std::string_view start_path = "" )
		  : m_state( get_range( std::forward<String>( jd ), start_path ) ) {

			static_assert(
			  daw::traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
			  "StringRaw must be like a string_view" );
			m_state.trim_left_checked( );
			daw_json_assert_weak( m_state.front( '[' ),
			                      "Arrays are expected to start with a [" );

			m_state.remove_prefix( );
			m_state.trim_left_checked( );
		}

		[[nodiscard]] constexpr value_type operator*( ) const {
			daw_json_assert_weak( m_state.has_more( ) and not m_state.in( ']' ),
			                      "Unexpected end of stream" );

			auto tmp = m_state;

#if defined( __cpp_constexpr_dynamic_alloc ) or                                \
  defined( DAW_JSON_NO_CONST_EXPR )
			auto const ae = daw::on_exit_success(
			  [&] { m_can_skip = std::distance( m_state.begin( ), tmp.begin( ) ); } );
			return json_details::parse_value<element_type>(
			  ParseTag<element_type::expected_type>{ }, tmp );
#else
			auto result = json_details::parse_value<element_type>(
			  ParseTag<element_type::expected_type>{ }, tmp );

			m_can_skip = std::distance( m_state.begin( ), tmp.begin( ) );
			return result;
#endif
		}

		[[nodiscard]] pointer operator->( ) const {
			return pointer{ operator*( ) };
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
			m_state.trim_left_checked( );
			if( m_state.in( ',' ) ) {
				m_state.remove_prefix( );
				m_state.trim_left_checked( );
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
			if( not( *this ) ) {
				return not rhs;
			}
			if( not rhs ) {
				return false;
			}
			return ( m_state.begin( ) == rhs.m_state.begin( ) );
		}

		[[nodiscard]] constexpr bool
		operator!=( json_array_iterator const &rhs ) const {
			if( not( *this ) ) {
				return static_cast<bool>( rhs );
			}
			if( not rhs ) {
				return true;
			}
			return m_state.begin( ) != rhs.m_state.begin( );
		}
	};

	template<typename JsonElement, bool IsUnCheckedInput = false>
	struct json_array_range {
		using iterator = json_array_iterator<JsonElement, IsUnCheckedInput>;

	private:
		iterator m_first{ };
		iterator m_last{ };

	public:
		constexpr json_array_range( ) = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_range, daw::remove_cvref_t<String>>> = nullptr>
		constexpr explicit json_array_range( String &&jd,
		                                     std::string_view start_path = "" )
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

	template<typename JsonElement>
	using json_array_range_unchecked = json_array_range<JsonElement, true>;

	template<typename JsonElement>
	using json_array_iterator_unchecked = json_array_iterator<JsonElement, true>;
} // namespace daw::json
