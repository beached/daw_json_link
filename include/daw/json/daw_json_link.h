// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_json_link_impl.h"
#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_parse_policy.h"
#include "impl/daw_json_serialize_impl.h"
#include "impl/daw_json_traits.h"
#include "impl/daw_json_value.h"

#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/daw_visit.h>
#include <daw/iterator/daw_back_inserter.h>

#include <array>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>

namespace daw::json {
	/**
	 *
	 * @tparam JsonMembers JSON classes that map the relation ship from the json
	 * data to the classes constructor
	 */
	template<typename... JsonMembers>
	struct json_member_list {
		using i_am_a_json_member_list = void;
		static_assert( ( json_details::is_a_json_type_v<JsonMembers> and ... ),
		               "Only JSON Link mapping types can appear in a "
		               "json_member_list(e.g. json_number, json_string...)" );
		static_assert(
		  not( is_no_name<JsonMembers> or ... ),
		  "All members must have a name and not no_name in a json_member_list" );
		/**
		 * Serialize a C++ class to JSON data
		 * @tparam OutputIterator An output iterator with a char value_type
		 * @tparam Args  tuple of values that map to the JSON members of v
		 * @tparam Value Value type being serialized
		 * @param it OutputIterator to append string data to
		 * @param args members from C++ class
		 * @param v value to be serialized as JSON object
		 * @return the OutputIterator it at final position
		 */
		template<typename OutputIterator, typename Value, typename... Args>
		[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
		serialize( OutputIterator it, std::tuple<Args...> const &args,
		           Value const &v ) {
			static_assert( json_details::CheckMatch<Value, sizeof...( Args ),
			                                        sizeof...( JsonMembers )>::value,
			               "Argument count is incorrect" );

			return json_details::serialize_json_class<JsonMembers...>(
			  it, std::index_sequence_for<Args...>{ }, args, v );
		}

		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam Range Input range type
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename T, typename Range>
		[[maybe_unused, nodiscard]] DAW_ONLY_FLATTEN static constexpr T
		parse_to_class( Range &rng ) {
			return json_details::parse_json_class<T, JsonMembers...>(
			  rng, std::index_sequence_for<JsonMembers...>{ } );
		}
	};

	template<typename... Members>
	struct json_data_contract<tuple_json_mapping<Members...>> {
		using type = json_member_list<Members...>;

		[[nodiscard, maybe_unused]] static inline auto const &
		to_json_data( tuple_json_mapping<Members...> const &value ) {
			return value.members;
		}
	};

	/***
	 * In a json_ordered_member_list, this allows specifying the position in the
	 * array to parse this member from
	 * @tparam Index Position in array where member is
	 * @tparam JsonMember type of value( e.g. int, json_string )
	 */
	template<std::size_t Index, typename JsonMember>
	struct ordered_json_member {
		using i_am_an_ordered_member = void;
		using i_am_a_json_type = void;
		static constexpr std::size_t member_index = Index;
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		using parse_to_t = typename json_details::unnamed_default_type_mapping<
		  JsonMember>::parse_to_t;
	};

	namespace json_details {
		template<typename JsonMember>
		struct ordered_member_wrapper
		  : json_details::unnamed_default_type_mapping<JsonMember> {};
	} // namespace json_details

	/***
	 * Allow extracting elements from a JSON array and constructing from it.
	 * Members can be either normal C++ no_name members, or an ordered_member with
	 * a position. All ordered members must have a value greater than the
	 * previous.  The first element in the list, unless it is specified as an
	 * ordered_member, is 0.  A non-ordered_member item will be 1 more than the
	 * previous item in the list.  All items must have an index greater than the
	 * previous.
	 * @tparam JsonMembers A list of json_TYPE mappings or a json_TYPE mapping
	 * wrapped into a ordered_json_member
	 */
	template<typename... JsonMembers>
	struct json_ordered_member_list {
		using i_am_a_json_member_list = void;
		using i_am_a_ordered_member_list = void;

