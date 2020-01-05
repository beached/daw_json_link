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

#include <daw/json/daw_json_link.h>

#include <iostream>
#include <string_view>

namespace tests {
	struct Coordinate {
		double lat;
		double lng;
	};

	struct UriList {
		std::vector<std::string> uris;
	};
} // namespace tests

template<>
struct daw::json::json_data_contract<tests::Coordinate> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_number<"lat">, json_number<"lng">>;
#else
	static inline constexpr char const lat[] = "lat";
	static inline constexpr char const lng[] = "lng";
	using type = json_member_list<json_number<lat>, json_number<lng>>;
#endif
};

template<>
struct daw::json::json_data_contract<tests::UriList> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_array<"uris", std::string>>;
#else
	static inline constexpr char const uris[] = "uris";
	using type = json_member_list<json_array<uris, std::string>>;
#endif
};

namespace tests {
	bool quotes_in_numbers( ) {
		static constexpr std::string_view data =
		  R"({"lat": "55.55", "lng": "12.34" })";
		try {
			Coordinate c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool bool_in_numbers( ) {
		static constexpr std::string_view data = R"({"lat": true, "lng": false })";
		try {
			Coordinate c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool invalid_numbers( ) {
		static constexpr std::string_view data =
		  R"({"lat": 1.23b34, "lng": 1234.4 })";
		try {
			Coordinate c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool invalid_strings( ) {
		std::string data =
		  R"({"uris": [ "http://www.example.com", "http://www.example.com/missing_quote ] })";
		try {
			UriList ul = daw::json::from_json<tests::UriList>( data );
			(void)ul;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool missing_array_element( ) {
		std::string data = "[1,2,,3]";
		try {
			std::vector<int> numbers = daw::json::from_json_array<int>( data );
			(void)numbers;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
} // namespace tests

#define expect_fail( Bool, Reason )                                            \
	while( not static_cast<bool>( Bool ) ) {                                     \
		std::cerr << "Fail: " << Reason << '\n';                                   \
		exit( 1 );                                                                 \
	}

int main( ) {
	expect_fail( tests::quotes_in_numbers( ),
	             "Failed to find unexpected quotes in numbers" );
	expect_fail( tests::bool_in_numbers( ),
	             "Failed to find a bool when a number was expected" );
	expect_fail( tests::invalid_numbers( ), "Failed to find an invalid number" );
	expect_fail( tests::invalid_strings( ), "Failed to find missing quote" );

	expect_fail( tests::missing_array_element( ),
	             "Failed to catch an empty array element e.g(1,,2)" );
}
