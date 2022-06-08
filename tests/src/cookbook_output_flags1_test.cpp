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
#include <vector>

struct OutputFlags1 {
	std::string s;
	std::vector<int> ary;
};

namespace daw::json {
	template<>
	struct json_data_contract<OutputFlags1> {
		static constexpr char const s[] = "s";
		static constexpr char const ary[] = "ary";
		using type = json_member_list<json_string<s>, json_array<ary, int>>;

		static inline auto to_json_data( OutputFlags1 const &v ) {
			return std::forward_as_tuple( v.s, v.ary );
		}
	};
} // namespace daw::json

int main( ) {
	using namespace daw::json::options;
	auto const v = OutputFlags1{ "Hello", { 1, 2, 3 } };
	daw::json::to_json( v, stdout );
	puts( "\n" );
	puts( daw::json::to_json( v, std::string{ },
	                          output_flags<SerializationFormat::Pretty> )
	        .c_str( ) );
	auto const v2 = OutputFlags1{ "😍😍 JSON is awesome", { 42 } };
	auto v2_json = daw::json::to_json(
	  v2, output_flags<SerializationFormat::Pretty,
	                   RestrictedStringOutput::OnlyAllow7bitsStrings> );
	puts( v2_json.c_str( ) );
	auto v2_2 = daw::json::from_json<OutputFlags1>( v2_json );
	puts( daw::json::to_json( v2_2 ).c_str( ) );
}
