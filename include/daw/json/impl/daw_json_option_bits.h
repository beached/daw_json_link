// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_traits.h>

#include <climits>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		using json_options_t = std::uint32_t;
		namespace json_details {

			template<typename>
			inline constexpr unsigned json_option_bits_width = 0;

			template<typename>
			inline constexpr auto default_json_option_value = [] {
				struct unknown_policy {};
				return unknown_policy{ };
			}( );

			template<typename Policy, typename Options>
			struct option_bits_start_impl;

			template<typename Policy, typename... Options>
			struct option_bits_start_impl<Policy, pack_list<Options...>> {
				static constexpr auto idx = daw::traits::pack_index_of_v<Policy, Options...>;
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
				DAW_ATTRIB_INLINE static constexpr unsigned calc( std::index_sequence<Is...> ) {
					return ( do_step<Is, idx>( ) + ... );
				}
			};

			template<typename... Options>
			struct option_list_impl {
				using type = pack_list<Options...>;

				static_assert(
				  ( json_option_bits_width<Options> + ... + 0 ) <=
				    ( sizeof( json_options_t ) * CHAR_BIT ),
				  "The size of json_options_t is not large enough "
				  "to safely hold all the bits of state.  Use a larger size." );
			};

			template<typename Option>
			inline constexpr bool is_option_flag = json_option_bits_width<Option> > 0;

			template<typename... Options>
			inline constexpr bool are_option_flags =
			  ( is_option_flag<Options> and ... );

			template<typename Option, typename Options>
			inline constexpr unsigned basic_option_bits_start =
			  option_bits_start_impl<Option, Options>::template calc<>(
			    std::make_index_sequence<pack_size_v<Options>>{ } );

			template<typename... JsonOptions>
			struct JsonOptionList;

			template<typename... OptionList, typename Option>
			static constexpr json_options_t
			set_bits_for( JsonOptionList<OptionList...>, Option e );

			template<typename... JsonOptions>
			struct JsonOptionList {
				using OptionList = typename option_list_impl<JsonOptions...>::type;

				template<typename Option>
				static inline constexpr unsigned option_bits_start =
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
				    ... | 0 );
				/***
				 * Create the parser options flag for BasicParseOption
				 * @tparam Options Option types that satisfy the `is_option_flag`
				 * trait.
				 * @param policies A list of parser options to change from the defaults.
				 * @return A json_options_t that encodes the options for the parser
				 */
				template<typename... Options>
				static constexpr json_options_t options( Options... options ) {
					static_assert( json_details::are_option_flags<Options...>,
					               "Only registered option types are allowed" );
					auto result = default_option_flag;
					if constexpr( sizeof...( Options ) > 0 ) {
						result |= ( set_bits_for( options ) | ... );
					}
					return result;
				}
			};

			template<typename... OptionList, typename Option>
			constexpr void set_bits_in( JsonOptionList<OptionList...>,
			                            json_options_t &value, Option e ) {
				static_assert( is_option_flag<Option>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<unsigned>( e );
				constexpr unsigned mask = (1U << json_option_bits_width<Option>)-1U;
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
				static_assert( are_option_flags<Options...>,
				               "Only registered policy types are allowed" );

				auto new_bits = static_cast<unsigned>( pol );
				constexpr unsigned mask = ( (1U << json_option_bits_width<Option>)-1U );
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
				return JsonOptionList<OptionList...>::set_bits_for( e );
			}

			template<typename Option, typename Result = Option,
			         typename... OptionList>
			constexpr Result get_bits_for( JsonOptionList<OptionList...>,
			                               json_options_t value ) {
				static_assert( std::is_enum_v<Option>,
				               "Only enum options are allowed" );
				static_assert( std::is_same_v<unsigned, std::underlying_type_t<Option>>,
				               "Looks like option was no specified correctly.  "
				               "Underlying type should be unsigned" );
				static_assert( is_option_flag<Option>,
				               "Only registered option types are allowed" );
				constexpr unsigned mask =
				  ( 1U << (JsonOptionList<OptionList...>::template option_bits_start<
				             Option> +
				           json_option_bits_width<Option>)) -
				  1U;
				value &= mask;
				value >>=
				  JsonOptionList<OptionList...>::template option_bits_start<Option>;
				return static_cast<Result>( Option{ value } );
			}

			template<typename... OptionList, typename... Options>
			constexpr json_options_t json_option( JsonOptionList<OptionList...>,
			                                      Options... options ) {
				return JsonOptionList<OptionList...>::options( options... );
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
	} // namespace DAW_JSON_VER
} // namespace daw::json
