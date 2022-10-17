// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "daw_json_link.h"

#include <daw/daw_traits.h>

#include <ciso646>
#include <iostream>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename T>
			inline constexpr bool is_opted_into_json_iostreams_v = requires {
				typename json_data_contract<T>::opt_into_iostreams;
			};

			template<typename T>
			inline constexpr bool is_container_opted_into_json_iostreams_v =
			  requires {
				typename T::value_type;
				requires( is_opted_into_json_iostreams_v<typename T::value_type> );
			};
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json

/// @brief An opt in ostream interface for types that have JSON mappings.
template<typename T>
requires( daw::json::json_details::is_opted_into_json_iostreams_v<T> ) //
  std::ostream &
  operator<<( std::ostream &os, T const &value ) {

	return daw::json::to_json( value, os );
}

/// @brief An opt in ostream interface for containers of types that have JSON
/// mappings.
template<typename Container>
requires( daw::json::json_details::is_container_opted_into_json_iostreams_v<
          Container> ) //
  std::ostream &
  operator<<( std::ostream &os, Container const &c ) {

	return daw::json::to_json_array( c, os );
}
