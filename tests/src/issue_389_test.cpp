// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <iostream>
#include <string>
#include <string_view>

#include <daw/daw_span.h>
#include <daw/json/daw_json_link.h>

struct test_document {
	std::string_view in;
};

namespace daw::json {
	template<>
	struct json_data_contract<test_document> {
		static inline constexpr char const _in[] = "in";
		using type = json_member_list<json_string<_in, std::string_view>>;

		static constexpr auto to_json_data( test_document const &doc ) {
			return std::forward_as_tuple( doc.in );
		}
	};
} // namespace daw::json

int main( ) {
	std::cout << "Building New Request..." << std::endl;

	constexpr std::string_view const test_string = "\x0D\x0ATEST\xFETEST\x0D\x0A";
	constexpr auto document = test_document{ test_string };
	std::string buff;
	auto request_json_payload = daw::json::to_json( document, buff );

	std::cout << "Request JSON: " << request_json_payload << std::endl;
	return 0;
}
