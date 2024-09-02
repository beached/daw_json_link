// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <optional>

struct Raw {
	daw::json::json_value raw_json;
};

namespace daw::json {
	template<>
	struct json_data_contract<Raw> {
		static constexpr char const raw_json[] = "raw_json";
		using type = json_member_list<json_raw<raw_json>>;

		static auto to_json_data( Raw const &v ) {
			return std::forward_as_tuple( v.raw_json );
		}
	};
} // namespace daw::json

static_assert( daw::json::from_json<Raw>( R"json({"raw_json":[1,2,3]})json" )
                 .raw_json.is_array( ) );

struct NullableRaw {
	std::optional<daw::json::json_value> raw_json;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableRaw> {
		static constexpr char const raw_json[] = "raw_json";
		using type = json_member_list<json_raw_null<raw_json>>;

		static auto to_json_data( Raw const &v ) {
			return std::forward_as_tuple( v.raw_json );
		}
	};
} // namespace daw::json

constexpr auto nr0 =
  daw::json::from_json<NullableRaw>( R"json({"raw_json":[1,2,3]})json" );
static_assert( nr0.raw_json );
static_assert( nr0.raw_json->is_array( ) );

int main( ) {
	//
}