		/**
		 * Serialize a C++ class to JSON data
		 * @tparam OutputIterator An output iterator with a char value_type
		 * @tparam Args  tuple of values that map to the JSON members
		 * @param it OutputIterator to append string data to
		 * @param args members from C++ class
		 * @return the OutputIterator it
		 */
		template<typename OutputIterator, typename Value, typename... Args>
		[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
		serialize( OutputIterator it, std::tuple<Args...> const &args,
		           Value const &v ) {
			static_assert( sizeof...( Args ) == sizeof...( JsonMembers ),
			               "Argument count is incorrect" );

			static_assert( ( json_details::is_a_json_type_v<
			                   json_details::ordered_member_wrapper<JsonMembers>> and
			                 ... ),
			               "Only value JSON types can be used" );
			return json_details::serialize_ordered_json_class<
			  json_details::ordered_member_wrapper<JsonMembers>...>(
			  it, std::index_sequence_for<Args...>{ }, args, v );
		}

		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam Range Input range type
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename T, typename Range>
		[[maybe_unused, nodiscard]] static inline constexpr T
		parse_to_class( Range &rng ) {
			return json_details::parse_ordered_json_class<
			  T, json_details::ordered_member_wrapper<JsonMembers>...>( rng );
		}
	};

	/***
	 * Parse a tagged variant like class where the tag member is in the same class
	 * that is being discriminated.  The container type, that will specialize
	 * json_data_constract on, must support the get_if, get_index
	 * @tparam TagMember JSON element to pass to Switcher. Does not have to be
	 * declared in member list
	 * @tparam Switcher A callable that returns an index into JsonClasses when
	 * passed the TagMember object
	 * @tparam JsonClasses List of alternative classes that are mapped via a
	 * json_data_contract
	 */
	template<typename TagMember, typename Switcher, typename... JsonClasses>
	struct json_submember_tagged_variant {
		using i_am_a_json_member_list = void;
		using i_am_a_submember_tagged_variant = void;

		/**
		 * Serialize a C++ class to JSON data
		 * @tparam OutputIterator An output iterator with a char value_type
		 * @param it OutputIterator to append string data to
		 * @return the OutputIterator it
		 */
		template<typename OutputIterator, typename Value>
		[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
		serialize( OutputIterator it, Value const &v ) {

			return daw::visit_nt( v, [&]( auto const &alternative ) {
				using Alternative = daw::remove_cvref_t<decltype( alternative )>;
				static_assert( ( std::is_same_v<Alternative, JsonClasses> or ... ),
				               "Unexpected alternative type" );
				static_assert( json_details::has_json_to_json_data_v<Alternative>,
				               "Alternative type does not have a to_json_data_member "
				               "in it's json_data_contract specialization" );

				return json_details::member_to_string<json_class<no_name, Alternative>>(
				  it, alternative );
			} );
		}

		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam Range Input range type
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename T, typename Range>
		[[maybe_unused, nodiscard]] static inline constexpr T
		parse_to_class( Range &rng ) {
			using tag_class_t = tuple_json_mapping<TagMember>;
			std::size_t const idx = [rng]( ) mutable {
				return Switcher{ }( std::get<0>(
				  json_details::parse_value<json_class<no_name, tag_class_t>>(
				    ParseTag<JsonParseTypes::Class>{ }, rng )
				    .members ) );
			}( );
			return json_details::parse_nth_class<0, T, false,
			                                     json_class<no_name, JsonClasses>...>(
			  idx, rng );
		}
	};

	/**************************************************
	 * Member types - These are the mapping classes to
	 * describe the constructor of your classes
	 **************************************************/

