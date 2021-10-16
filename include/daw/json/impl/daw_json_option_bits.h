// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_enums.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <climits>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		namespace json_details {
			template<typename>
			inline constexpr json_options_t json_option_bits_width = 0;

			template<typename>
			inline constexpr auto default_json_option_value = [] {
				struct unknown_policy {};
				return unknown_policy{ };
			}( );

			template<typename Policy, typename Options>
			struct option_bits_start_impl;

			template<typename Policy, typename... Options>
			struct option_bits_start_impl<Policy, pack_list<Options...>> {
				static constexpr auto idx = traits::pack_index_of_v<Policy, Options...>;
				static_assert( idx >= 0, "Policy is not registered" );
				using tp_policies = pack_list<Options...>;

				template<std::size_t Pos, int End>
				static constexpr unsigned do_step( ) {
					if constexpr( Pos >= static_cast<std::size_t>( End ) ) {
						return 0U;
					}
					return json_option_bits_width<pack_element_t<Pos, tp_policies>>;
				}

				template<std::size_t... Is>
				static constexpr json_options_t calc( std::index_sequence<Is...> ) {
					return ( do_step<Is, idx>( ) + ... );
				}
			};

			template<typename Policy, typename Policies>
			inline constexpr json_options_t basic_policy_bits_start =
			  option_bits_start_impl<Policy, Policies>::template calc(
			    std::make_index_sequence<pack_size_v<Policies>>{ } );

			template<typename... Options>
			struct option_list_impl {
				using type = pack_list<Options...>;

				static_assert( ( std::is_same_v<json_details::json_options_t,
				                                std::underlying_type_t<Options>> and
				                 ... ) );
				static_assert(
				  ( json_option_bits_width<Options> + ... ) <=
				    ( sizeof( json_options_t ) * CHAR_BIT ),
				  "The size of json_options_t is not large enough "
				  "to safely hold all the bits of state.  Use a larger size." );
			};

			template<typename Option>
			inline constexpr bool is_option_flag = json_option_bits_width<Option> > 0;

			template<typename Option, typename Options>
			inline constexpr json_options_t basic_option_bits_start =
			  option_bits_start_impl<Option, Options>::template calc(
			    std::make_index_sequence<pack_size_v<Options>>{ } );

			template<typename... JsonOptions>
			struct JsonOptionList;

			template<typename... OptionList, typename Option>
			static constexpr json_options_t
			set_bits_for( JsonOptionList<OptionList...>, Option e );

			template<std::size_t Pos, std::size_t Width, typename Unsigned>
			static constexpr /*cpp20 consteval*/ void
			set_nth_bits( Unsigned &u, Unsigned new_bits ) {
				static_assert( Width > 0, "Invalid option width" );
				static_assert( Width + Pos <= ( sizeof( Unsigned ) * CHAR_BIT ) );
				if( DAW_UNLIKELY( new_bits >= ( 1ULL << ( Width + Pos ) ) ) ) {
					// Programmer error, one of the option values is larger than the width
					std::terminate( );
				}
				constexpr auto rmask =
				  Pos == 0U ? Unsigned{ 0U }
				            : static_cast<Unsigned>( ( 1ULL << Pos ) - 1ULL );
				constexpr auto lmask =
				  static_cast<Unsigned>( ~( ( 1ULL << ( Pos + Width ) ) - 1ULL ) );
				constexpr auto mask = lmask | rmask;
				u &= mask;
				new_bits <<= Pos;
				u |= new_bits;
			}

			template<typename... JsonOptions>
			struct JsonOptionList {
				using OptionList = typename option_list_impl<JsonOptions...>::type;

				template<typename Option>
				static inline constexpr json_options_t option_bits_start =
				  basic_option_bits_start<Option, OptionList>;

				template<typename... OptionList, typename Option>
				static constexpr json_options_t set_bits_for( Option e ) {
					static_assert( is_option_flag<Option>,
					               "Only registered policy types are allowed" );
					auto new_bits = static_cast<json_options_t>( e );
					new_bits <<= option_bits_start<Option>;
					return new_bits;
				}

				static constexpr json_options_t default_option_flag =
				  ( set_bits_for<JsonOptions>(
				      default_json_option_value<JsonOptions> ) |
				    ... );

				/***
				 * Create the parser options flag for BasicParseOption
				 * @tparam Options Option types that satisfy the `is_option_flag`
				 * trait.
				 * @param policies A list of parser options to change from the defaults.
				 * @return A json_options_t that encodes the options for the parser
				 */
				template<typename... Options>
				static constexpr json_options_t options( Options... options ) {
					static_assert( ( json_details::is_option_flag<Options> and ... ),
					               "Only registered option types are allowed" );
					auto result = default_option_flag;
					if constexpr( sizeof...( Options ) > 0 ) {
						( set_nth_bits<option_bits_start<Options>,
						               json_option_bits_width<Options>>(
						    result, static_cast<json_options_t>( options ) ),
						  ... );
						// result |= ( set_bits_for( options ) | ... );
					}
					return result;
				}
			};

			template<typename... OptionList, typename Option>
			constexpr void set_bits_in( JsonOptionList<OptionList...>,
			                            json_options_t &value, Option e ) {
				static_assert( is_option_flag<Option>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<json_options_t>( e );
				constexpr json_options_t mask =
				  (1U << json_option_bits_width<Option>)-1U;
				new_bits &= mask;
				new_bits <<=
				  JsonOptionList<OptionList...>::template option_bits_start<Option>;
				value &= ~mask;
				value |= new_bits;
			}

			template<typename... OptionList, typename Option, typename... Options>
			constexpr json_options_t set_bits( JsonOptionList<OptionList...>,
			                                   json_options_t value, Option pol,
			                                   Options... pols ) {
				static_assert( ( is_option_flag<Options> and ... ),
				               "Only registered policy types are allowed" );

				auto new_bits = static_cast<json_options_t>( pol );
				constexpr json_options_t mask =
				  ( (1ULL << json_option_bits_width<Option>)-1ULL );
				new_bits &= mask;
				new_bits <<=
				  JsonOptionList<OptionList...>::template option_bits_start<Option>;
				value &= ~( mask << JsonOptionList<
				              OptionList...>::template option_bits_start<Option> );
				value |= new_bits;
				if constexpr( sizeof...( Options ) > 0 ) {
					return set_bits( JsonOptionList<OptionList...>{ }, value, pols... );
				} else {
					return value;
				}
			}

			template<typename... OptionList, typename Option>
			static constexpr json_options_t
			set_bits_for( JsonOptionList<OptionList...>, Option e ) {
				return JsonOptionList<OptionList...>::template set_bits_for( e );
			}

			template<typename Option, typename Result = Option,
			         typename... OptionList>
			constexpr Result get_bits_for( JsonOptionList<OptionList...>,
			                               json_options_t value ) {
				static_assert( std::is_enum_v<Option>,
				               "Only enum options are allowed" );
				static_assert(
				  std::is_same_v<json_options_t, std::underlying_type_t<Option>>,
				  "Looks like option was no specified correctly.  "
				  "Underlying type should be unsigned" );
				static_assert( is_option_flag<Option>,
				               "Only registered option types are allowed" );
				constexpr json_options_t mask =
				  ( 1ULL << (JsonOptionList<OptionList...>::template option_bits_start<
				               Option> +
				             json_option_bits_width<Option>)) -
				  1ULL;
				value &= mask;
				value >>=
				  JsonOptionList<OptionList...>::template option_bits_start<Option>;
				return static_cast<Result>( value );
			}

			template<typename... OptionList, typename... Options>
			constexpr json_options_t json_option( JsonOptionList<OptionList...>,
			                                      Options... options ) {
				return JsonOptionList<OptionList...>::template options( options... );
			}

			template<typename>
			struct default_option_flag_t;

			template<template<class...> class OptionList, typename... Options>
			struct default_option_flag_t<OptionList<Options...>> {
				static constexpr json_options_t value =
				  ( OptionList<Options...>::template set_bits_for<Options>(
				      default_json_option_value<Options> ) |
				    ... );
			};

			/***
			 * The defaults for all known policies encoded as a json_options_t
			 */
			template<typename OptionList>
			static inline constexpr json_options_t default_option_flag =
			  default_option_flag_t<OptionList>::value;

		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
