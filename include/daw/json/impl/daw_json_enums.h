// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <cstdint>
#include <string_view>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {
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

		/***
		 * Control whether a type can be missing or null.
		 * MustExist - members make it an error if their value is null or they are
		 * missing
		 * Nullable - members can have a value of null or be missing
		 * NullVisible - members must exist but can have a value of null
		 */
		enum class JsonNullable { MustExist, Nullable, NullVisible };

		template<JsonNullable nullable>
		inline constexpr bool is_nullable_json_value_v =
		  nullable != JsonNullable::MustExist;

		/***
		 * When not MustExist, the default Null type for unspecified _null json
		 * types
		 */
		inline constexpr JsonNullable JsonNullDefault = JsonNullable::Nullable;
		/***
		 * Check if the result of a numeric parse is within the range of the type
		 */
		enum class JsonRangeCheck { Never = false, CheckForNarrowing = true };

		enum class EightBitModes { DisallowHigh = false, AllowFull = true };

		/***
		 * Custom JSON types can be Strings(default), unquoted Literals, or a mix
		 */
		enum class CustomJsonTypes { String, Literal, Either };

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
		  is_nullable_json_value_v<Nullable> ? JsonParseTypes::Null : ParseType;

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
	} // namespace DAW_JSON_VER
} // namespace daw::json