	/**
	 * The member is a range checked number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T, LiteralAsStringOpt LiteralAsString,
	         typename Constructor, JsonRangeCheck RangeCheck,
	         JsonNullable Nullable>
	struct json_number {
		using i_am_a_json_type = void;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );

		static_assert( daw::is_arithmetic_v<base_type>,
		               "json_number requires an arithmetic type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;
		static_assert(
		  Nullable == JsonNullable::Never or
		    std::is_same_v<parse_to_t, std::invoke_result_t<Constructor>>,
		  "Default ctor of constructor must match that of base" );
		using constructor_t = Constructor;
		static constexpr daw::string_view name = Name;

		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<json_details::number_parse_type_v<base_type>, Nullable>;

		static constexpr JsonParseTypes base_expected_type =
		  json_details::number_parse_type_v<base_type>;

		static constexpr LiteralAsStringOpt literal_as_string = LiteralAsString;
		static constexpr JsonRangeCheck range_check = RangeCheck;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Number;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * The member is a nullable boolean
	 * @tparam Name name of json member
	 * @tparam T result type to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 */
	template<JSONNAMETYPE Name, typename T, LiteralAsStringOpt LiteralAsString,
	         typename Constructor, JsonNullable Nullable>
	struct json_bool {
		using i_am_a_json_type = void;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;
		using constructor_t = Constructor;

		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Bool, Nullable>;
		static constexpr JsonParseTypes base_expected_type = JsonParseTypes::Bool;

		static constexpr daw::string_view name = Name;
		static constexpr LiteralAsStringOpt literal_as_string = LiteralAsString;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Bool;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
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
	template<JSONNAMETYPE Name, typename String, typename Constructor,
	         JsonNullable EmptyStringNull, EightBitModes EightBitMode,
	         JsonNullable Nullable, AllowEscapeCharacter AllowEscape>
	struct json_string_raw {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = String;
		using base_type = json_details::unwrap_type<String, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;

		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::StringRaw, Nullable>;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::StringRaw;
		static constexpr JsonNullable empty_is_null = JsonNullable::Never;
		static constexpr EightBitModes eight_bit_mode = EightBitMode;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
		static constexpr AllowEscapeCharacter allow_escape_character = AllowEscape;
	};

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( InputIterator,
	 * InputIterator ).  If others are needed use the Constructor callable convert
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename String, typename Constructor,
	         JsonNullable EmptyStringNull, EightBitModes EightBitMode,
	         JsonNullable Nullable>
	struct json_string {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = String;
		using base_type = json_details::unwrap_type<String, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;

		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::StringEscaped, Nullable>;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::StringEscaped;
		static constexpr JsonNullable empty_is_null = EmptyStringNull;
		static constexpr EightBitModes eight_bit_mode = EightBitMode;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * Link to a JSON string representing a nullable date
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type to construct, by default is a time_point
	 * @tparam Constructor A Callable used to construct a T.
	 * Must accept a char pointer and size as argument to the date/time string.
	 */
	template<JSONNAMETYPE Name, typename T, typename Constructor,
	         JsonNullable Nullable>
	struct json_date {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t =
		  std::invoke_result_t<Constructor, char const *, std::size_t>;

		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Date, Nullable>;
		static constexpr JsonParseTypes base_expected_type = JsonParseTypes::Date;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * Link to a JSON class
	 * @tparam Name name of JSON member to link to
	 * @tparam T type that has specialization of
	 * daw::json::json_data_contract
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T, typename Constructor,
	         JsonNullable Nullable>
	struct json_class {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = T;
		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Class, Nullable>;
		static constexpr JsonParseTypes base_expected_type = JsonParseTypes::Class;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Class;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/***
	 * A type to hold the types for parsing variants.
	 * @tparam JsonElements Up to one of a JsonElement that is a JSON number,
	 * string, object, or array
	 */
	template<typename... JsonElements>
	struct json_variant_type_list {
		using i_am_variant_type_list = void;
		static_assert(
		  sizeof...( JsonElements ) <= 5U,
		  "There can be at most 5 items, one for each JsonBaseParseTypes" );
		using element_map_t =
		  std::tuple<json_details::unnamed_default_type_mapping<JsonElements>...>;
		static constexpr std::size_t base_map[5] = {
		  json_details::find_json_element<JsonBaseParseTypes::Number>(
		    { json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type... } ),
		  json_details::find_json_element<JsonBaseParseTypes::Bool>(
		    { json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type... } ),
		  json_details::find_json_element<JsonBaseParseTypes::String>(
		    { json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type... } ),
		  json_details::find_json_element<JsonBaseParseTypes::Class>(
		    { json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type... } ),
		  json_details::find_json_element<JsonBaseParseTypes::Array>(
		    { json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type... } ) };
	};

