// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_benchmark.h>
#if not defined( __cpp_constexpr_dynamic_alloc )
// constexpr dtor's are not available prior to c++20
#define DAW_JSON_NO_CONST_EXPR
#endif
#include <daw/json/daw_json_link.h>

#include <iostream>

struct A {
	int member;

	constexpr A( int a )
	  : member( a ) {}
	~A( ) = default;
	A( A const & ) = delete;
	A( A && ) = delete;
	A &operator=( A const & ) = delete;
	A &operator=( A && ) = delete;
};

namespace daw::json {
	template<>
	struct json_data_contract<A> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"some_num", int>>;

#else
		static inline constexpr char const i[] = "some_num";
		using type = json_member_list<json_number<i, int>>;
#endif
		static constexpr auto to_json_data( A const &v ) {
			return std::forward_as_tuple( v.member );
		}
	};
} // namespace daw::json

int main( int, char ** ) try {
	constexpr std::string_view json_data = R"({ "some_num": 1234 } )";
#if not defined( __cpp_constexpr_dynamic_alloc )
	daw::expecting( daw::json::from_json<A>( json_data ).member == 1234 );
#else
	static_assert( daw::json::from_json<A>( json_data ).member == 1234 );
#endif
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
