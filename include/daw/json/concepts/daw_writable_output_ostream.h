// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/impl/version.h>

#include "daw_writable_output_fwd.h"
#include <daw/json/impl/daw_json_assert.h>

#include <daw/daw_algorithm.h>
#include <daw/daw_character_traits.h>

#include <iostream>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			/// @brief Specialization for ostream &
			template<typename T>
			DAW_JSON_REQUIRES( std::is_base_of_v<std::ostream, T> )
			struct writable_output_trait<T DAW_JSON_ENABLEIF_S(
			  std::is_base_of_v<std::ostream, T> )> : std::true_type {

				template<typename... StringViews>
				static inline void write( std::ostream &os,
				                          StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );
					constexpr auto writer = []( std::ostream &o,
					                            auto sv ) DAW_JSON_CPP23_STATIC_CALL_OP {
						if( sv.empty( ) ) {
							return 0;
						}
						o.write( sv.data( ), static_cast<std::streamsize>( sv.size( ) ) );
						daw_json_ensure( static_cast<bool>( o ),
						                 daw::json::ErrorReason::OutputError );
						return 1;
					};
					(void)( writer( os, svs ), ... );
				}

				static inline void put( std::ostream &os, char c ) {
					os.put( c );
					daw_json_ensure( static_cast<bool>( os ),
					                 daw::json::ErrorReason::OutputError );
				}
			};
		} // namespace concepts
	} // namespace DAW_JSON_VER
} // namespace daw::json
