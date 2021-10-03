// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_option_bits.h"
#include "version.h"

namespace daw::json {
	inline namespace DAW_JSON_VER DAW_ATTRIB_HIDDEN {
		/***
		 * Choose whether to produce minified or pretty output
		 */
		enum class SerializationFormat : unsigned {
			/* Output JSON without whitespace surrounding members and values */
			Minified,
			/* Output JSON with indentation and newlines */
			Pretty
		}; // 1bit

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<SerializationFormat> = 1;

			template<>
			inline constexpr auto default_json_option_value<SerializationFormat> =
			  SerializationFormat::Minified;
		} // namespace json_details

		/***
		 * When Pretty mode is enabled, choose the indentation type.  Defaults to 2
		 * spaces.
		 */
		enum class IndentationType : unsigned {
			/* Use tabs for indentation */
			Tab,
			/* Use 0 spaces for indentation */
			Space0,
			/* Use 1 space for indentation */
			Space1,
			/* Use 2 spaces for indentation */
			Space2,
			/* Use 3 spaces for indentation */
			Space3,
			/* Use 4 spaces for indentation */
			Space4,
			/* Use 5 spaces for indentation */
			Space5,
			/* Use 8 spaces for indentation */
			Space8,
		};

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<IndentationType> = 3;

			template<>
			inline constexpr auto default_json_option_value<IndentationType> =
			  IndentationType::Space2;
		} // namespace json_details

		/***
		 * Allow for restricting the output of strings to 7bits
		 */
		enum class RestrictedStringOutput : unsigned {
			/* Do not impose any extra restrictions on string output during
			   serialization */
			None,
			/* Restrict all string member values and all member names to 7bits.  This
			   will result in escaping all values >= 0x7FS.  This can affect round
			   trips where the name contains high bits set*/
			OnlyAllow7bitsStrings
		};

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<RestrictedStringOutput> =
			  1;

			template<>
			inline constexpr auto default_json_option_value<RestrictedStringOutput> =
			  RestrictedStringOutput::None;
		} // namespace json_details

		enum class NewLineDelimiter : unsigned { n, rn };

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<NewLineDelimiter> = 1;

			template<>
			inline constexpr auto default_json_option_value<NewLineDelimiter> =
			  NewLineDelimiter::n;
		} // namespace json_details

		enum class OutputTrailingComma : unsigned { No, Yes };
		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<OutputTrailingComma> = 1;

			template<>
			inline constexpr auto default_json_option_value<OutputTrailingComma> =
			  OutputTrailingComma::No;
		} // namespace json_details

	} // namespace DAW_ATTRIB_HIDDEN
} // namespace daw::json
