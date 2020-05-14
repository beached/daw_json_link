// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include <iostream>
#include <string>
#include <utf8/unchecked.h>

#include <daw/cpp_17.h>
#include <daw/daw_algorithm.h>
#include <daw/daw_fnv1a_hash.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

namespace daw {
	namespace range {
		using char_iterator = char const *;
		using utf_iterator = utf8::unchecked::iterator<char_iterator>;
		using utf_val_type = utf_iterator::value_type;

		constexpr size_t hash_sequence( char_iterator first,
		                                char_iterator const last ) noexcept {
			return daw::fnv1a_hash(
			  first, static_cast<size_t>( daw::distance( first, last ) ) );
		}

		struct utf_range {
			using iterator = utf_iterator;
			using const_iterator = utf_iterator const;
			using reference = utf_iterator::reference;
			using value_type = utf_iterator::value_type;
			using const_reference = value_type const &;
			using difference_type = utf_iterator::difference_type;

		private:
			iterator m_begin{};
			iterator m_end{};
			size_t m_size = 0;

		public:
			constexpr utf_range( ) noexcept = default;

			constexpr utf_range( iterator Begin, iterator End ) noexcept(
			  std::is_nothrow_copy_constructible_v<iterator> )
			  : m_begin( Begin )
			  , m_end( End )
			  , m_size( static_cast<size_t>( daw::distance( Begin, End ) ) ) {}

			constexpr iterator
			begin( ) noexcept( std::is_nothrow_copy_constructible_v<iterator> ) {
				return m_begin;
			}

			constexpr const_iterator begin( ) const
			  noexcept( std::is_nothrow_copy_constructible_v<iterator> ) {
				return m_begin;
			}

			constexpr iterator
			end( ) noexcept( std::is_nothrow_copy_constructible_v<iterator> ) {
				return m_end;
			}

			constexpr const_iterator end( ) const noexcept {
				return m_end;
			}

			constexpr size_t size( ) const
			  noexcept( std::is_nothrow_copy_constructible_v<iterator> ) {
				return m_size;
			}

			constexpr bool empty( ) const
			  noexcept( std::is_nothrow_copy_constructible_v<iterator> ) {
				return m_size == 0;
			}

			constexpr utf_range &operator++( ) noexcept {
				++m_begin;
				--m_size;
				return *this;
			}

			constexpr utf_range operator++( int ) noexcept {
				utf_range result( *this );
				++( *this );
				return result;
			}

			constexpr void advance( size_t const n ) noexcept {
				assert( n <= m_size );
				utf8::unchecked::advance( m_begin, n );
				m_size -= n;
			}

			constexpr void safe_advance( size_t const count ) noexcept {
				if( count <= m_size ) {
					advance( count );
				} else {
					advance( m_size );
				}
			}

			constexpr utf_range &set( iterator Begin, iterator End,
			                          difference_type Size = -1 ) noexcept {
				m_begin = Begin;
				m_end = End;
				if( Size < 0 ) {
					m_size = static_cast<size_t>( daw::distance( m_begin, m_end ) );
				} else {
					m_size = static_cast<size_t>( Size );
				}
				return *this;
			}

			constexpr utf_range &set_begin( iterator Begin,
			                                difference_type Size = -1 ) noexcept {
				return set( Begin, this->m_end, Size );
			}

			constexpr utf_range &set_end( iterator End,
			                              difference_type Size = -1 ) noexcept {
				return set( this->m_begin, End, Size );
			}

			constexpr utf_range &operator+=( size_t const n ) noexcept {
				advance( n );
				return *this;
			}

			constexpr char_iterator raw_begin( ) const noexcept {
				return m_begin.base( );
			}

			constexpr char_iterator raw_end( ) const noexcept {
				return m_end.base( );
			}

			constexpr size_t raw_size( ) const noexcept {
				return static_cast<size_t>(
				  daw::distance( m_begin.base( ), m_end.base( ) ) );
			}

			constexpr utf_range copy( ) const noexcept {
				return *this;
			}

			constexpr utf_range substr( size_t pos, size_t length ) const noexcept {
				assert( pos + length <= size( ) );
				auto result = copy( );
				auto f = result.begin( ) + static_cast<difference_type>( pos );
				auto l = f + static_cast<difference_type>( length );
				result.set( f, l );
				return result;
			}

