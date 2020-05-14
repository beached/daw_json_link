// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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

#include <string>

#include "daw_algorithm.h"
#include "daw_move.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "iterator/daw_reverse_iterator.h"

namespace daw {
	template<typename CharT>
	class string_split_iterator {
		static constexpr size_t const npos = daw::basic_string_view<CharT>::npos;

		daw::basic_string_view<CharT> m_str{};
		daw::basic_string_view<CharT> m_delemiter{};
		size_t m_pos = npos;

		constexpr size_t find_next( ) const noexcept {
			if( m_str.empty( ) ) {
				return npos;
			}
			return m_str.find( m_delemiter, m_pos );
		}

		constexpr void move_next( ) {
			auto next_pos = find_next( );
			if( next_pos == npos ) {
				m_pos = npos;
				return;
			}
			m_pos = next_pos + m_delemiter.size( );
		}

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::basic_string_view<CharT>;
		using reference = daw::basic_string_view<CharT>;
		using pointer = daw::basic_string_view<CharT>;
		using difference_type = ptrdiff_t;

		constexpr string_split_iterator( ) noexcept = default;

		constexpr string_split_iterator(
		  daw::basic_string_view<CharT> str,
		  daw::basic_string_view<CharT> delemiter ) noexcept
		  : m_str( str )
		  , m_delemiter( delemiter )
		  , m_pos( 0 ) {}

		constexpr string_split_iterator &operator++( ) noexcept {
			move_next( );
			return *this;
		}

		constexpr string_split_iterator operator++( int ) const noexcept {
			string_split_iterator tmp{*this};
			tmp.move_next( );
			return tmp;
		}

		constexpr daw::basic_string_view<CharT> operator*( ) const noexcept {
			auto result{m_str};
			result.remove_prefix( m_pos );
			return result.substr( 0, find_next( ) - m_pos );
		}

		constexpr daw::basic_string_view<CharT> operator->( ) const noexcept {
			auto result{m_str};
			result.remove_prefix( m_pos );
			return result.substr( 0, find_next( ) - m_pos );
		}

		constexpr bool equal_to( string_split_iterator const &rhs ) const noexcept {
			if( m_pos == npos and rhs.m_pos == npos ) {
				return true;
			}
			return std::tie( m_str, m_pos, m_delemiter ) ==
			       std::tie( rhs.m_str, rhs.m_pos, rhs.m_delemiter );
		}
	};

	template<typename CharT>
	string_split_iterator( daw::basic_string_view<CharT>,
	                       daw::basic_string_view<CharT> )
	  ->string_split_iterator<CharT>;

	template<typename CharT>
	constexpr bool
	operator==( string_split_iterator<CharT> const &lhs,
	            string_split_iterator<CharT> const &rhs ) noexcept {
		return lhs.equal_to( rhs );
	}

	template<typename CharT>
	constexpr bool
	operator!=( string_split_iterator<CharT> const &lhs,
	            string_split_iterator<CharT> const &rhs ) noexcept {
		return !lhs.equal_to( rhs );
	}

	template<typename CharT>
	struct string_split_range {
		using iterator = string_split_iterator<CharT>;

	private:
		iterator m_first{};

	public:
		constexpr string_split_range( ) noexcept = default;

		constexpr string_split_range(
		  daw::basic_string_view<CharT> str,
		  daw::basic_string_view<CharT> delemiter ) noexcept
		  : m_first( str, delemiter ) {}

		constexpr iterator begin( ) const {
			return m_first;
		}

		constexpr iterator cbegin( ) const {
			return m_first;
		}

		constexpr iterator end( ) const {
			return iterator{};
		}

		constexpr iterator cend( ) const {
			return iterator{};
		}
	};

	template<typename CharT>
	string_split_range( daw::basic_string_view<CharT>,
	                    daw::basic_string_view<CharT> )
	  ->string_split_range<CharT>;

	template<typename CharT>
	string_split_range<CharT>
	split_string( std::basic_string<CharT> const &str,
	              daw::basic_string_view<CharT> delemiter ) noexcept {

		return {{str.data( ), str.size( )}, delemiter};
	}

	template<typename CharT>
	string_split_range<CharT>
	split_string( std::basic_string<CharT> const &str,
	              std::basic_string<CharT> &delemiter ) noexcept {

		return {{str.data( ), str.size( )}, {delemiter.data( ), delemiter.size( )}};
	}

	template<typename CharT>
	constexpr string_split_range<CharT>
	split_string( daw::basic_string_view<CharT> str,
	              daw::basic_string_view<CharT> delemiter ) noexcept {

		return {str, delemiter};
	}

	template<typename CharT, typename Bounds, std::ptrdiff_t Ex, size_t N>
	constexpr string_split_range<CharT>
	split_string( daw::basic_string_view<CharT, Bounds, Ex> str,
	              CharT const ( &delemiter )[N] ) noexcept {

		return {str, daw::basic_string_view<CharT, Bounds, N - 1>( delemiter )};
	}

	template<typename CharT, typename Bounds = default_string_view_bounds_type,
	         size_t N, size_t M>
	constexpr string_split_range<CharT>
	split_string( CharT const ( &str )[N],
	              CharT const ( &delemiter )[M] ) noexcept {

		return {daw::basic_string_view<CharT, Bounds, N - 1>( str ),
		        daw::basic_string_view<CharT, Bounds, M - 1>( delemiter )};
	}
} // namespace daw
