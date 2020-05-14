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

#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>

#include "../cpp_17.h"
#include "../daw_exception.h"
#include "../daw_move.h"
#include "daw_condition_variable.h"

namespace daw {
	template<typename>
	struct is_counter : std::false_type {};

	template<typename T>
	inline constexpr bool is_counter_v =
	  is_counter<daw::remove_cvref_t<T>>::value;

	template<typename>
	struct is_unique_counter : std::false_type {};

	template<typename T>
	inline constexpr bool is_unique_counter_v =
	  is_unique_counter<daw::remove_cvref_t<T>>::value;

	template<typename>
	struct is_shared_counter : std::false_type {};

	template<typename T>
	inline constexpr bool is_shared_counter_v =
	  is_shared_counter<daw::remove_cvref_t<T>>::value;

	template<typename Mutex, typename ConditionVariable>
	class basic_counter {
		mutable daw::basic_condition_variable<Mutex, ConditionVariable>
		  m_condition = daw::basic_condition_variable<Mutex, ConditionVariable>( );
		std::atomic_intmax_t m_count = 0;

		[[nodiscard]] auto stop_waiting( ) const {
			return [&]( ) -> bool { return static_cast<intmax_t>( m_count ) <= 0; };
		}

	public:
		basic_counter( ) = default;

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		explicit basic_counter( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : m_count( static_cast<intmax_t>( count ) ) {

			assert( count >= 0 );
		}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		basic_counter( Integer count, bool countered ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : m_count( static_cast<intmax_t>( count ) ) {

			assert( count >= 0 );
		}

		void decrement( ) {
			--m_count;
		}

		void increment( ) {
			++m_count;
		}

		void reset( ) {
			m_count = 0;
		}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		void reset( Integer count ) {
			assert( count >= 0 );

			m_count = static_cast<intmax_t>( count );
		}

		void notify( ) {
			decrement( );
			m_condition.notify_all( );
		}

		void notify_one( ) {
			decrement( );
			m_condition.notify_one( );
		}

		void wait( ) const {
			// Try a spin before we use the heavy guns
			for( size_t n = 0; n < 100; ++n ) {
				if( try_wait( ) ) {
					return;
				}
			}
			m_condition.wait( stop_waiting( ) );
		}

		[[nodiscard]] bool try_wait( ) const {
			return stop_waiting( )( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return m_condition.wait_for( rel_time, stop_waiting( ) );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto ) wait_until(
		  std::chrono::time_point<Clock, Duration> const &timeout_time ) const {
			return m_condition.wait_until( timeout_time, stop_waiting( ) );
		}
	}; // basic_counter

	template<typename Mutex, typename ConditionVariable>
	struct is_counter<basic_counter<Mutex, ConditionVariable>> : std::true_type {
	};

	using counter = basic_counter<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_unique_counter {
		using counter_t = basic_counter<Mutex, ConditionVariable>;
		std::unique_ptr<counter_t> counter = std::make_unique<counter_t>( );

	public:
		basic_unique_counter( ) = default;

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		explicit basic_unique_counter( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : counter( std::make_unique<counter_t>( count ) ) {

			assert( count >= 0 );
		}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		basic_unique_counter( Integer count, bool countered ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : counter( std::make_unique<counter_t>( count, countered ) ) {

			assert( count >= 0 );
		}

		basic_counter<Mutex, ConditionVariable> *release( ) {
			return counter.release( );
		}

		void notify( ) {
			assert( counter );
			counter->notify( );
		}

		void add_notifier( ) {
			assert( counter );
			counter->add_notifier( );
		}

		void set_counter( ) {
			assert( counter );
			counter->set_counter( );
		}

		void wait( ) const {
			assert( counter );
			counter->wait( );
		}

		[[nodiscard]] bool try_wait( ) const {
			assert( counter );
			return counter->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) const {
			assert( counter );
			return counter->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto ) wait_until(
		  std::chrono::time_point<Clock, Duration> const &timeout_time ) const {
			assert( counter );
			return counter->wait_until( timeout_time );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( counter );
		}
	}; // basic_unique_counter

	template<typename Mutex, typename ConditionVariable>
	struct is_unique_counter<basic_unique_counter<Mutex, ConditionVariable>>
	  : std::true_type {};

	using unique_counter =
	  basic_unique_counter<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_shared_counter {
		using counter_t = basic_counter<Mutex, ConditionVariable>;
		std::shared_ptr<counter_t> counter = std::make_shared<counter_t>( );

	public:
		basic_shared_counter( ) noexcept(
		  noexcept( std::make_shared<counter_t>( ) ) ) = default;

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		explicit basic_shared_counter( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : counter( std::make_shared<counter_t>( count ) ) {

			assert( count >= 0 );
		}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		basic_shared_counter( Integer count, bool countered ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : counter( std::make_shared<counter_t>( count, countered ) ) {

			assert( count >= 0 );
		}

		explicit basic_shared_counter(
		  basic_unique_counter<Mutex, ConditionVariable> &&other ) noexcept
		  : counter( other.release( ) ) {}

		void notify( ) {
			assert( counter );
			counter->notify( );
		}

		void add_notifier( ) {
			assert( counter );
			counter->add_notifier( );
		}

		void set_counter( ) {
			assert( counter );
			counter->set_counter( );
		}

		void wait( ) const {
			assert( counter );
			counter->wait( );
		}

		[[nodiscard]] bool try_wait( ) const {
			assert( counter );
			return counter->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) const {
			assert( counter );
			return counter->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto ) wait_until(
		  std::chrono::time_point<Clock, Duration> const &timeout_time ) const {
			assert( counter );
			return counter->wait_until( timeout_time );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( counter );
		}

		void increment( ) {
			counter->increment( );
		}

		void decrement( ) {
			counter->decrement( );
		}
	}; // basic_shared_counter

	template<typename Mutex, typename ConditionVariable>
	struct is_shared_counter<basic_shared_counter<Mutex, ConditionVariable>>
	  : std::true_type {};

	using shared_counter =
	  basic_shared_counter<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void wait_all( std::initializer_list<basic_counter<Mutex, ConditionVariable>>
	                 semaphores ) {
		for( auto &sem : semaphores ) {
			sem->wait( );
		}
	}
} // namespace daw
