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

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>

#include "impl/daw_iterator_range.h"
#include "impl/daw_json_link_impl.h"

namespace daw::json {
	template<typename... JsonMembers>
	struct class_description_t {
		template<typename OutputIterator, typename... Args>
		[[maybe_unused, nodiscard]] static constexpr OutputIterator
		serialize( OutputIterator it, std::tuple<Args...> &&args ) {
			static_assert( sizeof...( Args ) == sizeof...( JsonMembers ),
			               "Argument count is incorrect" );

			static_assert( ( impl::is_a_json_type_v<JsonMembers> and ... ),
			               "Only value json types can be used" );
			return impl::serialize_json_class<JsonMembers...>(
			  it, std::index_sequence_for<Args...>{}, std::move( args ) );
		}

		template<typename Result, bool TrustedInput>
		[[maybe_unused, nodiscard]] static constexpr Result
		parse( std::string_view sv ) {
			json_assert_untrusted( not sv.empty( ), "Cannot parse an empty string" );

			auto rng = impl::IteratorRange<char const *, char const *, TrustedInput>(
			  sv.data( ), sv.data( ) + sv.size( ) );
			return impl::parse_json_class<Result, JsonMembers...>(
			  rng, std::index_sequence_for<JsonMembers...>{} );
		}

		template<typename Result, typename First, typename Last, bool TrustedInput>
		[[maybe_unused, nodiscard]] static constexpr Result
		parse( impl::IteratorRange<First, Last, TrustedInput> &rng ) {
			json_assert_untrusted( rng.has_more( ), "Cannot parse an empty string" );
			return impl::parse_json_class<Result, JsonMembers...>(
			  rng, std::index_sequence_for<JsonMembers...>{} );
		}
	};

	// Member types

	/**
	 * Mark the member as nullable, optional.  The resulting type
	 * must be default constructible
	 * @tparam JsonMember the member being marked.
	 */
	template<typename JsonMember>
	struct json_nullable {
		using i_am_a_json_type = typename JsonMember::i_am_a_json_type;
		using parse_to_t = typename JsonMember::parse_to_t;
		using constructor_t = typename JsonMember::constructor_t;
		using sub_type = JsonMember;
		static constexpr JSONNAMETYPE name = JsonMember::name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Null;

		static_assert( std::is_invocable_v<constructor_t>,
		               "Specified constructor must be callable without arguments" );
	};

	/**
	 * The member is a number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam RangeCheck Check if the value will fit in the result
	 */
	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>,
	         bool RangeCheck = false>
	struct json_number {
		static_assert( std::is_invocable_v<Constructor, T>,
		               "Constructor must be callable with T" );

		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type =
		  std::is_floating_point_v<T>
		    ? JsonParseTypes::Real
		    : std::is_unsigned_v<T> ? JsonParseTypes::Unsigned
		                            : JsonParseTypes::Signed;
		static constexpr LiteralAsStringOpt literal_as_string = LiteralAsString;
		static constexpr bool range_check = RangeCheck;
	};

	/**
	 * The member is a range checked number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 */
	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>>
	using json_checked_number =
	  json_number<Name, T, LiteralAsString, Constructor, true>;

	/**
	 * The membrer is a boolean
	 * @tparam Name name of json member
	 * @tparam T result type to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 */
	template<JSONNAMETYPE Name, typename T = bool,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>>
	struct json_bool {
		static_assert( std::is_constructible_v<T, bool>,
		               "Supplied type but be constructable from a bool" );
		static_assert( std::is_invocable_v<Constructor, T>,
		               "Constructor must be callable with T" );

		static_assert( std::is_convertible_v<bool, T>,
		               "Supplied result type must be convertable from bool" );
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr LiteralAsStringOpt literal_as_string = LiteralAsString;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Bool;
	};

	/**
	 *
	 * @tparam Name of json member
	 * @tparam T result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *, size_t )
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 */
	template<JSONNAMETYPE Name, typename T = std::string,
	         typename Constructor = daw::construct_a_t<T>,
	         bool EmptyStringNull = false>
	struct json_string {
		static_assert(
		  std::is_invocable_v<Constructor, char const *, size_t>,
		  "Constructor must be callable with a char const * and a size_t" );

		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::String;
		static constexpr bool empty_is_null = EmptyStringNull;
	};

	/** Link to a JSON string representing a date
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type to consruct, by default is a time_point
	 * @tparam Constructor A Callable used to construct a T.
	 * Must accept a char pointer and size as argument to the date/time string.
	 */
	template<JSONNAMETYPE Name,
	         typename T = std::chrono::time_point<std::chrono::system_clock,
	                                              std::chrono::milliseconds>,
	         typename Constructor = parse_js_date>
	struct json_date {
		static_assert(
		  std::is_invocable_v<Constructor, char const *, size_t>,
		  "Constructor must be callable with a char const * and a size_t" );

		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Date;
	};

