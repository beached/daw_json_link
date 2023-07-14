// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace options {
			///
			/// @brief Serialization options used in to_json
			///
			inline namespace serialize_options {
				///
				/// @brief Choose whether to produce minified or pretty output
				///
				/// default: Minified
				///
				enum class SerializationFormat : unsigned {
					/* Output JSON without whitespace surrounding members and values */
					Minified,
					/* Output JSON with indentation and newlines */
					Pretty
				}; // 1bit

				///
				///@brief When Pretty mode is enabled, choose the indentation type.
				///
				/// default: Space2
				///
				enum class IndentationType : unsigned {
					/* Use tabs for indentation */
					Tab,
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
					/* Use 10 spaces for indentation */
					Space10,
				};

				///
				/// @brief Allow for restricting the output of strings to 7bits
				///
				/// default: None
				///
				enum class RestrictedStringOutput : unsigned {
					/* Do not impose any extra restrictions on string output during
					   serialization */
					None,
					/// When invalid UTF8 is encountered, throw an error
					ErrorInvalidUTF8,
					/* Restrict all string member values and all member names to 7bits.
					   This will result in escaping all values >= 0x7F.  This can affect
					   round trips where the name contains high bits set*/
					OnlyAllow7bitsStrings
				};

				///
				/// @brief When Pretty mode is enabled, set how newlines are expressed
				///
				/// default: n
				///
				enum class NewLineDelimiter : unsigned { n, rn };

				/// @brief When outputting arrays, follow final item with a comma
				///
				/// default: No
				///
				enum class OutputTrailingComma : unsigned { No, Yes };
			} // namespace serialize_options
		}   // namespace options
	}     // namespace DAW_JSON_VER
} // namespace daw::json
