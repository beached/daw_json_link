// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <map>
#include <string>
#include <string_view>

struct request {
	std::unordered_map<std::string, std::string> header;
};

namespace daw::json {
	template<>
	struct json_data_contract<request> {
#if defined( DAW_JSON_CNTTP_JSON_NAME )
		using type = json_member_list<json_key_value<
		  "header", std::unordered_map<std::string, std::string>, std::string>>;
#else
		static constexpr char const header[] = "header";
		using type = json_member_list<json_key_value<
		  header, std::unordered_map<std::string, std::string>, std::string>>;
#endif
		static auto to_json_data( request const &v ) {
			return std::forward_as_tuple( v.header );
		}
	};
} // namespace daw::json

int main( ) {
	constexpr std::string_view json_doc =
	  R"json({  "header": { "field1":  "a", "field2": "b" } })json";

	auto req = daw::json::from_json<request>( json_doc );
}
