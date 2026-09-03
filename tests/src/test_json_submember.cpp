// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <string_view>
#include <tuple>
#include <type_traits>

struct Submember {
	int object_value;
	std::string_view array_value;
};

namespace daw::json {
	template<>
	struct json_data_contract<Submember> {
		static constexpr char const object[] = "object";
		static constexpr char const object_path[] = "nested.value";
		static constexpr char const array[] = "array";
		static constexpr char const array_path[] = "[1].name";

		using type = json_member_list<
		  json_submember<object, object_path, int>,
		  json_submember<array, array_path,
		                 json_string_raw_no_name<std::string_view>>>;

		static constexpr auto to_json_data( Submember const &value ) {
			return std::forward_as_tuple( value.object_value, value.array_value );
		}
	};
} // namespace daw::json

struct SerializableSubmember {
	int value;
};

namespace daw::json {
	template<>
	struct json_data_contract<SerializableSubmember> {
		static constexpr char const data[] = "data";
		static constexpr char const value_path[] = "values[0].value";
		using type =
		  json_member_list<json_submember<data, value_path, int>>;

		static constexpr auto
		to_json_data( SerializableSubmember const &value ) {
			return std::forward_as_tuple( value.value );
		}
	};
} // namespace daw::json

#if defined( DAW_JSON_CNTTP_JSON_NAME )
using literal_submember = daw::json::json_submember<"object", "value", int>;
static_assert( std::is_same_v<literal_submember::parse_to_t, int> );
#endif

int main( ) {
	auto const root_array_value = daw::json::from_json<int>( "[1,2,3]", "[1]" );
	daw_ensure( root_array_value == 2 );

	auto const value0 = daw::json::from_json<Submember>(
	  R"json({"object":{"nested":{"value":42}},"array":[{"name":"first"},{"name":"second"}]})json" );
	daw_ensure( value0.object_value == 42 );
	daw_ensure( value0.array_value == "second" );

	auto const value1 = daw::json::from_json<Submember>(
	  R"json({"array":[{"name":"first"},{"name":"second"}],"ignored":true,"object":{"nested":{"value":42}}})json" );
	daw_ensure( value1.object_value == 42 );
	daw_ensure( value1.array_value == "second" );

	auto const serialized =
	  daw::json::to_json( SerializableSubmember{ 42 } );
	daw_ensure( serialized == R"json({"data":{"values":[{"value":42}]}})json" );
	daw_ensure(
	  daw::json::from_json<SerializableSubmember>( serialized ).value == 42 );

#if defined( DAW_USE_EXCEPTIONS )
	bool nonzero_array_index_threw = false;
	try {
		(void)daw::json::to_json( value0 );
	} catch( daw::json::json_exception const &ex ) {
		nonzero_array_index_threw = true;
		daw_ensure( ex.reason_type( ) == daw::json::ErrorReason::OutputError );
	}
	daw_ensure( nonzero_array_index_threw );

	bool missing_path_threw = false;
	try {
		(void)daw::json::from_json<Submember>(
		  R"json({"object":{"nested":{}},"array":[{},{}]})json" );
	} catch( daw::json::json_exception const &ex ) {
		missing_path_threw = true;
		daw_ensure( ex.reason_type( ) == daw::json::ErrorReason::JSONPathNotFound );
	}
	daw_ensure( missing_path_threw );

	bool scalar_outer_threw = false;
	try {
		(void)daw::json::from_json<Submember>(
		  R"json({"object":42,"array":[{},{}]})json" );
	} catch( daw::json::json_exception const &ex ) {
		scalar_outer_threw = true;
		daw_ensure( ex.reason_type( ) == daw::json::ErrorReason::InvalidJSONPath );
	}
	daw_ensure( scalar_outer_threw );
#endif
}
