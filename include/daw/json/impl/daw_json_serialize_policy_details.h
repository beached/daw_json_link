// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_option_bits.h"
#include "daw/json/impl/daw_json_parse_options_impl.h"
#include "daw/json/impl/daw_json_serialize_options_impl.h"

#include <daw/daw_constant.h>

#include <cstddef>
#include <string_view>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::serialization {
			using policy_list = typename option_list_impl<
			  options::SerializationFormat, options::IndentationType,
			  options::RestrictedStringOutput, options::NewLineDelimiter,
			  options::OutputTrailingComma>::type;

			template<typename Policy, typename Policies>
			inline constexpr unsigned basic_policy_bits_start =
			  option_bits_start_impl<Policy, Policies>::template calc<>(
			    std::make_index_sequence<pack_size_v<Policies>>{ } );

			template<typename Policy>
			inline constexpr unsigned policy_bits_start =
			  basic_policy_bits_start<Policy, policy_list>;

			template<typename Policy>
			static constexpr void set_bits_in( json_options_t &value, Policy e ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<unsigned>( e );
				using mask = daw::constant<(1U << json_option_bits_width<Policy>)-1U>;
				new_bits &= mask::value;
				new_bits <<= policy_bits_start<Policy>;
				value &= ~mask::value;
				value |= new_bits;
			}

			template<typename Policy, typename... Policies>
			static constexpr json_options_t set_bits( json_options_t value,
			                                          Policy pol, Policies... pols ) {
				static_assert( are_option_flags<Policies...>,
				               "Only registered policy types are allowed" );

				auto new_bits = static_cast<unsigned>( pol );
				using mask =
				  daw::constant<( (1U << json_option_bits_width<Policy>)-1U )>;
				new_bits &= mask::value;
				new_bits <<= policy_bits_start<Policy>;
				value &= ~( mask::value << policy_bits_start<Policy> );
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
			static constexpr json_options_t set_bits_for( Policy e ) {
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
			inline constexpr json_options_t default_policy_flag =
			  default_policy_flag_t<policy_list>::value;

			template<typename Policy, typename Result = Policy>
			static constexpr Result get_bits_for( json_options_t value ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				using mask = daw::constant<( 1U << (policy_bits_start<Policy> +
				                                    json_option_bits_width<Policy>)) -
				                           1U>;
				value &= mask::value;
				value >>= policy_bits_start<Policy>;
				return static_cast<Result>( Policy{ value } );
			}

			template<options::SerializationFormat, options::IndentationType>
			inline constexpr std::string_view generate_indent{ };

			template<>
			inline constexpr std::string_view generate_indent<
			  options::SerializationFormat::Pretty, options::IndentationType::Tab> =
			  "\t";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space1> = " ";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space2> = "  ";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space3> = "   ";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space4> = "    ";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space5> = "     ";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space8> = "        ";

			template<>
			inline constexpr std::string_view
			  generate_indent<options::SerializationFormat::Pretty,
			                  options::IndentationType::Space10> = "          ";
		} // namespace json_details::serialization
	} // namespace DAW_JSON_VER
} // namespace daw::json
