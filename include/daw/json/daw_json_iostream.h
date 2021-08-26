// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link.h"
#include "impl/version.h"

#include <daw/daw_traits.h>

#include <ciso646>
#include <iostream>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename T>
			using is_opted_into_json_iostreams =
			  typename json_data_contract<T>::opt_into_iostreams;

			template<typename Container>
			using is_container_opted_into_json_iostreams =
			  is_opted_into_json_iostreams<typename Container::value_type>;

			template<typename T>
			inline constexpr bool is_opted_into_json_iostreams_v =
			  daw::is_detected<is_opted_into_json_iostreams, T>::value;

			template<typename T>
			inline constexpr bool is_container_opted_into_json_iostreams_v =
			  daw::is_detected<is_container_opted_into_json_iostreams, T>::value;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json

template<typename T>
auto operator<<( std::ostream &os, T const &value ) -> std::enable_if_t<
  daw::json::json_details::is_opted_into_json_iostreams_v<T>, std::ostream &> {

	auto out_it = std::ostreambuf_iterator<char>( os );
	daw::json::to_json( value, out_it );
	return os;
}

template<typename Container>
auto operator<<( std::ostream &os, Container const &c ) -> std::enable_if_t<
  daw::json::json_details::is_container_opted_into_json_iostreams_v<Container>,
  std::ostream &> {

	auto out_it = std::ostreambuf_iterator<char>( os );
	(void)daw::json::to_json_array( c, out_it );
	return os;
}
