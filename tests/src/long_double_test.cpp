// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

struct Holder {
	long double v;
};

namespace daw::json {
	template<>
	struct json_data_contract<Holder> {
		static constexpr char const v[] = "v";
		using type = json_member_list<json_number<v, long double>>;
	};
} // namespace daw::json

namespace {
	// significant_digits/exponent chosen to hit both the ordinary fast path and
	// the edge cases that force the Eisel-Lemire/strtod fallback, including more
	// significant digits than fit in a double's mantissa.
	constexpr std::string_view cases[] = {
	  // ordinary power10 path
	  "3.14",
	  "-123.456e10",
	  "1.23456789012345678e15",
	  "1.23456789012345678e-15",
	  // values around the legacy double-precision threshold
	  "1e22",
	  "1e-22",
	  "1e23",
	  "1e-23",
	  // 80-bit long double power10 boundary (binary128 extends this to ±48)
	  "1e27",
	  "1e-27",
	  "1e28",
	  "1e-28",
	  // large-exponent fallback
	  "1.7976931348623157e308",
	  "2.2250738585072014e-308",
	  // exponent_p1 > 0: whole-part digit count exceeds max_exponent, so digits
	  // are discarded and exponent_p1 is positive
	  "12345678901234567890123e5",
	  // discarded nonzero fractional digits: forces parse_json_real_exact path
	  // for double/float via parse_truncated_lemire
	  "1.23456789012345678901",
	  // many significant digits
	  "123456789012345678901234567890.123456789",
	  // subnormal / deep-underflow long double (exercises power10 p < -max_exp
	  // and strtod underflow)
	  "1e-4932",
	  "3.3621031431120935e-4932",
	  // overflow / infinity
	  "1e400",
	  "1e-400",
	  // zero and negative zero
	  "-0.0",
	};

	[[nodiscard]] bool check_unknown_bounds( ) {
		for( auto const c : cases ) {
			long double const parsed =
			  daw::json::from_json<long double>( daw::string_view( c ) );
			auto const expected = std::strtold( std::string( c ).c_str( ), nullptr );
			if( not( parsed == expected or
			         ( parsed != parsed and expected != expected ) ) ) {
				std::cerr << "unknown_bounds FAIL: " << c
				          << "  parsed=" << parsed << " expected=" << expected << '\n';
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
				std::cerr << "known_bounds FAIL: " << c
				          << "  parsed=" << parsed << " expected=" << expected << '\n';
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
