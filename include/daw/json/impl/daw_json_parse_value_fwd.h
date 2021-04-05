// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_parse_common.h"
#include "daw_json_value.h"
#include "namespace.h"

#include <ciso646>

namespace DAW_JSON_NS::json_details {
	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringRaw>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringEscaped>,
	             ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>, ParseState & );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValueArray>,
	             ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Variant>, ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>,
	             ParseState &parse_state );

	template<typename JsonMember, bool KnownBounds = false, std::size_t N,
	         typename ParseState, bool B>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>, ParseState & );

	template<typename JsonMember, bool KnownBounds = false, typename ParseState>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unknown>, ParseState &parse_state );

#if( defined( _MSC_VER ) and not defined( __clang__ ) )
	// Lying to MSVC about being a random iterator causes issues I have not found
	// yet
	template<bool>
	inline constexpr bool can_random_v = false;
#else
	template<bool IsKnown>
	inline constexpr bool can_random_v = IsKnown;
#endif
} // namespace DAW_JSON_NS::json_details
