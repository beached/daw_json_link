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

#include "cpp_17.h"
#include "daw_traits.h"

#include <type_traits>

namespace daw {
	// overload_t/overload create a callable with the overloads of operator( )
	// provided
	//
	namespace overload_details {
		template<typename>
		struct fp_wrapper_t;

		template<typename ReturnType, typename... Arguments>
		struct fp_wrapper_t<ReturnType ( * )( Arguments... )> {
			std::add_pointer_t<ReturnType( Arguments... )> fp;

			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<decltype( fp ), Args...>,
			                          std::nullptr_t> = nullptr>
			inline constexpr ReturnType operator( )( Args &&... args ) const {
				return fp( std::forward<Args>( args )... );
			}
		};

		template<typename ReturnType, typename... Arguments>
		struct fp_wrapper_t<ReturnType ( & )( Arguments... )> {
			std::add_pointer_t<ReturnType( Arguments... )> fp;

			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<decltype( fp ), Args...>,
			                          std::nullptr_t> = nullptr>
			inline constexpr ReturnType operator( )( Args &&... args ) const {
				return fp( std::forward<Args>( args )... );
			}
		};

		template<typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template<typename Func>
		using fp_wrapper = std::conditional_t<std::is_class_v<remove_cvref_t<Func>>,
		                                      Func, fp_wrapper_t<Func>>;

		template<auto>
		struct lift_t;

		template<typename ReturnType, typename... Arguments,
		         ReturnType ( *fp )( Arguments... )>
		struct lift_t<fp> {
			template<typename... Args,
			         std::enable_if_t<std::is_invocable_v<decltype( fp ), Args...>,
			                          std::nullptr_t> = nullptr>
			inline constexpr ReturnType operator( )( Args &&... args ) const
			  noexcept( noexcept( fp( std::forward<Args>( args )... ) ) ) {
				return fp( std::forward<Args>( args )... );
			}
		};

	} // namespace overload_details

	template<auto fp>
	inline constexpr overload_details::lift_t<fp> lift =
	  overload_details::lift_t<fp>{};

	template<typename... Funcs>
	struct overload : overload_details::fp_wrapper<Funcs>... {
		using overload_details::fp_wrapper<Funcs>::operator( )...;

		constexpr overload( Funcs... fs )
		  : overload_details::fp_wrapper<Funcs>{fs}... {}
	};

	template<typename... Funcs>
	overload( Funcs &&... )
	  ->overload<std::conditional_t<std::is_class_v<remove_cvref_t<Funcs>>,
	                                remove_cvref_t<Funcs>, Funcs>...>;
} // namespace daw
