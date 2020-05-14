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

#include <cstdlib>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

#include "cpp_17.h"
#include "daw_traits.h"

namespace daw {
	namespace visit_details {
		namespace test {
			struct Tst {
				[[maybe_unused]] constexpr void operator( )( int ) const {}
			};
		} // namespace test

		// A simple overload class that is constructed via aggregate.
		template<typename... Fs>
		struct overload_t : Fs... {
			using Fs::operator( )...;
		};
		template<typename... Fs>
		overload_t( Fs... )->overload_t<Fs...>;

		template<typename F, typename... Fs>
		[[nodiscard, maybe_unused]] constexpr decltype( auto )
		overload( F &&f, Fs &&... fs ) noexcept {
			if constexpr( sizeof...( Fs ) > 0 ) {
				return overload_t{daw::traits::lift_func( std::forward<F>( f ) ),
				                  daw::traits::lift_func( std::forward<Fs>( fs ) )...};
			} else {
				if constexpr( std::is_function_v<daw::remove_cvref_t<F>> ) {
					static_assert(
					  daw::traits::func_traits<F>::arity == 1,
					  "Function pointers only valid for functions with 1 argument" );
				}
				return daw::traits::lift_func( std::forward<F>( f ) );
			}
		}

		template<size_t N, typename R, typename Variant, typename Visitor>
		[[nodiscard]] constexpr R visit_nt( Variant &&var, Visitor &&vis ) {
			using std::get;
			if constexpr( N == 0 ) {
				// If this check isnt there the compiler will generate
				// exception code, this stops that
				if( N != var.index( ) or
				    not std::is_invocable_v<Visitor, decltype( get<N>( var ) )> ) {
					std::abort( );
				}
				return std::forward<Visitor>( vis )(
				  get<N>( std::forward<Variant>( var ) ) );
			} else {
				if( var.index( ) == N ) {
					if( not std::is_invocable_v<Visitor, decltype( get<N>( var ) )> ) {
						std::abort( );
					}
					// If this check isnt there the compiler will generate
					// exception code, this stops that
					return std::forward<Visitor>( vis )(
					  get<N>( std::forward<Variant>( var ) ) );
				}
				return visit_nt<N - 1, R>( std::forward<Variant>( var ),
				                           std::forward<Visitor>( vis ) );
			}
		}

		template<typename>
		struct get_var_size;

		template<template<class...> class Variant, typename... Args>
		struct get_var_size<Variant<Args...>> {
			constexpr static inline auto value = sizeof...( Args );
		};

		template<typename Variant>
		inline constexpr size_t get_var_size_v =
		  get_var_size<daw::remove_cvref_t<Variant>>::value;

	} // namespace visit_details
	//**********************************************

	// Singe visitation visit.  Expects that variant is valid and not empty
	// The return type assumes that all the visitors have a result convertable
	// to that of visitor( get<0>( variant ) ) 's result
	template<typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr decltype( auto )
	visit_nt( Variant &&var, Visitors &&... visitors ) {

		constexpr size_t var_sz = daw::visit_details::get_var_size_v<Variant>;
		static_assert( var_sz > 0 );
		using std::get;
		using result_t =
		  decltype( daw::visit_details::overload( std::forward<Visitors>(
		    visitors )... )( get<0>( std::forward<Variant>( var ) ) ) );

		return daw::visit_details::visit_nt<var_sz - 1, result_t>(
		  std::forward<Variant>( var ),
		  daw::visit_details::overload( std::forward<Visitors>( visitors )... ) );
	}

	// Singe visitation visit with user choosable result.  Expects that variant is
	// valid and not empty
	template<typename Result, typename Variant, typename... Visitors>
	[[nodiscard, maybe_unused]] constexpr Result
	visit_nt( Variant &&var, Visitors &&... visitors ) {

		constexpr size_t var_sz = daw::visit_details::get_var_size_v<Variant>;
		static_assert( var_sz > 0 );

		return daw::visit_details::visit_nt<var_sz - 1, Result>(
		  std::forward<Variant>( var ),
		  daw::visit_details::overload( std::forward<Visitors>( visitors )... ) );
	}

	template<typename Value, typename... Visitors>
	inline constexpr bool
	  is_visitable_v = ( std::is_invocable_v<Visitors, Value> or ... );
} // namespace daw
