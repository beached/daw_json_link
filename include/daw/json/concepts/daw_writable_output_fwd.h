// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../impl/version.h"

#include <daw/daw_traits.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			/// @brief Writable output models write/putc methods to allow efficient
			/// output to buffers/FILE streams/ostreams/and containers with less
			/// allocation/size checks Specializations must have static T write( T,
			/// StringViews... ), static T put( T, char ), and static bool value.
			/// StringViews work will have a .size( ) and .data( ) member function,
			/// and have a character element type
			template<typename, typename = void>
			struct writable_output_trait : std::false_type {};

			template<typename T>
			inline constexpr bool is_writable_output_type_v =
			  writable_output_trait<T>::value;
		} // namespace concepts
	}   // namespace DAW_JSON_VER
} // namespace daw::json
