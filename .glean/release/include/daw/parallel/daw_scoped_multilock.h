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

#include <mutex>
#include <tuple>

namespace daw {
	namespace details {
		template<size_t N, size_t SZ>
		struct make_lock_guard {
			template<typename Results, typename TPLockables>
			void operator( )( Results &results, TPLockables &lockables ) const {
				auto current_lockable = std::get<N>( lockables );
				using current_lock_t =
				  std::lock_guard<std::decay_t<decltype( *current_lockable )>>;

				std::get<N>( results ) = std::make_unique<current_lock_t>(
				  *current_lockable, std::adopt_lock );
				make_lock_guard<N + 1, SZ>{}( results, lockables );
			}
		};

		template<size_t N>
		struct make_lock_guard<N, N> {
			template<typename Results, typename TPLockables>
			constexpr void operator( )( Results const &, TPLockables const & ) const
			  noexcept {}
		};
	} // namespace details

	template<typename... Lockables>
	class scoped_multilock {
		static_assert( sizeof...( Lockables ) > 0,
		               "Must specify at least 1 lockable" );
		std::tuple<std::unique_ptr<std::lock_guard<std::decay_t<Lockables>>>...>
		  m_locks;

		template<typename Arg>
		void _lock( Arg const & ) {}

		template<typename Arg1, typename Arg2, typename... Args>
		void _lock( Arg1 &arg1, Arg2 &arg2, Args &... args ) {
			std::lock( arg1, arg2, args... );
		}

	public:
		scoped_multilock( Lockables &... lockables )
		  : m_locks{} {
			_lock( lockables... );
			auto args = std::make_tuple<Lockables *...>( &lockables... );
			details::make_lock_guard<0, sizeof...( Lockables )>{}( m_locks, args );
		}

		~scoped_multilock( ) = default;
		scoped_multilock( scoped_multilock && ) noexcept = default;
		scoped_multilock &operator=( scoped_multilock && ) noexcept = default;

		scoped_multilock( scoped_multilock const & ) = delete;
		scoped_multilock &operator=( scoped_multilock const & ) = delete;
	};

	template<typename... Lockables>
	auto make_scoped_multilock( Lockables &... lockables ) {
		return scoped_multilock<Lockables...>{lockables...};
	}
} // namespace daw
