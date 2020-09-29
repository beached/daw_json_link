// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link_impl.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_value_fwd.h"

#include <daw/cpp_17.h>
#include <daw/daw_arith_traits.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

#include <chrono>
#include <ciso646>
#include <optional>
#include <string>

namespace daw::json {

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
	 * @tparam RangeCheck Check if thevalue will fit in the result
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
	         typename Constructor = daw::construct_a_t<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         JsonNullable Nullable = JsonNullable::Never,
	         AllowEscapeCharacter AllowEscape = AllowEscapeCharacter::Allow>
	struct json_string_raw;

	namespace json_details {
		template<typename WrappedItem, bool>
		struct wrapped_item_ctor_t_impl {
			constexpr WrappedItem operator( )( ) const {
				return daw::construct_a<WrappedItem>( );
			}

			template<typename... Args, std::enable_if_t<( sizeof...( Args ) > 0 ),
			                                            std::nullptr_t> = nullptr>
			constexpr auto operator( )( Args &&... args ) const {
				using wrapped_type =
				  daw::remove_cvref_t<decltype( *std::declval<WrappedItem>( ) )>;
				static_assert( std::is_constructible_v<WrappedItem, wrapped_type> );
				static_assert( std::is_constructible_v<wrapped_type, Args...> );
				return daw::construct_a<WrappedItem>(
				  daw::construct_a<wrapped_type>( std::forward<Args>( args )... ) );
			}
		};

		template<typename WrappedItem>
		struct wrapped_item_ctor_t_impl<WrappedItem, false>
		  : private daw::construct_a_t<WrappedItem> {
			using daw::construct_a_t<WrappedItem>::operator( );
		};
	} // namespace json_details

	template<typename WrappedItem>
	using wrapped_item_ctor_t = json_details::wrapped_item_ctor_t_impl<
	  WrappedItem, json_details::can_deref_v<WrappedItem>>;

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
	 * @tparam AllowEscape Tell parser if we know a \ or escape will be in the
	 * data
	 */
	template<JSONNAMETYPE Name, typename String = std::optional<std::string>,
	         typename Constructor = wrapped_item_ctor_t<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         AllowEscapeCharacter AllowEscape = AllowEscapeCharacter::Allow>
	using json_string_raw_null =
	  json_string_raw<Name, String, Constructor, EmptyStringNull, EightBitMode,
	                  JsonNullable::Nullable, AllowEscape>;

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
	         typename Constructor = wrapped_item_ctor_t<String>,
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
	 * @tparam T C++ type to construct, by default is a time_point
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
		 * are supported
		 * @tparam Nullable Can the member be missing or have a null value
		 */
		template<JSONNAMETYPE Name, typename JsonElement, typename Container,
		         typename Constructor = daw::construct_a_t<Container>,
		         JsonNullable Nullable = JsonNullable::Never>
		struct json_array_detect;

		namespace vector_detect {
			struct not_vector {};
			template<typename T, typename... Alloc>
			[[maybe_unused]] auto vector_test( daw::tag_t<std::vector<T, Alloc...>> )
			  -> T;

			template<typename T>
			[[maybe_unused]] not_vector vector_test( daw::tag_t<T> );

			template<typename T>
			using detector =
			  std::remove_reference_t<decltype( vector_test( daw::tag<T> ) )>;
		} // namespace vector_detect

		template<typename T>
		using is_vector = daw::not_trait<
		  std::is_same<vector_detect::detector<T>, vector_detect::not_vector>>;
	} // namespace json_details

	template<typename Mapped, bool Found = true>
	struct json_link_quick_map_type : std::bool_constant<Found> {
		using mapped_type = Mapped;
	};

	template<JSONNAMETYPE Name, typename T>
	constexpr auto json_link_quick_map( ) -> daw::if_t<
	  std::is_same_v<T, std::string_view>,
	  json_link_quick_map_type<json_string_raw<Name, std::string_view>>,
	  daw::if_t<std::is_same_v<T, daw::string_view>,
	            json_link_quick_map_type<json_string_raw<Name, daw::string_view>>,
	            daw::if_t<std::is_same_v<T, std::string>,
	                      json_link_quick_map_type<json_string<Name>>,
	                      daw::if_t<std::is_same_v<T, bool>,
	                                json_link_quick_map_type<json_bool<Name>>,
	                                json_link_quick_map_type<void, false>>>>>;
	/***
	 * Check if the current type has a quick map specialized for it
	 */
	template<typename T>
	inline constexpr bool has_json_link_quick_map_v =
	  decltype( json_link_quick_map<no_name, T>( ) )::value;

	/***
	 * Get the quick mapped json type for type T
	 */
	template<JSONNAMETYPE Name, typename T>
	using json_link_quick_map_t =
	  typename decltype( json_link_quick_map<Name, T>( ) )::mapped_type;

	namespace json_details {
		template<typename T, JSONNAMETYPE Name = no_name>
		using unnamed_default_type_mapping = daw::if_t<
		  json_details::is_a_json_type_v<T>, T,
		  daw::if_t<
		    has_json_data_contract_trait_v<T>, json_class<Name, T>,
		    daw::if_t<
		      has_json_link_quick_map_v<T>, json_link_quick_map_t<Name, T>,
		      daw::if_t<
		        std::disjunction_v<daw::is_arithmetic<T>, std::is_enum<T>>,
		        json_number<Name, T>,
		        daw::if_t<std::conjunction_v<cant_deref<T>, is_vector<T>>,
		                  json_array_detect<Name, vector_detect::detector<T>, T>,
		                  daw::json::missing_json_data_contract_for<T>>>>>>;

		template<typename T>
		using has_unnamed_default_type_mapping = daw::not_trait<
		  std::is_same<unnamed_default_type_mapping<T>,
		               daw::json::missing_json_data_contract_for<T>>>;
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
	 * are supported
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename JsonElement,
	         typename Container =
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
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
	 * are supported
	 */
	template<JSONNAMETYPE Name, typename JsonElement,
	         typename Container =
	           std::vector<typename json_details::unnamed_default_type_mapping<
	             JsonElement>::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>>
	using json_array_null = json_array<Name, JsonElement, Container, Constructor,
	                                   JsonNullable::Nullable>;

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
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
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
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>>
	using json_key_value_null =
	  json_key_value<Name, Container, JsonValueType, JsonKeyType, Constructor,
	                 JsonNullable::Nullable>;

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
	         typename JsonKeyType,
	         typename Constructor = daw::construct_a_t<Container>,
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
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType,
	         typename Constructor = daw::construct_a_t<Container>>
	using json_key_value_array_null =
	  json_key_value_array<Name, Container, JsonValueType, JsonKeyType,
	                       Constructor, JsonNullable::Nullable>;

	/**
	 * Allow parsing of a type that does not fit
	 * @tparam Name Name of JSON member to link to
	 * @tparam T type of value being constructed
	 * @tparam FromJsonConverter Callable that accepts a std::string_view of the
	 * range to parse.  The default requires an overload of from_string(
	 * daw::tag<T>, std::string_view ) that returns a T
	 * @tparam ToJsonConverter Returns a string from the value.  The default
	 * requires a to_string( T const & ) overload that returns a String like type
	 * @tparam CustomJsonType JSON type value is encoded as literal/string
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename FromJsonConverter = custom_from_converter_t<T>,
	         typename ToJsonConverter = custom_to_converter_t<T>,
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
			return daw::numeric_limits<std::size_t>::max( );
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
		[[maybe_unused]] constexpr unknown_variant_type<T>
		get_variant_type_list( T const * );

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
} // namespace daw::json
