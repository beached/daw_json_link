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

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		/***
		 * Allow for different optimizations.  Currently only the compile_time path
		 * is fully supported. The others may offer faster parsing. The default is
		 * compile_time, it provides constexpr parsing and generally is faster
		 * currently.
		 */
		enum class ExecModeTypes : json_details::json_options_t {
			compile_time,
			runtime, /* testing */
			simd     /* testing */
		};         // 2bits

		namespace json_details {
			template<>
			inline constexpr json_options_t json_option_bits_width<ExecModeTypes> = 2;

			template<>
			inline constexpr auto default_json_option_value<ExecModeTypes> =
			  ExecModeTypes::compile_time;
		} // namespace json_details

		enum class ValidateUTF8 : json_details::json_options_t { no, yes };
		namespace json_details {
			template<>
			inline constexpr json_options_t json_option_bits_width<ValidateUTF8> = 1;

			template<>
			inline constexpr auto default_json_option_value<ValidateUTF8> =
			  ValidateUTF8::no;
		} // namespace json_details

		/***
		 * Input is a zero terminated string.  If this cannot be detected, you can
		 * specify it here.  Offers some optimization possibilities in the parser.
		 * Default is no, to try and detect, but use the safer assumption that the
		 * buffer does not end in zero.
		 */
		enum class ZeroTerminatedString : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<ZeroTerminatedString> = 1;

			template<>
			inline constexpr auto default_json_option_value<ZeroTerminatedString> =
			  ZeroTerminatedString::no;
		} // namespace json_details

		/***
		 * Allow comments in JSON.  The supported modes are none, C++ style
		 * comments, and # hash style comments.  Default is none, no comments
		 * allowed
		 */
		enum class PolicyCommentTypes : json_details::json_options_t {
			none,
			cpp,
			hash
		}; // 2bits
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<PolicyCommentTypes> = 2;

			template<>
			inline constexpr auto default_json_option_value<PolicyCommentTypes> =
			  PolicyCommentTypes::none;
		} // namespace json_details

		/***
		 * If set to AllowFull, will use the per string settings.  If Disallow high
		 * then high eight bits are filtered for all strings
		 */
		enum class GlobalEightBitModes : json_details::json_options_t {
			DisallowHigh,
			AllowFull
		};

		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<GlobalEightBitModes> = 1;

			template<>
			inline constexpr auto default_json_option_value<GlobalEightBitModes> =
			  GlobalEightBitModes::AllowFull;
		} // namespace json_details

		/***
		 * Enable all structure, buffer, and type checking.  The default is yes, but
		 * no still does some checking and can be faster.
		 */
		enum class CheckedParseMode : json_details::json_options_t {
			yes,
			no
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t json_option_bits_width<CheckedParseMode> =
			  1;

			template<>
			inline constexpr auto default_json_option_value<CheckedParseMode> =
			  CheckedParseMode::yes;
		} // namespace json_details

		/***
		 * When document is minified, it is assumed that there is no whitespace in
		 * the document. Not all policies support this.
		 */
		enum class MinifiedDocument : json_details::json_options_t {
			no,
			yes
		}; // 1 bit
		namespace json_details {
			template<>
			inline constexpr json_options_t json_option_bits_width<MinifiedDocument> =
			  1;

			template<>
			inline constexpr auto default_json_option_value<MinifiedDocument> =
			  MinifiedDocument::no;
		} // namespace json_details

		/***
		 * Allow the escape character '\' in names.  This forces a slower parser and
		 * is generally not needed.  The default is no, and the end of string
		 * matching only needs to look for a `"`, not skip `\"` in names.
		 */
		enum class AllowEscapedNames : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<AllowEscapedNames> = 1;

			template<>
			inline constexpr auto default_json_option_value<AllowEscapedNames> =
			  AllowEscapedNames::no;
		} // namespace json_details

		/***
		 * Testing
		 * Use precise IEEE754 parsing of real numbers.  The default is no, and
		 * results is much faster parsing with very small errors of 0-2ulp.
		 */
		enum class IEEE754Precise : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t json_option_bits_width<IEEE754Precise> =
			  1;

			template<>
			inline constexpr auto default_json_option_value<IEEE754Precise> =
			  IEEE754Precise::no;
		} // namespace json_details

		/***
		 * If the hashes of all members being looked are unique, the lookup of names
		 * as they are found in the document stops at hash checking by default.  You
		 * can force a full string check by setting to yes.
		 */
		enum class ForceFullNameCheck : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<ForceFullNameCheck> = 1;

			template<>
			inline constexpr auto default_json_option_value<ForceFullNameCheck> =
			  ForceFullNameCheck::no;
		} // namespace json_details
		/***
		 * Set the default parser policy to require all JSON members in the parsed
		 * object be mapped. See also, the ignore_unknown_members and
		 * exact_class_mapping traits to set overrides
		 */
		enum class UseExactMappingsByDefault : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<UseExactMappingsByDefault> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<UseExactMappingsByDefault> =
			    UseExactMappingsByDefault::no;
		} // namespace json_details

		/***
		 * Continue checking that the data is whitespace or empty after the end of
		 * top level object is parsed from from_json
		 */
		enum class MustVerifyEndOfDataIsValid : json_details::json_options_t {
			no,
			yes
		};
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<MustVerifyEndOfDataIsValid> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<MustVerifyEndOfDataIsValid> =
			    MustVerifyEndOfDataIsValid::no;
		} // namespace json_details

		enum class ExpectLongStrings : json_details::json_options_t { no, yes };
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<ExpectLongStrings> = 1;

			template<>
			inline constexpr auto default_json_option_value<ExpectLongStrings> =
			  ExpectLongStrings::no;
		} // namespace json_details

		/***
		 * When enabled, the parser can temporarily set a character to the desired
		 * token. This allows for safe searching without bounds checking.  If the
		 * buffer is not mutable, it will not be enabled.
		 */
		enum class TemporarilyMutateBuffer : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<TemporarilyMutateBuffer> = 1;

			template<>
			inline constexpr auto default_json_option_value<TemporarilyMutateBuffer> =
			  TemporarilyMutateBuffer::no;
		} // namespace json_details

		/***
		 * Exclude characters under 0x20 that are not explicitly allowed.
		 */
		enum class ExcludeSpecialEscapes : json_details::json_options_t {
			no,
			yes
		}; // 1bit
		namespace json_details {
			template<>
			inline constexpr json_options_t
			  json_option_bits_width<ExcludeSpecialEscapes> = 1;

			template<>
			inline constexpr auto default_json_option_value<ExcludeSpecialEscapes> =
			  ExcludeSpecialEscapes::no;
		} // namespace json_details
		/* *****************************************
		 * Implementation details
		 */
		namespace json_details {
			using parse_policy_list =
			  typename option_list_impl<ExecModeTypes,
			                            ZeroTerminatedString,
			                            PolicyCommentTypes,
			                            CheckedParseMode,
			                            AllowEscapedNames,
			                            IEEE754Precise,
			                            ForceFullNameCheck,
			                            MinifiedDocument,
			                            UseExactMappingsByDefault,
			                            TemporarilyMutateBuffer,
			                            MustVerifyEndOfDataIsValid,
			                            ExcludeSpecialEscapes,
			                            ExpectLongStrings,
			                            GlobalEightBitModes,
			                            ValidateUTF8>::type;

			template<typename Policy>
			inline constexpr json_options_t parse_policy_bits_start =
			  basic_policy_bits_start<Policy, parse_policy_list>;

			template<typename Policy>
			constexpr void set_bits_in( json_options_t &value, Policy e ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<json_options_t>( e );
				constexpr json_options_t mask =
				  (1U << json_option_bits_width<Policy>)-1U;
				new_bits &= mask;
				new_bits <<= parse_policy_bits_start<Policy>;
				value &= ~mask;
				value |= new_bits;
			}

			template<typename Policy, typename... Policies>
			constexpr json_options_t
			set_bits( json_options_t value, Policy pol, Policies... pols ) {
				static_assert( ( is_option_flag<Policies> and ... ),
				               "Only registered policy types are allowed" );

				auto new_bits = static_cast<json_options_t>( pol );
				constexpr json_options_t mask =
				  ( (1U << json_option_bits_width<Policy>)-1U );
				new_bits &= mask;
				new_bits <<= parse_policy_bits_start<Policy>;
				value &= ~( mask << parse_policy_bits_start<Policy> );
				value |= new_bits;
				if constexpr( sizeof...( Policies ) > 0 ) {
					if constexpr( sizeof...( pols ) > 0 ) {
						return set_bits( value, pols... );
					} else {
						return value;
					}
				} else {
					return value;
				}
			}

			template<typename Policy>
			constexpr json_options_t set_bits_for( Policy e ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<json_options_t>( e );
				new_bits <<= parse_policy_bits_start<Policy>;
				return new_bits;
			}

			template<typename>
			struct default_policy_flag_t;

			template<typename... Policies>
			struct default_policy_flag_t<pack_list<Policies...>> {
				static constexpr json_options_t value =
				  ( set_bits_for<Policies>( default_json_option_value<Policies> ) |
				    ... );
			};

			/***
			 * The defaults for all known policies encoded as a json_options_t
			 */
			inline static constexpr json_options_t default_policy_flag =
			  default_policy_flag_t<parse_policy_list>::value;

			template<typename Policy, typename Result = Policy>
			constexpr Result get_bits_for( json_options_t value ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				constexpr json_options_t mask =
				  ( 1U << (parse_policy_bits_start<Policy> +
				           json_option_bits_width<Policy>)) -
				  1U;
				value &= mask;
				value >>= parse_policy_bits_start<Policy>;
				return static_cast<Result>( Policy{ value } );
			}
		} // namespace json_details
		// ***********************************************

		/***
		 * Create the parser options flag for BasicParsePolicy
		 * @tparam Policies Policy types that satisfy the `is_policy_flag` trait.
		 * @param policies A list of parser options to change from the defaults.
		 * @return A json_options_t that encodes the options for the parser
		 */
		template<typename... Policies>
		constexpr json_details::json_options_t parse_options(
		  Policies... policies ) {
			static_assert( ( json_details::is_option_flag<Policies> and ... ),
			               "Only registered policy types are allowed" );
			auto result = json_details::default_policy_flag;
			if constexpr( sizeof...( Policies ) > 0 ) {
				// result |= ( json_details::set_bits_for( policies ) | ... );
				( json_details::set_nth_bits<
				    json_details::basic_option_bits_start<
				      Policies,
				      json_details::parse_policy_list>,
				    json_details::json_option_bits_width<Policies>>(
				    result,
				    static_cast<json_details::json_options_t>( policies ) ),
				  ... );
			}
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
