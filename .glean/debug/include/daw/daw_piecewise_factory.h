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

#include <functional>
#include <optional>
#include <tuple>
#include <variant>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_move.h"
#include "daw_overload.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "daw_visit.h"

namespace daw {
	namespace impl {
		template<typename T, typename U>
		remove_cvref_t<T> remove_layer_func( std::variant<T, U> );

		template<typename T>
		remove_cvref_t<T> remove_layer_func( T );

		template<typename T>
		struct process_args_t {
			constexpr T operator( )( T const &value ) const {
				return value;
			}
			constexpr T operator( )( T &&value ) const {
				return daw::move( value );
			}

			template<typename Callable,
			         std::enable_if_t<
			           std::is_convertible_v<
			             decltype( std::declval<remove_cvref_t<Callable>>( )( ) ), T>,
			           std::nullptr_t> = nullptr>
			constexpr T operator( )( Callable &&f ) const {
				return std::forward<Callable>( f )( );
			}
			T operator( )( std::monostate ) const {
				daw::exception::daw_throw<std::invalid_argument>(
				  "Value cannot be empty" );
			}
		};

		template<typename T>
		constexpr T &&get_value( T &&value ) {
			return std::forward<T>( value );
		}

		template<typename... Args>
		constexpr auto get_value( std::variant<Args...> const &value ) {
			using T = decltype( remove_layer_func( value ) );
			return daw::visit_nt( value, process_args_t<T>{} );
		}

		template<typename... Args>
		constexpr auto get_value( std::variant<Args...> &&value ) {
			using T = decltype( remove_layer_func( value ) );
			return daw::visit_nt( daw::move( value ), process_args_t<T>{} );
		}

		template<size_t N, typename... Args>
		constexpr auto process_args( std::tuple<Args...> const &tp ) {
			return get_value( *std::get<N>( tp ) );
		}

		template<size_t N, typename... Args>
		constexpr auto process_args( std::tuple<Args...> &&tp ) {
			return get_value( *std::get<N>( daw::move( tp ) ) );
		}

		template<typename T, typename... Args, std::size_t... I>
		constexpr T piecewise_applier_impl( std::tuple<Args...> const &tp,
		                                    std::index_sequence<I...> ) {

			return construct_a<T>( process_args<I>( tp )... );
		}

		template<typename T, typename... Args, std::size_t... I>
		constexpr T piecewise_applier_impl( std::tuple<Args...> &&tp,
		                                    std::index_sequence<I...> ) {

			return construct_a<T>( process_args<I>( daw::move( tp ) )... );
		}

		template<
		  size_t N, bool, typename... Args, typename Value,
		  std::enable_if_t<( N >= sizeof...( Args ) ), std::nullptr_t> = nullptr>
		void set_tuple( size_t, std::tuple<Args...> &, Value && ) {
			daw::exception::daw_throw<std::out_of_range>( );
		}

		template<
		  size_t N, bool use_late, typename... Args, typename Value,
		  std::enable_if_t<( N < sizeof...( Args ) ), std::nullptr_t> = nullptr>
		constexpr void set_tuple( size_t index, std::tuple<Args...> &tp,
		                          Value &&value ) {
			if( N != index ) {
				set_tuple<N + 1, use_late>( index, tp, std::forward<Value>( value ) );
				return;
			}
			using tp_val_t = decltype( std::get<N>( tp ) );
			using value_t =
			  decltype( remove_layer_func( std::declval<tp_val_t>( ) ) );

			auto const setter = daw::overload(
			  [&]( value_t const &v ) { std::get<N>( tp ) = value_t{v}; },
			  [&]( value_t &&v ) { std::get<N>( tp ) = value_t{daw::move( v )}; },
			  [&]( std::function<value_t( )> const &f ) {
				  std::get<N>( tp ) = value_t{f( )};
			  },
			  [&]( std::function<value_t( )> &&f ) {
				  std::get<N>( tp ) = value_t{daw::move( f )( )};
			  },
			  []( ... ) noexcept {} );

			setter( std::forward<Value>( value ) );
		}

		template<typename T, typename... Args>
		constexpr T piecewise_applier( std::tuple<Args...> const &tp ) {
			return piecewise_applier_impl<T>(
			  tp, std::make_index_sequence<sizeof...( Args )>{} );
		}

		template<typename T, typename... Args>
		constexpr T piecewise_applier( std::tuple<Args...> &&tp ) {
			return piecewise_applier_impl<T>(
			  daw::move( tp ), std::make_index_sequence<sizeof...( Args )>{} );
		}

		template<typename T, bool use_late, typename... Args>
		struct piecewise_factory_impl_t {
		protected:
			using ttype_t =
			  std::conditional_t<use_late,
			                     std::tuple<std::optional<
			                       std::variant<Args, std::function<Args( )>>>...>,
			                     std::tuple<std::optional<Args>...>>;

			ttype_t m_args;

		public:
			constexpr T operator( )( ) const & {
				return piecewise_applier<T>( m_args );
			}

			constexpr T operator( )( ) & {
				return piecewise_applier<T>( m_args );
			}

			constexpr T operator( )( ) && {
				return piecewise_applier<T>( daw::move( m_args ) );
			}

		private:
			template<size_t N>
			using arg_at_t = std::tuple_element_t<N, std::tuple<Args...>>;

		public:
			template<size_t N>
			constexpr void set( arg_at_t<N> const &value ) {

				std::get<N>( m_args ) = value;
			}

			template<size_t N>
			constexpr void set( arg_at_t<N> &&value ) {

				std::get<N>( m_args ) = daw::move( value );
			}

			template<typename Value>
			constexpr void set( size_t index, Value &&value ) {
				set_tuple<0, use_late>( index, m_args, std::forward<Value>( value ) );
			}
		};
	} // namespace impl

	template<typename T, typename... Args>
	struct piecewise_factory_late_t
	  : private impl::piecewise_factory_impl_t<T, true, Args...> {

		using impl::piecewise_factory_impl_t<T, true,
		                                     Args...>::piecewise_factory_impl_t;
		using impl::piecewise_factory_impl_t<T, true, Args...>::set;
		using impl::piecewise_factory_impl_t<T, true, Args...>::operator( );

		template<size_t N, typename Callable,
		         std::enable_if_t<
		           std::is_convertible_v<decltype( std::declval<Callable>( )( ) ),
		                                 traits::nth_type<N, Args...>>,
		           std::nullptr_t> = nullptr>
		void set( Callable &&f ) {
			std::function<traits::nth_type<N, Args...>( )> func =
			  std::forward<Callable>( f );
			std::get<N>( this->m_args ) = daw::move( func );
		}
	};

	template<typename T, typename... Args>
	struct piecewise_factory_t
	  : private impl::piecewise_factory_impl_t<T, false, Args...> {

		using impl::piecewise_factory_impl_t<T, false,
		                                     Args...>::piecewise_factory_impl_t;
		using impl::piecewise_factory_impl_t<T, false, Args...>::set;
		using impl::piecewise_factory_impl_t<T, false, Args...>::operator( );
	};
} // namespace daw
