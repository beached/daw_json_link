// The MIT License (MIT)
//
// Copyright (c) 2016-2020 Darrell Wright
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

#include <stdexcept>
#include <type_traits>

#include "../daw_exception.h"
#include "../daw_move.h"

namespace daw {
	template<typename IteratorFirst, typename IteratorLast>
	class checked_iterator_proxy_t {
		enum flag_t : uint8_t {
			check_increment = 1,
			check_decrement = 2,
			check_dereference = 4
		};
		IteratorFirst current;
		IteratorFirst first;
		IteratorLast last;
		uint8_t flags;

		constexpr bool is_flag_set( flag_t flag ) const noexcept {
			return 0 != ( flags & flag );
		}

		constexpr auto get_flag_value( bool CheckIncrement, bool CheckDecrement,
		                               bool CheckDereference ) const noexcept {
			uint8_t result = CheckIncrement ? check_increment : 0;
			result |= CheckDecrement ? check_decrement : 0;
			result |= CheckDereference ? check_dereference : 0;

			return result;
		}

	public:
		checked_iterator_proxy_t( IteratorFirst it_first, IteratorLast it_last,
		                          bool CheckIncrement = true,
		                          bool CheckDecrement = true,
		                          bool CheckDereference = true )
		  : current{it_first}
		  , first{daw::move( it_first )}
		  , last{daw::move( it_last )}
		  , flags{
		      get_flag_value( CheckIncrement, CheckDecrement, CheckDereference )} {}

		checked_iterator_proxy_t( checked_iterator_proxy_t const & ) = default;
		checked_iterator_proxy_t( checked_iterator_proxy_t && ) = default;
		checked_iterator_proxy_t &
		operator=( checked_iterator_proxy_t const & ) = default;
		checked_iterator_proxy_t &
		operator=( checked_iterator_proxy_t && ) = default;
		checked_iterator_proxy_t( ) = default;
		~checked_iterator_proxy_t( ) = default;

		checked_iterator_proxy_t &operator++( ) {
			if( is_flag_set( check_increment ) and current == last ) {
				daw::exception::daw_throw<std::out_of_range>(
				  "Attempt to increment iterator past end of range" );
			}
			++current;
			return *this;
		}

		checked_iterator_proxy_t operator++( int ) const {
			checked_iterator_proxy_t result{*this};
			++result;
			return result;
		}

		checked_iterator_proxy_t &operator--( ) {
			if( is_flag_set( check_decrement ) and current == first ) {
				throw std::out_of_range(
				  "Attempt to decrement iterator past beginning of range" );
			}
			--current;
			return *this;
		}

		checked_iterator_proxy_t operator--( int ) const {
			checked_iterator_proxy_t result{*this};
			--result;
			return result;
		}

		auto &operator*( ) {
			if( is_flag_set( check_dereference ) and current == last ) {
				throw std::out_of_range(
				  "Attempt to dereference an iterator at end of range" );
			}
			return *current;
		}

		auto const &operator*( ) const {
			if( is_flag_set( check_dereference ) and current == last ) {
				throw std::out_of_range(
				  "Attempt to dereference an iterator at end of range" );
			}
			return *current;
		}

		auto &operator-> ( ) {
			if( is_flag_set( check_dereference ) and current == last ) {
				throw std::out_of_range(
				  "Attempt to access members an iterator at end of range" );
			}
			return current.operator->( );
		}

		auto const &operator-> ( ) const {
			if( is_flag_set( check_dereference ) and current == last ) {
				throw std::out_of_range(
				  "Attempt to access members an iterator at end of range" );
			}
			return current.operator->( );
		}

		template<typename IteratorFirst_lhs, typename IteratorFirst_rhs,
		         typename IteratorLast_lhs, typename IteratorLast_rhs>
		friend bool operator==(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  checked_iterator_proxy_t<IteratorFirst_rhs, IteratorLast_rhs> const
		    &rhs ) {
			return lhs.current == rhs.current;
		}

		template<typename IteratorFirst_lhs, typename IteratorLast_lhs,
		         typename Iterator_rhs>
		friend bool operator==(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  Iterator_rhs const &rhs ) {
			return lhs.current == rhs;
		}

		template<typename IteratorFirst_lhs, typename IteratorFirst_rhs,
		         typename IteratorLast_lhs, typename IteratorLast_rhs>
		friend bool operator!=(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  checked_iterator_proxy_t<IteratorFirst_rhs, IteratorLast_rhs> const
		    &rhs ) {
			return lhs.current != rhs.current;
		}

		template<typename IteratorFirst_lhs, typename IteratorLast_lhs,
		         typename Iterator_rhs>
		friend bool operator!=(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  Iterator_rhs const &rhs ) {
			return lhs.current != rhs;
		}
	}; // checked_iterator_proxy_t

	template<typename IteratorFirst, typename IteratorLast>
	auto make_checked_iterator_proxy( IteratorFirst first, IteratorLast last,
	                                  bool CheckIncrement = true,
	                                  bool CheckDecrement = true,
	                                  bool CheckDereference = true ) {
		return checked_iterator_proxy_t<IteratorFirst, IteratorLast>{
		  first, last, CheckIncrement, CheckDecrement, CheckDereference};
	}
} // namespace daw
