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

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>

#include "../cpp_17.h"
#include "../daw_move.h"
#include "../daw_value_ptr.h"

namespace daw {
	template<typename>
	struct is_semaphore : std::false_type {};

	template<typename T>
	inline constexpr bool is_semaphore_v =
	  is_semaphore<daw::remove_cvref_t<T>>::value;

	template<typename>
	struct is_shared_semaphore : std::false_type {};

	template<typename T>
	inline constexpr bool is_shared_semaphore_v =
	  is_shared_semaphore<daw::remove_cvref_t<T>>::value;

	template<typename Mutex, typename ConditionVariable>
	class basic_semaphore {
		value_ptr<Mutex> m_mutex = value_ptr<Mutex>( );
		value_ptr<ConditionVariable> m_condition = value_ptr<ConditionVariable>( );
		intmax_t m_count = 0;
		bool m_latched = true;

	public:
		basic_semaphore( ) = default;

		template<typename Int>
		explicit basic_semaphore( Int count )
		  : m_count( static_cast<intmax_t>( count ) )
		  , m_latched( true ) {}

		template<typename Int>
		basic_semaphore( Int count, bool latched )
		  : m_count( static_cast<intmax_t>( count ) )
		  , m_latched( latched ) {}

		void notify( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			++m_count;
			if( m_latched ) {
				m_condition->notify_one( );
			}
		}

		void add_notifier( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			--m_count;
		}

		void set_latch( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			m_latched = true;
			m_condition->notify_one( );
		}

		void wait( ) {
			for( size_t n = 0; n < 100; ++n ) {
				if( try_wait( ) ) {
					return;
				}
			}
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			m_condition->wait( lock, [&]( ) { return m_latched and m_count > 0; } );
			--m_count;
		}

		[[nodiscard]] bool try_wait( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			if( m_latched and m_count > 0 ) {
				--m_count;
				return true;
			}
			return false;
		}

		template<typename Rep, typename Period>
		[[nodiscard]] auto
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			auto status = m_condition->wait_for(
			  lock, rel_time, [&]( ) { return m_latched and m_count > 0; } );
			if( status ) {
				--m_count;
			}
			return status;
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] auto
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			auto status = m_condition->wait_until(
			  lock, timeout_time, [&]( ) { return m_latched and m_count > 0; } );
			if( status ) {
				--m_count;
			}
			return status;
		}
	}; // basic_semaphore

	template<typename Mutex, typename ConditionVariable>
	struct is_semaphore<basic_semaphore<Mutex, ConditionVariable>>
	  : std::true_type {};

	using semaphore = basic_semaphore<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_shared_semaphore {
		std::shared_ptr<basic_semaphore<Mutex, ConditionVariable>> m_semaphore;

	public:
		basic_shared_semaphore( )
		  : m_semaphore(
		      std::make_shared<basic_semaphore<Mutex, ConditionVariable>>( ) ) {}

		template<typename Int>
		explicit basic_shared_semaphore( Int count )
		  : m_semaphore(
		      std::make_shared<basic_semaphore<Mutex, ConditionVariable>>(
		        count ) ) {}

		template<typename Int>
		explicit basic_shared_semaphore( Int count, bool latched )
		  : m_semaphore(
		      std::make_shared<basic_semaphore<Mutex, ConditionVariable>>(
		        count, latched ) ) {}

		explicit basic_shared_semaphore(
		  basic_semaphore<Mutex, ConditionVariable> &&sem )
		  : m_semaphore(
		      std::make_shared<basic_semaphore<Mutex, ConditionVariable>>(
		        daw::move( sem ) ) ) {}

		void notify( ) {
			m_semaphore->notify( );
		}

		void add_notifier( ) {
			m_semaphore->add_notifier( );
		}

		void set_latch( ) {
			m_semaphore->set_latch( );
		}

		void wait( ) {
			m_semaphore->wait( );
		}

		[[nodiscard]] bool try_wait( ) {
			return m_semaphore->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return m_semaphore->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto )
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			return m_semaphore->wait_until( timeout_time );
		}
	}; // basic_shared_semaphore

	template<typename Mutex, typename ConditionVariable>
	struct is_shared_semaphore<basic_shared_semaphore<Mutex, ConditionVariable>>
	  : std::true_type {};

	using shared_semaphore =
	  basic_shared_semaphore<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void
	wait_all( std::initializer_list<basic_semaphore<Mutex, ConditionVariable>>
	            semaphores ) {
		for( auto &sem : semaphores ) {
			sem->wait( );
		}
	}

	template<typename T>
	struct waitable_value {
		shared_semaphore semaphore;
		T value;

		waitable_value( shared_semaphore sem, T val )
		  : semaphore( daw::move( sem ) )
		  , value( daw::move( val ) ) {}

		void wait( ) {
			semaphore.wait( );
		}
	};

	template<typename T>
	[[nodiscard]] auto make_waitable_value( shared_semaphore sem, T value ) {
		return waitable_value<T>{daw::move( sem ), daw::move( value )};
	}
} // namespace daw
