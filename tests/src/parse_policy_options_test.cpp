// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
//  Covers options::ZeroTerminatedString and options::MinifiedDocument, which
//  are policy bits that are never explicitly set by any other existing test.
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <string>
#include <string_view>
#include <vector>

struct PolicyOptionsHolder {
	int a;
	std::vector<int> b;
};

namespace daw::json {
	template<>
	struct json_data_contract<PolicyOptionsHolder> {
		static constexpr char a[] = "a";
		static constexpr char b[] = "b";
		using type = json_member_list<json_number<a, int>, json_array<b, int>>;
	};
} // namespace daw::json

int main( ) {
	using namespace daw::json;

	// `from_json` only sets ZeroTerminatedString::yes automatically when the
	// argument's *type* is std::string.  A std::string_view/char const * never
	// gets promoted, even when its buffer happens to be zero terminated.  These
	// two cases exercise explicitly overriding that type-based default in both
	// directions.
	{
		// Override a std::string argument (would default to "yes") to "no",
		// forcing the bounds-checked code path.
		std::string const buf = R"({"a":1,"b":[1,2,3]})";
		static constexpr auto policy =
		  options::parse_flags<options::ZeroTerminatedString::no>;
		auto const v = from_json<PolicyOptionsHolder>( buf, policy );
		daw_ensure( v.a == 1 );
		daw_ensure( ( v.b == std::vector<int>{ 1, 2, 3 } ) );
	}
	{
		// Override a std::string_view argument (would default to "no") to
		// "yes".  The backing buffer is genuinely zero terminated even though
		// the argument's type does not guarantee it.
		std::string const backing = R"({"a":4,"b":[4,5,6]})";
		std::string_view const sv( backing.data( ), backing.size( ) );
		static constexpr auto policy =
		  options::parse_flags<options::ZeroTerminatedString::yes>;
		auto const v = from_json<PolicyOptionsHolder>( sv, policy );
		daw_ensure( v.a == 4 );
		daw_ensure( ( v.b == std::vector<int>{ 4, 5, 6 } ) );
	}

	// MinifiedDocument::yes tells the parser to skip whitespace trimming
	// entirely.  It is only correct when the document truly has no whitespace.
	{
		std::string_view const sv = R"({"a":7,"b":[7,8,9]})";
		static constexpr auto policy =
		  options::parse_flags<options::MinifiedDocument::yes>;
		auto const v = from_json<PolicyOptionsHolder>( sv, policy );
		daw_ensure( v.a == 7 );
		daw_ensure( ( v.b == std::vector<int>{ 7, 8, 9 } ) );
	}
}