	/**
	 * @tparam Name name of JSON member to link to
	 * @tparam T C++ type being parsed to.  Must have a describe_json_class
	 * overload
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename Constructor = daw::construct_a_t<T>>
	struct json_class {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Class;
	};

	template<JSONNAMETYPE Name, typename T,
	         typename FromConverter = custom_from_converter_t<T>,
	         typename ToConverter = custom_to_converter_t<T>,
	         bool IsString = true>
	struct json_custom {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using to_converter_t = ToConverter;
		using from_converter_t = FromConverter;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Custom;
		static constexpr bool const is_string = IsString;
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
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonElement,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>>
	struct json_array {
		static_assert( impl::is_a_json_type_v<JsonElement> );
		using i_am_a_json_type = void;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_element_t = JsonElement;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Array;

		static_assert( json_element_t::name == no_name,
		               "All elements of json_array must be have no_name" );
	};

	/** Map a KV type json class { "Key String": ValueType, ... }
	 *  to a c++ class.  Keys are always string like and the destination
	 *  needs to be constructable with a pointer, size
	 *  @tparam Name name of JSON member to link to
	 *  @tparam Container type to put values in
	 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
	 *  json_string, ... ) @tparam JsonKeyType type of key in kv pair @tparam
	 *  Constructor A callable used to make Container, default will use the
	 *  Containers constructor.  Both normal and aggregate are supported @tparam
	 *  Appender A callable used to add elements to container.
	 */
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_kv_appender<Container>>
	struct json_key_value {
		static_assert( impl::is_a_json_type_v<JsonValueType> );
		static_assert( impl::is_a_json_type_v<JsonKeyType> );
		using i_am_a_json_type = void;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_element_t = JsonValueType;
		using json_key_t = JsonKeyType;
		static constexpr JSONNAMETYPE name = Name;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::KeyValue;
	};

	/**
	 * Parse json and construct a T as the result.  This method
	 * provides checked json
	 * @tparam T type that has a describe_json_class overload
	 * @param json_data Json string data
	 * @return A reified T constructed from json data
	 */
	template<typename T>
	[[maybe_unused, nodiscard]] constexpr T
	from_json( std::string_view json_data ) {
		return impl::from_json_impl<T, false>( json_data );
	}

	/**
	 * Parse json and construct a T as the result.  This method
	 * does not perform most checks on validity of data
	 * @tparam T type that has a describe_json_class overload
	 * @param json_data Json string data
	 * @return A reified T constructed from json data
	 */
	template<typename T>
	[[maybe_unused, nodiscard]] constexpr T
	from_json_trusted( std::string_view json_data ) {
		return impl::from_json_impl<T, true>( json_data );
	}

	/**
	 *
	 * @tparam Result std::string like type to put result into
	 * @tparam T Type that has json_parser_desription and to_json_data function
	 * overloads
	 * @param value  value to serialize
	 * @return  json string data
	 */
	template<typename Result = std::string, typename T>
	[[maybe_unused, nodiscard]] constexpr Result to_json( T &&value ) {
		static_assert(
		  impl::has_json_parser_description_v<T>,
		  "A function called describe_json_class must exist for type." );
		static_assert( impl::has_json_to_json_data_v<T>,
		               "A function called to_json_data must exist for type." );

		Result result{};
		(void)impl::json_parser_description_t<T>::template serialize(
		  daw::back_inserter( result ), to_json_data( std::forward<T>( value ) ) );
		return result;
	}

	namespace impl {
		template<bool TrustedInput, typename JsonElement,
		         typename Container = std::vector<typename JsonElement::parse_to_t>,
		         typename Constructor = daw::construct_a_t<Container>,
		         typename Appender = impl::basic_appender<Container>>
		[[maybe_unused, nodiscard]] constexpr Container
		from_json_array_impl( std::string_view json_data ) {
			using parser_t =
			  json_array<no_name, Container, JsonElement, Constructor, Appender>;

			auto rng =
			  daw::json::impl::IteratorRange<char const *, char const *, false>(
			    json_data.data( ),
			    json_data.data( ) + static_cast<ptrdiff_t>( json_data.size( ) ) );

			rng.trim_left_no_check( );
			json_assert_untrusted( rng.front( '[' ), "Expected array class" );

			return impl::parse_value<parser_t>( ParseTag<JsonParseTypes::Array>{},
			                                    rng );
		}
	} // namespace impl

	/**
	 * Parse json data where the root item is an array
	 * @tparam JsonElement The type of each element in array.  Must be one of the
	 * above json_XXX classes.  This version is checked
	 * @tparam Container Container to store values in
	 * @tparam Constructor Callable to construct Container with no arguments
	 * @tparam Appender Callable to call with JsonElement
	 * @param json_data Json string data containing array
	 * @return A Container containing parsed data from json string
	 */
	template<typename JsonElement,
	         typename Container = std::vector<typename JsonElement::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array( std::string_view json_data ) {
		return impl::from_json_array_impl<false, JsonElement, Container,
		                                  Constructor, Appender>( json_data );
	}

	/**
	 * Parse json data where the root item is an array
	 * @tparam JsonElement The type of each element in array.  Must be one of the
	 * above json_XXX classes.  This version isn't checked
	 * @tparam Container Container to store values in
	 * @tparam Constructor Callable to construct Container with no arguments
	 * @tparam Appender Callable to call with JsonElement
	 * @param json_data Json string data containing array
	 * @return A Container containing parsed data from json string
	 */
	template<typename JsonElement,
	         typename Container = std::vector<typename JsonElement::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>>
	[[maybe_unused, nodiscard]] constexpr Container
	from_json_array_trusted( std::string_view json_data ) {
		return impl::from_json_array_impl<true, JsonElement, Container, Constructor,
		                                  Appender>( json_data );
	}

	/**
	 * Serialize Container to a json array string
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
		result.back( ) = ']';
		return result;
	}
} // namespace daw::json

#define DAWJSONLINK_SENTINAL                                                   \
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" \
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0            " \
	"                 ,]\"}tfn"
