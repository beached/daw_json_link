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

struct B {
	A a;
};

namespace daw::json {
	template<>
	struct json_data_contract<A> {
		using force_aggregate_construction = void;
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"some_num", int>>;

#else
		static constexpr char const i[] = "some_num";
		using type = json_member_list<json_number<i, int>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( A const &v ) {
			return std::forward_as_tuple( v.member );
		}
	};

	template<>
	struct json_data_contract<B> {
		using force_aggregate_construction = void;
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_class<"a", A>>;

#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_class<a, A>>;
#endif
		static DAW_CONSTEXPR auto to_json_data( B const &v ) {
			return std::forward_as_tuple( v.a );
		}
	};
} // namespace daw::json

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	std::string const json_data = R"({ "some_num": 1234 } )";

	// Need runtime exec mode or constexpr with C++20 constexpr
	// destructors/is_constant_evaluated to ensure that rvo path is taken
	using policy_t = daw::json::BasicParsePolicy<daw::json::parse_options(
	  daw::json::ExecModeTypes::runtime )>;

	daw::expecting( daw::json::from_json<A, policy_t>( json_data ).member ==
	                1234 );

	std::string const json_data2 = R"({ "a": { "some_num": 1234 } } )";
	daw::expecting( daw::json::from_json<B, policy_t>( json_data2 ).a.member ==
	                1234 );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
