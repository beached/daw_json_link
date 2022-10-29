// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "../impl/version.h"

#include "daw_container_traits_fwd.h"

#include <daw/cpp_17.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			template<typename T>
			concept Container = requires( T &t ) {
				                    std::begin( t );
				                    std::end( t );
				                    typename T::value_type;
				                    requires requires( typename T::value_type v ) {
					                             t.insert( std::end( t ), v );
				                             };
			                    };

			template<typename T>
			requires( Container<T> )
			struct container_traits<T> : std::true_type {};

			template<typename T, std::size_t N>
			struct container_traits<std::array<T, N>> : std::true_type {
				static constexpr bool has_custom_constructor = true;

				/*
				template<typename Iterator>
				static constexpr std::array<T, N> construct( Iterator first, Iterator
				last ) {

				}*/
			};

			/// @brief Is the type deduced or specialized as a container
			template<typename T>
			inline constexpr bool is_container_v = container_traits<T>::value;
		} // namespace concepts
	}   // namespace DAW_JSON_VER
} // namespace daw::json
