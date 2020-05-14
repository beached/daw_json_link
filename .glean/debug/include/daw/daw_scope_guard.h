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

#include <exception>
#include <utility>

#include "daw_exchange.h"
#include "daw_move.h"
#include "daw_swap.h"
#include "daw_traits.h"

#if defined( __cpp_constexpr_dynamic_alloc ) and                               \
  defined( __cpp_lib_is_constant_evaluated )
#define CXDTOR constexpr
#define CXEVAL std::is_constant_evaluated( )
#define HAS_CXSTOR_AND_EVAL
#else
#define CXDTOR
#define CXEVAL
#endif

namespace daw {
	template<typename FunctionType>
	class ScopeGuard {
		FunctionType m_function;
		mutable bool m_is_active;

	public:
		ScopeGuard( ) = delete;
		ScopeGuard( const ScopeGuard & ) = delete;
		ScopeGuard &operator=( const ScopeGuard & ) = delete;

		constexpr ScopeGuard( FunctionType f ) noexcept
		  : m_function{daw::move( f )}
		  , m_is_active{true} {}

		constexpr ScopeGuard( ScopeGuard &&other ) noexcept
		  : m_function{daw::move( other.m_function )}
		  , m_is_active{daw::exchange( other.m_is_active, false )} {}

		constexpr ScopeGuard &operator=( ScopeGuard &&rhs ) noexcept {
			m_function = daw::move( rhs.m_function );
			m_is_active = daw::exchange( rhs.m_is_active, false );
			return *this;
		}

		CXDTOR ~ScopeGuard( ) noexcept {
			if( m_is_active ) {
				m_function( );
			}
		}

		constexpr void dismiss( ) const noexcept {
			m_function = nullptr;
			m_is_active = false;
		}

		[[nodiscard]] constexpr bool operator==( const ScopeGuard &rhs ) const
		  noexcept {
			return rhs.m_function == m_function and rhs.m_is_active == m_is_active;
		}
	}; // class ScopeGuard

	template<typename FunctionType>
	[[nodiscard]] constexpr ScopeGuard<FunctionType>
	on_scope_exit( FunctionType f ) noexcept {
		return ScopeGuard<FunctionType>( daw::move( f ) );
	}

	template<typename Handler>
	class on_exit_success {
		Handler on_exit_handler;

	public:
		constexpr on_exit_success( Handler &&h ) noexcept(
		  std::is_nothrow_move_constructible_v<Handler> )
		  : on_exit_handler( std::move( h ) ) {}

		constexpr on_exit_success( Handler const &h ) noexcept(
		  std::is_nothrow_copy_constructible_v<Handler> )
		  : on_exit_handler( h ) {}

		CXDTOR ~on_exit_success( ) noexcept( noexcept( on_exit_handler( ) ) ) {
#if defined( HAS_CXSTOR_AND_EVAL )
			if( CXEVAL ) {
				on_exit_handler( );
			} else {
#endif
				if( std::uncaught_exceptions( ) == 0 ) {
					on_exit_handler( );
				}
#if defined( HAS_CXSTOR_AND_EVAL )
			}
#endif
		}
	};

	template<typename Handler>
	on_exit_success( Handler )->on_exit_success<Handler>;
} // namespace daw
