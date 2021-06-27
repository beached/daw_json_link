// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_span.h>
#include <daw/daw_tuple_forward.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_schema.h>

#include <cassert>
#include <cstdio>
#include <map>
#include <string>
#include <tuple>
#include <variant>

struct Bar {
	int a;
	int b;
};

struct Umm {
	double a;
	int b;
};

struct Foo {
	int a = 1;
	double b = 2.2;
	std::string c = "3";
	std::vector<int> d = { 1, 2, 3, 4 };
	Bar e;
	std::map<std::string, Bar> f;
	std::map<int, float> g;
	std::variant<int, std::string, bool> h;
	std::variant<int, std::string, bool, Bar, Umm> i;
	std::optional<int> j;
	std::unique_ptr<int[]> l = std::unique_ptr<int[]>( new int[0] );
	int k;
};

struct IdentitySwitcher {
	constexpr std::size_t operator( )( std::size_t n ) const {
		return n;
	}

	inline std::size_t operator( )( Foo const &f ) const {
		return f.i.index( );
	}
};

template<typename T>
struct UniquePtrArrayCtor {
	constexpr std::unique_ptr<T[]> operator( )( ) const {
		return { };
	}

	template<typename Iterator>
	inline std::unique_ptr<T[]> operator( )( Iterator first, Iterator last ) {
		auto const sz = last - first;
		assert( sz >= 0 );
		auto result = std::unique_ptr<T[]>( new T[static_cast<std::size_t>( sz )] );
		std::copy( first, last, result.get( ) );
		return result;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Bar> {
		using type = json_ordered_member_list<int, int>;

		static inline auto to_json_data( Bar const &b ) {
			return std::forward_as_tuple( b.a, b.b );
		}
	};

	template<>
	struct json_data_contract<Umm> {
		using type = json_ordered_member_list<double, int>;

		static inline auto to_json_data( Umm const &b ) {
			return std::forward_as_tuple( b.a, b.b );
		}
	};

	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		static constexpr char const c[] = "c";
		static constexpr char const d[] = "d";
		static constexpr char const e[] = "e";
		static constexpr char const f[] = "f";
		static constexpr char const g[] = "g";
		static constexpr char const gkey[] = "k";
		static constexpr char const gvalue[] = "v";
		static constexpr char const h[] = "h";
		static constexpr char const type_mem[] = "type";
		static constexpr char const i[] = "i";
		static constexpr char const j[] = "j";
		static constexpr char const k[] = "k";
		static constexpr char const l[] = "l";
		using type = json_member_list<
		  json_link<a, int>, json_link<b, double>, json_link<c, std::string>,
		  json_link<d, std::vector<int>>, json_link<e, Bar>,
		  json_link<f, std::map<std::string, Bar>>,
		  json_key_value_array<g, std::map<int, float>, json_link<gvalue, float>,
		                       json_link<gkey, int>>,
		  json_variant<h, std::variant<int, std::string, bool>>,
		  json_tagged_variant<i, std::variant<int, std::string, bool, Bar, Umm>,
		                      json_link<type_mem, std::size_t>, IdentitySwitcher>,
		  json_link<j, std::optional<int>>,
		  json_sized_array<l, int, json_link<k, int>, std::unique_ptr<int[]>,
		                   UniquePtrArrayCtor<int>>,
		  json_link<k, int>>;

		static inline auto to_json_data( Foo const &v ) {
			return daw::forward_nonrvalue_as_tuple(
			  v.a, v.b, v.c, v.d, v.e, v.f, v.g, v.h, v.i, v.j,
			  daw::span( v.l.get( ), static_cast<std::size_t>( v.k ) ), v.k );
		}
	};
} // namespace daw::json

int main( ) {
	std::string result = daw::json::to_json_schema<Foo>( "", "Foo" );
	puts( result.c_str( ) );

	puts( "----\n" );
	std::string json_str = daw::json::to_json( Foo{ } );
	puts( json_str.c_str( ) );

	auto foo2 = daw::json::from_json<Foo>( json_str );
	(void)foo2;
}
