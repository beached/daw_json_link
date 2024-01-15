// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "concepts/daw_nullable_value.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_move.h>
#include <daw/daw_traits.h>

#include <memory>
#include <type_traits>

/***
 * Customization point traits
 *
 */
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename T, typename... Args>
			inline constexpr bool should_list_construct_v =
			  not std::is_constructible_v<T, Args...> and
			  daw::traits::is_list_constructible_v<T, Args...>;
		}
		/// @brief Default Constructor for a type.  It accounts for aggregate types
		/// and uses brace construction for them
		/// @tparam T type to construct
		template<typename T, typename = void>
		struct default_constructor {
			using i_am_the_default_default_constructor_type = void;

			template<typename... Args,
			         std::enable_if_t<std::is_constructible_v<T, Args...>,
			                          std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE DAW_JSON_CPP23_STATIC_CALL_OP constexpr T
			operator( )( Args &&...args ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST {

				return T( DAW_FWD( args )... );
			}

			template<
			  typename... Args,
			  std::enable_if_t<json_details::should_list_construct_v<T, Args...>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE DAW_JSON_CPP23_STATIC_CALL_OP constexpr T
			operator( )( Args &&...args ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST
			  noexcept( std::is_nothrow_constructible_v<T, Args...> ) {
				return T{ DAW_FWD( args )... };
			}
		};

		/// @brief Default constructor for nullable types.
		/// Specializations must accept accept an operator( )( ) that signifies a
		/// JSON null. Any other arguments only need to be valid to construct the
		/// type.
		template<typename T, typename = void>
		struct nullable_constructor : default_constructor<T> {
			/// used for types like string_view that have an empty state
			using default_constructor<T>::operator( );
		};

		template<typename T>
		struct nullable_constructor<
		  T,
		  std::enable_if_t<( concepts::nullable_impl::is_list_constructible_v<T> and
		                     not concepts::is_nullable_value_v<T> )>>
		  : default_constructor<T> {
			/// used for types like string_view that have an empty state
			using default_constructor<T>::operator( );
			[[nodiscard]] DAW_ATTRIB_INLINE DAW_JSON_CPP23_STATIC_CALL_OP constexpr T
			operator( )( ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				return T{ };
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
