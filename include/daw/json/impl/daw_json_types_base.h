// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/daw_json_enums.h"
#include "daw/json/impl/daw_json_link_types_aggregate.h"
#include "daw/json/impl/daw_json_traits.h"

#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_base {
			/// @brief Mark a member as nullable
			/// @tparam T type of the value being mapped to(e.g. std::optional<Foo>)
			/// @tparam JsonMember Json Type or type of value when present, deduced
			/// from T if not specified
			/// @tparam Constructor Specify a Constructor type or use
			/// the default nullable_constructor<T>
			template<typename T, typename JsonMember = use_default,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			struct json_nullable;

		} // namespace json_base
		namespace json_details {
			template<typename T>
			inline constexpr bool is_json_nullable_v = false;

			template<typename T, typename JsonMember, JsonNullable NullableType,
			         typename Constructor>
			inline constexpr bool is_json_nullable_v<
			  json_base::json_nullable<T, JsonMember, NullableType, Constructor>> =
			  true;

			template<typename T>
			struct json_empty_class {
				static_assert( std::is_empty_v<T>, "T is expected to empty" );
				using i_am_a_json_type = void;
				using i_am_a_deduced_empty_class = void;
				using wrapped_type = T;

				using constructor_t = default_constructor<T>;
				using parse_to_t = T;

				static constexpr auto expected_type = JsonParseTypes::Class;
				static constexpr auto underlying_json_type = JsonBaseParseTypes::Class;
			};

			template<typename T>
			struct json_ordered_class {
				static_assert( can_convert_to_tuple_v<T>, "T is expected to empty" );
				using i_am_a_json_type = void;
				using i_am_a_deduced_ordered_class = void;
				using wrapped_type = T;

				using constructor_t = default_constructor<T>;
				using parse_to_t = T;

				static constexpr auto expected_type = JsonParseTypes::Tuple;

				static constexpr auto underlying_json_type = JsonBaseParseTypes::Class;
			};
		} // namespace json_details

		namespace json_base {
#if defined( DAW_JSON_HAS_REFLECTION )
			template<typename T>
			struct json_reflected_class;
#endif
			template<typename T, typename Constructor = use_default>
			struct json_class;

			template<typename T, JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_class_null =
			  json_nullable<T, json_class<json_details::unwrapped_t<T>>, NullableType,
			                Constructor>;

			template<typename JsonElement, typename Container = use_default,
			         typename Constructor = use_default>
			struct json_array;

			template<typename T, typename FromJsonConverter = use_default,
			         typename ToJsonConverter = use_default,
			         json_options_t Options = json_custom_opts_def>
			struct json_custom;

			template<typename Variant, typename JsonElements = use_default,
			         typename Constructor = use_default>
			struct json_variant;

			template<typename T, typename TagMember, typename Switcher,
			         typename JsonElements = use_default,
			         typename Constructor = use_default>
			struct json_tagged_variant;

			template<typename T, json_options_t Options = string_raw_opts_def,
			         typename Constructor = use_default>
			struct json_string_raw;

			template<typename T, json_options_t Options = string_raw_opts_def,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_string_raw_null =
			  json_nullable<T, json_string_raw<json_details::unwrapped_t<T>, Options>,
			                NullableType, Constructor>;

			template<typename T, json_options_t Options = string_opts_def,
			         typename Constructor = use_default>
			struct json_string;

			template<typename T, json_options_t Options = string_opts_def,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_string_null =
			  json_nullable<T, json_string<json_details::unwrapped_t<T>, Options>,
			                NullableType, Constructor>;

			template<typename T, json_options_t Options = bool_opts_def,
			         typename Constructor = use_default>
			struct json_bool;

			template<typename T, json_options_t Options = bool_opts_def,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_bool_null =
			  json_nullable<T, json_bool<json_details::unwrapped_t<T>, Options>,
			                NullableType, Constructor>;

			template<typename T, typename Constructor = use_default>
			struct json_date;

			template<typename T, json_options_t Options = number_opts_def,
			         typename Constructor = use_default>
			struct json_number;

			template<typename T, json_options_t Options = number_opts_def,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_number_null =
			  json_nullable<T, json_number<json_details::unwrapped_t<T>, Options>,
			                NullableType, Constructor>;

			template<typename Container, typename JsonValueType = use_default,
			         typename JsonKeyType = use_default,
			         typename Constructor = use_default>
			struct json_key_value;

			template<typename Container, typename JsonValueType = use_default,
			         typename JsonKeyType = use_default,
			         typename Constructor = use_default>
			struct json_key_value_array;

			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_key_value_null =
			  json_nullable<Container,
			                json_key_value<json_details::unwrapped_t<Container>,
			                               JsonValueType, JsonKeyType>,
			                NullableType, Constructor>;

			template<typename Tuple, typename JsonTupleTypesList = use_default,
			         typename Constructor = use_default>
			struct json_tuple;

			template<typename Tuple, typename JsonTupleTypesList = use_default,
			         JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_tuple_null = json_nullable<
			  Tuple, json_tuple<json_details::unwrapped_t<Tuple>, JsonTupleTypesList>,
			  NullableType, Constructor>;

			/***
			 * json_raw allows for raw JSON access to the member data. It requires a
			 * type that is constructable from (char const *, std::size_t) arguments
			 * and for serialization requires that it can be passed to
			 * std::begin/std::end and the iterator returned has a value_type of
			 * char
			 * @tparam T type to hold raw JSON data, defaults to json_value
			 * @tparam Constructor A callable used to construct T.
			 */
			template<typename T, typename Constructor = use_default>
			struct json_raw;

			/***
			 * json_raw_null allows for raw JSON access to the nullable member data.
			 * It requires a type that is constructable from (char const *,
			 * std::size_t) arguments and for serialization requires that it can be
			 * passed to std::begin/std::end and the iterator returned has a
			 * value_type of char
			 * @tparam T type to hold raw JSON data, defaults to json_value
			 * @tparam Constructor A callable used to construct T.
			 */
			template<typename T, JsonNullable NullableType = JsonNullable::Nullable,
			         typename Constructor = use_default>
			using json_raw_null =
			  json_nullable<T, json_raw<json_details::unwrapped_t<T>>, NullableType,
			                Constructor>;
		} // namespace json_base
	} // namespace DAW_JSON_VER
} // namespace daw::json
