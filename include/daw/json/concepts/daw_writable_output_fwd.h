// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/impl/version.h>

#include <daw/daw_traits.h>

#include <cstddef>
#include <type_traits>
#include <utility>

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
			DAW_CPP20_CONCEPT is_writable_output_type_v =
			  writable_output_trait<T>::value;
		} // namespace concepts

		template<typename WritableOutput, typename... StringViews>
		constexpr WritableOutput &write_output( WritableOutput &out,
		                                        StringViews const &...svs ) {
			concepts::writable_output_trait<WritableOutput>::write( out, svs... );
			return out;
		}

		template<typename WritableOutput, typename CharT>
		constexpr WritableOutput &put_output( WritableOutput &out, CharT c ) {
			concepts::writable_output_trait<WritableOutput>::put( out, c );
			return out;
		}

		namespace concepts {
			/// @brief Writable outputs may optionally have a static
			/// reserve( T &, std::size_t ) to make room for that many more bytes
			template<typename WritableOutput, typename = void>
			inline constexpr bool has_output_reserve_v = false;

			template<typename WritableOutput>
			inline constexpr bool has_output_reserve_v<
			  WritableOutput,
			  std::void_t<decltype( writable_output_trait<WritableOutput>::reserve(
			    std::declval<WritableOutput &>( ), std::size_t{ } ) )>> = true;
		} // namespace concepts

		/// @brief Hint that about n more bytes will be written.  Outputs without
		/// a reserve ignore it, and writes still check their own room
		template<typename WritableOutput>
		constexpr WritableOutput &reserve_output( WritableOutput &out,
		                                          std::size_t n ) {
			if constexpr( concepts::has_output_reserve_v<WritableOutput> ) {
				concepts::writable_output_trait<WritableOutput>::reserve( out, n );
			}
			return out;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
