// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_constant.h>

#include <cstdint>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// The tags used by the parser to determine what parser to call.
		enum class JsonParseTypes : std::uint_fast8_t {
			Real,          /// Number - Floating Point
			Signed,        /// Number - Signed Integer
			Unsigned,      /// Number - Unsigned Integer
			Bool,          /// Boolean
			StringRaw,     /// String - A raw string as is.  Escapes are left in.
			StringEscaped, /// String - Fully processed string
			Date,          /// ISO 8601 Timestamp
			Class,         /// A class type with named members
			Array,         /// An array type with homogenous members
			SizedArray,    /// An array with a fixed size.  This allows for size_t/ptr
			               /// like combinations
			Null,          /// A null member
			KeyValue,      /// Class - Member names form the string key into a
			               /// key/value,map, or dictionary like type
			KeyValueArray, /// Array - Each element has a key and a value submember
			Custom,        /// Can be a literal, string, or either and allows for some
			               /// customized parsing
			Variant,       /// Any one of the basic json types
			               /// class/array/boolean/number/string
			VariantTagged, /// A variant with up to N types and a Switcher callable
			               /// that takes uses another member to determine what
			               /// type to parse.  This is often a type/value like pair
			VariantIntrusive, /// A variant type where the Switcher is based on a
			                  /// submember of the class being parsed
			Tuple,   /// Array - An array type where each element is mapped to the
			         /// member of a C++ class
			Unknown, /// Unknown type to parse.  This is used in raw processing
		};

		/// The fundamental JSON types
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

		///
		/// @brief Control how json_nullable members are serialized
		enum class JsonNullable : unsigned {
			/// @brief Default null property.  When serializing member isn't
			/// serialized
			Nullable,
			/// @brief Always serialize member and output null when value is not
			/// engaged
			NullVisible
		};

		/**
		 * Tag lookup for parsing overload selection
		 */
		template<JsonParseTypes v>
		using ParseTag = daw::constant<v>;

	} // namespace DAW_JSON_VER
} // namespace daw::json
