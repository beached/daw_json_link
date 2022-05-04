// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

// Ensure that we are always checking
#ifdef DAW_JSON_CHECK_DEBUG_ONLY
#undef DAW_JSON_CHECK_DEBUG_ONLY
#endif

#include <daw/json/daw_json_link.h>

#ifdef DAW_USE_EXCEPTIONS
#include <exception>
#endif
#include <iostream>
#include <optional>
#include <string_view>

namespace tests {
	struct Coordinate {
		double lat;
		double lng;
		std::optional<std::string> name;
	};

	struct UriList {
		std::vector<std::string> uris;
	};
} // namespace tests

template<>
struct daw::json::json_data_contract<tests::Coordinate> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_number<"lat">, json_number<"lng">,
	                              json_string<"name">>;
#else
	static constexpr char const lat[] = "lat";
	static constexpr char const lng[] = "lng";
	static constexpr char const name[] = "name";
	using type =
	  json_member_list<json_number<lat>, json_number<lng>, json_string<name>>;
#endif
};

template<>
struct daw::json::json_data_contract<tests::UriList> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_array<"uris", std::string>>;
#else
	static constexpr char const uris[] = "uris";
	using type = json_member_list<json_array<uris, std::string>>;
#endif
};

namespace tests {
	bool quotes_in_numbers( ) {
		std::string_view const data = R"({"lat": "55.55", "lng": "12.34" })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
	bool quotes_in_numbers_str( ) {
		std::string data = R"({"lat": "55.55", "lng": "12.34" })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool bool_in_numbers( ) {
		static DAW_CONSTEXPR std::string_view data =
		  R"({"lat": true, "lng": false })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool invalid_numbers( ) {
		static DAW_CONSTEXPR std::string_view data =
		  R"({"lat": 1.23b34, "lng": 1234.4 })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool missing_value_001( ) {
		static DAW_CONSTEXPR std::string_view data = R"({"lat": 1.23, "lng": })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool missing_value_002( ) {
		static DAW_CONSTEXPR std::string_view data = R"({"lat": , "lng": 1.23 })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool missing_value_003( ) {
		static DAW_CONSTEXPR std::string_view data =
		  R"({"name": "lat": 1.23, "lng": 1.34 })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool missing_member( ) {
		static DAW_CONSTEXPR std::string_view data = R"({"lng": 1.23 })";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool missing_closing_brace( ) {
		static DAW_CONSTEXPR std::string_view data =
		  R"({"lng": 1.23, "lat": 1.22 )";
		try {
			auto const c = daw::json::from_json<tests::Coordinate>( data );
			(void)c;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool invalid_strings( ) {
		std::string data =
		  R"({"uris": [ "http://www.example.com", "http://www.example.com/missing_quote ] })";
		try {
			auto const ul = daw::json::from_json<tests::UriList>( data );
			(void)ul;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool invalid_strings2( ) {
		std::string_view data =
		  R"({"uris": [ "http://www.ex\u4"\"ample.com", "http://www.example.com/missing_quote ] })";
		try {
			auto const ul = daw::json::from_json<tests::UriList>( data );
			(void)ul;
		} catch( daw::json::json_exception const &jex ) {
			if( jex.parse_location( ) ) {
				auto const dist = jex.parse_location( ) - data.data( );
				if( dist == 29 ) {
					return true;
				}
			}
			std::cerr << "Wrong exception: " << jex.reason( ) << '\n'
			          << to_formatted_string( jex ) << '\n';
		}
		return false;
	}
	bool invalid_strings2_str( ) {
		std::string data =
		  R"({"uris": [ "http://www.ex\u4"\"ample.com", "http://www.example.com/missing_quote ] })";
		try {
			auto const ul = daw::json::from_json<tests::UriList>( data );
			(void)ul;
		} catch( daw::json::json_exception const &jex ) {
			if( jex.parse_location( ) ) {
				auto const pos = jex.parse_location( ) - std::data( data );
				if( pos == 29 ) {
					return true;
				}
			}
			std::cerr << "Wrong exception: " << jex.reason( ) << '\n'
			          << to_formatted_string( jex ) << '\n';
			return false;
		} catch( ... ) { return false; }
		// Should never reach here
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

	bool very_large_signed_int( ) {
		std::string data =
		  "[123, 32767, 32768, -237462374673276894279832749832423479823246327846, "
		  "55 ]";
		try {
			using namespace daw::json;
			std::vector<intmax_t> numbers =
			  from_json_array<json_checked_number_no_name<intmax_t>>( data );
			(void)numbers;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool incomplete_null( ) {
		std::string data = "[nul]";
		try {
			using namespace daw::json;
			std::vector<std::optional<int>> numbers =
			  from_json_array<json_checked_number_null_no_name<std::optional<int>>>(
			    data );
			(void)numbers;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool incomplete_false( ) {
		std::string data = "[fa]";
		try {
			using namespace daw::json;
			std::vector<bool> bools = from_json_array<bool>( data );
			(void)bools;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool incomplete_true( ) {
		std::string data = "[tr]";
		try {
			using namespace daw::json;
			std::vector<bool> bools = from_json_array<bool>( data );
			(void)bools;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	bool bad_true( ) {
		std::string data = "[tree]";
		try {
			using namespace daw::json;
			std::vector<bool> bools = from_json_array<bool>( data );
			(void)bools;
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}
} // namespace tests

#ifdef DAW_USE_EXCEPTIONS
#define expect_fail( Bool, Reason )                                          \
	do {                                                                       \
		std::cout << "testing: "                                                 \
		          << "" #Bool "\n";                                              \
		try {                                                                    \
			if( not static_cast<bool>( Bool ) ) {                                  \
				std::cerr << "Fail: " << ( Reason ) << '\n';                         \
			}                                                                      \
		} catch( std::exception const &ex ) {                                    \
			std::cout << "Fail: " << ( Reason ) << "\nUnexpected std::exception\n" \
			          << ex.what( ) << '\n';                                       \
			last_uncaught_except = std::make_exception_ptr( ex );                  \
		} catch( ... ) {                                                         \
			std::cout << "Fail: " << ( Reason ) << "\nUnknown exception\n";        \
			last_uncaught_except = std::current_exception( );                      \
		}                                                                        \
	} while( false )
#else
#define expect_fail( Bool, Reason )              \
	std::cout << "testing: "                       \
	          << "" #Bool "\n";                    \
	if( not static_cast<bool>( Bool ) ) {          \
		std::cerr << "Fail: " << ( Reason ) << '\n'; \
	}                                              \
	while( false )
#endif

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
#ifdef DAW_USE_EXCEPTIONS
	std::exception_ptr last_uncaught_except = nullptr;
#endif
	expect_fail( tests::quotes_in_numbers( ),
	             "Failed to find unexpected quotes in numbers" );
	expect_fail( tests::quotes_in_numbers_str( ),
	             "Failed to find unexpected quotes in numbers" );
	expect_fail( tests::bool_in_numbers( ),
	             "Failed to find a bool when a number was expected" );
	expect_fail( tests::invalid_numbers( ), "Failed to find an invalid number" );
	expect_fail( tests::invalid_strings( ), "Failed to find missing quote" );
	expect_fail( tests::invalid_strings2( ), "Failed to find missing quote" );
	expect_fail( tests::invalid_strings2_str( ), "Failed to find missing quote" );

	expect_fail( tests::missing_array_element( ),
	             "Failed to catch an empty array element e.g(1,,2)" );

	expect_fail( tests::missing_value_001( ),
	             "Failed to catch a missing value that has a member name" );

	expect_fail( tests::missing_value_002( ),
	             "Failed to catch a missing value that has a member name" );

	expect_fail( tests::missing_value_003( ),
	             "Failed to catch a missing value that has a member name" );

	expect_fail( tests::missing_member( ),
	             "Failed to catch a missing required member" );

	expect_fail( tests::missing_closing_brace( ),
	             "Failed to catch a missing closing brace on object" );

	expect_fail( tests::very_large_signed_int( ),
	             "Failed to catch a very large signed number" );

	expect_fail( tests::incomplete_null( ),
	             "Incomplete null in array not caught" );

	expect_fail( tests::incomplete_true( ),
	             "Incomplete true in array not caught" );

	expect_fail( tests::incomplete_false( ),
	             "Incomplete false in array not caught" );

	expect_fail( tests::bad_true( ), "bad true value not caught" );
#ifdef DAW_USE_EXCEPTIONS
	if( last_uncaught_except ) {
		std::rethrow_exception( last_uncaught_except );
	}
#endif
	return 0;
}
#ifdef DAW_USE_EXCEPTIONS
catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Uncaught exception reached the end scope of main\n"
	          << std::flush;
	throw;
}
#endif