// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/daw_benchmark.h>
#include <daw/json/daw_json_link.h>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// Trailing commas are explicitly supported.  It is a cost to differentiate them
// and it "should" be no risk for corruption.  The tests here verify that it is
// working
struct string_trail {
	std::string a;
};

template<>
struct daw::json::json_data_contract<string_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_string<"a">>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_string<a>>;
#endif
};

bool test_string_trail( ) {
#ifdef DAW_USE_EXCEPTIONS
	try {
#endif
		static DAW_CONSTEXPR std::string_view json_data =
		  R"({"b": 5, "c": true, "a": "hello", } )";
		auto const result = daw::json::from_json<string_trail>( json_data );
		test_assert( result.a == "hello", "Unexpected result" );
#ifdef DAW_USE_EXCEPTIONS
	} catch( daw::json::json_exception const &je ) {
		std::cerr << "string trail parsing failed: " << je.reason( ) << '\n';
		return false;
	}
#endif
	return true;
}
/***********************************************/
struct string_raw_trail {
	std::string_view a;
};

template<>
struct daw::json::json_data_contract<string_raw_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_string_raw<"a", std::string_view>>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_string_raw<a, std::string_view>>;
#endif
};

DAW_CONSTEXPR bool test_string_raw_trail( ) {
	DAW_CONSTEXPR std::string_view json_data =
	  R"({"b": 5, "c": true, "a": "hello", } )";

	auto const result = daw::json::from_json<string_raw_trail>( json_data );

	test_assert( result.a == "hello", "Unexpected result" );
	return true;
}

/***********************************************/
struct int_trail {
	int a;
};

template<>
struct daw::json::json_data_contract<int_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_number<"a", int>>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_number<a, int>>;
#endif
};

DAW_CONSTEXPR bool test_int_trail( ) {
	DAW_CONSTEXPR std::string_view json_data =
	  R"({"b": 5, "c": true, "a": -5, } )";

	auto const result = daw::json::from_json<int_trail>( json_data );

	test_assert( result.a == -5, "Unexpected result" );
	return true;
}

/***********************************************/
struct unsigned_trail {
	unsigned a;
};

template<>
struct daw::json::json_data_contract<unsigned_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_number<"a", unsigned>>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_number<a, unsigned>>;
#endif
};

DAW_CONSTEXPR bool test_unsigned_trail( ) {
	DAW_CONSTEXPR std::string_view json_data =
	  R"({"b": 5, "c": true, "a": 5, } )";

	auto const result = daw::json::from_json<unsigned_trail>( json_data );

	test_assert( result.a == 5, "Unexpected result" );
	return true;
}
/***********************************************/
struct bool_trail {
	bool a;
};

template<>
struct daw::json::json_data_contract<bool_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_bool<"a">>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_bool<a>>;
#endif
};

DAW_CONSTEXPR bool test_bool_trail( ) {
	DAW_CONSTEXPR std::string_view json_data =
	  R"({"b": 5, "c": true, "a": true, } )";

	auto const result = daw::json::from_json<bool_trail>( json_data );

	test_assert( result.a, "Unexpected result" );
	return true;
}

/***********************************************/
struct object_trail {
	int_trail a;
};

template<>
struct daw::json::json_data_contract<object_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_class<"a", int_trail>>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_class<a, int_trail>>;
#endif
};

DAW_CONSTEXPR bool test_object_trail( ) {
	DAW_CONSTEXPR std::string_view json_data =
	  R"({"b": 5, "c": true, "a": { "b": true, "c": 1.234, "a": 5, }, } )";

	auto const result = daw::json::from_json<object_trail>( json_data );

	test_assert( result.a.a == 5, "Unexpected result" );
	return true;
}

/***********************************************/
struct array_member_trail {
	std::vector<int> a;
};

template<>
struct daw::json::json_data_contract<array_member_trail> {
#ifdef DAW_JSON_CNTTP_JSON_NAME
	using type = json_member_list<json_array<"a", int>>;
#else
	static constexpr char const a[] = "a";
	using type = json_member_list<json_array<a, int>>;
#endif
};

bool test_array_member_trail( ) {
	std::string a;

	static DAW_CONSTEXPR std::string_view json_data =
	  R"({"b": 5, "c": true, "a": [1,2,3,4], } )";
#ifdef DAW_USE_EXCEPTIONS
	try {
#endif
		auto const result = daw::json::from_json<array_member_trail>( json_data );
		test_assert( result.a.size( ) == 4, "Unexpected result" );
		test_assert( result.a[0] == 1, "Unexpected result" );
#ifdef DAW_USE_EXCEPTIONS
	} catch( daw::json::json_exception const &je ) {
		std::cerr << "array_member trail parsing failed: " << je.reason( ) << '\n';
		return false;
	}
#endif
	return true;
}

bool test_array_trail( ) {
	static DAW_CONSTEXPR std::string_view json_data = "[1,2,3,4,5,]";
#ifdef DAW_USE_EXCEPTIONS
	try {
#endif
		std::vector<int> const result =
		  daw::json::from_json_array<int>( json_data );
		test_assert( result.size( ) == 5, "Unexpected result" );
		test_assert( result[0] == 1, "Unexpected result" );
		test_assert( result[4] == 5, "Unexpected result" );
#ifdef DAW_USE_EXCEPTIONS
	} catch( daw::json::json_exception const &je ) {
		std::cerr << "array trail parsing failed: " << je.reason( ) << '\n';
		return false;
	}
#endif
	return true;
}

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	test_assert( test_int_trail( ),
	             "Signed integers fail to accommodate trailing strings" );
	test_assert( test_string_raw_trail( ),
	             "Raw strings fail to accommodate trailing strings" );
	test_assert( test_unsigned_trail( ),
	             "Unsigned integers fail to accommodate trailing strings" );
	test_assert( test_bool_trail( ),
	             "Bools fail to accommodate trailing strings" );
	test_assert( test_object_trail( ),
	             "Objects fail to accommodate trailing strings" );
	daw::expecting( test_string_trail( ) );
	daw::expecting( test_array_member_trail( ) );
	daw::expecting( test_array_trail( ) );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif
