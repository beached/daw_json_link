// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/optional.hpp>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>

#include "daw_json_link.h"
#include "daw_json_link_fixes.h"
#include "daw_json_link_file.h"
#include "daw_json_link_streams.h"
#include "daw_json_link_datetime.h"

#include <codecvt>

struct A : public daw::json::daw_json_link<A> {
	int a;
	double b;
	bool c;
	std::string d;
	boost::optional<std::string> e;

	A( ) : a{0}, b{1.2345}, c{true}, d{"sixseveneightnine"}, e{boost::none} {}
	~A( ) = default;
	A( A const & ) = default;
	A( A && ) = default;
	A &operator=( A const & ) = default;
	A &operator=( A && ) = default;

	static void json_link_map( ) {
		link_json_integer_fn( "aaaaaa", []( A &obj, int value ) { obj.a = std::move( value ); },
		                      []( A const &obj ) { return obj.a; } );
		link_json_real_fn( "bbbbbb", []( A &obj, double value ) { obj.b = std::move( value ); },
		                   []( A const &obj ) { return obj.b; } );
		link_json_boolean_fn( "cccccc", []( A &obj, bool value ) { obj.c = std::move( value ); },
		                      []( A const &obj ) { return obj.c; } );
		link_json_string_fn( "dddddd", []( A &obj, daw::string_view value ) { obj.d = value.to_string( ); },
		                     []( A const &obj ) { return obj.d; } );
		link_json_string_optional_fn( "e",
		                              []( A &obj, boost::optional<daw::string_view> value ) {
			                              obj.e = value ? boost::optional<std::string>{value->to_string( )}
			                                            : boost::optional<std::string>{boost::none};
		                              },
		                              []( A const &obj ) { return obj.e; } );
	}
};
struct A2 {
	int a;
	double b;
	bool c;
	std::string d;
};

struct B : public daw::json::daw_json_link<B> {
	A a;
	std::vector<int64_t> b;
	std::vector<double> c;
	std::vector<bool> d;
	std::vector<std::string> e;
	std::vector<A> f;

	B( ): a{ }, b{ }, c{ }, d{ }, e{ }, f{ } { }
	~B( ) = default;
	B( B const & ) = default;
	B( B && ) = default;
	B &operator=( B const & ) = default;
	B &operator=( B && ) = default;


	static void json_link_map( ) {
		link_json_object_fn( "aaaaaa", []( B &obj, A value ) { obj.a = std::move( value ); },
		                     []( B const &obj ) { return obj.a; } );
		link_json_integer_array_fn( "b", []( B &obj, auto value ) { obj.b.push_back( std::move( value ) ); },
		                            []( B const &obj ) { return obj.b; } );
		link_json_real_array_fn( "c", []( B &obj, auto value ) { obj.c.push_back( std::move( value ) ); },
		                            []( B const &obj ) { return obj.c; } );
		link_json_boolean_array_fn( "d", []( B &obj, auto value ) { obj.d.push_back( std::move( value ) ); },
		                            []( B const &obj ) { return obj.d; } );
		link_json_string_array_fn( "e", []( B &obj, auto value ) { obj.e.push_back( value.to_string( ) ); },
		                           []( B const &obj ) { return obj.e; } );
		link_json_object_array_fn( "f", []( B &obj, auto value ) { obj.f.push_back( std::move( value ) ); },
		                            []( B const &obj ) { return obj.f; } );
	}
};

struct C : public daw::json::daw_json_link<C> {
	A a;
	std::vector<int64_t> b;
	std::vector<double> c;
	std::vector<bool> d;
	std::vector<std::string> e;
	std::vector<A> f;
	boost::optional<std::string> g;

	static void json_link_map( ) {
		link_json_object( "aaaaaa", a );
		link_json_integer_array( "b", b );
		link_json_real_array( "c", c );
		link_json_boolean_array( "d", d );
		link_json_string_array( "e", e );
		link_json_object_array( "f", f );
		link_json_string_optional( "g", g, boost::none );
	}
};


auto make_path_str( std::string s ) {
#ifdef WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes( s );
}
#else
	return s;
}
#endif

std::string to_si_bytes( double d ) {
	using std::to_string;
	if( d < 1024.0 ) {
		return to_string( d ) + " bytes";
	}
	d /= 1024.0;
	if( d < 1024.0 ) {
		return to_string( d ) + " KB";
	}
	d /= 1024.0;
	if( d < 1024.0 ) {
		return to_string( d ) + " MB";
	}
	d /= 1024.0;
	if( d < 1024.0 ) {
		return to_string( d ) + " GB";
	}
	d /= 1024.0;
	return to_string( d ) + " TB";
}

int main( int argc, char **argv ) {
	using namespace std::string_literals;
	B b;
	std::cout << "Size of linked class->" << sizeof( A ) << " vs size of unlinked->" << sizeof( A2 ) << '\n';
	auto const str = b.to_json_string( );
	std::string const str_array = "[" + str + "," + str + "]";
	auto a = B::from_json_string( str ).result;
	std::cout << a.to_json_string( ) << '\n';

	std::cout << "Attemping json array '" << str_array << "'\n";
	auto c = B::from_json_array_string( str_array );

	{
		B b_test;
		std::stringstream ss{str};
		ss >> b_test;
	}
	std::cout << to_json_string( c ) << std::endl;
	{
		constexpr auto const SZ = 6'500'000;
		auto str_array2 = "["s + str;
		str_array2.reserve( (str.size( )+1)*SZ + 2 );
		for( size_t n=0; n<SZ; ++n ) {
			str_array2 += ","s + str;
		}
		str_array2 += "]"s;
		std::cout << "Using an string of size " << to_si_bytes( str_array2.size( ) ) << '\n';
		auto lapsed_time2 = daw::benchmark( [&str_array2]( ) {
			B::from_json_array_string( str_array2 );
		} );
		std::cout << "To process " << to_si_bytes( str_array2.size( ) ) << " bytes, it took " << lapsed_time2
		          << " seconds. " << to_si_bytes( str_array2.size( ) / lapsed_time2 ) << "/second\n";
	}
	/*
	if( boost::filesystem::exists( make_path_str( "test.json" ).data( ) ) ) {
		daw::filesystem::MemoryMappedFile<char> json_file{make_path_str( "test.json" ).data( )};
		daw::exception::daw_throw_on_false( json_file, "Failed to open test file 'test.json'" );
		std::cout << "Test file is of size " << to_si_bytes( json_file.size( ) ) << '\n';
		auto lapsed_time = daw::benchmark( []( ) -> void {
			auto const result = daw::json::array_from_file<B>( "test.json" );
		} );
		std::cout << "To process " << to_si_bytes( json_file.size( ) ) << " bytes, it took " << lapsed_time
		          << " seconds. " << to_si_bytes( json_file.size( ) / lapsed_time ) << "/second\n";
	}
	*/
/*
   // Create sample json file
	std::fstream s{"test.json", std::ios::binary | std::ios::out};
	s << '[';
	std::string const str = b.to_json_string( );
	auto const count = (1024ull*1024ull*1024ull*15ull)/str.size( );
	s << str;
	for( size_t n=0; n<count; ++n ) {
		s << ',' << str;
	}
	s << ']';
	*/
	return EXIT_SUCCESS;
}
