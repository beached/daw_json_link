// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../daw_json_parse_options.h"
#include "daw_json_option_bits.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_unreachable.h>

#include <climits>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace options {
			constexpr daw::string_view to_string( ExecModeTypes mode ) {
				switch( mode ) {
				case ExecModeTypes::compile_time:
					return "compile_time";
				case ExecModeTypes::runtime:
					return "runtime";
				case ExecModeTypes::simd:
					return "simd";
				}
				DAW_UNREACHABLE( );
			}
		} // namespace options

		namespace json_details {
			template<>
			inline constexpr unsigned json_option_bits_width<options::ExecModeTypes> =
			  2;

			template<>
			inline constexpr auto default_json_option_value<options::ExecModeTypes> =
			  options::ExecModeTypes::compile_time;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::ZeroTerminatedString> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::ZeroTerminatedString> =
			    options::ZeroTerminatedString::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::PolicyCommentTypes> = 2;

			template<>
			inline constexpr auto
			  default_json_option_value<options::PolicyCommentTypes> =
			    options::PolicyCommentTypes::none;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::CheckedParseMode> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::CheckedParseMode> =
			    options::CheckedParseMode::yes;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::MinifiedDocument> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::MinifiedDocument> =
			    options::MinifiedDocument::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::AllowEscapedNames> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::AllowEscapedNames> =
			    options::AllowEscapedNames::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::IEEE754Precise> = 1;

			template<>
			inline constexpr auto default_json_option_value<options::IEEE754Precise> =
			  options::IEEE754Precise::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::ForceFullNameCheck> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::ForceFullNameCheck> =
			    options::ForceFullNameCheck::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::UseExactMappingsByDefault> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::UseExactMappingsByDefault> =
			    options::UseExactMappingsByDefault::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::MustVerifyEndOfDataIsValid> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::MustVerifyEndOfDataIsValid> =
			    options::MustVerifyEndOfDataIsValid::no;

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::ExpectLongNames> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::ExpectLongNames> =
			    options::ExpectLongNames::no;

			/*
			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::TemporarilyMutateBuffer> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::TemporarilyMutateBuffer> =
			    options::TemporarilyMutateBuffer::no;
			    */

			template<>
			inline constexpr unsigned
			  json_option_bits_width<options::ExcludeSpecialEscapes> = 1;

			template<>
			inline constexpr auto
			  default_json_option_value<options::ExcludeSpecialEscapes> =
			    options::ExcludeSpecialEscapes::no;

			using policy_list = typename option_list_impl<
			  options::ExecModeTypes, options::ZeroTerminatedString,
			  options::PolicyCommentTypes, options::CheckedParseMode,
			  options::AllowEscapedNames, options::IEEE754Precise,
			  options::ForceFullNameCheck, options::MinifiedDocument,
			  options::UseExactMappingsByDefault, options::MustVerifyEndOfDataIsValid,
			  options::ExcludeSpecialEscapes, options::ExpectLongNames>::type;

			template<typename Policy, typename Policies>
			inline constexpr unsigned basic_policy_bits_start =
			  option_bits_start_impl<Policy, Policies>::template calc<>(
			    std::make_index_sequence<pack_size_v<Policies>>{ } );

			template<typename Policy>
			inline constexpr unsigned policy_bits_start =
			  basic_policy_bits_start<Policy, policy_list>;

			DAW_CONSTEVAL inline json_options_t set_bits( json_options_t value ) {
				return value;
			}

			template<typename PolicyFlag, typename... PolicyFlags,
			         std::enable_if_t<( is_option_flag<PolicyFlag> and
			                            ( is_option_flag<PolicyFlags> and ... ) ),
			                          std::nullptr_t> = nullptr>
			DAW_CONSTEVAL json_options_t set_bits( json_options_t value,
			                                       PolicyFlag pol,
			                                       PolicyFlags... pols ) {
				static_assert( ( is_option_flag<PolicyFlags> and ... ),
				               "Only registered policy types are allowed" );

				auto new_bits = static_cast<unsigned>( pol );
				constexpr unsigned mask =
				  ( (1U << json_option_bits_width<PolicyFlag>)-1U );
				new_bits &= mask;
				new_bits <<= policy_bits_start<PolicyFlag>;
				value &= ~( mask << policy_bits_start<PolicyFlag> );
				value |= new_bits;
				if constexpr( sizeof...( PolicyFlags ) > 0 ) {
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
			DAW_CONSTEVAL json_options_t set_bits_for( Policy e ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<json_options_t>( e );
				new_bits <<= policy_bits_start<Policy>;
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
			  default_policy_flag_t<policy_list>::value;

			template<typename Policy, typename Result = Policy>
			DAW_CONSTEVAL Result get_bits_for( json_options_t value ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				constexpr unsigned mask = ( 1U << (policy_bits_start<Policy> +
				                                   json_option_bits_width<Policy>)) -
				                          1U;
				value &= mask;
				value >>= policy_bits_start<Policy>;
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
		DAW_CONSTEVAL json_options_t parse_options( Policies... policies ) {
			static_assert( ( json_details::is_option_flag<Policies> and ... ),
			               "Only registered policy types are allowed" );
			auto result = json_details::default_policy_flag;
			if constexpr( sizeof...( Policies ) > 0 ) {
				result |= ( json_details::set_bits_for( policies ) | ... );
			}
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
