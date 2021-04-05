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
	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringRaw>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringEscaped>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>, Range & );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValueArray>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Variant>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, std::size_t N,
	         typename Range, bool B>
	[[nodiscard]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>, Range & );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unknown>, Range &rng );

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
