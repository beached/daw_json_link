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

#include <array>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "cpp_17.h"

namespace daw {
	namespace detail {
		template<typename T, std::size_t N, std::size_t... I>
		constexpr std::array<std::remove_cv_t<T>, N>
		to_array_impl( T ( &a )[N], std::index_sequence<I...> ) {
			return {{a[I]...}};
		}
	} // namespace detail

	template<typename T, std::size_t N>
	constexpr std::array<std::remove_cv_t<T>, N> to_array( T ( &a )[N] ) {
		return detail::to_array_impl( a, std::make_index_sequence<N>{} );
	}
	namespace details {
		template<typename>
		struct is_ref_wrapper : std::false_type {};

		template<typename T>
		struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

		template<typename T>
		using not_ref_wrapper = negation<is_ref_wrapper<std::decay_t<T>>>;

		template<typename D, typename...>
		struct return_type_helper {
			using type = D;
		};
		template<typename... Types>
		struct return_type_helper<void, Types...> : std::common_type<Types...> {
			static_assert( conjunction_v<not_ref_wrapper<Types>...>,
			               "Types cannot contain reference_wrappers when D is void" );
		};

		template<typename D, typename... Types>
		using return_type =
		  std::array<typename return_type_helper<D, Types...>::type,
		             sizeof...( Types )>;
	} // namespace details

	/*
	template<typename D = void, typename... Types>
	constexpr details::return_type<D, Types...> make_array( Types &&... t ) {
	  return {std::forward<Types>( t )...};
	}
	*/
	namespace make_array_impl {
		struct unspecified_type {};
	} // namespace make_array_impl
	template<
	  typename T = make_array_impl::unspecified_type, typename... Args,
	  std::enable_if_t<not std::is_same_v<T, void>, std::nullptr_t> = nullptr>
	constexpr auto make_array( Args &&... args ) {
		constexpr size_t sz = sizeof...( Args );
		if constexpr( std::is_same_v<T, make_array_impl::unspecified_type> ) {

			using result_t = std::common_type_t<daw::remove_cvref_t<Args>...>;
			return std::array<result_t, sz>{std::forward<Args>( args )...};
		} else {
			return std::array<T, sz>{std::forward<Args>( args )...};
		}
	}
	namespace make_array_impl {
		template<typename T, size_t N, size_t... Is>
		constexpr auto make_array_impl( T const( &&arry )[N],
		                                std::index_sequence<Is...> ) {
			return std::array<T, N>{std::move( arry[Is] )...};
		}
	} // namespace make_array_impl

	template<typename T, size_t N>
	constexpr auto make_array( T const( &&arry )[N] ) {
		return make_array_impl::make_array_impl( std::move( arry ),
		                                         std::make_index_sequence<N>{} );
	}

	template<typename... Ts>
	std::array<std::string, sizeof...( Ts )> make_string_array( Ts &&... t ) {
		return {std::string( std::forward<Ts>( t ) )...};
	}
} // namespace daw
