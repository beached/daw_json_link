// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_json_link_types_fwd.h"

#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_visit.h>

#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>

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
			static_assert( sizeof...( Args ) == sizeof...( JsonMembers ),
			               "Argument count is incorrect" );

			return json_details::serialize_json_class<JsonMembers...>(
			  it, std::index_sequence_for<Args...>{ }, args, v );
		}

		template<typename Constructor, typename T>
		using result_type =
		  json_details::json_class_parse_result_t<Constructor, JsonMembers...>;
		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam Range Input range type
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename JsonClass, typename Range>
		[[maybe_unused, nodiscard]] DAW_ONLY_FLATTEN static constexpr auto
		parse_to_class( Range &rng ) {
			static_assert( json_details::is_a_json_type_v<JsonClass> );
			static_assert( json_details::has_json_data_contract_trait_v<
			                 typename JsonClass::base_type>,
			               "Unexpected type" );
			return json_details::parse_json_class<JsonClass, JsonMembers...>(
			  rng, std::index_sequence_for<JsonMembers...>{ } );
		}
	};

	namespace json_details {
		template<typename T>
		struct json_identity {
			using type = T;
		};
	} // namespace json_details

	template<typename JsonMember>
	struct json_class_map {
		using i_am_a_json_member_list = void;
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		static_assert( json_details::is_a_json_type_v<json_member>,
		               "Only JSON Link mapping types can appear in a "
		               "json_class_map(e.g. json_number, json_string...)" );
		static_assert( is_no_name<json_member>,
		               "The JSONMember cannot be named, it does not make sense in "
		               "this context" );

		template<typename OutputIterator, typename Member, typename Value>
		[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
		serialize( OutputIterator it, Member const &m, Value const &v ) {
			using member_json_t = json_details::unnamed_default_type_mapping<Member>;
			return json_details::member_to_string<member_json_t>( it, m );
		}

		template<typename Constructor, typename T>
		using result_type =
		  json_details::json_class_parse_result_t<Constructor, json_member>;

		template<typename JsonClass, typename Range>
		[[maybe_unused, nodiscard]] DAW_ONLY_FLATTEN static constexpr auto
		parse_to_class( Range &rng ) {
			static_assert( json_details::is_a_json_type_v<JsonClass> );
			static_assert( json_details::has_json_data_contract_trait_v<
			                 typename JsonClass::base_type>,
			               "Unexpected type" );
			using Constructor = typename JsonClass::constructor_t;
			return json_details::construct_value<JsonClass>(
			  Constructor{ }, rng,
			  json_details::parse_value<json_member, false>(
			    ParseTag<json_member::expected_type>{ }, rng ) );
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
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using json_member = json_details::unnamed_default_type_mapping<JsonMember>;
		using parse_to_t = typename json_member::parse_to_t;
	};

	namespace json_details {
		template<typename JsonMember>
		struct ordered_member_wrapper
		  : json_details::unnamed_default_type_mapping<JsonMember> {
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
		};
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

		template<typename, typename JsonClass>
		using result_type = JsonClass;
		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam Range Input range type
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename JsonClass, typename Range>
		[[maybe_unused,
		  nodiscard]] static inline constexpr json_details::json_result<JsonClass>
		parse_to_class( Range &rng ) {
			static_assert( json_details::is_a_json_type_v<JsonClass> );
			static_assert( json_details::has_json_data_contract_trait_v<
			                 typename JsonClass::base_type>,
			               "Unexpected type" );
			return json_details::parse_ordered_json_class<
			  JsonClass, json_details::ordered_member_wrapper<JsonMembers>...>( rng );
		}
	};

	/***
	 * Parse a tagged variant like class where the tag member is in the same class
	 * that is being discriminated.  The container type, that will specialize
	 * json_data_construct on, must support the get_if, get_index
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

		// TODO: use Constructor result
		template<typename, typename JsonClass>
		using result_type = JsonClass;
		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam Range Input range type
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename JsonClass, typename Range>
		[[maybe_unused, nodiscard]] static inline constexpr json_details::
		  from_json_result_t<JsonClass>
		  parse_to_class( Range &rng ) {
			static_assert( json_details::is_a_json_type_v<JsonClass> );
			static_assert( json_details::has_json_data_contract_trait_v<
			                 typename JsonClass::base_type>,
			               "Unexpected type" );
			using tag_class_t = tuple_json_mapping<TagMember>;
			std::size_t const idx = [rng]( ) mutable {
				return Switcher{ }( std::get<0>(
				  json_details::parse_value<json_class<no_name, tag_class_t>>(
				    ParseTag<JsonParseTypes::Class>{ }, rng )
				    .members ) );
			}( );
			return json_details::parse_nth_class<0, JsonClass, false,
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
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
		               "Failed to detect base type" );
		// using parse_to_t = std::invoke_result_t<Constructor, base_type>;
		static_assert(
		  std::is_invocable_v<Constructor, char const *, char const *> );

		using parse_to_t =
		  std::invoke_result_t<Constructor, char const *, char const *>;

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
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
		               "Failed to detect base type" );
		using data_contract = json_data_contract_trait_t<base_type>;
		using parse_to_t = typename std::conditional_t<
		  force_aggregate_construction_v<base_type>,
		  json_details::json_identity<base_type>,
		  json_details::json_identity<typename data_contract::template result_type<
		    constructor_t, base_type>>>::type;

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
		static_assert(
		  ( json_details::has_unnamed_default_type_mapping_v<JsonElements> and
		    ... ),
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
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
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
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
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using json_element_t =
			  json_details::unnamed_default_type_mapping<JsonElement>;
			static_assert( traits::not_same_v<json_element_t, void>,
			               "Unknown JsonElement type." );
			static_assert( json_details::is_a_json_type_v<json_element_t>,
			               "Error determining element type" );
			using constructor_t = Constructor;

			using base_type = json_details::unwrap_type<Container, Nullable>;
			static_assert( traits::not_same_v<void, base_type>,
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
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using json_element_t =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( traits::not_same_v<json_element_t, void>,
		               "Unknown JsonElement type." );
		static_assert( json_details::is_a_json_type_v<json_element_t>,
		               "Error determining element type" );
		using constructor_t = Constructor;

		using base_type = json_details::unwrap_type<Container, Nullable>;
		static_assert( traits::not_same_v<void, base_type>,
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
		static_assert( traits::not_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonValueType>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using json_element_t =
		  json_details::unnamed_default_type_mapping<JsonValueType>;
		static_assert( traits::not_same_v<json_element_t, void>,
		               "Unknown JsonValueType type." );
		static_assert( json_element_t::name == no_name,
		               "Value member name must be the default no_name" );
		static_assert(
		  json_details::has_unnamed_default_type_mapping_v<JsonKeyType>,
		  "Missing specialization of daw::json::json_data_contract for class "
		  "mapping or specialization of daw::json::json_link_basic_type_map" );
		using json_key_t = json_details::unnamed_default_type_mapping<JsonKeyType>;
		static_assert( traits::not_same_v<json_key_t, void>,
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
		static_assert( traits::not_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		using json_key_t = json_details::unnamed_default_type_mapping<
		  JsonKeyType, json_details::default_key_name>;
		static_assert( traits::not_same_v<json_key_t, void>,
		               "Unknown JsonKeyType type." );
		static_assert( daw::string_view( json_key_t::name ) !=
		                 daw::string_view( no_name ),
		               "Must supply a valid key member name" );
		using json_value_t = json_details::unnamed_default_type_mapping<
		  JsonValueType, json_details::default_value_name>;

		using json_class_t =
		  json_class<no_name, tuple_json_mapping<json_key_t, json_value_t>>;
		static_assert( traits::not_same_v<json_value_t, void>,
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
		static_assert( traits::not_same_v<void, base_type>,
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
} // namespace daw::json