			std::u32string to_u32string( ) const noexcept;
			std::string to_raw_u8string( ) const noexcept;

			constexpr int compare( utf_range const &rhs ) const noexcept {
				auto it_lhs = begin( );
				auto it_rhs = rhs.begin( );
				enum compare_results_t {
					less_than = -1,
					equal_to = 0,
					greater_than = 1
				};

				while( it_lhs != end( ) && it_rhs != rhs.end( ) ) {
					auto const l = *it_lhs;
					auto const r = *it_rhs;

					if( l != r ) {
						if( l < r ) {
							return less_than;
						}
						return greater_than;
					}
					++it_lhs;
					++it_rhs;
				}
				if( it_lhs != end( ) ) {
					return greater_than;
				} else if( it_rhs != end( ) ) {
					return less_than;
				}
				return equal_to;
			}

			constexpr daw::string_view to_string_view( ) const noexcept {
				return daw::make_string_view_it( raw_begin( ), raw_end( ) );
			}
		}; // struct utf_range

		constexpr utf_range operator+( utf_range range, size_t const n ) noexcept {
			range.advance( n );
			return range;
		}

		constexpr utf_range create_char_range( utf_iterator first,
		                                       utf_iterator last ) noexcept {
			return {first, last};
		}

		constexpr utf_range create_char_range( daw::string_view str ) noexcept {
			auto it_begin = utf_iterator( str.begin( ) );
			auto it_end = utf_iterator( str.end( ) );
			return {it_begin, it_end};
		}

		template<size_t N>
		constexpr utf_range create_char_range( char const ( &str )[N] ) noexcept {
			return {str, N - 1};
		}

		constexpr utf_range create_char_range( char_iterator first,
		                                       char_iterator last ) noexcept {
			auto it_begin = utf_iterator( first );
			auto it_end = utf_iterator( last );
			return {it_begin, it_end};
		}

		constexpr utf_range create_char_range( char_iterator first ) noexcept {
			auto sv = daw::string_view( first );
			return create_char_range( sv );
		}

		constexpr bool operator==( utf_range const &lhs,
		                           utf_range const &rhs ) noexcept {
			return lhs.compare( rhs ) == 0;
		}

		constexpr bool operator==( utf_range const &lhs,
		                           daw::string_view const &rhs ) noexcept {
			return lhs == create_char_range( rhs );
		}

		constexpr bool operator!=( utf_range const &lhs,
		                           utf_range const &rhs ) noexcept {
			return lhs.compare( rhs ) != 0;
		}

		constexpr bool operator<( utf_range const &lhs,
		                          utf_range const &rhs ) noexcept {
			return lhs.compare( rhs ) < 0;
		}

		constexpr bool operator>( utf_range const &lhs,
		                          utf_range const &rhs ) noexcept {
			return lhs.compare( rhs ) > 0;
		}

		constexpr bool operator<=( utf_range const &lhs,
		                           utf_range const &rhs ) noexcept {
			return lhs.compare( rhs ) <= 0;
		}

		constexpr bool operator>=( utf_range const &lhs,
		                           utf_range const &rhs ) noexcept {
			return lhs.compare( rhs ) >= 0;
		}

		constexpr void clear( utf_range &str ) noexcept {
			str.advance( str.size( ) );
		}

		std::string to_string( utf_range const &str );

		template<typename OStream,
		         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, char>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, utf_range const &value ) {
			for( auto it = value.begin( ).base( ); it != value.end( ).base( );
			     ++it ) {
				os << *it;
			}
			return os;
		}

		constexpr daw::string_view to_string_view( utf_range const &str ) noexcept {
			return str.to_string_view( );
		}

		constexpr bool at_end( utf_range const &range ) noexcept {
			return range.size( ) == 0;
		}

		std::u32string to_u32string( utf_iterator first, utf_iterator last );

	} // namespace range

	std::string from_u32string( std::u32string const &other );
} // namespace daw

namespace std {
	template<>
	struct hash<daw::range::utf_range> {
		constexpr size_t operator( )( daw::range::utf_range const &value ) const
		  noexcept {
			return daw::range::hash_sequence( value.begin( ).base( ),
			                                  value.end( ).base( ) );
		}
	};
} // namespace std
