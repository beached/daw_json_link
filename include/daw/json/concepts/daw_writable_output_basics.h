// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/impl/version.h>

#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw_writable_output_fwd.h"
#include "impl/daw_writable_output_details.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_character_traits.h>
#include <daw/daw_string_view.h>

#include <cstdio>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			/// @brief Specialization for character pointer
			template<typename T>
			DAW_JSON_REQUIRES(
			  writeable_output_details::is_char_sized_character_v<T> or
			  writeable_output_details::is_byte_type_v<T> )
			struct writable_output_trait<
			  T * DAW_JSON_ENABLEIF_S(
			        writeable_output_details::is_char_sized_character_v<T> or
			        writeable_output_details::is_byte_type_v<T> )> : std::true_type {

				template<typename... StringViews>
				static constexpr void write( T *&ptr, StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );
					daw_json_ensure( ptr, daw::json::ErrorReason::OutputError );
					constexpr auto writer = []( T *&p,
					                            auto sv ) DAW_JSON_CPP23_STATIC_CALL_OP {
						if( sv.empty( ) ) {
							return 0;
						}
						p = writeable_output_details::copy_to_buffer( p, sv );
						return 1;
					};
					(void)( writer( ptr, svs ), ... );
				}

				static constexpr void put( T *&ptr, char c ) {
					daw_json_ensure( ptr, daw::json::ErrorReason::OutputError );
					*ptr = static_cast<T>( c );
					++ptr;
				}
			};

			/// @brief Specialization for a span to a buffer with a fixed size
			template<typename T>
			DAW_JSON_REQUIRES( writeable_output_details::is_span_like_range_v<
			                   T, typename T::value_type> )
			struct writable_output_trait<
			  T DAW_JSON_ENABLEIF_S( writeable_output_details::is_span_like_range_v<
			                         T, typename T::value_type> )> : std::true_type {
				using CharT = typename T::value_type;

				template<typename... StringViews>
				static constexpr void write( T &out, StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );
					daw_json_ensure( out.size( ) >= ( std::size( svs ) + ... ),
					                 daw::json::ErrorReason::OutputError );
					constexpr auto writer = []( T &s,
					                            auto sv ) DAW_JSON_CPP23_STATIC_CALL_OP {
						if( sv.empty( ) ) {
							return 0;
						}
						(void)writeable_output_details::copy_to_buffer( s.data( ), sv );
						s = s.subspan( sv.size( ) );
						return 1;
					};
					(void)( writer( out, svs ), ... );
				}

				static constexpr void put( T &out, char c ) {
					daw_json_ensure( not out.empty( ),
					                 daw::json::ErrorReason::OutputError );
					*out.data( ) = static_cast<CharT>( c );
					out.remove_prefix( 1 );
				}
			};

			/// @brief Specialization for a resizable container like vector/string
			template<typename Container>
			DAW_JSON_REQUIRES(
			  writeable_output_details::is_string_like_writable_output_v<
			    Container, typename Container::value_type> )
			struct writable_output_trait<Container DAW_JSON_ENABLEIF_S(
			  writeable_output_details::is_string_like_writable_output_v<
			    Container, typename Container::value_type> )> : std::true_type {
				using CharT = typename Container::value_type;

				template<typename... StringViews>
				static inline void write( Container &out, StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );
					auto const start_pos = out.size( );
					auto const total_size = ( std::size( svs ) + ... );
					out.resize( start_pos + total_size );

					constexpr auto writer = []( CharT *&p,
					                            auto sv ) DAW_JSON_CPP23_STATIC_CALL_OP {
						if( sv.empty( ) ) {
							return 0;
						}
						p = writeable_output_details::copy_to_buffer( p, sv );
						return 1;
					};
					auto *ptr = out.data( ) + start_pos;
					(void)( writer( ptr, svs ), ... );
				}

				static inline void put( Container &out, char c ) {
					out.push_back( static_cast<CharT>( c ) );
				}
			};

			/// @brief Specialization for output iterators
			template<typename T>
			DAW_JSON_REQUIRES(
			  writeable_output_details::is_writable_output_iterator_v<T> )
			struct writable_output_trait<T DAW_JSON_ENABLEIF_S(
			  writeable_output_details::is_writable_output_iterator_v<T> )>
			  : std::true_type {

				template<typename... StringViews>
				static constexpr void write( T &it, StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );

					constexpr auto writer = []( T &i, auto sv )
					                          DAW_JSON_CPP23_STATIC_CALL_OP {
						                          for( char c : daw::string_view( sv ) ) {
							                          *i = c;
							                          ++i;
						                          }
						                          return 0;
					                          };
					(void)( writer( it, svs ) | ... );
				}

				static constexpr void put( T &it, char c ) {
					*it = c;
					++it;
				}
			};
		} // namespace concepts
	} // namespace DAW_JSON_VER
} // namespace daw::json