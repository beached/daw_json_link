// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_writer.h"

#include <daw/daw_ensure.h>

#include <array>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace {
	template<typename JsonClass, typename T>
	void ensure_write_value( T const &value, std::string_view expected ) {
		auto out = std::string{ };
		auto writer = daw::json::json_writer( out );
		writer.template write_value<JsonClass>( value );
		writer.finalize( );
		if( out != expected ) {
			std::cerr << "Expected: " << expected << "\nActual:   " << out << '\n';
		}
		daw_ensure( out == expected );
	}

	struct CustomFromJson {
		[[maybe_unused]] int operator( )( std::string_view ) const {
			return 0;
		}
	};

	struct CustomToJson {
		std::string operator( )( int value ) const {
			return std::to_string( value );
		}
	};

	using TaggedValue = std::variant<std::string, int, bool>;

	struct TaggedObject {
		std::string name;
		TaggedValue value;
	};

	struct TaggedSwitcher {
		[[maybe_unused]] constexpr std::size_t operator( )( int type ) const {
			return static_cast<std::size_t>( type );
		}

		int operator( )( TaggedObject const &value ) const {
			return static_cast<int>( value.value.index( ) );
		}
	};

	struct SizedObject {
		std::size_t size;
		std::vector<int> values;
	};

	struct SizedVectorConstructor {
		[[maybe_unused]] std::vector<int>
		operator( )( int const *first, int const *last, std::size_t ) const {
			return { first, last };
		}
	};

	struct IntrusiveA {
		int kind = 0;
		int value = 0;
	};

	struct IntrusiveB {
		int kind = 1;
		std::string value;
	};

	using IntrusiveValue = std::variant<IntrusiveA, IntrusiveB>;

	struct IntrusiveSwitcher {
		[[maybe_unused]] constexpr std::size_t operator( )( int type ) const {
			return static_cast<std::size_t>( type );
		}
	};

#if defined( DAW_JSON_HAS_REFLECTION )
	struct ReflectedObject {
		int number;
		bool flag;
	};
#endif
} // namespace

namespace daw::json {
	template<>
	struct json_data_contract<TaggedObject> {
		static constexpr char name[] = "name";
		static constexpr char type_member[] = "type";
		static constexpr char value[] = "value";
		using type_t = json_number<type_member, int>;
		using type = json_member_list<
		  json_string<name>,
		  json_tagged_variant<value, TaggedValue, type_t, TaggedSwitcher>>;

		static auto to_json_data( TaggedObject const &object ) {
			return std::forward_as_tuple( object.name, object.value );
		}
	};

	template<>
	struct json_data_contract<SizedObject> {
		static constexpr char size[] = "size";
		static constexpr char values[] = "values";
		using size_type = json_number<size, std::size_t>;
		using type = json_member_list<
		  size_type, json_sized_array<values, int, size_type, std::vector<int>,
		                              SizedVectorConstructor>>;

		static auto to_json_data( SizedObject const &object ) {
			return std::forward_as_tuple( object.size, object.values );
		}
	};

	template<>
	struct json_data_contract<IntrusiveA> {
		static constexpr char kind[] = "kind";
		static constexpr char value[] = "value";
		using type =
		  json_member_list<json_number<kind, int>, json_number<value, int>>;

		static constexpr auto to_json_data( IntrusiveA const &object ) {
			return std::forward_as_tuple( object.kind, object.value );
		}
	};

	template<>
	struct json_data_contract<IntrusiveB> {
		static constexpr char kind[] = "kind";
		static constexpr char value[] = "value";
		using type = json_member_list<json_number<kind, int>, json_string<value>>;

		static constexpr auto to_json_data( IntrusiveB const &object ) {
			return std::forward_as_tuple( object.kind, object.value );
		}
	};

	template<>
	struct json_data_contract<IntrusiveValue> {
		static constexpr char kind[] = "kind";
		using type = json_type_alias<json_intrusive_variant_no_name<
		  IntrusiveValue, json_number<kind, int>, IntrusiveSwitcher>>;
	};
} // namespace daw::json

