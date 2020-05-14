// MIT License
//
// Copyright (c) 2018-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a cop
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, cop, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once

#include <memory>

/// This abstracts a mutex but allows for the owning class to be copied.
namespace daw {
	template<typename Mutex>
	class alignas( alignof( Mutex ) ) copiable_mutex {
		std::unique_ptr<Mutex> m_mutex;

	public:
		copiable_mutex( ) noexcept
		  : m_mutex( std::make_unique<Mutex>( ) ) {}

		copiable_mutex( copiable_mutex const & ) noexcept
		  : m_mutex( std::make_unique<Mutex>( ) ) {}

		copiable_mutex &operator=( copiable_mutex const & ) {
			// The caller should ensure locks are held if needed
			// m_mutex.reset( std::make_unique<Mutex>( ) );
			return *this;
		}

		copiable_mutex( copiable_mutex && ) noexcept = default;
		copiable_mutex &operator=( copiable_mutex && ) noexcept = default;
		~copiable_mutex( ) = default;

		void lock( ) {
			m_mutex->lock( );
		}

		bool try_lock( ) {
			return m_mutex->try_lock( );
		}

		void unlock( ) {
			return m_mutex->unlock( );
		}

		decltype( auto ) native_handle( ) {
			return m_mutex->native_handle( );
		}

		operator Mutex const &( ) const noexcept {
			return *m_mutex;
		}

		operator Mutex &( ) noexcept {
			return *m_mutex;
		}
	};
} // namespace daw
