// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "../daw_readable_value.h"

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
		/***
		 * Default Constructor for a type.  It accounts for aggregate types and uses
		 * brace construction for them
		 * @tparam T type to construct
		 */
		template<typename T, typename = void>
		struct default_constructor {
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr T operator( )( ) const
			  noexcept( std::is_nothrow_default_constructible_v<T> ) {
				return T{ };
			}

			template<typename... Args,
			         std::enable_if_t<(sizeof...( Args ) > 0 and
			                           std::is_constructible_v<T, Args...>),
			                          std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr T
			operator( )( Args &&...args ) const {

				return T( DAW_FWD( args )... );
			}

			template<typename... Args,
			         std::enable_if_t<(sizeof...( Args ) > 0 and
			                           not std::is_constructible_v<T, Args...> and
			                           traits::is_list_constructible_v<T, Args...>),
			                          std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr T
			operator( )( Args &&...args ) const
			  noexcept( std::is_nothrow_constructible_v<T, Args...> ) {
				return T{ DAW_FWD( args )... };
			}
		};

		template<typename, typename = void>
		inline constexpr bool is_default_constructor_v = false;

		template<typename T>
		inline constexpr bool is_default_constructor_v<default_constructor<T>> =
		  true;

		template<typename T, typename... Ignore>
		struct is_default_constructor
		  : std::bool_constant<is_default_constructor_v<T, Ignore...>> {};

		///
		/// @brief Default constructor for nullable types.
		/// Specializations must accept accept an operator( )( ) that signifies a
		/// JSON null. Any other arguments only need to be valid to construct the
		/// type.
		///
		template<typename T, typename = void>
		struct nullable_constructor : default_constructor<T> {
			/// used for types like string_view that have an empty state
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
