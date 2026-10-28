// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"
#include "daw/simd/daw_simd.h"

#if defined( DAW_HAS_SIMD )

#include "daw/json/impl/daw_json_simd_iterator_bool.h"
#include "daw/json/impl/daw_json_simd_iterator_class.h"
#include "daw/json/impl/daw_json_simd_iterator_number.h"
#include "daw/json/impl/daw_json_simd_iterator_string.h"

#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_details {
			template<typename JsonMember>
			struct unknown_json_simd_block_iterator_error;

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			using json_simd_block_iterator = std::conditional_t<
			  ( JsonMember::underlying_json_type == JsonBaseParseTypes::Bool ),
			  json_simd_block_iterator_bool<JsonMember, CharT, PolicyFlags...>,
			  std::conditional_t<
			    ( JsonMember::underlying_json_type == JsonBaseParseTypes::Number ),
			    json_simd_block_iterator_number<JsonMember, CharT, PolicyFlags...>,
			    std::conditional_t<
			      ( JsonMember::underlying_json_type == JsonBaseParseTypes::String ),
			      json_simd_block_iterator_string<JsonMember, CharT, PolicyFlags...>,
			      std::conditional_t<
			        ( JsonMember::underlying_json_type == JsonBaseParseTypes::Class ),
			        json_simd_block_iterator_class<JsonMember, CharT, PolicyFlags...>,
			        unknown_json_simd_block_iterator_error<JsonMember>>>>>;
		} // namespace json_details::simd_details

		namespace experimental {
			/**
			 * Input iterator over JSON values located using native SIMD-sized
			 * classified blocks.
			 * @tparam JsonMember The JSON Link mapping used to parse each value.
			 */
			template<typename JsonMember, typename CharT = char, auto... PolicyFlags>
			using json_simd_block_iterator =
			  json_details::simd_details::json_simd_block_iterator<
			    json_details::json_deduced_type<JsonMember>, CharT, PolicyFlags...>;

			/// @brief A range of json_simd_block_iterators
			/// @tparam JsonElement Type of each element in array
			template<typename JsonElement, auto... PolicyFlags>
			struct json_simd_array_range {
				using ParsePolicy = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using iterator =
				  json_simd_block_iterator<JsonElement, char, PolicyFlags...>;

			private:
				iterator m_first{ };
				iterator m_last{ };

			public:
				explicit json_simd_array_range( ) = default;

				explicit constexpr json_simd_array_range( daw::string_view jd )
				  : m_first( std::string_view( jd.data( ), jd.size( ) ) ) {}

				explicit constexpr json_simd_array_range( daw::string_view jd,
				                                          daw::string_view start_path )
				  : m_first( std::string_view( jd.data( ), jd.size( ) ), start_path ) {}

				/// @return first item in range
				[[nodiscard]] constexpr iterator begin( ) const {
					return m_first;
				}

				/// @return one past last item in range
				[[nodiscard]] constexpr iterator end( ) const {
					return m_last;
				}

				/// @brief Are there any elements in range
				/// @return true when begin( ) == end( )
				[[nodiscard]] constexpr bool empty( ) const {
					return m_first == m_last;
				}
			};
		} // namespace experimental
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
