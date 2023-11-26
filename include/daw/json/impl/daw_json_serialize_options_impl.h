// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_option_bits.h"
#include <daw/json/daw_json_serialize_options.h>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename>
			inline constexpr bool is_output_option_v = false;

			template<>
			inline constexpr bool is_output_option_v<options::SerializationFormat> =
			  true;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::SerializationFormat> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::SerializationFormat> =
			    options::SerializationFormat::Minified;

			template<>
			inline constexpr bool is_output_option_v<options::IndentationType> = true;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::IndentationType> = 3;

			template<>
			inline constexpr auto
			  default_json_option_value<options::IndentationType> =
			    options::IndentationType::Space2;

			template<>
			inline constexpr bool
			  is_output_option_v<options::RestrictedStringOutput> = true;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::RestrictedStringOutput> = 2;

			template<>
			inline constexpr auto
			  default_json_option_value<options::RestrictedStringOutput> =
			    options::RestrictedStringOutput::ErrorInvalidUTF8;

			template<>
			inline constexpr bool is_output_option_v<options::NewLineDelimiter> =
			  true;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::NewLineDelimiter> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::NewLineDelimiter> =
			    options::NewLineDelimiter::n;

			template<>
			inline constexpr bool is_output_option_v<options::OutputTrailingComma> =
			  true;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::OutputTrailingComma> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::OutputTrailingComma> =
			    options::OutputTrailingComma::No;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
