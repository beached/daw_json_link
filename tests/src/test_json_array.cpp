// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <optional>
#include <vector>

struct Array {
	std::vector<int> v;
};

namespace daw::json {
	template<>
	struct json_data_contract<Array> {
		static constexpr char const v[] = "v";
		using type = json_member_list<json_array<v, int, std::vector<int>>>;

		static constexpr auto to_json_data( Array const &a ) {
			return std::forward_as_tuple( a.v );
		}
	};
} // namespace daw::json

struct NullableArray {
	std::optional<std::vector<int>> v;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableArray> {
		static constexpr char const v[] = "v";
		using type =
		  json_member_list<json_array_null<v, std::optional<std::vector<int>>>>;

		static constexpr auto to_json_data( NullableArray const &a ) {
			return std::forward_as_tuple( a.v );
		}
	};
} // namespace daw::json

struct NullableArray2 {
	std::vector<int> v;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableArray2> {
		static constexpr char const v[] = "v";
		using type = json_member_list<json_array_null<v, std::vector<int>>>;

		static constexpr auto to_json_data( NullableArray const &a ) {
			return std::forward_as_tuple( a.v );
		}
	};
} // namespace daw::json

int main( ) {
	{
		auto a0 = daw::json::from_json<Array>( R"json({"v":[1,2,3,4]})json" );
		daw_ensure( a0.v == std::vector<int>{ 1, 2, 3, 4 } );
	}

	{
		auto na0 =
		  daw::json::from_json<NullableArray>( R"json({"v":[1,2,3,4]})json" );
		daw_ensure( na0.v );
		daw_ensure( na0.v == std::vector<int>{ 1, 2, 3, 4 } );
	}

	{
		auto na1 = daw::json::from_json<NullableArray>( R"json({})json" );
		daw_ensure( not na1.v );
	}

	{
		auto na20 =
		  daw::json::from_json<NullableArray2>( R"json({"v":[1,2,3,4]})json" );
		daw_ensure( not na20.v.empty( ) );
		daw_ensure( na20.v == std::vector<int>{ 1, 2, 3, 4 } );
	}

	{
		auto na21 = daw::json::from_json<NullableArray2>( R"json({})json" );
		daw_ensure( na21.v.empty( ) );
	}
}
