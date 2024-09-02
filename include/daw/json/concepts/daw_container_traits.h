// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/impl/version.h>

#include "daw_container_traits_fwd.h"

#include <daw/cpp_17.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			namespace container_detect {
				DAW_JSON_MAKE_REQ_TRAIT(
				  is_container_v, (void)( std::begin( std::declval<T &>( ) ) ),
				  (void)( std::end( std::declval<T &>( ) ) ),
				  (void)( std::declval<typename T::value_type>( ) ),
				  (void)( std::declval<T &>( ).insert(
				    std::end( std::declval<T &>( ) ),
				    std::declval<typename T::value_type>( ) ) ) );
			} // namespace container_detect

			template<typename T>
			DAW_JSON_REQUIRES( container_detect::is_container_v<T> )
			struct container_traits<T DAW_JSON_ENABLEIF_S(
			  container_detect::is_container_v<T> )> : std::true_type {};

			template<typename T, std::size_t N>
			struct container_traits<std::array<T, N>> : std::true_type {};

			/// @brief Is the type deduced or specialized as a container
			template<typename T>
			inline constexpr bool is_container_v = container_traits<T>::value;
		} // namespace concepts
	} // namespace DAW_JSON_VER
} // namespace daw::json
