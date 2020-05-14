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

namespace daw {

	template<typename T>
	struct RandomIterator {
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		pointer m_pointer;

	public:
		constexpr RandomIterator( ) noexcept
		  : m_pointer{nullptr} {} // TODO: Is this the correct behaviour

		constexpr RandomIterator( T *const ptr ) noexcept
		  : m_pointer{ptr} {}

		constexpr RandomIterator &operator=( T *const rhs ) noexcept {
			m_pointer = rhs;
			return *this;
		}

		constexpr pointer ptr( ) const noexcept {
			return m_pointer;
		}

		constexpr RandomIterator &operator+=( std::ptrdiff_t n ) {
			m_pointer += n;
			return *this;
		}

		constexpr RandomIterator &operator-=( std::ptrdiff_t n ) {
			m_pointer -= n;
			return *this;
		}

		constexpr reference operator*( ) {
			return *m_pointer;
		}

		constexpr const_reference operator*( ) const {
			return *m_pointer;
		}

		constexpr pointer operator->( ) const noexcept {
			return m_pointer;
		}

		constexpr RandomIterator &operator++( ) noexcept {
			++m_pointer;
			return *this;
		}

		constexpr RandomIterator operator++( int ) noexcept {
			auto result = RandomIterator{*this};
			++m_pointer;
			return result;
		}

		constexpr RandomIterator &operator--( ) noexcept {
			--m_pointer;
			return *this;
		}

		constexpr RandomIterator operator--( int ) noexcept {
			auto result = RandomIterator{*this};
			--m_pointer;
			return result;
		}

		constexpr RandomIterator operator+( std::ptrdiff_t const &n ) noexcept {
			auto old = this->m_ptr;
			this->m_ptr += n;
			auto temp{*this};
			this->m_ptr = old;
			return temp;
		}

		constexpr RandomIterator operator-( std::ptrdiff_t const &n ) noexcept {
			auto old = this->m_ptr;
			this->m_ptr -= n;
			auto temp{*this};
			this->m_ptr = old;
			return temp;
		}

		constexpr friend bool operator==( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer == rhs.m_pointer;
		}

		constexpr friend bool operator!=( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer != rhs.m_pointer;
		}

		constexpr friend bool operator<( RandomIterator const &lhs,
		                                 RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer < rhs.m_pointer;
		}

		constexpr friend bool operator>( RandomIterator const &lhs,
		                                 RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer > rhs.m_pointer;
		}

		constexpr friend bool operator<=( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer <= rhs.m_pointer;
		}

		constexpr friend bool operator>=( RandomIterator const &lhs,
		                                  RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer >= rhs.m_pointer;
		}

		constexpr friend std::ptrdiff_t
		operator-( RandomIterator const &lhs, RandomIterator const &rhs ) noexcept {
			return lhs.m_pointer - rhs.m_pointer;
		}
	}; // RandomIterator

	template<typename T>
	constexpr auto make_random_iterator( T *const ptr ) noexcept {
		return RandomIterator<T>{ptr};
	}

	template<typename T>
	constexpr auto make_const_random_iterator( T *const ptr ) noexcept {
		return RandomIterator<T const>{ptr};
	}

} // namespace daw
