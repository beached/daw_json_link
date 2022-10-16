// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_parse_common.h"

#include <ciso646>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Real> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Signed> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::Unsigned> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Null> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Bool> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::StringRaw> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::StringEscaped> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Date> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Custom> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Class> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::KeyValue> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::KeyValueArray> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Array> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::SizedArray> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Variant> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::VariantTagged> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::VariantIntrusive> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Tuple> );

			template<typename JsonMember, bool KnownBounds = false,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Unknown> );

		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
