// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cstdlib>
#include <string>
#include <string_view>

struct Holder {
	long double v;
};

namespace daw::json {
	template<>
	struct json_data_contract<Holder> {
#if defined( DAW_JSON_CNTTP_JSON_NAME )
		using type = json_member_list<json_number<"v", long double>>;
#else
		static constexpr char const v[] = "v";
		using type = json_member_list<json_number<v, long double>>;
#endif
	};
} // namespace daw::json

namespace {
	// significant_digits/exponent chosen to hit both the ordinary fast path and
	// the edge cases that force the Eisel-Lemire/strtod fallback: exponent
	// magnitude > 22, and more significant digits than fit in a double's
	// mantissa.
	constexpr std::string_view cases[] = {
	  "3.14",
	  "-123.456e10",
	  "1.7976931348623157e308",
	  "2.2250738585072014e-308",
	  "123456789012345678901234567890.123456789",
	  "-0.0",
	  "1e400",
	  "1e-400",
	};

	[[nodiscard]] bool check_unknown_bounds( ) {
		for( auto const c : cases ) {
			auto const parsed =
			  daw::json::from_json<long double>( daw::string_view( c ) );
			auto const expected = std::strtold( std::string( c ).c_str( ), nullptr );
			if( not( parsed == expected or
			         ( parsed != parsed and expected != expected ) ) ) {
				return false;
			}
		}
		return true;
	}

	[[nodiscard]] bool check_known_bounds( ) {
		for( auto const c : cases ) {
			auto const doc = "{\"v\":" + std::string( c ) + "}";
			auto const parsed = daw::json::from_json<Holder>( doc ).v;
			auto const expected = std::strtold( std::string( c ).c_str( ), nullptr );
			if( not( parsed == expected or
			         ( parsed != parsed and expected != expected ) ) ) {
				return false;
			}
		}
		return true;
	}
} // namespace

int main( ) {
	if( not check_unknown_bounds( ) ) {
		return 1;
	}
	if( not check_known_bounds( ) ) {
		return 1;
	}
}
