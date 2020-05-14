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

#include <tuple>

#include "daw_traits.h"

namespace daw {
	template<typename T>
	struct function_info : function_info<decltype( &T::operator( ) )> {};
	// For generic types, directly use the result of the signature of its
	// 'operator()'

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... )> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... ) const> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... ) volatile> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... ) volatile const> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

#if __cplusplus >= 201703L
	// Volatile overloads, only >= c++17
	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... ) noexcept> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )( Args... ) const noexcept> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )(
	  Args... ) volatile noexcept> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_info<ReturnType ( ClassType::* )(
	  Args... ) volatile const noexcept> {
		static constexpr size_t const arity = sizeof...( Args );

		using result_type = ReturnType;
		using args_tuple = std::tuple<Args...>;
		using decayed_args_tuple = std::tuple<std::decay_t<Args>...>;

		template<size_t i>
		struct arg {
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};
#endif

	template<
	  typename Function,
	  std::enable_if_t<!std::is_function_v<std::remove_reference_t<Function>>,
	                   std::nullptr_t> = nullptr>
	constexpr decltype( auto ) make_callable( Function &&func ) noexcept {
		return std::forward<Function>( func );
	}

	namespace function_impl {
		template<typename Function>
		struct fp_callable_t {
			static_assert( std::is_function_v<Function> );
			Function *fp = nullptr;

			template<
			  typename... Args,
			  std::enable_if_t<!std::is_same_v<void, daw::traits::invoke_result_t<
			                                           Function, Args...>>,
			                   std::nullptr_t> = nullptr>
			constexpr decltype( auto ) operator( )( Args &&... args ) const
			  noexcept( daw::traits::is_nothrow_callable_v<
			            std::add_pointer_t<Function>, Args...> ) {

				return fp( std::forward<Args>( args )... );
			}

			template<
			  typename... Args,
			  std::enable_if_t<
			    std::is_same_v<void, daw::traits::invoke_result_t<Function, Args...>>,
			    std::nullptr_t> = nullptr>
			constexpr void operator( )( Args &&... args ) const
			  noexcept( daw::traits::is_nothrow_callable_v<
			            std::add_pointer_t<Function>, Args...> ) {

				fp( std::forward<Args>( args )... );
			}
		};
	} // namespace function_impl

	template<typename Function, std::enable_if_t<std::is_function_v<Function>,
	                                             std::nullptr_t> = nullptr>
	constexpr auto make_callable( Function *func ) noexcept {
		return function_impl::fp_callable_t<Function>{func};
	}
} // namespace daw
