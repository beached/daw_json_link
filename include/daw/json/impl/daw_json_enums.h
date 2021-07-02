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
			SizedArray,
			Null,
			KeyValue,
			KeyValueArray,
			Custom,
			Variant,
			VariantTagged,
			Tuple,
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
		enum class JsonNullable : unsigned { MustExist, Nullable, NullVisible };

		/***
		 * When not MustExist, the default Null type for unspecified _null json
		 * types
		 */
		inline constexpr JsonNullable JsonNullDefault = JsonNullable::Nullable;

		namespace json_details {
			template<JsonNullable nullable>
			using is_nullable_json_value =
			  std::bool_constant<nullable != JsonNullable::MustExist>;

			template<JsonNullable nullable>
			inline constexpr bool is_nullable_json_value_v =
			  is_nullable_json_value<nullable>::value;
		} // namespace json_details

		inline namespace details {
			template<JsonParseTypes ParseType, JsonNullable Nullable>
			inline constexpr JsonParseTypes get_parse_type_v =
			  json_details::is_nullable_json_value_v<Nullable> ? JsonParseTypes::Null
			                                                   : ParseType;
		} // namespace details

		/**
		 * Tag lookup for parsing overload selection
		 */
		template<JsonParseTypes v>
		using ParseTag = std::integral_constant<JsonParseTypes, v>;

	} // namespace DAW_JSON_VER
} // namespace daw::json
