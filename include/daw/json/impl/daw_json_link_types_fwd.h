// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_enums.h"
#include "daw_json_link_types_iso8601.h"
#include "daw_json_parse_class.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_value_fwd.h"
#include "daw_json_traits.h"
#include "version.h"

#include <daw/cpp_17.h>
#include <daw/daw_arith_traits.h>
#include <daw/daw_fwd_pack_apply.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

#include <chrono>
#include <ciso646>
#include <optional>
#include <string>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		/**
		 * NOTE:
		 * Some of the basic json types used for deduction are in
		 * daw_json_parse_common.h
		 *
		 */

		/***
		 * A type to hold the types for parsing tagged variants.
		 * @tparam JsonElements a list of types that can be parsed,
		 */
		template<typename... JsonElements>
		struct json_variant_type_list {
			using i_am_variant_type_list = void;
			using element_map_t =
			  fwd_pack<json_details::json_deduced_type<JsonElements>...>;
		};

		namespace json_details {
			template<JsonBaseParseTypes PT>
			constexpr std::size_t
			find_json_element( std::initializer_list<JsonBaseParseTypes> pts ) {
				std::size_t idx = 0;
				for( auto const &pt : pts ) {
					if( pt == PT ) {
						return idx;
					}
					++idx;
				}
				return ( daw::numeric_limits<std::size_t>::max )( );
			}

			template<typename T>
			struct unknown_variant_type {
				using i_am_variant_type_list = void;
			};

			template<typename... Ts>
			struct missing_default_type_mapping {
				using i_am_variant_type_list = void;
			};

			template<typename... Ts>
			[[maybe_unused]] constexpr std::conditional_t<
			  std::conjunction<has_json_deduced_type<Ts>...>::value,
			  json_variant_type_list<json_deduced_type<Ts>...>,
			  missing_default_type_mapping<json_deduced_type<Ts>...>>
			get_variant_type_list( std::variant<Ts...> const * );

			template<typename T>
			using underlying_nullable_type = decltype( *std::declval<T>( ) );

			template<typename>
			struct unable_to_determine_underlying_nullable_type;

			template<typename T>
			using detected_underlying_nullable_type = std::remove_reference_t<
			  daw::detected_or_t<unable_to_determine_underlying_nullable_type<T>,
			                     underlying_nullable_type,
			                     T>>;

			template<typename T>
			using is_nullable_type = daw::is_detected<underlying_nullable_type, T>;

			template<typename T>
			inline constexpr bool is_nullable_type_v = is_nullable_type<T>::value;

			template<typename T>
			[[maybe_unused]] constexpr unknown_variant_type<T>
			get_variant_type_list( T const * );

			/// Allow specialization of variant like types to extract the alternative
			/// pack
			template<typename, typename = void>
			struct variant_alternatives_list;

			template<typename... Ts>
			struct variant_alternatives_list<std::variant<Ts...>> {
				using type = std::conditional_t<
				  std::conjunction<has_json_deduced_type<Ts>...>::value,
				  json_variant_type_list<json_deduced_type<Ts>...>,
				  missing_default_type_mapping<json_deduced_type<Ts>...>>;
			};

			template<typename, typename = void>
			struct tuple_types_list;

			template<template<class...> class Tuple, typename... Ts>
			struct tuple_types_list<Tuple<Ts...>> {
				static_assert( std::conjunction_v<has_deduced_type_mapping<Ts>...>,
				               "Missing mapping for type in tuple" );

				using types = std::tuple<json_deduced_type<Ts>...>;
			};

			template<JsonNullable, typename>
			struct cannot_deduce_variant_element_types;

			template<JsonNullable Nullable, typename Variant>
			using determine_variant_element_types = std::conditional_t<
			  std::disjunction_v<daw::not_trait<is_nullable_json_value<Nullable>>,
			                     daw::not_trait<is_nullable_type<Variant>>>,
			  variant_alternatives_list<Variant>,
			  std::conditional_t<
			    is_nullable_type_v<Variant>,
			    variant_alternatives_list<detected_underlying_nullable_type<Variant>>,
			    cannot_deduce_variant_element_types<Nullable, Variant>>>;

			template<JsonNullable, typename>
			struct cannot_deduce_tuple_types_list;

			template<JsonNullable Nullable, typename Tuple>
			using determine_tuple_element_types = std::conditional_t<
			  std::disjunction_v<daw::not_trait<is_nullable_json_value<Nullable>>,
			                     daw::not_trait<is_nullable_type<Tuple>>>,
			  typename tuple_types_list<Tuple>::type,
			  std::conditional_t<is_nullable_type_v<Tuple>,
			                     typename tuple_types_list<
			                       detected_underlying_nullable_type<Tuple>>::type,
			                     cannot_deduce_tuple_types_list<Nullable, Tuple>>>;
		} // namespace json_details

		template<typename... Ts>
		struct json_tuple_types_list {
			static_assert(
			  std::conjunction_v<json_details::has_json_deduced_type<Ts>...>,
			  "Missing mapping for type in tuple" );
			using types = std::tuple<json_details::json_deduced_type<Ts>...>;
		};

		namespace json_details {
			template<typename T>
			struct ensure_mapped {
				static_assert( is_a_json_type_v<T>,
				               "The supplied type does not have a json_data_contract" );
				using type = T;
			};

			template<typename T>
			using ensure_mapped_t = typename ensure_mapped<T>::type;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
