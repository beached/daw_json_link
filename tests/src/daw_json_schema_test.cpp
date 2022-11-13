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
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <variant>

struct Bar {
	int a = 9;
	int type = 0;
};

struct Umm {
	double a = 12.1;
	int type = 1;
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
	std::tuple<int, double> m{ 99, 98.8 };
	std::variant<Bar, Umm> n{ Umm{} };
	std::tuple<double, std::string, int, std::variant<Bar, Umm>> o{ 1.1, "2", 1,
	                                                                Umm{} };
};

struct FooBoo {
	int a = 1;
	double b = 2.2;
	std::string_view c = "3";
	Bar e{ };
	std::variant<int, std::string_view, bool> h{ 5 };
	std::variant<int, std::string_view, bool, Bar, Umm> i{ bool{} };
	std::optional<int> j{ };
	int k{ };
	std::tuple<int, double> m{ 99, 98.8 };
	std::variant<Bar, Umm> n{ Umm{} };
	std::tuple<double, std::string_view, int, std::variant<Bar, Umm>> o{
	  1.1, "2", 1, Umm{} };
};

template<typename... Ts>
struct IdentitySwitcher {
	constexpr std::size_t operator( )( std::size_t n ) const {
		assert( n < sizeof...( Ts ) );
		return n;
	}

	inline std::size_t operator( )( Foo const &f ) const {
		return f.i.index( );
	}

	inline std::size_t operator( )( FooBoo const &f ) const {
		return f.i.index( );
	}
};

template<typename T>
struct UniquePtrArrayCtor {
	constexpr std::unique_ptr<T[]> operator( )( ) const {
		return { };
	}

	template<typename Iterator>
	inline std::unique_ptr<T[]> operator( )( Iterator first, Iterator last,
	                                         std::size_t sz ) const {
		auto result = std::unique_ptr<T[]>( new T[static_cast<std::size_t>( sz )] );
		auto out_last = std::copy( first, last, result.get( ) );
		(void)out_last;
		auto const elements_copied = out_last - result.get( );
		(void)elements_copied;
		assert( elements_copied >= 0 );
		assert( static_cast<std::size_t>( elements_copied ) == sz );

		return result;
	}
};

namespace daw::json {
	inline constexpr char const mem_a[] = "a";
	inline constexpr char const mem_b[] = "b";
	inline constexpr char const mem_c[] = "c";
	inline constexpr char const mem_d[] = "d";
	inline constexpr char const mem_e[] = "e";
	inline constexpr char const mem_f[] = "f";
	inline constexpr char const mem_g[] = "g";
	inline constexpr char const gkey[] = "k";
	inline constexpr char const gvalue[] = "v";
	inline constexpr char const mem_h[] = "h";
	inline constexpr char const mem_type[] = "type";
	inline constexpr char const mem_i[] = "i";
	inline constexpr char const mem_j[] = "j";
	inline constexpr char const mem_k[] = "k";
	inline constexpr char const mem_l[] = "l";
	inline constexpr char const mem_m[] = "m";
	inline constexpr char const mem_n[] = "n";
	inline constexpr char const mem_o[] = "o";

	template<>
	struct json_data_contract<Bar> {
		using type = json_tuple_member_list<int, int>;

		static constexpr auto to_json_data( Bar const &b ) {
			return std::forward_as_tuple( b.a, b.type );
		}
	};

	template<>
	struct json_data_contract<Umm> {
		using type = json_tuple_member_list<double, int>;

		static constexpr auto to_json_data( Umm const &b ) {
			return std::forward_as_tuple( b.a, b.type );
		}
	};

	template<>
	struct json_data_contract<Foo> {
		using force_aggregate_construction = void;

