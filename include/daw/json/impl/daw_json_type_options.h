// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_enums.h"
#include "daw_json_option_bits.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/***
		 * Control whether a type can be missing or null.
		 * MustExist - members make it an error if their value is null or they are
		 * missing
		 * Nullable - members can have a value of null or be missing
		 * NullVisible - members must exist but can have a value of null
		 */
		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<JsonNullable> = 2;

			template<>
			inline constexpr auto default_json_option_value<JsonNullable> =
			  JsonNullable::MustExist;
		} // namespace json_details

		/**
		 * Allows having literals parse that are encoded as strings. It allows
		 * one to have it be Never true, Maybe true or Always true.  This controls
		 * whether the parser will Never remove quotes, check if quotes exist, or
		 * Always remove quotes around the literal
		 */
		enum class LiteralAsStringOpt : unsigned { Never, Maybe, Always }; // 2bits

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<LiteralAsStringOpt> = 2;

			template<>
			inline constexpr auto default_json_option_value<LiteralAsStringOpt> =
			  LiteralAsStringOpt::Never;
		} // namespace json_details

		/***
		 * Check if the result of a numeric parse is within the range of the type
		 */
		enum class JsonRangeCheck : unsigned { Never, CheckForNarrowing }; // 1bit

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<JsonRangeCheck> = 1;

			template<>
			inline constexpr auto default_json_option_value<JsonRangeCheck> =
			  JsonRangeCheck::Never;
		} // namespace json_details

		// json_number
		using number_opts_t =
		  json_details::JsonOptionList<JsonNullable, LiteralAsStringOpt,
		                               JsonRangeCheck>;

		inline constexpr auto number_opts = number_opts_t{ };
		inline constexpr json_details::json_options_t number_opts_def =
		  number_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t number_opt( Options... options ) {
			return number_opts_t::options( options... );
		}

		// json_bool
		using bool_opts_t =
		  json_details::JsonOptionList<JsonNullable, LiteralAsStringOpt>;

		inline constexpr auto bool_opts = bool_opts_t{ };
		inline constexpr json_details::json_options_t bool_opts_def =
		  bool_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t bool_opt( Options... options ) {
			return bool_opts_t::options( options... );
		}

		/***
		 * Treat empty string values as a null when parsing
		 */
		enum class EmptyStringNull : unsigned { no, yes }; // 1bit

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<EmptyStringNull> = 1;

			template<>
			inline constexpr auto default_json_option_value<EmptyStringNull> =
			  EmptyStringNull::no;
		} // namespace json_details

		enum class EightBitModes : unsigned {
			DisallowHigh = false,
			AllowFull = true
		}; // 1bit

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<EightBitModes> = 1;

			template<>
			inline constexpr auto default_json_option_value<EightBitModes> =
			  EightBitModes::AllowFull;
		} // namespace json_details

		// json_string
		using string_opts_t =
		  json_details::JsonOptionList<JsonNullable, EightBitModes,
		                               EmptyStringNull>;

		inline constexpr auto string_opts = string_opts_t{ };
		inline constexpr json_details::json_options_t string_opts_def =
		  string_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t string_opt( Options... options ) {
			return string_opts_t::options( options... );
		}

		/***
		 * In RAW String processing, if we know that there are no escaped double
		 * quotes \" we can stop at the first double quote
		 */
		enum class AllowEscapeCharacter : unsigned {
			Allow,             /// Full string processing to skip escaped characters
			NotBeforeDblQuote, /// There will never be a \" sequence inside the
			                   /// string.  This allows very fast parsing
		};
		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<AllowEscapeCharacter> =
			  1;

			template<>
			inline constexpr auto default_json_option_value<AllowEscapeCharacter> =
			  AllowEscapeCharacter::Allow;
		} // namespace json_details

		// json_string_raw
		using string_raw_opts_t =
		  json_details::JsonOptionList<JsonNullable, EightBitModes, EmptyStringNull,
		                               AllowEscapeCharacter>;

		inline constexpr auto string_raw_opts = string_raw_opts_t{ };
		inline constexpr json_details::json_options_t string_raw_opts_def =
		  string_raw_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t
		string_raw_opt( Options... options ) {
			return string_raw_opts_t::options( options... );
		}

		// json_class
		using class_opts_t = json_details::JsonOptionList<JsonNullable>;
		inline constexpr auto class_opts = class_opts_t{ };
		inline constexpr json_details::json_options_t class_opts_def =
		  class_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t class_opt( Options... options ) {
			return class_opts_t::options( options... );
		}

		// json_tuple
		using tuple_opts_t = json_details::JsonOptionList<JsonNullable>;
		inline constexpr auto tuple_opts = tuple_opts_t{ };
		inline constexpr json_details::json_options_t tuple_opts_def =
		  tuple_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t tuple_opt( Options... options ) {
			return tuple_opts_t::options( options... );
		}

		/***
		 * Custom JSON types can be Strings(default), unquoted Literals, or a mix
		 * String - Parser always expects a JSON string.  Will surround serialized
		 * value with double quotes
		 * Literal - Parser will expect a valid JSON literal number, bool, null
		 * Any - Experimental - Parser will return any valid JSON value excluding
		 * leading whitespace. strings will be quoted.  Any is suitable for
		 * constructing a json_value to allow adhock parsing if json_raw is not
		 * suitable
		 */
		enum class JsonCustomTypes : unsigned { String, Literal, Any }; // 2 bits

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<JsonCustomTypes> = 2;

			template<>
			inline constexpr auto default_json_option_value<JsonCustomTypes> =
			  JsonCustomTypes::String;
		} // namespace json_details

		// json_custom
		using json_custom_opts_t =
		  json_details::JsonOptionList<JsonNullable, JsonCustomTypes>;

		inline constexpr auto json_custom_opts = json_custom_opts_t{ };
		inline constexpr json_details::json_options_t json_custom_opts_def =
		  json_custom_opts_t::default_option_flag;

		template<typename... Options>
		constexpr json_details::json_options_t
		json_custom_opt( Options... options ) {
			return json_custom_opts_t::options( options... );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
