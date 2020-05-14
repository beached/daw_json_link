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

#include <cstddef>
#include <functional>
#include <memory>
#include <new>
#include <type_traits>

#include "daw_exception.h"
#include "daw_move.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "daw_virtual_base.h"

namespace daw {
	namespace func_impl {
		template<typename T>
		using is_boolable_detect =
		  decltype( static_cast<bool>( std::declval<T>( ) ) );

		template<typename T>
		inline constexpr bool is_boolable_v =
		  daw::is_detected_v<is_boolable_detect, T>;

		template<typename T>
		using has_empty_member_detect = decltype( std::declval<T>( ).empty( ) );

		template<typename T>
		inline constexpr bool has_empty_member_v =
		  daw::is_detected_v<has_empty_member_detect, T>;

		template<size_t StorageSize, typename Base>
		struct function_storage {
			std::aligned_storage_t<StorageSize> m_data;

			function_storage( function_storage const & ) noexcept = default;
			function_storage( function_storage && ) noexcept = default;

			function_storage &operator=( function_storage const &rhs ) {
				if( this == &rhs ) {
					return *this;
				}
				clean( );
				m_data = rhs.m_data;
				return *this;
			}

			function_storage &operator=( function_storage &&rhs ) noexcept {
				if( this == &rhs ) {
					return *this;
				}
				clean( );
				m_data = std::move( rhs.m_data );
				return *this;
			}

			template<
			  typename Func,
			  std::enable_if_t<!std::is_same_v<std::decay_t<Func>, function_storage>,
			                   std::nullptr_t> = nullptr>
			function_storage( Func &&f ) {
				static_assert( sizeof( std::decay_t<Func> ) <= StorageSize );
				store( std::forward<Func>( f ) );
			}

			Base *ptr( ) noexcept {
				return reinterpret_cast<Base *>( &m_data );
			}

			Base const *ptr( ) const noexcept {
				return reinterpret_cast<Base const *>( &m_data );
			}

			void clean( ) {
				std::destroy_at( ptr( ) );
			}

			~function_storage( ) {
				clean( );
			}

			template<typename Func>
			void store( Func &&f ) {
				static_assert( sizeof( std::decay_t<Func> ) <= StorageSize );
				static_assert( std::is_base_of_v<Base, Func>,
				               "Can only store children of func_base" );
				new( &m_data )
				  daw::remove_cvref_t<std::decay_t<Func>>( std::forward<Func>( f ) );
			}

			Base &operator*( ) noexcept {
				return *ptr( );
			}

			Base const &operator*( ) const noexcept {
				return *ptr( );
			}

			Base *operator->( ) noexcept {
				return ptr( );
			}

			Base const *operator->( ) const noexcept {
				return ptr( );
			}
		};

		template<typename Result, typename... FuncArgs>
		struct function_base
		  : daw::virtual_base<function_base<Result, FuncArgs...>> {
			virtual Result operator( )( FuncArgs... ) const = 0;
			virtual Result operator( )( FuncArgs... ) = 0;
			virtual bool empty( ) const = 0;

			function_base( ) noexcept = default;
		};

		template<typename Result, typename... FuncArgs>
		struct empty_child final : function_base<Result, FuncArgs...> {
			[[noreturn]] Result operator( )( FuncArgs... ) const override {
				daw::exception::daw_throw<std::bad_function_call>( );
			}
			[[noreturn]] Result operator( )( FuncArgs... ) override {
				daw::exception::daw_throw<std::bad_function_call>( );
			}

			bool empty( ) const override {
				return true;
			}
		};

		template<typename...>
		struct function_child;

		template<typename Func, typename Result, typename... FuncArgs>
		struct function_child<Func, Result, FuncArgs...> final
		  : function_base<Result, FuncArgs...>,
		    enable_copy_constructor<Func>,
		    enable_move_constructor<Func>,
		    enable_copy_assignment<Func>,
		    enable_move_assignment<Func> {

			Func m_func;

			template<typename F, std::enable_if_t<
			                       !std::is_same_v<std::decay_t<F>, function_child>,
			                       std::nullptr_t> = nullptr>
			function_child( F &&func ) noexcept(
			  std::is_nothrow_constructible_v<Func, F> )
			  : m_func( std::forward<F>( func ) ) {}

			Result operator( )( FuncArgs... args ) override {
				daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
				if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
					// daw::invoke( m_func, daw::move( args )... );
					m_func( args... );
				} else {
					// return daw::invoke( m_func, daw::move( args )... );
					return m_func( args... );
				}
			}

			Result operator( )( FuncArgs... args ) const override {
				daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
				if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
					// daw::invoke( m_func, daw::move( args )... );
					m_func( args... );
				} else {
					// return daw::invoke( m_func, daw::move( args )... );
					return m_func( args... );
				}
			}

