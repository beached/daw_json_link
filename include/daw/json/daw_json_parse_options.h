// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace options {
			/// @brief Options used in from_json/json_iterator/json_range to control
			/// global parsing options
			inline namespace parser_options {
				/// @brief Select the code paths used when parsing.  When the compiler
				/// can detect constant evaluation, default_mode, runtime and simd all
				/// use the constexpr code paths during constant evaluation and the
				/// fastest available ones, including SIMD, at runtime.  compile_time
				/// always uses the constexpr code paths, even at runtime.  Without
				/// detection, default_mode and compile_time use the constexpr code
				/// paths, runtime adds the non-constexpr ones and simd adds SIMD.
				///
				/// default: default_mode
				///
				enum class ExecModeTypes : unsigned {
					/// @brief Use the best available code path that can be detected
					default_mode,
					/// @brief Only use code paths that can run at compile time, even at
					/// runtime
					compile_time,
					/// @brief Allow code paths that use non-compile time methods.  Only
					/// differs from default_mode when constant evaluation cannot be
					/// detected
					runtime,
					/// @brief Allow code paths that use SIMD intrinsics.  Only differs
					/// from default_mode when constant evaluation cannot be detected
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

				enum class AllowStringMutation : unsigned {
					/// @brief String is not allowed to be mutated.
					no,
					/// @brief String maybe mutated during parsing.  This is useful for
					/// json_string_insitu
					yes
				}; // 1 bit

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
				///@brief Use precise IEEE754 parsing of real numbers.  When yes, float
				/// and double are correctly rounded using Eisel-Lemire, with a fallback
				/// for the rare cases it cannot resolve.  When no, parsing is faster
				/// but results can have small errors of 0-2ulp.
				///
				/// default: yes
				///
				enum class IEEE754Precise : unsigned { no, yes }; // 1bit

				///
				///@brief No longer has an effect, member names are always matched
				/// exactly.  Kept so that existing code compiles
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
				  "This is not used" )]] TemporarilyMutateBuffer : unsigned {
					no,
					yes
				}; // 1bit

				///
				/// @brief Exclude characters under 0x20 that are not explicitly
				/// allowed.
				///
				/// default: no
				///
				enum class ExcludeSpecialEscapes : unsigned { no, yes }; // 1bit

				///
				/// @brief Elements to reserve up front for arrays parsed into
				/// std::vector when their size is not known.  When a parser already
				/// knows an array's element count, e.g. it was skipped before being
				/// parsed, that count can be used to reserve the exact size instead.
				///
				/// default: none
				///
				enum class ArrayReserve : unsigned {
					none,
					elements_128,
					elements_512,
					elements_1024
				}; // 4bits

				///
				/// @brief How the SIMD parsers, from_json_simd and
				/// json_simd_block_iterator, check the classes and arrays they skip.
				/// With no, like simdjson's On-Demand, only the depth is counted: {
				/// and [ add one, } and ] take one away.  With yes, the bracket
				/// kinds, member names, colons and commas are checked too, as is the
				/// spelling of skipped numbers and literals, and nesting is limited to
				/// DAW_JSON_STRICT_SKIP_MAX_DEPTH (64 by default).  As in the checked
				/// scalar skips, which have the same depth limit, skipped numbers may
				/// have a leading + or leading zeros.  The contents of skipped strings
				/// are not checked either way
				///
				/// default: no
				///
				enum class StrictSkipChecks : unsigned { no, yes }; // 1bit
			} // namespace parser_options
		} // namespace options
	} // namespace DAW_JSON_VER
} // namespace daw::json
