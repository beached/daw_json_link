// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <daw/daw_benchmark.h>
#include <daw/json/daw_json_link.h>

#include <string_view>

namespace test_001 {
	struct Coordinate {
		double lat;
		double lng;
	};
} // namespace test_001

template<>
struct daw::json::json_data_contract<test_001::Coordinate> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_number<"lat">, json_number<"lng">>;
#else
	static inline constexpr char const lat[] = "lat";
	static inline constexpr char const lng[] = "lng";
	using type = json_member_list<json_number<lat>, json_number<lng>>;
#endif
};

namespace test_001 {
	bool quotes_in_numbers( ) {
		static constexpr std::string_view data =
		  R"({"lat": "55.55", "lng": "12.34" })";
		try {
			Coordinate c = daw::json::from_json<test_001::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool bool_in_numbers( ) {
		static constexpr std::string_view data = R"({"lat": true, "lng": false })";
		try {
			Coordinate c = daw::json::from_json<test_001::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
} // namespace test_001

int main( ) {
	daw::expecting( test_001::quotes_in_numbers( ) );
	daw::expecting( test_001::bool_in_numbers( ) );
}