	/***
	 * Link to a nullable JSON variant
	 * @tparam Name name of JSON member to link to
	 * @tparam T type that has specialization of
	 * daw::json::json_data_contract
	 * @tparam JsonElements a json_variant_type_list
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 */
	template<JSONNAMETYPE Name, typename T, typename JsonElements,
	         typename Constructor, JsonNullable Nullable>
	struct json_variant {
		using i_am_a_json_type = void;
		static_assert(
		  std::is_same_v<typename JsonElements::i_am_variant_type_list, void>,
		  "Expected a json_variant_type_list" );
		using constructor_t = Constructor;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = T;
		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Variant, Nullable>;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Variant;

		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::None;
		using json_elements = JsonElements;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/***
	 * Link to a nullable variant like data type that is discriminated via another
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
	 */
	template<JSONNAMETYPE Name, typename T, typename TagMember, typename Switcher,
	         typename JsonElements, typename Constructor, JsonNullable Nullable>
	struct json_tagged_variant {
		using i_am_a_json_type = void;
		using i_am_a_tagged_variant = void;
		static_assert(
		  std::is_same_v<typename JsonElements::i_am_tagged_variant_type_list,
		                 void>,
		  "Expected a json_member_list" );

		static_assert( std::is_same_v<typename TagMember::i_am_a_json_type, void>,
		               "JSON member types must be passed as "
		               "TagMember" );
		using constructor_t = Constructor;
		using wrapped_type = T;
		using tag_member = TagMember;
		using switcher = Switcher;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = T;
		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::VariantTagged, Nullable>;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::VariantTagged;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::None;
		using json_elements = JsonElements;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * Allow parsing of a nullable type that does not fit
	 * @tparam Name Name of JSON member to link to
	 * @tparam T type of value being constructed
	 * @tparam FromJsonConverter Callable that accepts a std::string_view of the
	 * range to parse
	 * @tparam ToJsonConverter Returns a string from the value
	 * @tparam CustomJsonType JSON type value is encoded as literal/string
	 */
	template<JSONNAMETYPE Name, typename T, typename FromJsonConverter,
	         typename ToJsonConverter, CustomJsonTypes CustomJsonType,
	         JsonNullable Nullable>
	struct json_custom {
		using i_am_a_json_type = void;
		using to_converter_t = ToJsonConverter;
		using from_converter_t = FromJsonConverter;
		using constructor_t = FromJsonConverter;

		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t =
		  std::invoke_result_t<FromJsonConverter, std::string_view>;
		static_assert( std::is_invocable_v<FromJsonConverter, std::string_view>,
		               "FromConverter must be callable with a std::string_view" );
		static_assert(
		  std::is_invocable_v<ToJsonConverter, parse_to_t> or
		    std::is_invocable_r_v<char const *, ToJsonConverter, char const *,
		                          parse_to_t>,
		  "ToConverter must be callable with T or T and and OutputIterator" );
		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Custom, Nullable>;
		static constexpr JsonParseTypes base_expected_type = JsonParseTypes::Custom;
		static constexpr CustomJsonTypes custom_json_type = CustomJsonType;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	namespace json_details {
		template<JSONNAMETYPE Name, typename JsonElement, typename Container,
		         typename Constructor, JsonNullable Nullable>
		struct json_array_detect {
			using i_am_a_json_type = void;
			using json_element_t =
			  json_details::unnamed_default_type_mapping<JsonElement>;
			static_assert( not std::is_same_v<json_element_t, void>,
			               "Unknown JsonElement type." );
			static_assert( json_details::is_a_json_type_v<json_element_t>,
			               "Error determining element type" );
			using constructor_t = Constructor;

			using base_type = json_details::unwrap_type<Container, Nullable>;
			static_assert( not std::is_same_v<void, base_type>,
			               "Failed to detect base type" );
			using parse_to_t = std::invoke_result_t<Constructor>;
			static constexpr daw::string_view name = Name;
			static constexpr JsonParseTypes expected_type =
			  get_parse_type_v<JsonParseTypes::Array, Nullable>;
			static constexpr JsonParseTypes base_expected_type =
			  JsonParseTypes::Array;

			static_assert( json_element_t::name == no_name,
			               "All elements of json_array must be have no_name" );
			static constexpr JsonBaseParseTypes underlying_json_type =
			  JsonBaseParseTypes::Array;
			static constexpr bool nullable = Nullable == JsonNullable::Nullable;
		};
	} // namespace json_details

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
	template<JSONNAMETYPE Name, typename JsonElement, typename Container,
	         typename Constructor, JsonNullable Nullable>
	struct json_array {
		using i_am_a_json_type = void;
		using json_element_t =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<json_element_t, void>,
		               "Unknown JsonElement type." );
		static_assert( json_details::is_a_json_type_v<json_element_t>,
		               "Error determining element type" );
		using constructor_t = Constructor;

