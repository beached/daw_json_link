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
#include "daw_json_traits.h"

#include <daw/daw_attributes.h>
#include <daw/daw_traits.h>
#include <daw/daw_tuple_forward.h>

#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			DAW_JSON_MAKE_REQ_TRAIT( has_to_tuple_v,
			                         to_tuple( std::declval<T const &>( ) ) );

			template<typename T,
			         std::enable_if_t<has_to_tuple_v<T>, std::nullptr_t> = nullptr>
			constexpr auto to_tuple_impl( T const &value ) {
				return to_tuple( value );
			}

			DAW_JSON_MAKE_REQ_TRAIT( can_convert_to_tuple_v,
			                         to_tuple_impl( std::declval<T const &>( ) ) );

			template<typename T>
			using tp_from_struct_binding_result_t =
			  daw::remove_cvref_t<decltype( to_tuple_impl( std::declval<T>( ) ) )>;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
