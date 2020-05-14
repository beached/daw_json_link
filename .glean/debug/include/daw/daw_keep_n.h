// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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
#include <functional>
#include <limits>
#include <type_traits>

#include "daw_move.h"

namespace daw {
	enum class keep_n_order { ascending, descending };
	namespace keep_n_impl {
		template<keep_n_order Order, typename Function>
		struct keep_n_pred {
			Function func = {};

			constexpr keep_n_pred( ) noexcept(
			  std::is_nothrow_constructible_v<Function> ) = default;
			constexpr keep_n_pred( Function const &f ) noexcept(
			  std::is_nothrow_copy_constructible_v<Function> )
			  : func( f ) {}
			constexpr keep_n_pred( Function &&f ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : func( daw::move( f ) ) {}

			template<typename... Args>
			constexpr decltype( auto ) operator( )( Args &&... args ) {
				if constexpr( Order == keep_n_order::ascending ) {
					return func( std::forward<Args>( args )... );
				} else {
					return !func( std::forward<Args>( args )... );
				}
			}

			template<typename... Args>
			constexpr decltype( auto ) operator( )( Args &&... args ) const {
				if constexpr( Order == keep_n_order::ascending ) {
					return func( std::forward<Args>( args )... );
				} else {
					return !func( std::forward<Args>( args )... );
				}
			}
		};
	} // namespace keep_n_impl

	template<typename T, size_t MaxItems,
	         keep_n_order Order = keep_n_order::ascending,
	         typename Predicate = std::less<>>
	class keep_n {
		using values_type = std::array<T, MaxItems>;
		static constexpr auto const m_pred =
		  keep_n_impl::keep_n_pred<Order, Predicate>{};

	public:
		using value_type = typename values_type::value_type;
		using difference_type = typename values_type::difference_type;
		using size_type = typename values_type::size_type;
		using reference = typename values_type::reference;
		using const_reference = typename values_type::const_reference;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;

	private:
		values_type m_values{};

	public:
		constexpr keep_n( value_type const &default_value ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> ) {
			for( auto &v : m_values ) {
				v = default_value;
			}
		}

		constexpr void insert( value_type const &v ) {
			for( size_t n = 0; n < MaxItems; ++n ) {
				if( m_pred( v, m_values[n] ) ) {
					for( size_t m = ( MaxItems - 1U ); m > n; --m ) {
						m_values[m] = daw::move( m_values[m - 1] );
					}
					m_values[n] = v;
					break;
				}
			}
		}

		constexpr void insert( value_type &&v ) {
			for( size_t n = 0; n < MaxItems; ++n ) {
				if( m_pred( v, m_values[n] ) ) {
					for( size_t m = MaxItems - 1; m > n; --m ) {
						m_values[m] = daw::move( m_values[m - 1] );
					}
					m_values[n] = daw::move( v );
					break;
				}
			}
		}

		constexpr const_reference operator[]( size_type index ) const noexcept {
			return m_values[index];
		}

		constexpr const_iterator begin( ) const noexcept {
			return m_values.begin( );
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return m_values.cbegin( );
		}

		constexpr const_iterator end( ) const noexcept {
			return m_values.end( );
		}

		constexpr const_iterator cend( ) const noexcept {
			return m_values.cend( );
		}

		constexpr size_type size( ) const noexcept {
			return MaxItems;
		}

		constexpr size_type capacity( ) const noexcept {
			return MaxItems;
		}

		constexpr const_reference front( ) const noexcept {
			return m_values.front( );
		}

		constexpr const_reference back( ) const noexcept {
			return m_values.back( );
		}
	};
} // namespace daw
