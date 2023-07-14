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
	constexpr std::string_view const test_string = "\x0D\x0ATEST\xFETEST\x0D\x0A";
	constexpr auto document = test_document{ test_string };
	std::string buff;
	try {
		auto request_json_payload = daw::json::to_json( document, buff );
		std::cerr << "Expected error\n";
		return 1;
	} catch( daw::json::json_exception const & jex ) {
		if( jex.reason_type()	!= daw::json::ErrorReason::InvalidStringHighASCII ) {
			std::cerr << "Unexpected JSON error " << to_formatted_string( jex ) << '\n';
			return 1;
		}
	}

	return 0;
}
