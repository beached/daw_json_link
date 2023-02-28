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
			/// @brief Options used in from_json/json_iterator/json_range to control
			/// global parsing options
			inline namespace parser_options {
				/// @brief Allow for different optimizations.  Currently only the
				/// compile_time path is fully supported. The others may offer faster
				/// parsing. The default is compile_time, it provides constexpr parsing
				/// and generally is faster currently.
				///
				/// default: compile_time
				///
				enum class ExecModeTypes : unsigned {
					/// @brief Only use code paths that can run at compile time
					compile_time,
					/// @brief *testing* Allow code paths that use non-compile time
					/// methods
					runtime,
					/// @brief *testing* Allow code paths that use SIMD intrinsics
					simd
				}; // 2bits

				///
				/// @brief Input is a zero terminated string.  If this cannot be
				/// detected, you can specify it here.  Offers some optimization
				/// possibilities in the parser. Default is no, to try and detect, but
				/// use the safer assumption that the buffer does not end in zero.
				///
				/// default: no
				enum class ZeroTerminatedString : unsigned {
					/// @brief Bounds checking does not take advantage of assuming a
					/// terminated string
					no,
					/// @brief The string passed to `from_json` is zero terminated
					yes
				}; // 1bit

				///
				/// @brief Allow comments in JSON.  The supported modes are none, C++
				/// style comments, and # hash style comments.  Default is none, no
				/// comments allowed
				///
				/// default: none
				///
				enum class PolicyCommentTypes : unsigned {
					/// @brief Do not allow comments in JSON whitespace
					none,
					/// @brief Allow C++ style and C style comments in whitespace
					cpp,
					/// @brief Allow comments starting with a hash symbol #
					hash
				}; // 2bits

				///
				/// @brief Enable all structure, buffer, and type checking.  The default
				/// is yes, but no still does some checking and can be faster.
				///
				/// default: yes
				enum class CheckedParseMode : unsigned { yes, no }; // 1bit

				///
				/// @brief When document is minified, it is assumed that there is no
				/// whitespace in
				/// the document. Not all policies support this.
				///
				/// default: no
				enum class MinifiedDocument : unsigned { no, yes }; // 1 bit

				///
				///@brief Allow the escape character '\' in names.  This forces a slower
				/// parser and is generally not needed.  The default is no, and the end
				/// of string matching only needs to look for a `"`, not skip `\"` in
				/// names.
				///
				/// default: no
				///
				enum class AllowEscapedNames : unsigned { no, yes }; // 1bit

				///
				///@note Testing
				///@brief Use precise IEEE754 parsing of real numbers.  The default is
				/// no,
				/// and results is much faster parsing with very small errors of 0-2ulp.
				///
				/// default: no
				///
				enum class IEEE754Precise : unsigned { no, yes }; // 1bit

				///
				///@brief If the hashes of all members being looked are unique, the
				/// lookup
				/// of names as they are found in the document stops at hash checking by
				/// default.  You can force a full string check by setting to yes.
				///
				/// default: no
				///
				enum class ForceFullNameCheck : unsigned { no, yes }; // 1bit

				///
				/// @brief Set the default parser policy to require all JSON members in
				/// the parsed object be mapped. See also, the ignore_unknown_members
				/// and exact_class_mapping traits to set overrides
				///
				/// default: no
				///
				enum class UseExactMappingsByDefault : unsigned { no, yes }; // 1bit

				///
				/// @brief Continue checking that the data is whitespace or empty after
				/// the end of top level object is parsed from from_json
				///
				/// default: no
				///
				enum class MustVerifyEndOfDataIsValid : unsigned { no, yes };

				///
				/// @brief Optimize name hashing for longer strings
				///
				/// default: no
				///
				enum class ExpectLongNames : unsigned { no, yes };

				///
				/// @brief When enabled, the parser can temporarily set a character to
				/// the desired token. This allows for safe searching without bounds
				/// checking. If the buffer is not mutable, it will not be enabled.
				///
				/// default: no
				///

				enum class [[deprecated(
				  "This is not used" )]] TemporarilyMutateBuffer : unsigned{
				  no, yes }; // 1bit

				///
				/// @brief Exclude characters under 0x20 that are not explicitly
				/// allowed.
				///
				/// default: no
				///
				enum class ExcludeSpecialEscapes : unsigned { no, yes }; // 1bit
			} // namespace parser_options
		}   // namespace options
	}     // namespace DAW_JSON_VER
} // namespace daw::json
