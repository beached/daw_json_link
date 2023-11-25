// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_enums.h"
#include <daw/daw_tuple_forward.h>

#include <daw/daw_attributes.h>

#include <tuple>

#if defined( __cpp_concepts )
#if __cpp_concepts >= 201907L
#define DAW_JSON_HAS_CPP20_CONCEPTS
#endif
#endif
// TODO find feature flag for lambdas in unevaluated contexts
#if defined( __cpp_lambdas )
// #if __cpp_lambdas >= 201907L
#define DAW_JSON_HAS_CPP20_UNEVAL_LAMBDAS
// #endif
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			// A type that can be implicitly converted to *anything*
			struct Anything {
				template<typename T>
				operator T( ) const; // We don't need to define this function
			};

			template<typename T, typename Is, typename = void>
			inline constexpr bool is_aggregate_constructible_from_n_impl_v = false;

			template<typename T, std::size_t... Is>
			inline constexpr bool is_aggregate_constructible_from_n_impl_v<
			  T, std::index_sequence<Is...>,
			  std::void_t<decltype( T{ ( void( Is ), Anything{ } )... } )>> = true;

			template<typename T, std::size_t N>
			inline constexpr bool is_aggregate_constructible_from_n_v =
			  is_aggregate_constructible_from_n_impl_v<T,
			                                           std::make_index_sequence<N>>;

			template<typename... Ts>
			void tp_from_struct_binding( std::tuple<Ts...> );

			template<typename T>
			constexpr auto tp_from_struct_binding( T &&value ) {
				using type = std::remove_cvref_t<T>;
				if constexpr( not std::is_class_v<type> or not std::is_aggregate_v<type> ) {
					return;
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 8> ) {
					auto &&[x0, x1, x2, x3, x4, x5, x6, x7] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple(
					  DAW_FWD( x0 ), DAW_FWD( x1 ), DAW_FWD( x2 ), DAW_FWD( x3 ),
					  DAW_FWD( x4 ), DAW_FWD( x5 ), DAW_FWD( x6 ), DAW_FWD( x7 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 7> ) {
					auto &&[x0, x1, x2, x3, x4, x5, x6] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple(
					  DAW_FWD( x0 ), DAW_FWD( x1 ), DAW_FWD( x2 ), DAW_FWD( x3 ),
					  DAW_FWD( x4 ), DAW_FWD( x5 ), DAW_FWD( x6 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 6> ) {
					auto &&[x0, x1, x2, x3, x4, x5] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple(
					  DAW_FWD( x0 ), DAW_FWD( x1 ), DAW_FWD( x2 ), DAW_FWD( x3 ),
					  DAW_FWD( x4 ), DAW_FWD( x5 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 5> ) {
					auto &&[x0, x1, x2, x3, x4] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple( DAW_FWD( x0 ), DAW_FWD( x1 ),
					                                        DAW_FWD( x2 ), DAW_FWD( x3 ),
					                                        DAW_FWD( x4 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 4> ) {
					auto &&[x0, x1, x2, x3] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple(
					  DAW_FWD( x0 ), DAW_FWD( x1 ), DAW_FWD( x2 ), DAW_FWD( x3 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 3> ) {
					auto &&[x0, x1, x2] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple( DAW_FWD( x0 ), DAW_FWD( x1 ),
					                                        DAW_FWD( x2 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 2> ) {
					auto &&[x0, x1] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple( DAW_FWD( x0 ),
					                                        DAW_FWD( x1 ) );
				} else if constexpr( is_aggregate_constructible_from_n_v<type, 1> ) {
					auto &&[x0] = DAW_FWD( value );
					return daw::forward_nonrvalue_as_tuple( DAW_FWD( x0 ) );
				}
			}

			template<typename T>
			inline constexpr bool can_convert_to_tuple_v =
			  not std::is_same_v<void, decltype( tp_from_struct_binding(
			                             std::declval<T>( ) ) )>;

			template<typename T>
			using tp_from_struct_binding_result_t =
			  std::remove_cvref_t<decltype( tp_from_struct_binding(
			    std::declval<T>( ) ) )>;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