		using type = json_member_list<
		  json_link<mem_a, int>, json_link<mem_b, double>,
		  json_link<mem_c, std::string>, json_link<mem_d, std::vector<int>>,
		  json_link<mem_e, Bar>, json_link<mem_f, std::map<std::string, Bar>>,
		  json_key_value_array<mem_g, std::map<int, float>,
		                       json_link<gvalue, float>, json_link<gkey, int>>,
		  json_variant<mem_h, std::variant<int, std::string, bool>>,
		  json_tagged_variant<mem_i, std::variant<int, std::string, bool, Bar, Umm>,
		                      json_link<mem_type, std::size_t>,
		                      IdentitySwitcher<int, std::string, bool, Bar, Umm>>,
		  json_link<mem_j, std::optional<int>>,
		  json_sized_array<mem_l, int, json_link<mem_k, int>,
		                   std::unique_ptr<int[]>, UniquePtrArrayCtor<int>>,
		  json_link<mem_k, int>, json_tuple<mem_m, std::tuple<int, double>>,
		  json_intrusive_variant<mem_n, std::variant<Bar, Umm>,
		                         json_tuple_member<1, std::size_t>,
		                         IdentitySwitcher<Bar, Umm>>,
		  json_tuple<mem_o,
		             std::tuple<double, std::string, int, std::variant<Bar, Umm>>,
		             json_tuple_types_list<
		               double, std::string, int,
		               json_tagged_variant_no_name<
		                 std::variant<Bar, Umm>, json_tuple_member<2, std::size_t>,
		                 IdentitySwitcher<Bar, Umm>>>>>;

		static inline auto to_json_data( Foo const &v ) {
			return daw::forward_nonrvalue_as_tuple(
			  v.a, v.b, v.c, v.d, v.e, v.f, v.g, v.h, v.i, v.j,
			  daw::span( v.l.get( ), static_cast<std::size_t>( v.k ) ), v.k, v.m, v.n,
			  v.o );
		}
	};

	template<>
	struct json_data_contract<FooBoo> {
		using force_aggregate_construction = void;

		using type = json_member_list<
		  json_link<mem_a, int>, json_link<mem_b, double>,
		  json_link<mem_c, std::string_view>, json_link<mem_e, Bar>,
		  json_variant<mem_h, std::variant<int, std::string_view, bool>>,
		  json_tagged_variant<
		    mem_i, std::variant<int, std::string_view, bool, Bar, Umm>,
		    json_link<mem_type, std::size_t>,
		    IdentitySwitcher<int, std::string_view, bool, Bar, Umm>>,
		  json_link<mem_j, std::optional<int>>,

		  json_link<mem_k, int>, json_tuple<mem_m, std::tuple<int, double>>,
		  json_intrusive_variant<mem_n, std::variant<Bar, Umm>,
		                         json_tuple_member<1, std::size_t>,
		                         IdentitySwitcher<Bar, Umm>>,
		  json_tuple<
		    mem_o,
		    std::tuple<double, std::string_view, int, std::variant<Bar, Umm>>,
		    json_tuple_types_list<
		      double, std::string_view, int,
		      json_tagged_variant_no_name<std::variant<Bar, Umm>,
		                                  json_tuple_member<2, std::size_t>,
		                                  IdentitySwitcher<Bar, Umm>>>>>;

		static inline auto to_json_data( FooBoo const &v ) {
			return daw::forward_nonrvalue_as_tuple( v.a, v.b, v.c, v.e, v.h, v.i, v.j,
			                                        v.k, v.m, v.n, v.o );
		}
	};
} // namespace daw::json

int main( ) {
	using namespace daw::json::options;
	std::string result = daw::json::to_json_schema<Foo>(
	  "", "Foo", output_flags<SerializationFormat::Pretty> );
	puts( result.c_str( ) );
	puts( "----\n" );
	std::string json_str1 =
	  daw::json::to_json( Foo{ }, output_flags<SerializationFormat::Pretty> );
	puts( json_str1.c_str( ) );
	puts( "\n----\n\n" );

	auto foo2 = daw::json::from_json<Foo>( json_str1 );
	(void)foo2;

	std::string json_str2 = daw::json::to_json( FooBoo{ } );
	puts( json_str2.c_str( ) );
	auto fooboo = daw::json::from_json<FooBoo>( json_str2 );
	std::cout << "\n----------------------------------------\n"
	          << daw::json::to_json( fooboo ) << '\n';
}