			bool empty( ) const override {
				if constexpr( func_impl::has_empty_member_v<Func> ) {
					return m_func.empty( );
				} else if constexpr( func_impl::is_boolable_v<Func> ) {
					return !static_cast<bool>( m_func );
				}
				return false;
			}
		};

		template<size_t MaxSize, typename function_base, typename function_child,
		         typename empty_child, typename Func>
		function_storage<MaxSize, function_base> store_if_not_empty( Func &&f ) {
			static_assert( sizeof( std::decay_t<Func> ) <= MaxSize );
			if constexpr( func_impl::has_empty_member_v<Func> ) {
				if( f.empty( ) ) {
					return {empty_child( )};
				}
			} else if constexpr( func_impl::is_boolable_v<Func> ) {
#if defined( __GNUC__ ) && !defined( __clang__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#pragma GCC diagnostic ignored "-Waddress"
#endif
				if( !static_cast<bool>( f ) ) {
					return {empty_child( )};
				}
#if defined( __GNUC__ ) && !defined( __clang__ )
#pragma GCC diagnostic pop
#endif
			}
			return {function_child( std::forward<Func>( f ) )};
		}

		template<size_t Sz, size_t MaxSz>
		constexpr void validate_size( ) {
			static_assert( Sz <= MaxSz,
			               "Size of function is larger than allocated space" );
		}

	} // namespace func_impl

	template<size_t, typename>
	class function;

	template<size_t MaxSize, typename Result, typename... FuncArgs>
	class function<MaxSize, Result( FuncArgs... )> {
		using function_base = func_impl::function_base<Result, FuncArgs...>;
		using empty_child = func_impl::empty_child<Result, FuncArgs...>;

		template<typename Func>
		using function_child = func_impl::function_child<Func, Result, FuncArgs...>;

		func_impl::function_storage<MaxSize, function_base> m_storage;

		template<typename Func>
		decltype( auto ) store_if_not_empty( Func &&f ) {
			func_impl::validate_size<sizeof( std::decay_t<Func> ), MaxSize>( );
			return func_impl::store_if_not_empty<MaxSize, function_base,
			                                     function_child<Func>, empty_child>(
			  std::forward<Func>( f ) );
		}

	public:
		function( ) noexcept
		  : m_storage( empty_child{} ) {}

		function( std::nullptr_t )
		  : m_storage( empty_child{} ) {}

		function( function const & ) = default;
		function( function && ) noexcept = default;
		function &operator=( function const & ) = default;
		function &operator=( function && ) noexcept = default;
		~function( ) = default;

		template<size_t N,
		         std::enable_if_t<( N <= MaxSize ), std::nullptr_t> = nullptr>
		function( function<N, Result( FuncArgs... )> const &other )
		  : m_storage( *other.m_storage ) {}

		template<size_t N,
		         std::enable_if_t<( N > MaxSize ), std::nullptr_t> = nullptr>
		function( function<N, Result( FuncArgs... )> const &other ) = delete;

		template<size_t N,
		         std::enable_if_t<( N <= MaxSize ), std::nullptr_t> = nullptr>
		function &operator=( function<N, Result( FuncArgs... )> const &other ) {
			m_storage = *other.m_storage;
			return *this;
		}

		template<size_t N,
		         std::enable_if_t<( N > MaxSize ), std::nullptr_t> = nullptr>
		function &
		operator=( function<N, Result( FuncArgs... )> const &other ) = delete;

		template<typename Func,
		         std::enable_if_t<
		           daw::all_true_v<!std::is_same_v<std::decay_t<Func>, function>,
		                           !std::is_function_v<Func>>,
		           std::nullptr_t> = nullptr>
		function( Func &&f )
		  : m_storage( store_if_not_empty( std::forward<Func>( f ) ) ) {

			func_impl::validate_size<sizeof( std::decay_t<Func> ), MaxSize>( );
			static_assert( std::is_invocable_r_v<Result, Func, FuncArgs...>,
			               "Function isn't callable with FuncArgs" );
		}

		template<typename Func,
		         std::enable_if_t<
		           daw::all_true_v<!std::is_same_v<std::decay_t<Func>, function>,
		                           !std::is_function_v<Func>>,
		           std::nullptr_t> = nullptr>
		function &operator=( Func &&f ) {
			func_impl::validate_size<sizeof( std::decay_t<Func> ), MaxSize>( );
			static_assert( std::is_invocable_r_v<Result, Func, FuncArgs...>,
			               "Function isn't callable with FuncArgs" );
			m_storage = store_if_not_empty( std::forward<Func>( f ) );
			return *this;
		}

		template<typename... Args>
		Result operator( )( Args &&... args ) {
			daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
			function_base &f = *m_storage;
			if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
				f( std::forward<Args>( args )... );
			} else {
				return f( std::forward<Args>( args )... );
			}
		}

		template<typename... Args>
		Result operator( )( Args &&... args ) const {
			daw::exception::precondition_check<std::bad_function_call>( !empty( ) );
			function_base const &f = *m_storage;
			if constexpr( std::is_same_v<std::decay_t<Result>, void> ) {
				f( std::forward<Args>( args )... );
			} else {
				return f( std::forward<Args>( args )... );
			}
		}

		bool empty( ) const {
			return m_storage->empty( );
		}

		explicit operator bool( ) const {
			return !empty( );
		}
	};
} // namespace daw
