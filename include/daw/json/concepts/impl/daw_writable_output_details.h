// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/impl/version.h>

#include <daw/json/impl/daw_json_assert.h>

#include <daw/daw_algorithm.h>
#include <daw/daw_character_traits.h>
#include <daw/daw_string_view.h>

#include <cstdio>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts::writeable_output_details {
			template<typename T>
			constexpr T *copy_to_buffer( T *buff, daw::string_view source ) {
#if defined( DAW_IS_CONSTANT_EVALUATED )
				if( DAW_IS_CONSTANT_EVALUATED( ) ) {
#endif
					daw::algorithm::transform_n( source.data( ), buff, source.size( ),
					                             []( auto c )
					                               DAW_JSON_CPP23_STATIC_CALL_OP {
						                               return static_cast<T>( c );
					                               } );
#if defined( DAW_IS_CONSTANT_EVALUATED )
				} else {
					memcpy( buff, source.data( ), source.size( ) );
				}
#endif
				return buff + source.size( );
			}

			template<typename T>
			inline constexpr bool is_byte_type_v =
			  std::is_same_v<T, std::byte> or std::is_same_v<T, unsigned char>;

			template<typename T>
			inline constexpr bool is_char_sized_character_v = false;

			template<>
			inline constexpr bool is_char_sized_character_v<char> = true;

#if defined( __cpp_lib_char8_t )
#if __cpp_lib_char8_t >= 201907L
			template<>
			inline constexpr bool is_char_sized_character_v<char8_t> = true;
#endif
#endif

			template<typename T, typename CharT>
			using span_like_range_test =
			  decltype( (void)( std::declval<T &>( ).subspan( 1 ) ),
			            (void)( std::declval<std::size_t &>( ) =
			                      std::declval<T &>( ).size( ) ),
			            (void)( std::declval<bool &>( ) =
			                      std::declval<T &>( ).empty( ) ),
			            (void)( *std::declval<T &>( ).data( ) =
			                      std::declval<CharT>( ) ) );
			template<typename T, typename CharT>
			inline constexpr bool is_span_like_range_v =
			  daw::is_detected_v<span_like_range_test, T, CharT> and
			  ( writeable_output_details::is_char_sized_character_v<CharT> or
			    writeable_output_details::is_byte_type_v<CharT> );

			template<typename T, typename CharT>
			using resizable_contiguous_range_test =
			  decltype( (void)( std::declval<T &>( ).resize( std::size_t{ 0 } ) ),
			            (void)( std::declval<T &>( ).size( ) ),
			            (void)( *std::declval<T &>( ).data( ) ),
			            (void)( *std::declval<T &>( ).data( ) =
			                      std::declval<CharT>( ) ),
			            (void)( std::declval<T &>( ).push_back(
			              std::declval<CharT>( ) ) ),
			            (void)( static_cast<CharT>( 'a' ) ) );

			template<typename Container, typename CharT>
			inline constexpr bool is_resizable_contiguous_range_v =
			  daw::is_detected_v<resizable_contiguous_range_test, Container, CharT>;

			template<typename Container, typename CharT>
			inline constexpr bool is_string_like_writable_output_v =
			  (writeable_output_details::is_char_sized_character_v<CharT> or
			   writeable_output_details::is_byte_type_v<
			     CharT>)and writeable_output_details::
			    is_resizable_contiguous_range_v<Container, CharT>;

			template<typename T>
			using is_writable_output_iterator_test =
			  decltype( *std::declval<T &>( ) = 'c', ++std::declval<T &>( ) );

			template<typename T>
			inline constexpr bool is_writable_output_iterator_v =
			  not std::is_pointer_v<T> and
			  daw::is_detected_v<is_writable_output_iterator_test, T>;
		} // namespace concepts::writeable_output_details
	} // namespace DAW_JSON_VER
} // namespace daw::json