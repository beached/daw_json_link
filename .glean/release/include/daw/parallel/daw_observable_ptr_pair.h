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

#include <new>
#include <variant>

#include "../daw_move.h"
#include "../daw_visit.h"
#include "daw_observable_ptr.h"

namespace daw {
	namespace impl {
		template<typename Func, typename... Args>
		using has_void_result =
		  decltype( std::declval<Func>( )( std::declval<Args>( )... ) );

		template<typename Func, typename... Args>
		constexpr bool has_void_result_v =
		  is_detected_v<has_void_result, Func, Args...>;
	} // namespace impl

	template<typename T>
	class observable_ptr_pair {
		// union_pair_t<observable_ptr<T>, observer_ptr<T>> m_ptrs;
		std::variant<observable_ptr<T>, observer_ptr<T>> m_ptrs;

	public:
		observable_ptr_pair( )
		  : m_ptrs{observable_ptr<T>{}} {}

		observable_ptr_pair( T *ptr )
		  : m_ptrs{observable_ptr<T>{ptr}} {}

		observable_ptr_pair( observable_ptr_pair &&other ) noexcept
		  : m_ptrs{daw::move( other.m_ptrs )} {}

		observable_ptr_pair &operator=( observable_ptr_pair &&rhs ) noexcept {
			if( this != &rhs ) {
				m_ptrs = daw::move( rhs.m_ptrs );
			}
			return *this;
		}

		observable_ptr_pair( observable_ptr_pair const &other )
		  : m_ptrs{
		      daw::visit_nt( other.m_ptrs, []( auto const &v ) -> decltype( auto ) {
			      return v.get_observer( );
		      } )} {}

		observable_ptr_pair &operator=( observable_ptr_pair const &rhs ) {
			if( this != &rhs ) {
				m_ptrs =
				  daw::visit_nt( rhs.m_ptrs, []( auto const &v ) -> decltype( auto ) {
					  return v.get_observer( );
				  } );
			}
			return *this;
		}

		template<typename Visitor>
		decltype( auto ) apply_visitor( Visitor vis ) {
			return daw::visit_nt( m_ptrs, daw::move( vis ) );
		}

		template<typename Visitor>
		decltype( auto ) apply_visitor( Visitor vis ) const {
			return daw::visit_nt( m_ptrs, daw::move( vis ) );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor vis ) {
			return daw::visit_nt( m_ptrs, [&]( auto &p ) -> decltype( auto ) {
				return vis( *p.borrow( ) );
			} );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor vis ) const {
			return daw::visit_nt( m_ptrs, [&]( auto const &p ) -> decltype( auto ) {
				return vis( *p.borrow( ) );
			} );
		}

		decltype( auto ) operator-> ( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.operator->( ); } );
		}

		decltype( auto ) operator*( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.operator*( ); } );
		}

		operator bool( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.operator bool( ); } );
		}

		operator observer_ptr<T>( ) const {
			return apply_visitor( []( auto const &obs_ptr ) {
				return obs_ptr.operator observer_ptr<T>( );
			} );
		}

		decltype( auto ) get_observer( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.get_observer( ); } );
		}

		decltype( auto ) borrow( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.borrow( ); } );
		}

		decltype( auto ) try_borrow( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.try_borrow( ); } );
		}

		decltype( auto ) get( ) const {
			return apply_visitor(
			  []( auto const &obs_ptr ) { return obs_ptr.get( ); } );
		}

		template<typename Callable>
		decltype( auto ) lock( Callable c ) const {
			return apply_visitor( [&c]( auto const &obs_ptr ) {
				return obs_ptr.lock( daw::move( c ) );
			} );
		}
	};

	template<typename T, typename... Args>
	[[nodiscard]] observable_ptr_pair<T> make_observable_ptr_pair(
	  Args &&... args ) noexcept( std::is_nothrow_constructible_v<T, Args...> ) {

		try {
			T *tmp = new T( std::forward<Args>( args )... );
			if( !tmp ) {
				return observable_ptr_pair<T>( nullptr );
			}
			return observable_ptr_pair<T>( tmp );
		} catch( std::bad_alloc const & ) { std::abort( ); }
	}
} // namespace daw
