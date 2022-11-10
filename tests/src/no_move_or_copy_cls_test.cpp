// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

// define DAW_JSON_ENABLE_FULL_RVO so that all paths will use RVO when possible to return. 
#define DAW_JSON_ENABLE_FULL_RVO

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

static_assert( daw::json::is_pinned_type_v<A> );

struct B {
	A a;
};

namespace daw::json {
	template<>
	struct json_data_contract<A> {
		using force_aggregate_construction = void;
		static constexpr char const i[] = "some_num";
		using type = json_member_list<json_number<i, int>>;
		static DAW_CONSTEXPR auto to_json_data( A const &v ) {
			return std::forward_as_tuple( v.member );
		}
	};

	template<>
	struct json_data_contract<B> {
		using force_aggregate_construction = void;
#ifdef DAW_JSON_CNTTP_JSON_NAME
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
	static constexpr auto policy_v =
	  daw::json::options::parse_flags<daw::json::options::ExecModeTypes::runtime>;

	daw::expecting( daw::json::from_json<A>( json_data, policy_v ).member ==
	                1234 );

	std::string const json_data2 = R"({ "a": { "some_num": 1234 } } )";
	daw::expecting( daw::json::from_json<B>( json_data2, policy_v ).a.member ==
	                1234 );
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
