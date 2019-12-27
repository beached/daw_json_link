// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <string>

#include <daw/daw_utility.h>

#include "daw_json_link_impl.h"

namespace daw::json {
	// Forward Declaration for some literal types used in json_arrays

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
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
	         JsonNullable Nullable = JsonNullable::Never>
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
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonRangeCheck RangeCheck = JsonRangeCheck::Never>
	using json_number_null = json_number<Name, T, LiteralAsString, Constructor,
	                                     RangeCheck, JsonNullable::Nullable>;

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
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	using json_checked_number =
	  json_number<Name, T, LiteralAsString, Constructor,
	              JsonRangeCheck::CheckForNarrowing, Nullable>;

	/**
	 * The member is a nullable range checked number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. optional<double>, optional<int>,
	 * optional<unsigned>...) to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 */
	template<JSONNAMETYPE Name, typename T = std::optional<double>,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>>
	using json_checked_number_null =
	  json_number<Name, T, LiteralAsString, Constructor,
	              JsonRangeCheck::CheckForNarrowing, JsonNullable::Nullable>;

	/**
	 * The member is a boolean
	 * @tparam Name name of json member
	 * @tparam T result type to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T = bool,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_bool;

	/**
	 * The member is a nullable boolean
	 * @tparam Name name of json member
	 * @tparam T result type to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 */
	template<JSONNAMETYPE Name, typename T = std::optional<bool>,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>>
	using json_bool_null =
	  json_bool<Name, T, LiteralAsString, Constructor, JsonNullable::Nullable>;

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *, size_t )
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * arguments
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename String = std::string,
	         typename Constructor = daw::construct_a_t<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_string_raw;

	/**
	 * Member is a nullable escaped string and requires unescaping and escaping of
	 * string data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *, size_t )
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * arguments
	 */
	template<JSONNAMETYPE Name, typename String = std::optional<std::string>,
	         typename Constructor = daw::construct_a_t<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull>
	using json_string_raw_null =
	  json_string_raw<Name, String, Constructor, EmptyStringNull, EightBitMode,
	                  JsonNullable::Nullable>;

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *, size_t )
	 * @tparam Appender Allows appending characters to the output object
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename String = std::string,
	         typename Constructor = daw::construct_a_t<String>,
	         typename Appender = impl::basic_appender<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_string;

	/**
	 * Member is a nullable escaped string and requires unescaping and escaping of
	 * string data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *, size_t )
	 * @tparam Appender Allows appending characters to the output object
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 */
	template<JSONNAMETYPE Name, typename String = std::optional<std::string>,
	         typename Constructor =
	           daw::construct_a_t<std::optional<std::string>>,
	         typename Appender = impl::basic_appender<std::string>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull>
	using json_string_null =
	  json_string<Name, String, Constructor, Appender, EmptyStringNull,
	              EightBitMode, JsonNullable::Nullable>;

	/**
	 * Link to a JSON string representing a date
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type to consruct, by default is a time_point
	 * @tparam Constructor A Callable used to construct a T.
	 * Must accept a char pointer and size as argument to the date/time string.
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name,
	         typename T = std::chrono::time_point<std::chrono::system_clock,
	                                              std::chrono::milliseconds>,
	         typename Constructor = parse_js_date<JsonNullable::Never>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_date;

	/**
	 * Link to a JSON string representing a nullable date
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type to consruct, by default is a time_point
	 * @tparam Constructor A Callable used to construct a T.
	 * Must accept a char pointer and size as argument to the date/time string.
	 */
	template<JSONNAMETYPE Name,
	         typename T = std::optional<std::chrono::time_point<
	           std::chrono::system_clock, std::chrono::milliseconds>>,
	         typename Constructor = parse_js_date<JsonNullable::Nullable>>
	using json_date_null =
	  json_date<Name, T, Constructor, JsonNullable::Nullable>;

	/**
	 * Link to a JSON class
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type being parsed to.  Must have a describe_json_class
	 * overload
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_class;

	/**
	 * Link to a nullable JSON class
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type being parsed to.  Must have a describe_json_class
	 * overload
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename Constructor = daw::construct_a_t<T>>
	using json_class_null =
	  json_class<Name, T, Constructor, JsonNullable::Nullable>;

	/**
	 * Allow parsing of a type that does not fit
	 * @tparam Name Name of JSON member to link to
	 * @tparam T type of value being constructed
	 * @tparam FromConverter Callable that accepts a std::string_view of the range
	 * to parse
	 * @tparam ToConverter Returns a string from the value
	 * @tparam CustomJsonType JSON type value is encoded as literal/string
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename FromConverter = custom_from_converter_t<T>,
	         typename ToConverter = custom_to_converter_t<T>,
	         CustomJsonTypes CustomJsonType = CustomJsonTypes::String,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_custom;

	/**
	 * Allow parsing of a nullable type that does not fit
	 * @tparam Name Name of JSON member to link to
	 * @tparam T type of value being constructed
	 * @tparam FromConverter Callable that accepts a std::string_view of the range
	 * to parse
	 * @tparam ToConverter Returns a string from the value
	 * @tparam CustomJsonType JSON type value is encoded as literal/string
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename FromConverter = custom_from_converter_t<T>,
	         typename ToConverter = custom_to_converter_t<T>,
	         CustomJsonTypes CustomJsonType = CustomJsonTypes::String>
	using json_custom_null = json_custom<Name, T, FromConverter, ToConverter,
	                                     CustomJsonType, JsonNullable::Nullable>;

	template<JsonBaseParseTypes PT>
	constexpr size_t
	find_json_element( std::initializer_list<JsonBaseParseTypes> pts ) {
		size_t idx = 0;
		for( auto const &pt : pts ) {
			if( pt == PT ) {
				return idx;
			}
			++idx;
		}
		return std::numeric_limits<size_t>::max( );
	}

	/***
	 * A type to hold the types for parsing variants.
	 * @tparam JsonElements Up to one of a JsonElement that is a JSON number,
	 * string, object, or array
	 */
	template<typename... JsonElements>
	struct json_variant_type_list {
		using i_am_variant_element_list = void;
		static_assert(
		  sizeof...( JsonElements ) <= 5U,
		  "There can be at most 5 items, one for each JsonBaseParseTypes" );
		using element_map_t = std::tuple<JsonElements...>;
		static constexpr size_t base_map[5] = {
		  find_json_element<JsonBaseParseTypes::Number>(
		    {JsonElements::underlying_json_type...} ),
		  find_json_element<JsonBaseParseTypes::Bool>(
		    {JsonElements::underlying_json_type...} ),
		  find_json_element<JsonBaseParseTypes::String>(
		    {JsonElements::underlying_json_type...} ),
		  find_json_element<JsonBaseParseTypes::Class>(
		    {JsonElements::underlying_json_type...} ),
		  find_json_element<JsonBaseParseTypes::Array>(
		    {JsonElements::underlying_json_type...} )};
	};

	/***
	 * Link to a variant like data type.  The JSON member can be any one of the
	 * json types.  This precludes having more than one class type or array
	 * type(including their specialized keyvalue mappings) or
	 * string-enum/int-enum.
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type being parsed to.  Must have a describe_json_class
	 * overload
	 * @tparam JsonElements a json_variant_type_list
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 * @tparam Nullable Can the member be missing or have a null value	 *
	 */
	template<JSONNAMETYPE Name, typename T, typename JsonElements,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_variant;

	/***
	 * Link to a nullable JSON variant
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type being parsed to.  Must have a describe_json_class
	 * overload
	 * @tparam JsonElements a json_variant_type_list
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 */
	template<JSONNAMETYPE Name, typename T, typename JsonElements,
	         typename Constructor = daw::construct_a_t<T>>
	using json_variant_null =
	  json_variant<Name, T, JsonElements, Constructor, JsonNullable::Nullable>;

	namespace impl {
		template<typename T, JSONNAMETYPE Name = no_name>
		using ary_val_t = std::conditional_t<
		  is_a_json_type_v<T>, T,
		  std::conditional_t<
		    has_json_parser_description_v<T>, json_class<Name, T>,
		    std::conditional_t<
		      std::is_same_v<T, bool>, json_bool<Name, T>,
		      std::conditional_t<std::is_arithmetic_v<T> or std::is_enum_v<T>,
		                         json_number<Name, T>,
		                         std::conditional_t<daw::traits::is_string_v<T>,
		                                            json_string<Name, T>, void>>>>>;
	}

	/** Link to a JSON array
	 * @tparam Name name of JSON member to link to
	 * @tparam Container type of C++ container being constructed(e.g.
	 * vector<int>)
	 * @tparam JsonElement Json type being parsed e.g. json_number,
	 * json_string...
	 * @tparam Constructor A callable used to make Container,
	 * default will use the Containers constructor.  Both normal and aggregate
	 * are supported @tparam Appender Callable used to add items to the
	 * container.  The parsed type from JsonElement
	 * passed to it
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename JsonElement,
	         typename Container =
	           std::vector<typename impl::ary_val_t<JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_array;

	/** Link to a nullable JSON array
	 * @tparam Name name of JSON member to link to
	 * @tparam Container type of C++ container being constructed(e.g.
	 * vector<int>)
	 * @tparam JsonElement Json type being parsed e.g. json_number,
	 * json_string...
	 * @tparam Constructor A callable used to make Container,
	 * default will use the Containers constructor.  Both normal and aggregate
	 * are supported @tparam Appender Callable used to add items to the
	 * container.  The parsed type from JsonElement
	 * passed to it
	 */
	template<JSONNAMETYPE Name, typename JsonElement,
	         typename Container =
	           std::vector<typename impl::ary_val_t<JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>>
	using json_array_null = json_array<Name, JsonElement, Container, Constructor,
	                                   Appender, JsonNullable::Nullable>;

	/** Map a KV type json class { "Key String": ValueType, ... }
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
	 *  @tparam Appender A callable used to add elements to container.
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_kv_appender<Container>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_key_value;

	/** Map a nullable KV type json class { "Key String": ValueType, ... }
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
	 *  @tparam Appender A callable used to add elements to container.
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_kv_appender<Container>>
	using json_key_value_null =
	  json_key_value<Name, Container, JsonValueType, JsonKeyType, Constructor,
	                 Appender, JsonNullable::Nullable>;

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
	 *  @tparam Appender A callable used to add elements to container.
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_kv_appender<Container>,
	         JsonNullable Nullable = JsonNullable::Never>
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
	 *  @tparam JsonKeyType type of key in kv pair.  If specific json member type
	 * isn't specified, the key name defaults to "key"
	 *  @tparam Constructor A callable used to make Container, default will use
	 * the Containers constructor.  Both normal and aggregate are supported
	 *  @tparam Appender A callable used to add elements to container.
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_kv_appender<Container>>
	using json_key_value_array_null =
	  json_key_value_array<Name, Container, JsonValueType, JsonKeyType,
	                       Constructor, Appender, JsonNullable::Nullable>;

} // namespace daw::json