		using base_type = json_details::unwrap_type<Container, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Array, Nullable>;
		static constexpr JsonParseTypes base_expected_type = JsonParseTypes::Array;

		static_assert( json_element_t::name == no_name,
		               "All elements of json_array must be have no_name" );
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Array;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

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
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType, typename Constructor, JsonNullable Nullable>
	struct json_key_value {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using base_type = json_details::unwrap_type<Container, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		using json_element_t =
		  json_details::unnamed_default_type_mapping<JsonValueType>;
		static_assert( not std::is_same_v<json_element_t, void>,
		               "Unknown JsonValueType type." );
		static_assert( json_element_t::name == no_name,
		               "Value member name must be the default no_name" );

		using json_key_t = json_details::unnamed_default_type_mapping<JsonKeyType>;
		static_assert( not std::is_same_v<json_key_t, void>,
		               "Unknown JsonKeyType type." );
		static_assert( json_key_t::name == no_name,
		               "Key member name must be the default no_name" );

		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::KeyValue, Nullable>;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::KeyValue;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Class;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * Map a KV type json array [ {"key": ValueOfKeyType, "value":
	 * ValueOfValueType},... ] to a c++ class. needs to be constructable with a
	 * pointer, size
	 *  @tparam Name name of JSON member to link to
	 *  @tparam Container type to put values in
	 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
	 *  json_string, ... ).  If specific json member type isn't specified, the
	 * member name defaults to "value"
	 *  @tparam JsonKeyType type of key in kv pair.  If specific json member type
	 * isn't specified, the key name defaults to "key"
	 *  @tparam Constructor A callable used to make Container, default will use
	 * the Containers constructor.  Both normal and aggregate are supported
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType, typename Constructor, JsonNullable Nullable>
	struct json_key_value_array {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using base_type = json_details::unwrap_type<Container, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		using json_key_t = json_details::unnamed_default_type_mapping<
		  JsonKeyType, json_details::default_key_name>;
		static_assert( not std::is_same_v<json_key_t, void>,
		               "Unknown JsonKeyType type." );
		static_assert( daw::string_view( json_key_t::name ) !=
		                 daw::string_view( no_name ),
		               "Must supply a valid key member name" );
		using json_value_t = json_details::unnamed_default_type_mapping<
		  JsonValueType, json_details::default_value_name>;

