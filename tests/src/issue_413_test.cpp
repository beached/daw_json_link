// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <cstdlib>

struct Thing {
	std::uint8_t u;
};

namespace daw::json {
	template<>
	struct json_data_contract<Thing> {
		static constexpr char const u[] = "u";
		using type = json_member_list<json_number<
		  u, std::uint8_t,
		  options::number_opt( options::JsonRangeCheck::CheckForNarrowing )>>;
	};
} // namespace daw::json

int main( ) {
	constexpr char const json_doc[] = R"({"u": 256})";
	try {
		auto thing = daw::json::from_json<Thing>( json_doc );
		(void)thing;
		std::cout << "Expected and out of range error\n";
		return EXIT_FAILURE;
	} catch( daw::json::json_exception const &e ) {
		if( e.reason_type() != daw::json::ErrorReason::NumberOutOfRange ) {
			std::cout << "Expected and out of range error\n";
			return EXIT_FAILURE;
		}
	}
}