int main( ) {
	using namespace daw::json;

	ensure_write_value<json_checked_number_no_name<int>>( 42, "42" );
	ensure_write_value<json_string_raw_no_name<std::string>>(
	  std::string{ R"(line\nbreak)" }, R"("line\nbreak")" );

	using timestamp_t = std::chrono::time_point<std::chrono::system_clock,
	                                            std::chrono::milliseconds>;
	auto const timestamp =
	  datetime::civil_to_time_point( 2024, 9, 2, 1, 14, 54, 0 );
	ensure_write_value<json_date_no_name<timestamp_t>>(
	  timestamp, R"("2024-09-02T01:14:54Z")" );

	ensure_write_value<json_custom_no_name<int, CustomFromJson, CustomToJson>>(
	  42, R"("42")" );
	ensure_write_value<
	  json_custom_lit_no_name<int, CustomFromJson, CustomToJson>>( 42, "42" );

	ensure_write_value<json_tuple_no_name<std::tuple<int, bool, std::string>>>(
	  std::tuple{ 1, true, std::string{ "two" } }, R"([1,true,"two"])" );
	ensure_write_value<json_key_value_no_name<std::map<std::string, int>>>(
	  std::map<std::string, int>{ { "a", 1 }, { "b", 2 } }, R"({"a":1,"b":2})" );
	ensure_write_value<json_key_value_array_no_name<std::map<std::string, int>>>(
	  std::map<std::string, int>{ { "a", 1 }, { "b", 2 } },
	  R"([{"key":"a","value":1},{"key":"b","value":2}])" );

	using Variant = std::variant<int, std::string, bool, std::vector<int>>;
	ensure_write_value<json_variant_no_name<Variant>>(
	  Variant{ std::string{ "x" } }, R"("x")" );
	ensure_write_value<json_variant_no_name<Variant>>(
	  Variant{ std::vector<int>{ 1, 2 } }, "[1,2]" );

	ensure_write_value<json_raw_no_name<std::string>>(
	  std::string{ R"({"raw":[1,2]})" }, R"({"raw":[1,2]})" );

	ensure_write_value<json_class_no_name<TaggedObject>>(
	  TaggedObject{ "tagged", 42 }, R"({"type":1,"name":"tagged","value":42})" );
	ensure_write_value<json_class_no_name<SizedObject>>(
	  SizedObject{ 3, { 1, 2, 3 } }, R"({"size":3,"values":[1,2,3]})" );
	ensure_write_value<json_intrusive_variant_no_name<
	  IntrusiveValue,
	  json_number<json_data_contract<IntrusiveValue>::kind, int>,
	  IntrusiveSwitcher>>( IntrusiveValue{ IntrusiveB{ 1, "intrusive" } },
	                       R"({"kind":1,"value":"intrusive"})" );

#if defined( DAW_JSON_HAS_REFLECTION )
	ensure_write_value<json_reflected_class_no_name<ReflectedObject>>(
	  ReflectedObject{ 7, true }, R"({"number":7,"flag":true})" );
#endif

	using decimal_number =
	  json_number_no_name<double,
	                      options::number_opt(
	                        options::FPOutputFormat::Decimal )>;
	{
		auto out = std::string{ };
		auto writer = json_writer( out );
		writer.write_value<decimal_number>( 10.0 );
		writer.finalize( );
		daw_ensure( out == "10.0" );
	}
	{
		auto out = std::string{ };
		auto writer = json_writer( out );
		writer.open_object( );
		writer.write_key_value<decimal_number>( "value", 10.0 );
		writer.close_object( );
		daw_ensure( out == R"({"value":10.0})" );
	}
	{
		auto out = std::string{ };
		auto writer = json_writer( out );
		writer.open_array( );
		writer.write_array_values<decimal_number>( std::array{ 10.0, 20.0 } );
		writer.write_array_values<decimal_number>( { 30.0, 40.0 } );
		writer.close_array( );
		daw_ensure( out == "[10.0,20.0,30.0,40.0]" );
	}
	{
		auto out = std::string{ };
		auto writer = json_writer( out );
		writer.write_string<json_bool_no_name<>>( true );
		writer.finalize( );
		daw_ensure( out == R"("true")" );
	}
}
