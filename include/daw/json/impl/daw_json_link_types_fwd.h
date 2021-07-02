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
	inline namespace DAW_JSON_VER {
		/**
		 * NOTE:
		 * Some of the basic json types used for deduction are in
		 * daw_json_parse_common.h
		 *
		 */

		/**
		 * Link to a JSON class
		 * @tparam Name name of JSON member to link to
		 * @tparam T type that has specialization of
		 * daw::json::json_data_contract
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename T,
		         typename Constructor = default_constructor<T>,
		         json_details::json_options_t Options = class_opts_def>
		struct json_class;

		/**
		 * Link to a nullable JSON class
		 * @tparam Name name of JSON member to link to
		 * @tparam T type that has specialization of
		 * daw::json::json_data_contract
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<JSONNAMETYPE Name, typename T,
		         typename Constructor = nullable_constructor<T>,
		         json_details::json_options_t Options = class_opts_def>
		using json_class_null =
		  json_class<Name, T, Constructor,
		             json_details::class_opts_set<Options, JsonNullDefault>>;

		/**
		 * The member is a number
		 * @tparam Name name of json member
		 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
		 * Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 * @tparam RangeCheck Check if the value will fit in the result
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename T = double,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = default_constructor<T>>
		struct json_number;
		/**
		 * The member is a nullable number
		 * @tparam Name name of json member
		 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
		 * Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 * @tparam RangeCheck Check if the value will fit in the result
		 */
		template<JSONNAMETYPE Name, typename T = std::optional<double>,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = nullable_constructor<T>>
		using json_number_null =
		  json_number<Name, T,
		              json_details::number_opts_set<Options, JsonNullDefault>,
		              Constructor>;

		/**
		 * The member is a range checked number
		 * @tparam Name name of json member
		 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
		 * Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename T = double,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = default_constructor<T>>
		using json_checked_number = json_number<
		  Name, T,
		  json_details::number_opts_set<Options, JsonRangeCheck::CheckForNarrowing>,
		  Constructor>;

		/**
		 * The member is a boolean
		 * @tparam Name name of json member
		 * @tparam T result type to pass to Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename T = bool,
		         json_details::json_options_t Options = bool_opts_def,
		         typename Constructor = default_constructor<T>>
		struct json_bool;

		/**
		 * The member is a nullable boolean
		 * @tparam Name name of json member
		 * @tparam T result type to pass to Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 */
		template<JSONNAMETYPE Name, typename T = std::optional<bool>,
		         json_details::json_options_t Options = bool_opts_def,
		         typename Constructor = default_constructor<T>>
		using json_bool_null =
		  json_bool<Name, T, json_details::bool_opts_set<Options, JsonNullDefault>,
		            Constructor>;

		/**
		 * Member is an escaped string and requires unescaping and escaping of
		 * string data
		 * @tparam Name of json member
		 * @tparam String result type constructed by Constructor
		 * @tparam Constructor a callable taking as arguments ( char const *,
		 * std::size_t )
		 * @tparam EmptyStringNull if string is empty, call Constructor with no
		 * arguments
		 * @tparam EightBitMode Allow filtering of characters with the MSB set
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename String = std::string,
		         json_details::json_options_t Options = string_opts_def,
		         typename Constructor = default_constructor<String>>
		struct json_string;

		/**
		 * Member is a nullable escaped string and requires unescaping and escaping
		 * of string data
		 * @tparam Name of json member
		 * @tparam String result type constructed by Constructor
		 * @tparam Constructor a callable taking as arguments ( InputIterator,
		 * InputIterator ).  If others are needed use the Constructor callable
		 * convert
		 * @tparam EmptyStringNull if string is empty, call Constructor with no
		 * arguments
		 * @tparam EightBitMode Allow filtering of characters with the MSB set
		 */
		template<JSONNAMETYPE Name, typename String = std::optional<std::string>,
		         json_details::json_options_t Options = string_opts_def,
		         typename Constructor = nullable_constructor<String>>
		using json_string_null =
		  json_string<Name, String,
		              json_details::string_opts_set<Options, JsonNullDefault>,
		              Constructor>;

		/**
		 * Member is an escaped string and requires unescaping and escaping of
		 * string data
		 * @tparam Name of json member
		 * @tparam String result type constructed by Constructor
		 * @tparam Constructor a callable taking as arguments ( char const *,
		 * std::size_t )
		 * @tparam EmptyStringNull if string is empty, call Constructor with no
		 * arguments
		 * @tparam EightBitMode Allow filtering of characters with the MSB set
		 * arguments
		 * @tparam Nullable Can the member be missing or have a null value
		 * @tparam AllowEscape Tell parser if we know a \ or escape will be in the
		 * data
		 */
		template<JSONNAMETYPE Name, typename String = std::string,
		         json_details::json_options_t Options = string_raw_opts_def,
		         typename Constructor = default_constructor<String>>
		struct json_string_raw;

		/**
		 * Member is a nullable escaped string and requires unescaping and escaping
		 * of string data.  Not all invalid codepoints are detected
		 * @tparam Name of json member
		 * @tparam String result type constructed by Constructor
		 * @tparam Constructor a callable taking as arguments ( char const *,
		 * std::size_t )
		 * @tparam EmptyStringNull if string is empty, call Constructor with no
		 * arguments
		 * @tparam EightBitMode Allow filtering of characters with the MSB set
		 * arguments
		 * @tparam AllowEscape Tell parser if we know a \ or escape will be in the
		 * data
		 */
		template<JSONNAMETYPE Name, typename String = std::optional<std::string>,
		         json_details::json_options_t Options = string_raw_opts_def,
		         typename Constructor = nullable_constructor<String>>
		using json_string_raw_null = json_string_raw<
		  Name, String, json_details::string_raw_opts_set<Options, JsonNullDefault>,
		  Constructor>;

		/**
		 * The member is a nullable range checked number
		 * @tparam Name name of json member
		 * @tparam T type of number(e.g. optional<double>, optional<int>,
		 * optional<unsigned>...) to pass to Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 */
		template<JSONNAMETYPE Name, typename T = std::optional<double>,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = nullable_constructor<T>>
		using json_checked_number_null = json_number<
		  Name, T,
		  json_details::number_opts_set<Options, JsonRangeCheck::CheckForNarrowing,
		                                JsonNullDefault>,
		  Constructor>;

		/** Map a KV type json class { "Key StringRaw": ValueType, ... }
		 *  to a c++ class.  Keys are Always string like and the destination
		 *  needs to be constructable with a pointer, size
		 *  @tparam Name name of JSON member to link to
		 *  @tparam Container type to put values in
		 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
		 *  json_string, ... ). It also supports basic types like numbers, bool, and
		 * mapped classes and enums(mapped to numbers)
		 *  @tparam JsonKeyType type of key in kv pair.  As with value it supports
		 * basic types too
		 *  @tparam Constructor A callable used to make Container, default will use
		 * the Containers constructor.  Both normal and aggregate are supported
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename Container,
		         typename JsonValueType = typename Container::mapped_type,
		         typename JsonKeyType = json_base::json_string<std::string>,
		         typename Constructor = default_constructor<Container>,
		         JsonNullable Nullable = JsonNullable::MustExist>
		struct json_key_value;

		/** Map a nullable KV type json class { "Key StringRaw": ValueType, ... }
		 *  to a c++ class.  Keys are Always string like and the destination
		 *  needs to be constructable with a pointer, size
		 *  @tparam Name name of JSON member to link to
		 *  @tparam Container type to put values in
		 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
		 *  json_string, ... ). It also supports basic types like numbers, bool, and
		 * mapped classes and enums(mapped to numbers)
		 *  @tparam JsonKeyType type of key in kv pair.  As with value it supports
		 * basic types too
		 *  @tparam Constructor A callable used to make Container, default will use
		 * the Containers constructor.  Both normal and aggregate are supported
		 */
		template<JSONNAMETYPE Name, typename Container,
		         typename JsonValueType = typename Container::mapped_type,
		         typename JsonKeyType = typename Container::key_type,
		         typename Constructor = nullable_constructor<Container>>
		using json_key_value_null =
		  json_key_value<Name, Container, JsonValueType, JsonKeyType, Constructor,
		                 JsonNullDefault>;

		/**
		 * Link to a JSON string representing a date
		 * @tparam Name name of JSON member to link to
		 * @tparam T C++ type to construct, must be a std::chrono::time_point
		 * @tparam Constructor A Callable used to construct a T.
		 * Must accept a char pointer and size as argument to the date/time string.
		 * The default is an iso8601 timestamp parser
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name,
		         typename T = std::chrono::time_point<std::chrono::system_clock,
		                                              std::chrono::milliseconds>,
		         typename Constructor =
		           construct_from_iso8601_timestamp<JsonNullable::MustExist>,
		         JsonNullable Nullable = JsonNullable::MustExist>
		struct json_date;

		/**
		 * Link to a JSON string representing a nullable date
		 * @tparam Name name of JSON member to link to
		 * @tparam T C++ type to construct, by default is a time_point
		 * @tparam Constructor A Callable used to construct a T.
		 * Must accept a char pointer and size as argument to the date/time string.
		 */
		template<JSONNAMETYPE Name,
		         typename T = std::optional<std::chrono::time_point<
		           std::chrono::system_clock, std::chrono::milliseconds>>,
		         typename Constructor =
		           construct_from_iso8601_timestamp<JsonNullDefault>>
		using json_date_null = json_date<Name, T, Constructor, JsonNullDefault>;

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

		/** Link to a JSON array
		 * @tparam Name name of JSON member to link to
		 * @tparam Container type of C++ container being constructed(e.g.
		 * vector<int>)
		 * @tparam JsonElement Json type being parsed e.g. json_number,
		 * json_string...
		 * @tparam Constructor A callable used to make Container,
		 * default will use the Containers constructor.  Both normal and aggregate
		 * are supported
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<
		  JSONNAMETYPE Name, typename JsonElement,
		  typename Container = std::vector<
		    typename json_details::json_deduced_type<JsonElement>::parse_to_t>,
		  typename Constructor = default_constructor<Container>,
		  JsonNullable Nullable = JsonNullable::MustExist>
		struct json_array;

		/** Link to a nullable JSON array
		 * @tparam Name name of JSON member to link to
		 * @tparam Container type of C++ container being constructed(e.g.
		 * vector<int>)
		 * @tparam JsonElement Json type being parsed e.g. json_number,
		 * json_string...
		 * @tparam Constructor A callable used to make Container,
		 * default will use the Containers constructor.  Both normal and aggregate
		 * are supported
		 */
		template<
		  JSONNAMETYPE Name, typename JsonElement,
		  typename Container = std::vector<
		    typename json_details::json_deduced_type<JsonElement>::parse_to_t>,
		  typename Constructor = nullable_constructor<Container>>
		using json_array_null =
		  json_array<Name, JsonElement, Container, Constructor, JsonNullDefault>;

		/**
		 * Map a KV type json array [ {"key": ValueOfKeyType, "value":
		 * ValueOfValueType},... ] to a c++ class. needs to be constructable with a
		 * pointer, size
		 *  @tparam Name name of JSON member to link to
		 *  @tparam Container type to put values in
		 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
		 *  json_string, ... ).  If specific json member type isn't specified, the
		 * member name defaults to "value"
		 *  @tparam JsonKeyType type of key in kv pair.  If specific json member
		 * type isn't specified, the key name defaults to "key"
		 *  @tparam Constructor A callable used to make Container, default will use
		 * the Containers constructor.  Both normal and aggregate are supported
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename Container,
		         typename JsonValueType = typename Container::mapped_type,
		         typename JsonKeyType = typename Container::key_type,
		         typename Constructor = default_constructor<Container>,
		         JsonNullable Nullable = JsonNullable::MustExist>
		struct json_key_value_array;

		/**
		 * Map a nullable KV type json array [ {"key": ValueOfKeyType, "value":
		 * ValueOfValueType},... ] to a c++ class. needs to be constructable with a
		 * pointer, size
		 *  @tparam Name name of JSON member to link to
		 *  @tparam Container type to put values in
		 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
		 *  json_string, ... ).  If specific json member type isn't specified, the
		 * member name defaults to "value"
		 *  @tparam JsonKeyType type of key in kv pair.  If specific json member
		 * type isn't specified, the key name defaults to "key"
		 *  @tparam Constructor A callable used to make Container, default will use
		 * the Containers constructor.  Both normal and aggregate are supported
		 */
		template<JSONNAMETYPE Name, typename Container,
		         typename JsonValueType = typename Container::mapped_type,
		         typename JsonKeyType = typename Container::key_type,
		         typename Constructor = nullable_constructor<Container>>
		using json_key_value_array_null =
		  json_key_value_array<Name, Container, JsonValueType, JsonKeyType,
		                       Constructor, JsonNullDefault>;

		/**
		 * Allow parsing of a type that does not fit
		 * @tparam Name Name of JSON member to link to
		 * @tparam T type of value being constructed
		 * @tparam FromJsonConverter Callable that accepts a std::string_view of the
		 * range to parse.  The default requires an overload of from_string(
		 * daw::tag<T>, std::string_view ) that returns a T
		 * @tparam ToJsonConverter Returns a string from the value.  The default
		 * requires a to_string( T const & ) overload that returns a String like
		 * type
		 * @tparam JsonRawType JSON type value is encoded as literal/string/any
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         json_details::json_options_t Options = json_raw_opts_def>
		struct json_raw;

		/**
		 * Allow parsing of a nullable type that does not fit
		 * @tparam Name Name of JSON member to link to
		 * @tparam T type of value being constructed
		 * @tparam FromJsonConverter Callable that accepts a std::string_view of the
		 * range to parse
		 * @tparam ToJsonConverter Returns a string from the value
		 * @tparam JsonRawType JSON type value is encoded as literal/string
		 */
		template<JSONNAMETYPE Name, typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         json_details::json_options_t Options = json_raw_opts_def>
		using json_raw_null =
		  json_raw<Name, T, FromJsonConverter, ToJsonConverter,
		           json_details::json_raw_opts_set<Options, JsonNullDefault>>;

		template<JSONNAMETYPE Name, typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         json_details::json_options_t Options = json_raw_opts_def>
		using json_raw_lit =
		  json_raw<Name, T, FromJsonConverter, ToJsonConverter,
		           json_details::json_raw_opts_set<Options, JsonRawTypes::Literal>>;

		template<JSONNAMETYPE Name, typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         json_details::json_options_t Options = json_raw_opts_def>
		using json_raw_lit_null =
		  json_raw<Name, T, FromJsonConverter, ToJsonConverter,
		           json_details::json_raw_opts_set<Options, JsonRawTypes::Literal,
		                                           JsonNullDefault>>;

		template<JSONNAMETYPE Name, typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         JsonRawTypes JsonRawType = JsonRawTypes::String,
		         JsonNullable Nullable = JsonNullable::MustExist>
		using json_custom [[deprecated(
		  "Use JsonRawTypes/json_raw... Removing in JSON Link v4" )]] =
		  json_raw<Name, T, FromJsonConverter, ToJsonConverter,
		           json_details::json_raw_opts_set<json_raw_opts_def, JsonRawType,
		                                           Nullable>>;

		template<JSONNAMETYPE Name, typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         JsonRawTypes JsonRawType = JsonRawTypes::String>
		using json_custom_null [[deprecated(
		  "Use JsonRawTypes/json_raw... Removing in JSON Link v4" )]] =
		  json_raw<Name, T, FromJsonConverter, ToJsonConverter,
		           json_details::json_raw_opts_set<json_raw_opts_def, JsonRawType,
		                                           JsonNullDefault>>;

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
				return daw::numeric_limits<std::size_t>::max( );
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

			template<typename T>
			using detected_underlying_nullable_type =
			  std::remove_reference_t<daw::detected_t<underlying_nullable_type, T>>;

			template<typename T>
			inline constexpr bool is_nullable_type =
			  daw::is_detected<underlying_nullable_type, T>::value;

			template<typename T>
			[[maybe_unused]] constexpr unknown_variant_type<T>
			get_variant_type_list( T const * );

			template<JsonNullable, typename>
			struct cannot_deduce_variant_element_types;

			template<JsonNullable Nullable, typename Variant>
			using determine_variant_element_types = std::conditional_t<
			  not is_nullable_json_value_v<Nullable> or not is_nullable_type<Variant>,
			  std::remove_reference_t<decltype( get_variant_type_list(
			    std::declval<Variant const *>( ) ) )>,
			  std::conditional_t<
			    is_nullable_type<Variant>,
			    std::remove_reference_t<decltype( get_variant_type_list(
			      std::declval<
			        detected_underlying_nullable_type<Variant> const *>( ) ) )>,
			    cannot_deduce_variant_element_types<Nullable, Variant>>>;
		} // namespace json_details

		/***
		 * Link to a variant like data type.  The JSON member can be any one of the
		 * json types.  This precludes having more than one class type or array
		 * type(including their specialized keyvalue mappings) or
		 * string-enum/int-enum.
		 * @tparam Name name of JSON member to link to
		 * @tparam T type of value to construct
		 * @tparam JsonElements a json_variant_type_list
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 * @tparam Nullable Can the member be missing or have a null value	 *
		 */
		template<
		  JSONNAMETYPE Name, typename T,
		  typename JsonElements =
		    json_details::determine_variant_element_types<JsonNullDefault, T>,
		  typename Constructor = default_constructor<T>,
		  JsonNullable Nullable = JsonNullable::MustExist>
		struct json_variant;

		/***
		 * Link to a nullable JSON variant
		 * @tparam Name name of JSON member to link to
		 * @tparam T type that has specialization of
		 * daw::json::json_data_contract
		 * @tparam JsonElements a json_variant_type_list
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<
		  JSONNAMETYPE Name, typename T,
		  typename JsonElements =
		    json_details::determine_variant_element_types<JsonNullDefault, T>,
		  typename Constructor = nullable_constructor<T>>
		using json_variant_null =
		  json_variant<Name, T, JsonElements, Constructor, JsonNullDefault>;

		/***
		 * Link to a variant like data type that is discriminated via another
		 * member.
		 * @tparam Name name of JSON member to link to
		 * @tparam T type of value to construct
		 * @tparam TagMember JSON element to pass to Switcher. Does not have to be
		 * declared in member list
		 * @tparam Switcher A callable that returns an index into JsonElements when
		 * passed the TagMember object in parent member list
		 * @tparam JsonElements a json_tagged_variant_type_list, defaults to type
		 * elements of T when T is a std::variant and they are all auto mappable
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 * @tparam Nullable Can the member be missing or have a null value	 *
		 */
		template<
		  JSONNAMETYPE Name, typename T, typename TagMember, typename Switcher,
		  typename JsonElements = json_details::determine_variant_element_types<
		    JsonNullable::MustExist, T>,
		  typename Constructor = default_constructor<T>,
		  JsonNullable Nullable = JsonNullable::MustExist>
		struct json_tagged_variant;

		template<
		  JSONNAMETYPE Name, typename JsonElement, typename SizeMember,
		  typename Container = std::vector<
		    typename json_details::json_deduced_type<JsonElement>::parse_to_t>,
		  typename Constructor = default_constructor<Container>,
		  JsonNullable Nullable = JsonNullable::MustExist>
		struct json_sized_array;

		/***
		 * Link to a nullable variant like data type that is discriminated via
		 * another member.
		 * @tparam Name name of JSON member to link to
		 * @tparam T type of value to construct
		 * @tparam TagMember JSON element to pass to Switcher. Does not have to be
		 * declared in member list
		 * @tparam Switcher A callable that returns an index into JsonElements when
		 * passed the TagMember object in parent member list
		 * @tparam JsonElements a json_tagged_variant_type_list, defaults to type
		 * elements of T when T is a std::variant and they are all auto mappable
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<
		  JSONNAMETYPE Name, typename T, typename TagMember, typename Switcher,
		  typename JsonElements =
		    json_details::determine_variant_element_types<JsonNullDefault, T>,
		  typename Constructor = nullable_constructor<T>>
		using json_tagged_variant_null =
		  json_tagged_variant<Name, T, TagMember, Switcher, JsonElements,
		                      Constructor, JsonNullDefault>;

		template<JSONNAMETYPE Name, typename Tuple,
		         typename Constructor = default_constructor<Tuple>,
		         json_details::json_options_t Options = tuple_opts_def>
		struct json_tuple;

		template<JSONNAMETYPE Name, typename Tuple,
		         typename Constructor = default_constructor<Tuple>,
		         json_details::json_options_t Options = tuple_opts_def>

		using json_tuple_null =
		  json_tuple<Name, Tuple, Constructor,
		             json_details::tuple_opts_set<Options, JsonNullDefault>>;

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
