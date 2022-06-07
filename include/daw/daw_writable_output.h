// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/daw_json_assert.h"
#include "daw_writable_output_fwd.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_character_traits.h>
#include <daw/daw_span.h>
#include <daw/daw_traits.h>

#include <cstdio>
#include <iostream>
#include <string>

namespace daw {
	namespace writeable_output_details {
		template<typename T>
		constexpr T *copy_to_buffer( T *buff, daw::string_view source ) {
#if defined( DAW_IS_CONSTANT_EVALUATED )
			if( DAW_IS_CONSTANT_EVALUATED( ) ) {
#endif
				daw::algorithm::transform_n( source.data( ), buff, source.size( ),
				                             []( auto c ) {
					                             return static_cast<T>( c );
				                             } );
#if defined( DAW_IS_CONSTANT_EVALUATED )
			} else {
				memcpy( buff, source.data( ), source.size( ) );
			}
#endif
			return buff + source.size( );
		}
	} // namespace writeable_output_details

	/// @brief Specialization for character pointer
	template<typename T>
	struct writable_output_trait<
	  T *, std::enable_if_t<(traits::is_character_type_v<T> or
	                         std::is_same_v<T, std::byte>)and sizeof( T ) == 1>>
	  : std::true_type {

		template<typename... StringViews>
		static constexpr void write( T *&ptr, StringViews... svs ) {
			static_assert( sizeof...( StringViews ) > 0 );
			daw_json_ensure( ptr, daw::json::ErrorReason::OutputError );
			constexpr auto writer = []( T *&p, auto sv ) {
				if( sv.empty( ) ) {
					return true;
				}
				p = writeable_output_details::copy_to_buffer( p, sv );
				return true;
			};
			(void)( writer( ptr, svs ) | ... );
		}

		static constexpr void put( T *&ptr, char c ) {
			daw_json_ensure( ptr, daw::json::ErrorReason::OutputError );
			*ptr = static_cast<T>( c );
			++ptr;
		}
	};

	/// @brief Specialization for ostream &
	template<>
	struct writable_output_trait<std::ostream &> : std::true_type {

		template<typename... StringViews>
		static inline void write( std::ostream &os, StringViews... svs ) {
			static_assert( sizeof...( StringViews ) > 0 );
			constexpr auto writer = []( std::ostream &o, auto sv ) {
				if( sv.empty( ) ) {
					return true;
				}
				o.write( sv.data( ), static_cast<std::streamsize>( sv.size( ) ) );
				daw_json_ensure( static_cast<bool>( o ),
				                 daw::json::ErrorReason::OutputError );
				return true;
			};
			(void)( writer( os, svs ) | ... );
		}

		static inline void put( std::ostream &os, char c ) {
			os.put( c );
			daw_json_ensure( static_cast<bool>( os ),
			                 daw::json::ErrorReason::OutputError );
		}
	};

	/// @brief Specialization for FILE * streams
	template<>
	struct writable_output_trait<std::FILE *> : std::true_type {

		template<typename... StringViews>
		static inline void write( std::FILE *fp, StringViews... svs ) {
			static_assert( sizeof...( StringViews ) > 0 );
			constexpr auto writer = []( std::FILE *f, auto sv ) {
				if( sv.empty( ) ) {
					return true;
				}
				auto ret = std::fwrite( sv.data( ), 1, sv.size( ), f );
				daw_json_ensure( ret == sv.size( ),
				                 daw::json::ErrorReason::OutputError );
				return true;
			};
			(void)( writer( fp, svs ) | ... );
		}

		static inline void put( std::FILE *f, char c ) {
			auto ret = std::fputc( c, f );
			daw_json_ensure( ret == c, daw::json::ErrorReason::OutputError );
		}
	};

	/// @brief Specialization for a span to a buffer with a fixed size
	template<typename T>
	struct writable_output_trait<
	  daw::span<T>,
	  std::enable_if_t<(traits::is_character_type_v<T> or
	                    std::is_same_v<T, std::byte>)and sizeof( T ) == 1>>
	  : std::true_type {
		template<typename... StringViews>
		static constexpr void write( daw::span<T> &out, StringViews... svs ) {
			static_assert( sizeof...( StringViews ) > 0 );
			daw_json_ensure( out.size( ) >= ( std::size( svs ) + ... ),
			                 daw::json::ErrorReason::OutputError );
			constexpr auto writer = []( daw::span<T> &s, auto sv ) {
				if( sv.empty( ) ) {
					return true;
				}
				(void)writeable_output_details::copy_to_buffer( s.data( ), sv );
				s.remove_prefix( sv.size( ) );
				return true;
			};
			(void)( writer( out, svs ) | ... );
		}

		static constexpr void put( span<T> &out, char c ) {
			daw_json_ensure( not out.empty( ), daw::json::ErrorReason::OutputError );
			out[0] = static_cast<T>( c );
			out.remove_prefix( );
		}
	};

	/// @brief Specialization for a span to a buffer with a fixed size
	template<typename T, typename Traits, typename Alloc>
	struct writable_output_trait<
	  std::basic_string<T, Traits, Alloc>,
	  std::enable_if_t<traits::is_character_type_v<T> and sizeof( T ) == 1>>
	  : std::true_type {

		template<typename... StringViews>
		static inline void write( std::basic_string<T, Traits, Alloc> &out,
		                          StringViews... svs ) {
			static_assert( sizeof...( StringViews ) > 0 );
			auto const start_pos = out.size( );
			auto const total_size = ( std::size( svs ) + ... );
			out.resize( start_pos + total_size );

			constexpr auto writer = []( T *&p, auto sv ) {
				if( sv.empty( ) ) {
					return true;
				}
				p = writeable_output_details::copy_to_buffer( p, sv );
				return true;
			};
			auto *ptr = out.data( ) + start_pos;
			(void)( writer( ptr, svs ) | ... );
		}

		static inline void put( std::basic_string<T, Traits, Alloc> &out, char c ) {
			out.push_back( c );
		}
	};

	namespace writeable_output_details {
		template<typename T>
		using is_writable_output_iterator_test =
		  decltype( *std::declval<T &>( ) = 'c', ++std::declval<T &>( ) );

		template<typename T>
		inline constexpr bool is_writable_output_iterator_v =
		  not std::is_pointer_v<T> and
		  daw::is_detected_v<is_writable_output_iterator_test, T>;
	} // namespace writeable_output_details

	/// @brief Specialization for output iterators
	template<typename T>
	struct writable_output_trait<
	  T, std::enable_if_t<
	       writeable_output_details::is_writable_output_iterator_v<T>>>
	  : std::true_type {

		template<typename... StringViews>
		static constexpr void write( T &it, StringViews... svs ) {
			static_assert( sizeof...( StringViews ) > 0 );

			constexpr auto writer = []( T &i, auto sv ) {
				for( char c : daw::string_view( sv ) ) {
					*i = c;
					++i;
				}
				return true;
			};
			(void)( writer( it, svs ) | ... );
		}

		static constexpr void put( T &it, char c ) {
			*it = c;
			++it;
		}
	};
} // namespace daw
