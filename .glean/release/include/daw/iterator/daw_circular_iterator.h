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

#include <iterator>

#include "../daw_algorithm.h"
#include "../daw_move.h"
#include "../daw_traits.h"

namespace daw {
	namespace impl {
		template<typename C>
		using has_size_member = decltype( std::declval<C>( ).size( ) );

		template<typename Container,
		         std::enable_if_t<is_detected_v<has_size_member, Container>,
		                          std::nullptr_t> = nullptr>
		constexpr size_t container_size( Container &c ) {
			return c.size( );
		}

		template<typename Container,
		         std::enable_if_t<!is_detected_v<has_size_member, Container>,
		                          std::nullptr_t> = nullptr>
		constexpr size_t container_size( Container &c ) {
			return static_cast<size_t>(
			  daw::distance( std::cbegin( c ), std::cend( c ) ) );
		}
	} // namespace impl

	template<typename Container>
	struct circular_iterator {
		using iterator = decltype( std::begin( std::declval<Container &>( ) ) );
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using value_type = typename std::iterator_traits<iterator>::value_type;
		using iterator_category = std::random_access_iterator_tag;
		using reference = typename std::iterator_traits<iterator>::reference;

	private:
		static_assert(
		  std::is_same_v<
		    std::random_access_iterator_tag,
		    typename std::iterator_traits<iterator>::iterator_category>,
		  "Container iterators must be randomly accessable" );

		Container *m_container;
		difference_type m_position;

		constexpr difference_type get_offset( difference_type n ) const noexcept {
			auto const sz =
			  static_cast<difference_type>( impl::container_size( *m_container ) );
			n += m_position;
			if( n < 0 ) {
				n *= -1;
				n %= sz;
				return sz - n;
			} else if( n >= sz ) {
				return n % sz;
			}
			return n;
		}

		constexpr decltype( auto ) get_iterator( difference_type n ) const {
			auto result = std::next( std::begin( *m_container ), n );
			return result;
		}

	public:
		constexpr circular_iterator( ) noexcept
		  : m_container{nullptr}
		  , m_position{0} {}

		constexpr circular_iterator( Container &container ) noexcept
		  : m_container{&container}
		  , m_position{0} {}

		constexpr circular_iterator( Container *container ) noexcept
		  : m_container{container}
		  , m_position{0} {}

		constexpr circular_iterator( Container &container, iterator i ) noexcept
		  : m_container{&container}
		  , m_position{daw::distance( std::begin( container ), daw::move( i ) )} {}

		constexpr circular_iterator( Container *container, iterator i ) noexcept
		  : m_container{container}
		  , m_position{daw::distance( std::begin( *container ), daw::move( i ) )} {}

		constexpr circular_iterator( circular_iterator const &other ) noexcept
		  : m_container{other.m_container}
		  , m_position{other.m_position} {}

		constexpr circular_iterator( circular_iterator &&other ) noexcept
		  : m_container{other.m_container}
		  , m_position{other.m_position} {}

		constexpr circular_iterator &
		operator=( circular_iterator const &rhs ) noexcept {
			if( this != &rhs ) {
				m_container = rhs.m_container;
				m_position = rhs.m_position;
			}
			return *this;
		}

		constexpr circular_iterator &operator=( circular_iterator &&rhs ) noexcept {
			m_container = rhs.m_container;
			m_position = rhs.m_position;
			return *this;
		}

		~circular_iterator( ) noexcept = default;

		constexpr circular_iterator &operator+=( difference_type n ) noexcept {
			m_position = get_offset( n );
			return *this;
		}

		constexpr circular_iterator &operator-=( difference_type n ) {
			m_position = get_offset( -n );
			return *this;
		}

		constexpr decltype( auto ) operator*( ) {
			return *get_iterator( m_position );
		}

		constexpr decltype( auto ) operator-> ( ) noexcept {
			return *get_iterator( m_position );
		}

		constexpr circular_iterator &operator++( ) noexcept {
			m_position = get_offset( 1 );
			return *this;
		}

		constexpr circular_iterator operator++( int ) noexcept {
			auto result = circular_iterator{*this};
			m_position = get_offset( 1 );
			return result;
		}

		constexpr circular_iterator &operator--( ) noexcept {
			m_position = get_offset( -1 );
			return *this;
		}

		constexpr circular_iterator operator--( int ) noexcept {
			auto result = circular_iterator{*this};
			m_position = get_offset( -1 );
			return result;
		}

		constexpr circular_iterator operator+( difference_type n ) noexcept {
			circular_iterator tmp{*this};
			tmp += n;
			return tmp;
		}

		constexpr circular_iterator operator-( difference_type n ) noexcept {
			circular_iterator tmp{*this};
			tmp -= n;
			return tmp;
		}

		constexpr circular_iterator end( ) noexcept {
			circular_iterator tmp{*this};
			tmp.m_position =
			  static_cast<difference_type>( impl::container_size( *m_container ) );
			return tmp;
		}
		constexpr friend bool operator==( circular_iterator const &lhs,
		                                  circular_iterator const &rhs ) noexcept {
			return lhs.m_position == rhs.m_position &&
			       lhs.m_container == rhs.m_container;
		}

		constexpr friend bool operator!=( circular_iterator const &lhs,
		                                  circular_iterator const &rhs ) noexcept {
			return lhs.m_position != rhs.m_position or
			       lhs.m_container != rhs.m_container;
		}

		constexpr friend bool operator<( circular_iterator const &lhs,
		                                 circular_iterator const &rhs ) noexcept {
			daw::exception::DebugAssert( lhs.m_container == rhs.m_container,
			                             "Attempt to compare difference containers" );
			return lhs.m_position < rhs.m_position;
		}

		constexpr friend bool operator>( circular_iterator const &lhs,
		                                 circular_iterator const &rhs ) noexcept {
			daw::exception::DebugAssert( lhs.m_container == rhs.m_container,
			                             "Attempt to compare difference containers" );
			return lhs.m_position > rhs.m_position;
		}

		constexpr friend bool operator<=( circular_iterator const &lhs,
		                                  circular_iterator const &rhs ) noexcept {
			daw::exception::DebugAssert( lhs.m_container == rhs.m_container,
			                             "Attempt to compare difference containers" );
			return lhs.m_position <= rhs.m_position;
		}

		constexpr friend bool operator>=( circular_iterator const &lhs,
		                                  circular_iterator const &rhs ) noexcept {
			daw::exception::DebugAssert( lhs.m_container == rhs.m_container,
			                             "Attempt to compare difference containers" );
			return lhs.m_position >= rhs.m_position;
		}

		constexpr friend difference_type operator-( circular_iterator const &lhs,
		                                            circular_iterator const &rhs ) {
			daw::exception::DebugAssert(
			  lhs.m_container == rhs.m_container,
			  "Can only find difference of iterators from same container" );
			return lhs.m_position - rhs.m_position;
		}
	}; // circular_iterator

	template<typename Container>
	constexpr auto make_circular_iterator( Container &container ) noexcept {
		return circular_iterator<Container>{container};
	}

	template<typename Container, typename Iterator>
	constexpr auto make_circular_iterator( Container &container,
	                                       Iterator it ) noexcept {
		return circular_iterator<Container>{container, daw::move( it )};
	}
} // namespace daw