		using json_class_t =
		  json_class<no_name, tuple_json_mapping<json_key_t, json_value_t>>;
		static_assert( not std::is_same_v<json_value_t, void>,
		               "Unknown JsonValueType type." );
		static_assert( daw::string_view( json_value_t::name ) !=
		                 daw::string_view( no_name ),
		               "Must supply a valid value member name" );
		static_assert( daw::string_view( json_key_t::name ) !=
		                 daw::string_view( json_value_t::name ),
		               "Key and Value member names cannot be the same" );
		static constexpr daw::string_view name = Name;
		static constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::KeyValueArray, Nullable>;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::KeyValueArray;
		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Array;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/***
	 * An untyped JSON value
	 */
	using json_value = basic_json_value<NoCommentSkippingPolicyChecked>;

	/***
	 * A name/value pair of string_view/json_value
	 */
	using json_pair = basic_json_pair<NoCommentSkippingPolicyChecked>;

	/***
	 * Do not parse this member but return a T that can be used to
	 * parse later.  Any whitespace surrounding the value may not be preserved.
	 *
	 * @tparam Name name of JSON member to link to
	 * @tparam T destination type.  Must be constructable from a char const * and
	 * a std::size_t
	 * @tparam Constructor A callable used to construct T.  The
	 * @tparam Nullable Does the value have to exist in the document or can it
	 * have a null value
	 */
	template<JSONNAMETYPE Name, typename T = json_value,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_delayed {
		using i_am_a_json_type = void;
		using wrapped_type = json_value;
		using constructor_t = Constructor;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );

		using parse_to_t =
		  std::invoke_result_t<Constructor, char const *, char const *>;
		static constexpr daw::string_view name = Name;

		static constexpr JsonParseTypes expected_type = JsonParseTypes::Unknown;
		static constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Unknown;

		static constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::None;
		static constexpr bool nullable = Nullable == JsonNullable::Nullable;
	};

	/**
	 * Construct the JSONMember from the JSON document argument.
	 * @tparam JsonMember any bool, arithmetic, string, string_view,
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A reified T constructed from JSON data
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json( std::string_view json_data ) {
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		auto rng = ParsePolicy( json_data.data( ),
		                        json_data.data( ) +
		                          static_cast<ptrdiff_t>( json_data.size( ) ) );

		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/**
	 * Construct the JSONMember from the JSON document argument.
	 * @tparam JsonMember any bool, arithmetic, string, string_view,
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A reified T constructed from JSON data
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false, typename Allocator>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json_alloc( std::string_view json_data, Allocator const &alloc ) {
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		//static_assert( json_details::is_allocator_v<Allocator> );
		char const *f = json_data.data( );
		char const *l = f + static_cast<ptrdiff_t>( json_data.size( ) );
		auto rng = ParsePolicy::with_allocator( f, l, alloc );

		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @tparam JsonMember The type of the item being parsed
	 * @param json_data JSON string data
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json( std::string_view json_data, std::string_view member_path ) {
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		daw_json_assert( not member_path.empty( ), ErrorReason::EmptyJSONPath );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) } );
		if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename json_member::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @tparam JsonMember The type of the item being parsed
	 * @param json_data JSON string data
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false, typename Allocator>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json_alloc( std::string_view json_data, std::string_view member_path,
	                 Allocator &alloc ) {
		static_assert( json_details::is_allocator_v<Allocator> );
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		daw_json_assert( not member_path.empty( ), ErrorReason::EmptyJSONPath );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) },
		  alloc );
		if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename json_member::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a value from a json_value
	 * @tparam JsonMember The type of the item being parsed
	 * @param value JSON data, see basic_json_value
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false, typename Range>
	[[maybe_unused, nodiscard]] inline constexpr auto
	from_json( basic_json_value<Range> value ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		auto const json_data = value.get_string_view( );
		auto rng = ParsePolicy( json_data.data( ),
		                        json_data.data( ) +
		                          static_cast<ptrdiff_t>( json_data.size( ) ) );

		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @param value JSON data, see basic_json_value
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam JsonMember The type of the item being parsed
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A value reified from the JSON data member
	 * @throws daw::json::json_exception
	 */
	template<typename JsonMember,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         bool KnownBounds = false, typename Range>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json( basic_json_value<Range> value, std::string_view member_path ) {
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		auto const json_data = value.get_string_view( );
		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { std::data( member_path ), std::size( member_path ) } );
		if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename json_member::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		return json_details::parse_value<json_member, KnownBounds>(
		  ParseTag<json_member::expected_type>{ }, rng );
	}

