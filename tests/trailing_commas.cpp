// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_string<"a">>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_string<a>>;
#endif
};

bool test_string_trail( ) {
	try {
		static constexpr std::string_view json_data =
		  R"({"b": 5, "c": true, "a": "hello", } )";
		string_trail const result = daw::json::from_json<string_trail>( json_data );
		daw_json_assert( result.a == "hello", "Unexpected result" );
	} catch( daw::json::json_exception const &je ) {
		std::cerr << "string trail parsing failed: " << je.reason( ) << '\n';
		return false;
	}
	return true;
}
/***********************************************/
struct string_raw_trail {
	std::string_view a;
};

template<>
struct daw::json::json_data_contract<string_raw_trail> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_string_raw<"a", std::string_view>>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_string_raw<a, std::string_view>>;
#endif
};

constexpr bool test_string_raw_trail( ) {
	constexpr std::string_view json_data =
	  R"({"b": 5, "c": true, "a": "hello", } )";

	string_raw_trail const result =
	  daw::json::from_json<string_raw_trail>( json_data );

	daw_json_assert( result.a == "hello", "Unexpected result" );
	return true;
}

/***********************************************/
struct int_trail {
	int a;
};

template<>
struct daw::json::json_data_contract<int_trail> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_number<"a", int>>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_number<a, int>>;
#endif
};

constexpr bool test_int_trail( ) {
	constexpr std::string_view json_data = R"({"b": 5, "c": true, "a": -5, } )";

	int_trail const result = daw::json::from_json<int_trail>( json_data );

	daw_json_assert( result.a == -5, "Unexpected result" );
	return true;
}

/***********************************************/
struct unsigned_trail {
	unsigned a;
};

template<>
struct daw::json::json_data_contract<unsigned_trail> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_number<"a", unsigned>>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_number<a, unsigned>>;
#endif
};

constexpr bool test_unsigned_trail( ) {
	constexpr std::string_view json_data = R"({"b": 5, "c": true, "a": 5, } )";

	unsigned_trail const result =
	  daw::json::from_json<unsigned_trail>( json_data );

	daw_json_assert( result.a == 5, "Unexpected result" );
	return true;
}
/***********************************************/
struct bool_trail {
	bool a;
};

template<>
struct daw::json::json_data_contract<bool_trail> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_bool<"a">>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_bool<a>>;
#endif
};

constexpr bool test_bool_trail( ) {
	constexpr std::string_view json_data = R"({"b": 5, "c": true, "a": true, } )";

	bool_trail const result = daw::json::from_json<bool_trail>( json_data );

	daw_json_assert( result.a, "Unexpected result" );
	return true;
}

/***********************************************/
struct object_trail {
	int_trail a;
};

template<>
struct daw::json::json_data_contract<object_trail> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_class<"a", int_trail>>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_class<a, int_trail>>;
#endif
};

constexpr bool test_object_trail( ) {
	constexpr std::string_view json_data =
	  R"({"b": 5, "c": true, "a": { "b": true, "c": 1.234, "a": 5, }, } )";

	object_trail const result = daw::json::from_json<object_trail>( json_data );

	daw_json_assert( result.a.a == 5, "Unexpected result" );
	return true;
}

/***********************************************/
struct array_member_trail {
	std::vector<int> a;
};

template<>
struct daw::json::json_data_contract<array_member_trail> {
#ifdef __cpp_nontype_template_parameter_class
	using type = json_member_list<json_array<"a", int>>;
#else
	static inline constexpr char const a[] = "a";
	using type = json_member_list<json_array<a, int>>;
#endif
};

bool test_array_member_trail( ) {
	static constexpr std::string_view json_data =
	  R"({"b": 5, "c": true, "a": [1,2,3,4], } )";
	try {
		array_member_trail const result =
		  daw::json::from_json<array_member_trail>( json_data );
		daw_json_assert( result.a.size( ) == 4, "Unexpected result" );
		daw_json_assert( result.a[0] == 1, "Unexpected result" );
	} catch( daw::json::json_exception const &je ) {
		std::cerr << "array_member trail parsing failed: " << je.reason( ) << '\n';
		return false;
	}
	return true;
}

bool test_array_trail( ) {
	static constexpr std::string_view json_data = "[1,2,3,4,5,]";
	try {
		std::vector<int> const result =
		  daw::json::from_json_array<int>( json_data );
		daw_json_assert( result.size( ) == 5, "Unexpected result" );
		daw_json_assert( result[0] == 1, "Unexpected result" );
		daw_json_assert( result[4] == 5, "Unexpected result" );
	} catch( daw::json::json_exception const &je ) {
		std::cerr << "array trail parsing failed: " << je.reason( ) << '\n';
		return false;
	}
	return true;
}

int main( ) {
	daw_json_assert( test_int_trail( ),
	                 "Signed integers fail to accommodate trailing strings" );
	daw_json_assert( test_string_raw_trail( ),
	                 "Raw strings fail to accommodate trailing strings" );
	daw_json_assert( test_unsigned_trail( ),
	                 "Unsigned integers fail to accommodate trailing strings" );
	daw_json_assert( test_bool_trail( ),
	                 "Bools fail to accommodate trailing strings" );
	daw_json_assert( test_object_trail( ),
	                 "Objects fail to accommodate trailing strings" );
	daw::expecting( test_string_trail( ) );
	daw::expecting( test_array_member_trail( ) );
	daw::expecting( test_array_trail( ) );
}
