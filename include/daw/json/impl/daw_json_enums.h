// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstdint>
#include <string_view>
#include <utility>

namespace daw::json {
	enum class JsonParseTypes : std::uint_fast8_t {
		Real,
		Signed,
		Unsigned,
		Bool,
		StringRaw,
		StringEscaped,
		Date,
		Class,
		Array,
		FixedArray,
		Null,
		KeyValue,
		KeyValueArray,
		Custom,
		Variant,
		VariantTagged,
		Unknown
	};

	enum class JsonBaseParseTypes : std::uint_fast8_t {
		Number,
		Bool,
		String,
		Class,
		Array,
		Null,
		None
	};

	constexpr std::string_view to_string( JsonBaseParseTypes pt ) {
		switch( pt ) {
		case JsonBaseParseTypes::Number:
			return "Number";
		case JsonBaseParseTypes::Bool:
			return "Bool";
		case JsonBaseParseTypes::String:
			return "String";
		case JsonBaseParseTypes::Class:
			return "Class";
		case JsonBaseParseTypes::Array:
			return "Array";
		case JsonBaseParseTypes::Null:
			return "Null";
		case JsonBaseParseTypes::None:
		default:
			return "None";
		}
	}

	enum class JsonNullable { Never = false, Nullable = true };
	enum class JsonRangeCheck { Never = false, CheckForNarrowing = true };
	enum class EightBitModes { DisallowHigh = false, AllowFull = true };
	enum class CustomJsonTypes { Literal, String, Either };

	/***
	 * In RAW String processing, if we know that there are no escaped double
	 * quotes \" we can stop at the first double quote
	 */
	enum class AllowEscapeCharacter {
		/*Full string processing to skip escaped characters*/ Allow,
		/*There will never be a \" sequence inside the string*/ NotBeforeDblQuote
	};

	template<JsonParseTypes ParseType, JsonNullable Nullable>
	inline constexpr JsonParseTypes get_parse_type_v =
	  Nullable == JsonNullable::Never ? ParseType : JsonParseTypes::Null;

	/**
	 * Tag lookup for parsing overload selection
	 */
	template<JsonParseTypes v>
	using ParseTag = std::integral_constant<JsonParseTypes, v>;

	/**
	 * Allows having literals parse that are encoded as strings. It allows
	 * one to have it be Never true, Maybe true or Always true.  This controls
	 * whether the parser will Never remove quotes, check if quotes exist, or
	 * Always remove quotes around the literal
	 */
	enum class LiteralAsStringOpt : std::uint8_t { Never, Maybe, Always };
} // namespace daw::json
