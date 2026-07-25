// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cstdio>
#include <string>

struct Holder {
	double v;
};

template<>
struct daw::json::json_data_contract<Holder> {
	static constexpr char const v[] = "v";
	using type = json_member_list<json_number<v, double>>;

	static constexpr auto to_json_data( Holder const & h ) {
		return std::forward_as_tuple( h.v );
	}
};

int main( ) {
	static constexpr daw::string_view doc = R"({"v":0.33333333333333333333})"; // 20 sig digits
	static constexpr auto result = daw::json::from_json<Holder>( doc );
	static_assert( result.v > 0.33 and result.v < 0.34 );
	std::printf( "%.17g\n", result.v );
	auto const str_result = daw::json::to_json( result );
	std::puts( str_result.c_str(  ) );
	std::string const doc2 = R"({"v":0.33333333333333333333})"; // 20 sig digits
	auto const result2 = daw::json::from_json<Holder>( doc2 );
	auto const diff = result.v - result2.v;
	std::printf( "%0.17g\n", diff );
}