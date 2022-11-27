// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_exception.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_value_state.h>

#include <cstddef>
#include <iostream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

struct A {
	int a;
	double b;
};

struct B {
	std::string b;
	std::vector<int> c;
};

struct Foo {
	std::optional<std::variant<A, B>> member;
};

namespace daw::json {
	template<>
	struct json_data_contract<A> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		using type = json_member_list<json_number<a, int>, json_number<b>>;

		static constexpr auto to_json_data( A const &value ) {
			return std::forward_as_tuple( value.a, value.b );
		}
	};

	template<>
	struct json_data_contract<B> {
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		using type = json_member_list<json_string<b>, json_array<c, int>>;

		static constexpr auto to_json_data( B const &value ) {
			return std::forward_as_tuple( value.b, value.c );
		}
	};
} // namespace daw::json

struct FooConstructor {
	inline Foo operator( )( ) const {
		return Foo{ };
	}

	inline std::variant<A, B> operator( )( char const *ptr,
	                                       std::size_t sz ) const {
		using result_t = std::variant<A, B>;
		auto jv = daw::json::json_value( ptr, sz );
		assert( jv.is_class( ) );
		auto state = daw::json::json_value_state( jv );
		auto const has_a = state.index_of( "a" ) < state.size( );
		auto const has_b = state.index_of( "b" ) < state.size( );
		auto const has_c = state.index_of( "c" ) < state.size( );
		if( has_a and has_b ) {
			return result_t{ daw::json::from_json<A>( std::string_view( ptr, sz ) ) };
		}
		if( has_b and has_c ) {
			return result_t{ daw::json::from_json<B>( std::string_view( ptr, sz ) ) };
		}
		daw::exception::daw_throw<std::runtime_error>( "Unknown Foo subtype" );
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const member[] = "member";
		using type = json_member_list<
		  json_nullable<member, std::optional<std::variant<A, B>>,
		                json_raw_no_name<std::variant<A, B>, FooConstructor>>>;

		static inline auto to_json_data( Foo const &foo ) {
			if( not foo.member ) {
				return std::tuple<std::optional<std::string>>{ };
			}
			return std::tuple<std::optional<std::string>>{ std::visit(
			  []( auto const &value ) {
				  return to_json( value );
			  },
			  *foo.member ) };
		}
	};
} // namespace daw::json

int main( ) {
	char const json_data[] = R"json(
[
	{
		"member": {
			"a": 5,
			"b": 1.234
		}
	},
	{
		"member": {
			"b": "Hello World",
			"c": [1,2,3,4]
  	}
	}
]
)json";

#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
	try {
#endif
		auto foo_ary = daw::json::from_json_array<Foo>( json_data );
		assert( foo_ary.size( ) == 2 );
		assert( foo_ary[0].member );
		assert( foo_ary[0].member->index( ) == 0 );
		A const &a = std::get<0>( *foo_ary[0].member );
		(void)a;
		assert( a.a == 5 );
		assert( foo_ary[1].member );
		assert( foo_ary[1].member->index( ) == 1 );
		B const &b = std::get<1>( *foo_ary[1].member );
		(void)b;
		assert( b.b == "Hello World" );
		assert( ( b.c == std::vector{ 1, 2, 3, 4 } ) );
		std::cout << "Serialized to: \n"
		          << daw::json::to_json_array( foo_ary ) << "\n";
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or \
  defined( _CPPUNWIND )
	} catch( daw::json::json_exception const &jex ) {
		std::cerr << "JSON Exception while parsing: " << jex.reason( ) << '\n';
		exit( EXIT_FAILURE );
	} catch( std::exception const &ex ) {
		std::cerr << "Exception while parsing: " << ex.what( ) << '\n';
		exit( EXIT_FAILURE );
	}
#endif
}
