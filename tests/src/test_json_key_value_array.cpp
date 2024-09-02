// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <map>
#include <optional>

struct KVArray {
	std::map<std::string, int> kv;
};

namespace daw::json {
	template<>
	struct json_data_contract<KVArray> {
		static constexpr char const kv[] = "kv";
		using type =
		  json_member_list<json_key_value_array<kv, std::map<std::string, int>>>;

		static inline auto to_json_data( KVArray const &v ) {
			return std::forward_as_tuple( v.kv );
		}
	};
} // namespace daw::json

struct NullableKVArray {
	std::optional<std::map<std::string, int>> kv;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableKVArray> {
		static constexpr char const kv[] = "kv";
		using type = json_member_list<
		  json_key_value_array_null<kv, std::optional<std::map<std::string, int>>>>;

		static inline auto to_json_data( NullableKVArray const &v ) {
			return std::forward_as_tuple( v.kv );
		}
	};
} // namespace daw::json

struct NullableKVArray2 {
	std::map<std::string, int> kv;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableKVArray2> {
		static constexpr char const kv[] = "kv";
		using type = json_member_list<
		  json_key_value_array_null<kv, std::map<std::string, int>>>;

		static inline auto to_json_data( NullableKVArray2 const &v ) {
			return std::forward_as_tuple( v.kv );
		}
	};
} // namespace daw::json

int main( ) {
	{
		auto kv0 = daw::json::from_json<KVArray>(
		  R"json({"kv":[{"key":"meaning of life","value":42}]})json" );
		daw_ensure( kv0.kv.find( "meaning of life" ) != kv0.kv.end( ) );
	}
	{
		auto nkv0 = daw::json::from_json<NullableKVArray>(
		  R"json({"kv":[{"key":"meaning of life","value":42}]})json" );
		daw_ensure( nkv0.kv );
		daw_ensure( nkv0.kv->find( "meaning of life" ) != nkv0.kv->end( ) );
	}
	{
		auto nkv0 = daw::json::from_json<NullableKVArray>( R"json({})json" );
		daw_ensure( not nkv0.kv );
	}
	{
		auto nkv20 = daw::json::from_json<NullableKVArray2>(
		  R"json({"kv":[{"key":"meaning of life","value":42}]})json" );
		daw_ensure( not nkv20.kv.empty( ) );
		daw_ensure( nkv20.kv.find( "meaning of life" ) != nkv20.kv.end( ) );
	}
	{
		auto nkv20 = daw::json::from_json<NullableKVArray2>( R"json({})json" );
		daw_ensure( nkv20.kv.empty( ) );
	}
}