// copyright (c) darrell wright
//
// distributed under the boost software license, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link.h"
#include "impl/namespace.h"

#include <daw/daw_traits.h>

#include <ciso646>
#include <iostream>
#include <type_traits>

namespace DAW_JSON_NS::json_details {
	template<typename T>
	using is_opted_into_json_iostreams =
	  typename json_data_contract<T>::opt_into_iostreams;

	template<typename Container>
	using is_container_opted_into_json_iostreams =
	  is_opted_into_json_iostreams<typename Container::value_type>;

	template<typename T>
	inline constexpr bool is_opted_into_json_iostreams_v =
	  daw::is_detected_v<is_opted_into_json_iostreams, T>;

	template<typename T>
	inline constexpr bool is_container_opted_into_json_iostreams_v =
	  daw::is_detected_v<is_container_opted_into_json_iostreams, T>;
} // namespace DAW_JSON_NS::json_details

template<
  typename T,
  std::enable_if_t<DAW_JSON_NS::json_details::is_opted_into_json_iostreams_v<T>,
                   std::nullptr_t> = nullptr>
std::ostream &operator<<( std::ostream &os, T const &value ) {
	auto out_it = std::ostreambuf_iterator<char>( os );
	DAW_JSON_NS::to_json( value, out_it );
	return os;
}

template<typename Container,
         std::enable_if_t<DAW_JSON_NS::json_details::
                            is_container_opted_into_json_iostreams_v<Container>,
                          std::nullptr_t> = nullptr>
std::ostream &operator<<( std::ostream &os, Container const &c ) {
	auto out_it = std::ostreambuf_iterator<char>( os );
	*out_it++ = '[';
	auto first = std::begin( c );
	auto last = std::end( c );
	if( first != last ) {
		out_it = DAW_JSON_NS::to_json( *first, out_it );
		++first;
		while( first != last ) {
			*out_it = ',';
			out_it = DAW_JSON_NS::to_json( *first, out_it );
			++first;
		}
	}
	*out_it++ = ']';
	return os;
}
