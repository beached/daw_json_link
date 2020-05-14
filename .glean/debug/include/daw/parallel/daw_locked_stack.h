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

#include <mutex>
#include <optional>
#include <vector>

#include "../daw_value_ptr.h"
#include "daw_semaphore.h"

namespace daw {
	template<typename T>
	struct locked_stack_t {
		using value_type = T;

	private:
		daw::semaphore m_semaphore;
		mutable value_ptr<std::mutex> m_mutex;
		std::vector<value_type> m_items;

	public:
		locked_stack_t( )
		  : m_semaphore( )
		  , m_mutex( )
		  , m_items( ) {}

		std::optional<value_type> try_pop_back( ) {
			if( !m_semaphore.try_wait( ) ) {
				return std::nullopt;
			}
			std::lock_guard<std::mutex> lock( *m_mutex );
			if( m_items.empty( ) ) {
				return std::nullopt;
			}
			auto result = m_items.back( );
			m_items.pop_back( );
			return result;
		}

		value_type pop_back( ) {
			m_semaphore.wait( );
			std::lock_guard<std::mutex> lock( *m_mutex );
			if( m_items.empty( ) ) {
				return {};
			}
			auto result = m_items.back( );
			m_items.pop_back( );
			return result;
		}

		value_type pop_back2( ) {
			while( true ) {
				m_semaphore.wait( );
				std::lock_guard<std::mutex> lock( *m_mutex );
				if( !m_items.empty( ) ) {
					auto result = m_items.back( );
					m_items.pop_back( );
					return result;
				}
			}
		}

		template<typename U>
		bool pop( U &result ) {
			m_semaphore.wait( );
			std::lock_guard<std::mutex> lock( *m_mutex );
			if( m_items.empty( ) ) {
				return false;
			}
			result = m_items.back( );
			m_items.pop_back( );
			return true;
		}

		template<typename U>
		void push_back( U &&value ) {
			std::lock_guard<std::mutex> lock( *m_mutex );
			m_items.push_back( std::forward<U>( value ) );
			m_semaphore.notify( );
		}

		template<typename U>
		bool push( U &&value ) {
			push_back( std::forward<U>( value ) );
			return true;
		}

		template<typename... Args>
		void emplace_back( Args &&... args ) {
			std::lock_guard<std::mutex> lock( *m_mutex );
			m_items.emplace_back( std::forward<Args>( args )... );
			m_semaphore.notify( );
		}

		bool empty( ) {
			if( m_semaphore.try_wait( ) ) {
				m_semaphore.notify( );
				return false;
			}
			return true;
		}

		size_t size( ) {
			if( m_semaphore.try_wait( ) ) {
				std::lock_guard<std::mutex> lock( *m_mutex );
				auto result = m_items.size( );
				m_semaphore.notify( );
				return result;
			}
			return 0;
		}

		std::vector<value_type> copy( ) const {
			std::lock_guard<std::mutex> lock( *m_mutex );
			return m_items;
		}
	}; // locked_stack_t

} // namespace daw
