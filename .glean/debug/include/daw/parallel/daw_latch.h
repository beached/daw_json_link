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
	struct is_latch : std::false_type {};

	template<typename T>
	inline constexpr bool is_latch_v = is_latch<daw::remove_cvref_t<T>>::value;

	template<typename>
	struct is_unique_latch : std::false_type {};

	template<typename T>
	inline constexpr bool is_unique_latch_v =
	  is_unique_latch<daw::remove_cvref_t<T>>::value;

	template<typename>
	struct is_shared_latch : std::false_type {};

	template<typename T>
	inline constexpr bool is_shared_latch_v =
	  is_shared_latch<daw::remove_cvref_t<T>>::value;

	template<typename Mutex, typename ConditionVariable>
	class basic_latch {
		mutable daw::basic_condition_variable<Mutex, ConditionVariable>
		  m_condition = daw::basic_condition_variable<Mutex, ConditionVariable>( );
		std::atomic_intmax_t m_count = 1;

		[[nodiscard]] auto stop_waiting( ) const {
			return [&]( ) -> bool { return static_cast<intmax_t>( m_count ) <= 0; };
		}

		void decrement( ) {
			--m_count;
		}

	public:
		basic_latch( ) = default;

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		explicit basic_latch( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : m_count( static_cast<intmax_t>( count ) ) {}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		basic_latch( Integer count, bool latched ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : m_count( static_cast<intmax_t>( count ) ) {}

		void reset( ) {
			m_count = 1;
		}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		void reset( Integer count ) {

			m_count = static_cast<intmax_t>( count );
		}

		void add_notifier( ) {
			++m_count;
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
	}; // basic_latch

	template<typename Mutex, typename ConditionVariable>
	struct is_latch<basic_latch<Mutex, ConditionVariable>> : std::true_type {};

	using latch = basic_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_unique_latch {
		using latch_t = basic_latch<Mutex, ConditionVariable>;
		std::unique_ptr<latch_t> latch = std::make_unique<latch_t>( );

	public:
		constexpr basic_unique_latch( ) = default;

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		explicit basic_unique_latch( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_unique<latch_t>( count ) ) {}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		basic_unique_latch( Integer count, bool latched ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_unique<latch_t>( count, latched ) ) {}

		basic_latch<Mutex, ConditionVariable> *release( ) {
			return latch.release( );
		}

		void add_notifier( ) {
			assert( latch );
			latch->add_notifier( );
		}

		void notify( ) {
			assert( latch );
			latch->notify( );
		}

		void set_latch( ) {
			assert( latch );
			latch->set_latch( );
		}

		void wait( ) const {
			assert( latch );
			latch->wait( );
		}

		[[nodiscard]] bool try_wait( ) const {
			assert( latch );
			return latch->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) const {
			assert( latch );
			return latch->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto ) wait_until(
		  std::chrono::time_point<Clock, Duration> const &timeout_time ) const {
			assert( latch );
			return latch->wait_until( timeout_time );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( latch );
		}
	}; // basic_unique_latch

	template<typename Mutex, typename ConditionVariable>
	struct is_unique_latch<basic_unique_latch<Mutex, ConditionVariable>>
	  : std::true_type {};

	using unique_latch = basic_unique_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_shared_latch {
		using latch_t = basic_latch<Mutex, ConditionVariable>;
		std::shared_ptr<latch_t> latch = std::make_shared<latch_t>( );

	public:
		basic_shared_latch( ) = default;

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		explicit basic_shared_latch( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_shared<latch_t>( count ) ) {}

		template<typename Integer,
		         std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>,
		                          std::nullptr_t> = nullptr>
		basic_shared_latch( Integer count, bool latched ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_shared<latch_t>( count, latched ) ) {}

		explicit basic_shared_latch(
		  basic_unique_latch<Mutex, ConditionVariable> &&other ) noexcept
		  : latch( other.release( ) ) {}

		void add_notifier( ) {
			assert( latch );
			latch->add_notifier( );
		}

		void notify( ) {
			assert( latch );
			latch->notify( );
		}

		void set_latch( ) {
			assert( latch );
			latch->set_latch( );
		}

		void wait( ) const {
			assert( latch );
			latch->wait( );
		}

		[[nodiscard]] bool try_wait( ) const {
			assert( latch );
			return latch->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) const {
			assert( latch );
			return latch->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto ) wait_until(
		  std::chrono::time_point<Clock, Duration> const &timeout_time ) const {
			assert( latch );
			return latch->wait_until( timeout_time );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( latch );
		}
	}; // basic_shared_latch

	template<typename Mutex, typename ConditionVariable>
	struct is_shared_latch<basic_shared_latch<Mutex, ConditionVariable>>
	  : std::true_type {};

	using shared_latch = basic_shared_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void wait_all(
	  std::initializer_list<basic_latch<Mutex, ConditionVariable>> semaphores ) {
		for( auto &sem : semaphores ) {
			sem->wait( );
		}
	}
} // namespace daw
