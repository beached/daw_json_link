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

#include "../cpp_17.h"
#include "../daw_value_ptr.h"
#include "daw_unique_mutex.h"

#include <mutex>
#include <optional>
#include <utility>

namespace daw {
	template<typename T, typename Mutex = std::mutex>
	class lockable_value_t {
		class locked_value_t {
			std::unique_lock<Mutex> m_lock;
			std::reference_wrapper<T> m_value;

			locked_value_t( std::unique_lock<Mutex> &&lck, T &value )
			  : m_lock( std::move( lck ) )
			  , m_value( value ) {}

			friend lockable_value_t;

		public:
			using value_type = T;
			using reference = T &;
			using const_reference = std::add_const_t<T> &;
			using pointer = T *;
			using const_pointer = std::add_const_t<T> const *;

			locked_value_t( locked_value_t const &other ) = delete;
			locked_value_t &operator=( locked_value_t const &other ) = delete;

			locked_value_t( locked_value_t &&other ) noexcept = default;
			locked_value_t &operator=( locked_value_t && ) noexcept = default;

			void release( ) noexcept {
				m_lock.unlock( );
			}

			reference get( ) noexcept {
				return m_value.get( );
			}

			const_reference get( ) const noexcept {
				return m_value.get( );
			}

			reference operator*( ) noexcept {
				return get( );
			}

			const_reference operator*( ) const noexcept {
				return get( );
			}

			pointer operator->( ) noexcept {
				return &m_value.get( );
			}

			const_pointer operator->( ) const noexcept {
				return &m_value.get( );
			}
		}; // locked_value_t

		class const_locked_value_t {
			std::unique_lock<Mutex> m_lock;
			std::reference_wrapper<std::add_const_t<T>> m_value;

			const_locked_value_t( std::unique_lock<Mutex> &&lck,
			                      std::add_const_t<T> &value )
			  : m_lock( std::move( lck ) )
			  , m_value( value ) {}

			friend lockable_value_t;

		public:
			using value_type = std::add_const_t<T>;
			using reference = std::add_const_t<T> &;
			using const_reference = std::add_const_t<T> &;
			using pointer = std::add_const_t<T> *;
			using const_pointer = std::add_const_t<T> *;

			const_locked_value_t( const_locked_value_t const &other ) = delete;
			const_locked_value_t &
			operator=( const_locked_value_t const &other ) = delete;

			const_locked_value_t( const_locked_value_t &&other ) noexcept = default;
			const_locked_value_t &
			operator=( const_locked_value_t && ) noexcept = default;

			void release( ) noexcept {
				m_lock.unlock( );
			}

			const_reference get( ) const noexcept {
				return m_value.get( );
			}

			const_reference operator*( ) const noexcept {
				return get( );
			}

			const_pointer operator->( ) const noexcept {
				return &m_value.get( );
			}
		}; // locked_value_t

		mutable daw::basic_unique_mutex<Mutex> m_mutex =
		  daw::basic_unique_mutex<Mutex>( );
		daw::value_ptr<T> m_value = daw::value_ptr<T>( );

	public:
		lockable_value_t( ) = default;

		template<typename U,
		         std::enable_if_t<
		           not std::is_same_v<lockable_value_t, daw::remove_cvref_t<U>>,
		           std::nullptr_t> = nullptr>
		explicit lockable_value_t( U &&value ) noexcept(
		  noexcept( daw::value_ptr<T>( std::forward<U>( value ) ) ) )
		  : m_value( std::forward<U>( value ) ) {}

		locked_value_t get( ) {
			return {std::unique_lock<Mutex>( m_mutex.get( ) ), *m_value};
		}

		const_locked_value_t get( ) const {
			return {std::unique_lock<Mutex>( m_mutex.get( ) ), *m_value};
		}

		std::optional<locked_value_t> try_get( ) {
			auto lck = std::unique_lock<Mutex>( m_mutex.get( ), std::try_to_lock );
			if( not lck.owns_lock( ) ) {
				return {};
			}
			return {std::move( lck ), *m_value};
		}

		std::optional<const_locked_value_t> try_get( ) const {
			auto lck = std::unique_lock<Mutex>( m_mutex.get( ), std::try_to_lock );
			if( not lck.owns_lock( ) ) {
				return {};
			}
			return {std::move( lck ), *m_value};
		}

		locked_value_t operator*( ) {
			return get( );
		}

		const_locked_value_t operator*( ) const {
			return get( );
		}
	}; // lockable_value_t
} // namespace daw
