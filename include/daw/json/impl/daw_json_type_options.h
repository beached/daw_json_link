// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_enums.h"
#include "daw_json_option_bits.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace options {
			/// @brief Controls the ability to parse numbers that are encoded as
			/// strings.
			///
			enum class LiteralAsStringOpt : unsigned {
				/// Never allow parsing this member as a string.  It is a parser error
				/// if
				/// this member is encoded as a string
				Never,
				/// Never allow parsing this member as a string.  It is a parser error
				/// if
				/// this member is encoded as a string.
				Maybe,
				/// Only allow parsing this member as a string.  It is an error for this
				/// member to not be encoded as a string.
				Always
			}; // 2bits
		}    // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::LiteralAsStringOpt> = 2;

			template<>
			inline constexpr auto
			  default_json_option_value<options::LiteralAsStringOpt> =
			    options::LiteralAsStringOpt::Never;
		} // namespace json_details

		namespace options {
			/// @brief Control if narrowing checks are performed
			enum class JsonRangeCheck : unsigned {
				/// Do not attempt to check for narrowing when parsing integral types
				Never,
				/// Attempt to check for narrowing when parsing integral types
				CheckForNarrowing
			}; // 1bit
		}    // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::JsonRangeCheck> = 1;

			template<>
			inline constexpr auto default_json_option_value<options::JsonRangeCheck> =
			  options::JsonRangeCheck::Never;
		} // namespace json_details

		namespace options {
			/// @brief When outputting floating point numbers, control whether Inf/NaN
			/// values can be parsed/serialized.  This requires that the
			/// LiteralAsString option be set to Maybe or Always
			enum class JsonNumberErrors : unsigned {
				/// Do not allow serialization/parsing of NaN/Inf
				None,
				/// Allow NaN to be expressed/parsed if number can be a string
				AllowNaN,
				/// Allow Inf/-Inf to be expressed/parsed if number can be a string
				AllowInf,
				/// Allow NaN/Inf/-Inf to be expressed/parsed if number can be a string
				AllowNanInf
			};
		} // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::JsonNumberErrors> = 2;

			template<>
			inline constexpr auto
			  default_json_option_value<options::JsonNumberErrors> =
			    options::JsonNumberErrors::None;
		} // namespace json_details

		namespace options {
			/// @brief Control the floating point output format
			enum class FPOutputFormat : unsigned {
				/// Automatically choose between decimal and scientific output formats
				Auto,
				/// Always format in terms of an exponent <whole>[.fraction]e<exponent>
				Scientific,
				/// Always format in <whole>.<fraction>
				Decimal
			};
		} // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::FPOutputFormat> = 2;

			template<>
			inline constexpr auto default_json_option_value<options::FPOutputFormat> =
			  options::FPOutputFormat::Auto;
		} // namespace json_details

		// json_number
		using number_opts_t = json_details::JsonOptionList<
		  options::LiteralAsStringOpt, options::JsonRangeCheck,
		  options::JsonNumberErrors, options::FPOutputFormat>;

		inline constexpr auto number_opts = number_opts_t{ };
		inline constexpr json_options_t number_opts_def =
		  number_opts_t::default_option_flag;

		namespace options {
			template<typename... Options>
			constexpr json_options_t number_opt( Options... options ) {
				return number_opts_t::options( options... );
			}
		} // namespace options

		// json_bool
		using bool_opts_t =
		  json_details::JsonOptionList<options::LiteralAsStringOpt>;

		inline constexpr auto bool_opts = bool_opts_t{ };
		inline constexpr json_options_t bool_opts_def =
		  bool_opts_t::default_option_flag;

		namespace options {
			template<typename... Options>
			constexpr json_options_t bool_opt( Options... options ) {
				return bool_opts_t::options( options... );
			}
		} // namespace options

		namespace options {
			/// @brief Controls whether any string character has the high bit set. If
			/// restricted, the member will escape any character with the high bit set
			/// and when parsing will throw if the high bit is encountered.
			/// This allows 7bit JSON encoding.
			enum class EightBitModes : unsigned {
				/// Escape any character with the high bit set and throw when
				/// encountered
				/// during parse
				DisallowHigh,
				/// Allow the full 8bits in output without escaping
				AllowFull
			}; // 1bit
		}    // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<options::EightBitModes> =
			  1;

			template<>
			inline constexpr auto default_json_option_value<options::EightBitModes> =
			  options::EightBitModes::AllowFull;
		} // namespace json_details

		// json_string
		using string_opts_t = json_details::JsonOptionList<options::EightBitModes>;

		inline constexpr auto string_opts = string_opts_t{ };
		inline constexpr json_options_t string_opts_def =
		  string_opts_t::default_option_flag;

		namespace options {
			template<typename... Options>
			constexpr json_options_t string_opt( Options... options ) {
				return string_opts_t::options( options... );
			}
		} // namespace options

		namespace options {
			/// @brief In RAW String processing, if we know that there are no escaped
			/// double quotes \" we can stop at the first double quote. This allows
			/// faster string parsing
			///
			enum class AllowEscapeCharacter : unsigned {
				/// Full string processing to skip escaped characters
				Allow,
				/// There will never be a \" sequence inside the string.  This allows
				/// faster parsing
				NoEscapedDblQuote,
			};
		} // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::AllowEscapeCharacter> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::AllowEscapeCharacter> =
			    options::AllowEscapeCharacter::Allow;
		} // namespace json_details

		// json_string_raw
		using string_raw_opts_t =
		  json_details::JsonOptionList<options::EightBitModes,
		                               options::AllowEscapeCharacter>;

		inline constexpr auto string_raw_opts = string_raw_opts_t{ };
		inline constexpr json_options_t string_raw_opts_def =
		  string_raw_opts_t::default_option_flag;

		namespace options {
			template<typename... Options>
			constexpr json_options_t string_raw_opt( Options... options ) {
				return string_raw_opts_t::options( options... );
			}
		} // namespace options

		namespace options {
			/// @brief Custom JSON types can be Strings, unquoted Literals, or
			/// a mix.
			///
			enum class JsonCustomTypes : unsigned {
				///  Parser always expects a JSON string.  Will surround serialized
				///  value
				///  with double quotes
				String,
				/// Parser will expect a valid JSON literal number, bool, null
				Literal,
				/// Experimental - Parser will return any valid JSON value excluding
				/// leading whitespace. strings will be quoted.  Any is suitable for
				/// constructing a json_value to allow adhoc parsing if json_raw is not
				/// suitable
				Any
			}; // 2 bits
		}    // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::JsonCustomTypes> = 2;

			template<>
			inline constexpr auto
			  default_json_option_value<options::JsonCustomTypes> =
			    options::JsonCustomTypes::String;
		} // namespace json_details

		// json_custom
		using json_custom_opts_t =
		  json_details::JsonOptionList<options::JsonCustomTypes>;

		inline constexpr auto json_custom_opts = json_custom_opts_t{ };
		inline constexpr json_options_t json_custom_opts_def =
		  json_custom_opts_t::default_option_flag;

		namespace options {
			template<typename... Options>
			constexpr json_options_t json_custom_opt( Options... options ) {
				return json_custom_opts_t::options( options... );
			}
		} // namespace options
	}   // namespace DAW_JSON_VER
} // namespace daw::json
