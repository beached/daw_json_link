// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_iterator_range.h"
#include "daw_json_parse_common.h"

namespace daw::json::json_details {

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringRaw>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringEscaped>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>, Range & );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValueArray>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Variant>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>, Range &rng );
} // namespace daw::json::json_details