	/**
	 *
	 * @tparam OutputIterator Iterator to character data to
	 * @tparam JsonClass Type that has json_parser_description and to_json_data
	 * function overloads
	 * @param value  value to serialize
	 * @param out_it result to serialize to
	 */
	template<typename Value,
	         typename JsonClass =
	           typename json_details::unnamed_default_type_mapping<Value>,
	         typename OutputIterator>
	[[maybe_unused]] constexpr OutputIterator to_json( Value const &value,
	                                                   OutputIterator out_it ) {
		if constexpr( std::is_pointer_v<OutputIterator> ) {
			daw_json_assert( out_it, ErrorReason::NullOutputIterator );
		}

		out_it = json_details::member_to_string<JsonClass>( out_it, value );
		return out_it;
	}

	/**
	 * Serialize a value to JSON.  Some types(std::string, string_view. integer's
	 * and floating point numbers do not need a mapping setup).  For user classes,
	 * a json_data_contract specialization is needed.
	 * @tparam Result std::string like type to put result into
	 * @tparam JsonClass Type that has json_parser_description and to_json_data
	 * function overloads
	 * @param value  value to serialize
	 * @return  JSON string data
	 */
	template<typename Result = std::string, typename Value,
	         typename JsonClass =
	           typename json_details::unnamed_default_type_mapping<Value>>
	[[maybe_unused, nodiscard]] constexpr Result to_json( Value const &value ) {
		Result result{ };
		to_json<Value, JsonClass>( value, daw::back_inserter( result ) );
		return result;
	}

	/**
	 * Parse JSON data where the root item is an array
	 * @tparam JsonElement The type of each element in array.  Must be one of
	 * the above json_XXX classes.  This version is checked
	 * @tparam Container Container to store values in
	 * @tparam Constructor Callable to construct Container with no arguments
	 * @param json_data JSON string data containing array
	 * @param member_path A dot separated path of member names to start parsing
	 * from. Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @tparam KnownBounds The bounds of the json_data are known to contain the
	 * whole value
	 * @return A Container containing parsed data from JSON string
	 * @throws daw::json::json_exception
	 */
	template<typename JsonElement,
	         typename Container =
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         typename Constructor = daw::construct_a_t<Container>,
	         bool KnownBounds = false>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array( std::string_view json_data,
	                 std::string_view member_path = "" ) {
		daw_json_assert( not json_data.empty( ), ErrorReason::EmptyJSONDocument );
		daw_json_assert( member_path.data( ) != nullptr,
		                 ErrorReason::EmptyJSONPath );
		using element_type =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<element_type, void>,
		               "Unknown JsonElement type." );

		using parser_t = json_array<no_name, JsonElement, Container, Constructor>;

		auto [is_found, rng] = json_details::find_range<ParsePolicy>(
		  json_data, { member_path.data( ), member_path.size( ) } );

