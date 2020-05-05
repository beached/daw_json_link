// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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

#include "daw_json_link_impl.h"
#include "daw_json_parse_name.h"

#include <daw/daw_utility.h>

#include <chrono>
#include <optional>
#include <string>

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
	         JsonNullable Nullable = JsonNullable::Never,
	         SIMDModes SIMDMode = SIMDModes::None>
	struct json_number;

	/**
	 * The member is a nullable number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam RangeCheck Check if thevalue will fit in the result
	 */
	template<JSONNAMETYPE Name, typename T = std::optional<double>,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonRangeCheck RangeCheck = JsonRangeCheck::Never>
	using json_number_null = json_number<Name, T, LiteralAsString, Constructor,
	                                     RangeCheck, JsonNullable::Nullable>;

#ifdef DAW_ALLOW_SSE3
	/**
	 * The member is a nullable number.  Requires at least 16bytes padding after
	 * any value
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam RangeCheck Check if the value will fit in the result
	 */
	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
	         JsonNullable Nullable = JsonNullable::Never>
	using json_number_sse3 = json_number<Name, T, LiteralAsString, Constructor,
	                                     RangeCheck, Nullable, SIMDModes::SSE3>;
#endif
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

#ifdef DAW_ALLOW_SSE3
	/**
	 * The member is a range checked number Requires at least 16bytes padding
	 * after any value
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
	using json_checked_number_sse3 =
	  json_number_sse3<Name, T, LiteralAsString, Constructor,
	                   JsonRangeCheck::CheckForNarrowing, Nullable>;
#endif
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
	 * @tparam Constructor a callable taking as arguments ( char const *,
	 * std::size_t )
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
	 * string data.  Not all invalid codepoints are detected
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *,
	 * std::size_t )
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
	 * @tparam Constructor a callable taking as arguments ( char const *,
	 * std::size_t )
	 * @tparam Appender Allows appending characters to the output object
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename String = std::string,
	         typename Constructor = daw::construct_a_t<String>,
	         typename Appender = json_details::basic_appender<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_string;

	/**
	 * Member is a nullable escaped string and requires unescaping and escaping of
	 * string data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *,
	 * std::size_t )
	 * @tparam Appender Allows appending characters to the output object
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 */
	template<JSONNAMETYPE Name, typename String = std::optional<std::string>,
	         typename Constructor =
	           daw::construct_a_t<std::optional<std::string>>,
	         typename Appender = json_details::basic_appender<std::string>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull>
	using json_string_null =
	  json_string<Name, String, Constructor, Appender, EmptyStringNull,
	              EightBitMode, JsonNullable::Nullable>;

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
	           construct_from_iso8601_timestamp<JsonNullable::Never>,
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
	         typename Constructor =
	           construct_from_iso8601_timestamp<JsonNullable::Nullable>>
	using json_date_null =
	  json_date<Name, T, Constructor, JsonNullable::Nullable>;

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
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
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
	         typename Constructor = daw::construct_a_t<T>>
	using json_class_null =
	  json_class<Name, T, Constructor, JsonNullable::Nullable>;

	namespace json_details {
		/** Link to a JSON array that has been detected
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
		template<JSONNAMETYPE Name, typename JsonElement, typename Container,
		         typename Constructor = daw::construct_a_t<Container>,
		         typename Appender = json_details::basic_appender<Container>,
		         JsonNullable Nullable = JsonNullable::Never>
		struct json_array_detect;

		namespace vector_detect {
			template<typename T, typename... Alloc>
			[[maybe_unused]] auto vector_test( daw::tag_t<std::vector<T, Alloc...>> ) -> T;

			template<typename T>
			[[maybe_unused]] int vector_test( daw::tag_t<T> );

			template<typename T>
			using detector =
			  std::remove_reference_t<decltype( vector_test( daw::tag<T> ) )>;
		} // namespace vector_detect

		template<typename T>
		inline constexpr bool is_vector_v =
		  daw::is_detected_v<vector_detect::detector, T>;

		template<typename T, JSONNAMETYPE Name = no_name>
		using unnamed_default_type_mapping = std::conditional_t<
		  json_details::is_a_json_type_v<T>, T,
		  std::conditional_t<
		    has_json_data_contract_trait_v<T>, json_class<Name, T>,
		    std::conditional_t<
		      std::is_same_v<T, bool>, json_bool<Name, T>,
		      std::conditional_t<
		        std::is_arithmetic_v<T> or std::is_enum_v<T>, json_number<Name, T>,
		        std::conditional_t<
		          std::is_same_v<T, std::string_view>, json_string_raw<Name, T>,
		          std::conditional_t<
		            std::is_same_v<T, daw::string_view>, json_string_raw<Name, T>,
		            std::conditional_t<
		              daw::traits::is_string_v<T>, json_string<Name, T>,
		              std::conditional_t<
		                not can_deref_v<T> and is_vector_v<T>,
		                json_array_detect<Name, vector_detect::detector<T>, T>,
		                daw::json::missing_json_data_contract_for<T>>>>>>>>>;

		template<typename T>
		using has_unnamed_default_type_mapping = daw::not_trait<
		  std::is_same<unnamed_default_type_mapping<T>,
		               daw::json::missing_json_data_contract_for<T>>>;

		template<typename JsonMember, bool IsUnCheckedInput>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_member_impl( std::string_view json_data ) {
			using json_member = unnamed_default_type_mapping<JsonMember>;
			auto rng = IteratorRange<char const *, IsUnCheckedInput>(
			  json_data.data( ),
			  json_data.data( ) + static_cast<ptrdiff_t>( json_data.size( ) ) );

			return json_details::parse_value<json_member>(
			  ParseTag<json_member::expected_type>{ }, rng );
		}

	} // namespace json_details

	/***
	 * A type to hold the types for parsing tagged variants.
	 * @tparam JsonElements a list of types that can be parsed,
	 */
	template<typename... JsonElements>
	struct json_tagged_variant_type_list {
		using i_am_tagged_variant_type_list = void;
		using element_map_t =
		  std::tuple<json_details::unnamed_default_type_mapping<JsonElements>...>;
	};

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
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = json_details::basic_appender<Container>,
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
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = json_details::basic_appender<Container>>
	using json_array_null = json_array<Name, JsonElement, Container, Constructor,
	                                   Appender, JsonNullable::Nullable>;

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
	 *  @tparam Appender A callable used to add elements to container.
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = json_details::basic_kv_appender<Container>,
	         JsonNullable Nullable = JsonNullable::Never>
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
	 *  @tparam Appender A callable used to add elements to container.
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = json_details::basic_kv_appender<Container>>
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
	         typename Appender = json_details::basic_kv_appender<Container>,
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
	         typename Appender = json_details::basic_kv_appender<Container>>
	using json_key_value_array_null =
	  json_key_value_array<Name, Container, JsonValueType, JsonKeyType,
	                       Constructor, Appender, JsonNullable::Nullable>;

	/**
	 * Allow parsing of a type that does not fit
	 * @tparam Name Name of JSON member to link to
	 * @tparam T type of value being constructed
	 * @tparam FromConverter Callable that accepts a std::string_view of the range
	 * to parse.  The default requires an overload of from_string( daw::tag<T>,
	 * std::string_view ) that returns a T
	 * @tparam ToConverter Returns a string from the value.  The default requires
	 * a to_string( T const & ) overload that returns a String like type
	 * @tparam CustomJsonType JSON type value is encoded as literal/string
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename FromConverter = custom_from_converter_t<T>,
	         typename ToConverter = custom_to_converter_t<T>,
	         CustomJsonTypes CustomJsonType = CustomJsonTypes::Either,
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
	         CustomJsonTypes CustomJsonType = CustomJsonTypes::Either>
	using json_custom_null = json_custom<Name, T, FromConverter, ToConverter,
	                                     CustomJsonType, JsonNullable::Nullable>;

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
			return std::numeric_limits<std::size_t>::max( );
		}

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
	template<JSONNAMETYPE Name, typename T, typename JsonElements,
	         typename Constructor = daw::construct_a_t<T>,
	         JsonNullable Nullable = JsonNullable::Never>
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
	template<JSONNAMETYPE Name, typename T, typename JsonElements,
	         typename Constructor = daw::construct_a_t<T>>
	using json_variant_null =
	  json_variant<Name, T, JsonElements, Constructor, JsonNullable::Nullable>;

	namespace json_details {
		template<typename T>
		struct unknown_variant_type {
			using i_am_tagged_variant_type_list = void;
		};

		template<typename... Ts>
		struct missing_default_type_mapping {
			using i_am_tagged_variant_type_list = void;
		};

		template<typename... Ts>
		[[maybe_unused]] constexpr std::conditional_t<
		  std::conjunction_v<has_unnamed_default_type_mapping<Ts>...>,
		  json_tagged_variant_type_list<unnamed_default_type_mapping<Ts>...>,
		  missing_default_type_mapping<unnamed_default_type_mapping<Ts>...>>
		get_variant_type_list( std::variant<Ts...> const * );

		template<typename T>
		using underlying_nullable_type = decltype( *std::declval<T>( ) );

		template<typename T>
		using detected_underlying_nullable_type =
		  std::remove_reference_t<daw::detected_t<underlying_nullable_type, T>>;

		template<typename T>
		inline constexpr bool is_nullable_type =
		  daw::is_detected_v<underlying_nullable_type, T>;

		template<typename T>
		[[maybe_unused]] constexpr unknown_variant_type<T> get_variant_type_list( T const * );

		struct cannot_deduce_variant_element_types {};

		template<JsonNullable Nullable, typename Variant>
		using determine_variant_element_types = std::conditional_t<
		  Nullable == JsonNullable::Never or not is_nullable_type<Variant>,
		  std::remove_reference_t<decltype(
		    get_variant_type_list( std::declval<Variant const *>( ) ) )>,
		  std::conditional_t<
		    is_nullable_type<Variant>,
		    std::remove_reference_t<decltype( get_variant_type_list(
		      std::declval<
		        detected_underlying_nullable_type<Variant> const *>( ) ) )>,
		    cannot_deduce_variant_element_types>>;
	} // namespace json_details

	/***
	 * Link to a variant like data type that is discriminated via another member.
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
	  typename JsonElements =
	    json_details::determine_variant_element_types<JsonNullable::Never, T>,
	  typename Constructor = daw::construct_a_t<T>,
	  JsonNullable Nullable = JsonNullable::Never>
	struct json_tagged_variant;

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
	template<
	  JSONNAMETYPE Name, typename T, typename TagMember, typename Switcher,
	  typename JsonElements =
	    json_details::determine_variant_element_types<JsonNullable::Nullable, T>,
	  typename Constructor = daw::construct_a_t<T>>
	using json_tagged_variant_null =
	  json_tagged_variant<Name, T, TagMember, Switcher, JsonElements, Constructor,
	                      JsonNullable::Nullable>;

	namespace json_details {

		template<typename JsonMember, bool IsUnCheckedInput>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_member_impl( std::string_view json_data,
		                       std::string_view member_path ) {
			using json_member = unnamed_default_type_mapping<JsonMember>;
			auto [is_found, rng] = json_details::find_range<IsUnCheckedInput>(
			  json_data, { member_path.data( ), member_path.size( ) } );
			if constexpr( json_member::expected_type == JsonParseTypes::Null ) {
				if( not is_found ) {
					return typename json_member::constructor_t{ }( );
				}
			} else {
				daw_json_assert( is_found,
				                 "Could not find member and type isn't Nullable" );
			}
			return json_details::parse_value<json_member>(
			  ParseTag<json_member::expected_type>{ }, rng );
		}

	} // namespace json_details
} // namespace daw::json
