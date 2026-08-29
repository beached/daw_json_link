// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/daw_json_simd.h"
#include "daw/json/impl/version.h"

#if defined( DAW_JSON_HAS_SIMD )

#include "daw/json/impl/daw_json_simd_iterator_bool.h"
#include "daw/json/impl/daw_json_simd_iterator_class.h"
#include "daw/json/impl/daw_json_simd_iterator_number.h"
#include "daw/json/impl/daw_json_simd_iterator_string.h"

#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_details {
			template<typename JsonMember>
			struct unknown_json_simd_block_iterator_error;
		} // namespace json_details::simd_details

		inline namespace experimental {
			/**
			 * Input iterator over JSON values located using native SIMD-sized
			 * classified blocks.
			 * @tparam JsonMember The JSON Link mapping used to parse each value.
			 */
			template<typename JsonMember, typename CharT = char, auto... PolicyFlags>
			using json_simd_block_iterator = std::conditional_t<
			  ( JsonMember::underlying_json_type == JsonBaseParseTypes::Bool ),
			  json_details::simd_details::json_simd_block_iterator_bool<
			    JsonMember, CharT, PolicyFlags...>,
			  std::conditional_t<
			    ( JsonMember::underlying_json_type == JsonBaseParseTypes::Number ),
			    json_details::simd_details::json_simd_block_iterator_number<
			      JsonMember, CharT, PolicyFlags...>,
			    std::conditional_t<
			      ( JsonMember::underlying_json_type == JsonBaseParseTypes::String ),
			      json_details::simd_details::json_simd_block_iterator_string<
			        JsonMember, CharT, PolicyFlags...>,
			      std::conditional_t<
			        ( JsonMember::underlying_json_type == JsonBaseParseTypes::Class ),
			        json_details::simd_details::json_simd_block_iterator_class<
			          JsonMember, CharT, PolicyFlags...>,
			        json_details::simd_details::unknown_json_simd_block_iterator_error<
			          JsonMember>>>>>;
		} // namespace experimental
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