		if constexpr( parser_t::expected_type == JsonParseTypes::Null ) {
			if( not is_found ) {
				return typename parser_t::constructor_t{ }( );
			}
		} else {
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
		}
		rng.trim_left_unchecked( );
#if defined( _MSC_VER ) and not defined( __clang__ )
		// Work around MSVC ICE
		daw_json_assert( rng.is_opening_bracket_checked( ),
		                 ErrorReason::InvalidArrayStart, rng );
#else
		using Range = daw::remove_cvref_t<decltype( rng )>;
		daw_json_assert_weak( rng.is_opening_bracket_checked( ),
		                      ErrorReason::InvalidArrayStart, rng );
#endif

		return json_details::parse_value<parser_t, KnownBounds>(
		  ParseTag<JsonParseTypes::Array>{ }, rng );
	}

	namespace json_details {
		/***
		 * Tag type to indicate that the element of a Container is not being
		 * specified.  This is the default.
		 */
		struct auto_detect_array_element {};
	} // namespace json_details

	/**
	 * Serialize a container to JSON.  This convenience method allows for easier
	 * serialization of containers when the root of the document is an array
	 * Serialize Container
	 * @tparam Container Type of Container to serialize the elements of
	 * @tparam OutputIterator Iterator to write data to
	 * @param c Container containing data to serialize.
	 * @return OutputIterator with final state of iterator
	 */
	template<typename JsonElement = json_details::auto_detect_array_element,
	         typename Container, typename OutputIterator>
	[[maybe_unused]] constexpr OutputIterator
	to_json_array( Container const &c, OutputIterator out_it ) {
		static_assert(
		  daw::traits::is_container_like_v<daw::remove_cvref_t<Container>>,
		  "Supplied container must support begin( )/end( )" );

		if constexpr( std::is_pointer_v<OutputIterator> ) {
			daw_json_assert( out_it, ErrorReason::NullOutputIterator );
		}
		*out_it++ = '[';
		bool is_first = true;
		for( auto const &v : c ) {
			using v_type = daw::remove_cvref_t<decltype( v )>;
			constexpr bool is_auto_detect_v =
			  std::is_same_v<JsonElement, json_details::auto_detect_array_element>;
			using JsonMember =
			  std::conditional_t<is_auto_detect_v,
			                     json_details::unnamed_default_type_mapping<v_type>,
			                     JsonElement>;

			static_assert(
			  not std::is_same_v<
			    JsonMember, daw::json::missing_json_data_contract_for<JsonElement>>,
			  "Unable to detect unnamed mapping" );
			static_assert( not std::is_same_v<JsonElement, JsonMember> );
			if( is_first ) {
				is_first = false;
			} else {
				*out_it++ = ',';
			}
			out_it = json_details::member_to_string<JsonMember>( out_it, v );
		}
		// The last character will be a ',' prior to this
		*out_it++ = ']';
		return out_it;
	}

	/**
	 * Serialize a container to JSON.  This convenience method allows for easier
	 * serialization of containers when the root of the document is an array
	 * Serialize Container
	 * @tparam Container Type of Container to serialize the elements of
	 * @param c Container containing data to serialize.
	 * @return A std::string containing the serialized elements of c
	 */
	template<typename Result = std::string,
	         typename JsonElement = json_details::auto_detect_array_element,
	         typename Container>
	[[maybe_unused, nodiscard]] constexpr Result to_json_array( Container &&c ) {
		static_assert(
		  daw::traits::is_container_like_v<daw::remove_cvref_t<Container>>,
		  "Supplied container must support begin( )/end( )" );

		Result result{ };
		auto out_it = json_details::basic_appender<Result>( result );
		to_json_array<JsonElement>( c, out_it );
		return result;
	}

} // namespace daw::json

#if not defined( DAW_JSON_DISABLE_JSON_STRING_LITERAL )

/***
 * Construct a json_value from a string literal
 * @return A json_value representing the json document
 */
constexpr daw::json::json_value operator"" _dawjson( char const *ptr,
                                                     std::size_t sz ) {
	return daw::json::json_value( std::string_view( ptr, sz ) );
}
#endif
