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

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <daw/daw_benchmark.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_string_view.h>

#include "daw_json_link.h"
#include <codecvt>

struct A : public daw::json::daw_json_link<A> {
	int a;
	double b;
	bool c;
	std::string d;

	static void json_link_map( ) {
		link_json_integer_fn( "aaaaaa", []( A &obj, int value ) { obj.a = std::move( value ); },
		                      []( A const &obj ) { return obj.a; } );
		link_json_real_fn( "bbbbbb", []( A &obj, double value ) { obj.b = std::move( value ); },
		                   []( A const &obj ) { return obj.b; } );
		link_json_boolean_fn( "cccccc", []( A &obj, bool value ) { obj.c = std::move( value ); },
		                      []( A const &obj ) { return obj.c; } );
		link_json_string_fn( "dddddd", []( A &obj, std::string value ) { obj.d = std::move( value ); },
		                     []( A const &obj ) { return obj.d; } );
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

	static void json_link_map( ) {
		link_json_object_fn( "aaaaaa", []( B &obj, A value ) { obj.a = std::move( value ); },
		                     []( B const &obj ) { return obj.a; } );
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
	std::cout << "Size of linked class->" << sizeof( A ) << " vs size of unlinked->" << sizeof( A2 ) << '\n';
	//constexpr daw::string_view const str = "{ \"a\": { \"a\" : 5, \"b\" : 6.6, \"c\" : true, \"d\": \"hello\" }}";
	constexpr daw::string_view const str = "{ \"aaaaaa\": { \"aaaaaa\" : 55555, \"bbbbbb\" : 6666666.6, \"cccccc\" : true, \"dddddd\": \"fddffdffffffffffhello\" }}";
	std::string const str_array = "[" + str.to_string( ) + "," + str.to_string( ) + "]";
	auto a = B::from_json_string( str ).result;
	std::cout << a.to_json_string( ) << '\n';

	std::cout << "Attemping json array '" << str_array << "'\n";
	auto c = B::from_json_array_string( str_array );

	std::cout << to_json_string( c ) << std::endl;
	{
		constexpr auto const SZ = 10'000'000;
		auto str_array2 = "["s + str.to_string( );
		str_array2.reserve( (str.size( )+1)*SZ + 2 );
		for( size_t n=0; n<SZ; ++n ) {
			str_array2 += ","s + str.to_string( );
		}
		str_array2 += "]"s;
		auto lapsed_time2 = daw::benchmark( [&str_array2]( ) {
			B::from_json_array_string( str_array2 );
		} );
		std::cout << "To process " << to_si_bytes( str_array2.size( ) ) << " bytes, it took " << lapsed_time2
		          << " seconds. " << to_si_bytes( str_array2.size( ) / lapsed_time2 ) << "/second\n";
	}
	if( boost::filesystem::exists( make_path_str( "test.json" ).data( ) ) ) {
		daw::filesystem::MemoryMappedFile<char> json_file{make_path_str( "test.json" ).data( )};
		daw::exception::daw_throw_on_false( json_file, "Failed to open test file 'test.json'" );
		auto lapsed_time = daw::benchmark( [&json_file]( ) {
			B::from_json_array_string( daw::string_view{json_file.data( ), json_file.size( )} );
		} );
		std::cout << "To process " << to_si_bytes( json_file.size( ) ) << " bytes, it took " << lapsed_time
		          << " seconds. " << to_si_bytes( json_file.size( ) / lapsed_time ) << "/second\n";
	}
	return EXIT_SUCCESS;
}
