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
#include <daw/daw_string_view.h>

#include <cstdio>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			/// @brief Specialization for FILE * streams
			template<>
			struct writable_output_trait<std::FILE *> : std::true_type {

				template<typename... StringViews>
				static inline void write( std::FILE *fp, StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );
					constexpr auto writer = []( std::FILE *f,
					                            auto sv ) DAW_JSON_CPP23_STATIC_CALL_OP {
						if( sv.empty( ) ) {
							return 0;
						}
						auto ret = std::fwrite( sv.data( ), 1, sv.size( ), f );
						daw_json_ensure( ret == sv.size( ),
						                 daw::json::ErrorReason::OutputError );
						return 1;
					};
					(void)( writer( fp, svs ), ... );
				}

				static inline void put( std::FILE *f, char c ) {
					auto ret = std::fputc( c, f );
					daw_json_ensure( ret == c, daw::json::ErrorReason::OutputError );
				}
			};
		} // namespace concepts
	} // namespace DAW_JSON_VER
} // namespace daw::json