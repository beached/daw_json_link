// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "impl/daw_iterator_range.h"
#include "impl/daw_json_link_impl.h"
#include "impl/daw_json_link_types_fwd.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iterator/daw_back_inserter.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
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
		/**
		 * Serialize a C++ class to JSON data
		 * @tparam OutputIterator An output iterator with a char value_type
		 * @tparam Args  tuple of values that map to the JSON members
		 * @param it OutputIterator to append string data to
		 * @param args members from C++ class
		 * @return the OutputIterator it
		 */
		template<typename OutputIterator, typename Value, typename... Args>
		[[maybe_unused, nodiscard]] static constexpr OutputIterator
		serialize( OutputIterator it, std::tuple<Args...> const &args,
		           Value const &v ) {
			static_assert( sizeof...( Args ) == sizeof...( JsonMembers ),
			               "Argument count is incorrect" );

			static_assert( ( json_details::is_a_json_type_v<JsonMembers> and ... ),
			               "Only value JSON types can be used" );
			return json_details::serialize_json_class<JsonMembers...>(
			  it, std::index_sequence_for<Args...>{}, args, v );
		}

		/**
		 *
		 * Parse JSON data and construct a C++ class.  This is used by parse_value
		 * to get back into a mode with a JsonMembers...
		 * @tparam T The result of parsing json_class
		 * @tparam First type of first iterator in range
		 * @tparam Last type of last iterator in range
		 * @tparam IsUnCheckedInput Is the input trusted, less checking is done
		 * @param rng JSON data to parse
		 * @return A T object
		 */
		template<typename T, typename First, typename Last, bool IsUnCheckedInput>
		[[maybe_unused, nodiscard]] static constexpr T
		parse( json_details::IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
			daw_json_assert_weak( rng.has_more( ), "Cannot parse an empty string" );
			return json_details::parse_json_class<T, JsonMembers...>(
			  rng, std::index_sequence_for<JsonMembers...>{} );
		}
	};

	// Member types
	template<JSONNAMETYPE Name, typename T, LiteralAsStringOpt LiteralAsString,
	         typename Constructor, JsonRangeCheck RangeCheck,
	         JsonNullable Nullable, SIMDModes SIMDMode>
	struct json_number {
		using i_am_a_json_type = void;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;
		static_assert(
		  Nullable == JsonNullable::Never or
		    std::is_same_v<parse_to_t, std::invoke_result_t<Constructor>>,
		  "Default ctor of constructor must match that of base" );
		using constructor_t = Constructor;
		static inline constexpr JSONNAMETYPE name = Name;

		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<json_details::number_parse_type_v<base_type>, Nullable>;

		static inline constexpr JsonParseTypes base_expected_type =
		  json_details::number_parse_type_v<base_type>;

		static inline constexpr LiteralAsStringOpt literal_as_string =
		  LiteralAsString;
		static inline constexpr JsonRangeCheck range_check = RangeCheck;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Number;
		static inline constexpr SIMDModes simd_mode = SIMDMode;
	};

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

		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Bool, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Bool;

		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr LiteralAsStringOpt literal_as_string =
		  LiteralAsString;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Bool;
	};

	template<JSONNAMETYPE Name, typename String, typename Constructor,
	         JsonNullable EmptyStringNull, EightBitModes EightBitMode,
	         JsonNullable Nullable>
	struct json_string_raw {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = String;
		using base_type = json_details::unwrap_type<String, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;

		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::StringRaw, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::StringRaw;
		static inline constexpr JsonNullable empty_is_null = EmptyStringNull;
		static inline constexpr EightBitModes eight_bit_mode = EightBitMode;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
	};

	template<JSONNAMETYPE Name, typename String, typename Constructor,
	         typename Appender, JsonNullable EmptyStringNull,
	         EightBitModes EightBitMode, JsonNullable Nullable>
	struct json_string {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = String;
		using base_type = json_details::unwrap_type<String, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, base_type>;
		using appender_t = Appender;

		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::StringEscaped, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::StringEscaped;
		static inline constexpr JsonNullable empty_is_null = EmptyStringNull;
		static inline constexpr EightBitModes eight_bit_mode = EightBitMode;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
	};

	template<JSONNAMETYPE Name, typename T, typename Constructor,
	         JsonNullable Nullable>
	struct json_date {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using wrapped_type = T;
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor, char const *, size_t>;

		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Date, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Date;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
	};

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
		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Class, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Class;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Class;
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
		static inline constexpr size_t base_map[5] = {
		  json_details::find_json_element<JsonBaseParseTypes::Number>(
		    {json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type...} ),
		  json_details::find_json_element<JsonBaseParseTypes::Bool>(
		    {json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type...} ),
		  json_details::find_json_element<JsonBaseParseTypes::String>(
		    {json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type...} ),
		  json_details::find_json_element<JsonBaseParseTypes::Class>(
		    {json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type...} ),
		  json_details::find_json_element<JsonBaseParseTypes::Array>(
		    {json_details::unnamed_default_type_mapping<
		      JsonElements>::underlying_json_type...} )};
	};

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
		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Variant, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Variant;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::None;
		using json_elements = JsonElements;
	};

	template<JSONNAMETYPE Name, typename T, typename TagMember, typename Switcher,
	         typename JsonElements, typename Constructor, JsonNullable Nullable>
	struct json_tagged_variant {
		using i_am_a_json_type = void;
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
		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::VariantTagged, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::VariantTagged;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::None;
		using json_elements = JsonElements;
	};

	template<JSONNAMETYPE Name, typename T, typename FromConverter,
	         typename ToConverter, CustomJsonTypes CustomJsonType,
	         JsonNullable Nullable>
	struct json_custom {
		using i_am_a_json_type = void;
		using to_converter_t = ToConverter;
		using from_converter_t = FromConverter;
		// TODO explore using char const *, size_t pair for ctor
		using base_type = json_details::unwrap_type<T, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<FromConverter, std::string_view>;

		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Custom, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Custom;
		static inline constexpr CustomJsonTypes custom_json_type = CustomJsonType;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::String;
	};

	template<JSONNAMETYPE Name, typename JsonElement, typename Container,
	         typename Constructor, typename Appender, JsonNullable Nullable>
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
		using appender_t = Appender;
		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::Array, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::Array;

		static_assert( json_element_t::name == no_name,
		               "All elements of json_array must be have no_name" );
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Array;
	};

	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType, typename Constructor, typename Appender,
	         JsonNullable Nullable>
	struct json_key_value {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using base_type = json_details::unwrap_type<Container, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		using appender_t = Appender;
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

		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::KeyValue, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::KeyValue;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Class;
	};

	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType, typename Constructor, typename Appender,
	         JsonNullable Nullable>
	struct json_key_value_array {
		using i_am_a_json_type = void;
		using constructor_t = Constructor;
		using base_type = json_details::unwrap_type<Container, Nullable>;
		static_assert( not std::is_same_v<void, base_type>,
		               "Failed to detect base type" );
		using parse_to_t = std::invoke_result_t<Constructor>;
		using appender_t = Appender;
		using json_key_t = json_details::unnamed_default_type_mapping<
		  JsonKeyType, json_details::default_key_name>;
		static_assert( not std::is_same_v<json_key_t, void>,
		               "Unknown JsonKeyType type." );
		static_assert( daw::string_view( json_key_t::name ) != daw::string_view( no_name ),
		               "Must supply a valid key member name" );
		using json_value_t = json_details::unnamed_default_type_mapping<
		  JsonValueType, json_details::default_value_name>;
		static_assert( not std::is_same_v<json_value_t, void>,
		               "Unknown JsonValueType type." );
		static_assert( json_value_t::name != no_name,
		               "Must supply a valid value member name" );
		static_assert( json_key_t::name != json_value_t::name,
		               "Key and Value member names cannot be the same" );
		static inline constexpr JSONNAMETYPE name = Name;
		static inline constexpr JsonParseTypes expected_type =
		  get_parse_type_v<JsonParseTypes::KeyValueArray, Nullable>;
		static inline constexpr JsonParseTypes base_expected_type =
		  JsonParseTypes::KeyValueArray;
		static inline constexpr JsonBaseParseTypes underlying_json_type =
		  JsonBaseParseTypes::Array;
	};

	/**
	 * Parse JSON and construct a T as the result.  This method
	 * provides checked json
	 * @tparam JsonClass type that has specialization of
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @return A reified T constructed from JSON data
	 */
	template<typename JsonClass>
	[[maybe_unused, nodiscard]] constexpr JsonClass
	from_json( std::string_view json_data ) {
		static_assert( json_details::has_json_data_contract_trait_v<JsonClass>,
		               "Expected a typed that has been mapped via specialization "
		               "of daw::json::json_data_contract" );
		return json_details::from_json_member_impl<JsonClass, false>( json_data );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.
	 * @tparam JsonMember The type of the item being parsed
	 * @param json_data JSON string data
	 * @param member_path A dot separated path of member names, default is the
	 * root.  Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @return A value reified from the JSON data member
	 */
	template<typename JsonMember>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json( std::string_view json_data, std::string_view member_path ) {
		return json_details::from_json_member_impl<JsonMember, false>(
		  json_data, member_path );
	}

	/**
	 * Parse JSON and construct a JsonClass as the result.  This method
	 * does not perform most checks on validity of data
	 * @tparam JsonClass type that has specialization of
	 * daw::json::json_data_contract
	 * @param json_data JSON string data
	 * @return A reified JsonClass constructed from JSON data
	 */
	template<typename JsonClass>
	[[maybe_unused, nodiscard]] constexpr JsonClass
	from_json_unchecked( std::string_view json_data ) {
		static_assert( json_details::has_json_data_contract_trait_v<JsonClass>,
		               "Expected a typed that has been mapped via specialization "
		               "of daw::json::json_data_contract" );
		return json_details::from_json_member_impl<JsonClass, true>( json_data );
	}

	/***
	 * Parse a JSONMember from the json_data starting at member_path.  This method
	 * performs less checking than the non-unchecked method
	 * @tparam JsonMember The type of the item being parsed
	 * @param json_data JSON string data
	 * @param member_path A dot separated path of member names, default is the
	 * root. Array indices are specified with square brackets e.g. [5] is the 6th
	 * item.
	 * @return A value reified from the JSON data member
	 */
	template<typename JsonMember>
	[[maybe_unused, nodiscard]] constexpr auto
	from_json_unchecked( std::string_view json_data,
	                     std::string_view member_path ) {
		return json_details::from_json_member_impl<JsonMember, true>( json_data,
		                                                              member_path );
	}

	/**
	 *
	 * @tparam Result std::string like type to put result into
	 * @tparam JsonClass Type that has json_parser_desription and to_json_data
	 * function overloads
	 * @param value  value to serialize
	 * @return  JSON string data
	 */
	template<typename Result = std::string, typename JsonClass>
	[[maybe_unused, nodiscard]] constexpr Result
	to_json( JsonClass const &value ) {
		static_assert( json_details::has_json_data_contract_trait_v<JsonClass>,
		               "Expected a typed that has been mapped via specialization "
		               "of daw::json::json_data_contract" );
		static_assert( json_details::has_json_to_json_data_v<JsonClass>,
		               "A function called to_json_data must exist for type." );

		Result result{};
		(void)
		  json_details::json_data_contract_trait_t<JsonClass>::template serialize(
		    daw::back_inserter( result ),
		    daw::json::json_data_contract<JsonClass>::to_json_data( value ),
		    value );
		return result;
	}

	namespace json_details {

		template<bool IsUnCheckedInput, typename JsonElement, typename Container,
		         typename Constructor, typename Appender>
		[[maybe_unused, nodiscard]] constexpr Container
		from_json_array_impl( std::string_view json_data,
		                      std::string_view member_path ) {
			using parser_t =
			  json_array<no_name, JsonElement, Container, Constructor, Appender>;

			auto [is_found, rng] = json_details::find_range<IsUnCheckedInput>(
			  json_data, {member_path.data( ), member_path.size( )} );
			if constexpr( parser_t::expected_type == JsonParseTypes::Null ) {
				if( not is_found ) {
					return typename parser_t::constructor_t{}( );
				}
			} else {
				daw_json_assert( is_found, "Could not find specified member" );
			}
			rng.trim_left_no_check( );
			daw_json_assert_weak( rng.front( '[' ), "Expected array class" );

			return parse_value<parser_t>( ParseTag<JsonParseTypes::Array>{}, rng );
		}

	} // namespace json_details

	/**
	 * Parse JSON data where the root item is an array
	 * @tparam JsonElement The type of each element in array.  Must be one of
	 * the above json_XXX classes.  This version is checked
	 * @tparam Container Container to store values in
	 * @tparam Constructor Callable to construct Container with no arguments
	 * @tparam Appender Callable to call with JsonElement
	 * @param json_data JSON string data containing array
	 * @param member_path A dot separated path of member names to start parsing
	 * from. Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @return A Container containing parsed data from JSON string
	 */
	template<typename JsonElement,
	         typename Container =
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = json_details::basic_appender<Container>>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array( std::string_view json_data,
	                 std::string_view member_path = "" ) {
		using element_type =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<element_type, void>,
		               "Unknown JsonElement type." );

		return json_details::from_json_array_impl<false, element_type, Container,
		                                          Constructor, Appender>(
		  json_data, member_path );
	}

	/**
	 * Parse JSON data where the root item is an array but do less checking
	 * @tparam JsonElement The type of each element in array.  Must be one of
	 * the above json_XXX classes.  This version isn't checked
	 * @tparam Container Container to store values in
	 * @tparam Constructor Callable to construct Container with no arguments
	 * @tparam Appender Callable to call with JsonElement
	 * @param json_data JSON string data containing array
	 * @param member_path A dot separated path of member names to start parsing
	 * from. Array indices are specified with square brackets e.g. [5] is the 6th
	 * item
	 * @return A Container containing parsed data from JSON string
	 */
	template<typename JsonElement,
	         typename Container =
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = json_details::basic_appender<Container>>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array_unchecked( std::string_view json_data,
	                           std::string_view member_path = "" ) {
		using element_type =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<element_type, void>,
		               "Unknown JsonElement type." );

		return json_details::from_json_array_impl<true, element_type, Container,
		                                          Constructor, Appender>(
		  json_data, member_path );
	}

	/**
	 * Serialize Container to a JSON array string
	 * @tparam Result std::string like type to serialize to
	 * @tparam Container Type of Container to serialize
	 * @param c Data to serialize
	 * @return A string containing the serialized elements of c
	 */
	template<typename Result = std::string, typename Container>
	[[maybe_unused, nodiscard]] constexpr Result to_json_array( Container &&c ) {
		static_assert(
		  daw::traits::is_container_like_v<daw::remove_cvref_t<Container>>,
		  "Supplied container must support begin( )/end( )" );

		Result result = "[";
		for( auto const &v : c ) {
			result += to_json( v );
			result += ',';
		}
		// The last character will be a ',' prior to this
		result.back( ) = ']';
		return result;
	}

	namespace json_details {

		template<typename... Args>
		constexpr void is_unique_ptr_test_impl( std::unique_ptr<Args...> const & );

		template<typename T>
		using is_unique_ptr_test =
		  decltype( is_unique_ptr_test_impl( std::declval<T>( ) ) );

		template<typename T>
		inline constexpr bool is_unique_ptr_v =
		  daw::is_detected_v<is_unique_ptr_test, T>;

	} // namespace json_details

	template<typename T>
	struct UniquePtrConstructor {
		static_assert( not json_details::is_unique_ptr_v<T>,
		               "T should be the type contained in the unique_ptr" );

		constexpr std::unique_ptr<T> operator( )( ) const {
			return {};
		}

		template<typename Arg, typename... Args>
		std::unique_ptr<T> operator( )( Arg &&arg, Args &&... args ) const {
			return std::make_unique<T>( std::forward<Arg>( arg ),
			                            std::forward<Args>( args )... );
		}
	};
} // namespace daw::json
