// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
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

#include <daw/daw_memory_mapped_file.h>

#include "daw/json/daw_json_link.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace daw::cookbook_dates3 {
	using my_timepoint = std::chrono::time_point<std::chrono::system_clock,
	                                             std::chrono::milliseconds>;

	my_timepoint seconds_since_epoch_to_tp( int64_t seconds ) {
		return my_timepoint{} + std::chrono::seconds( seconds );
	}

	struct MyClass3 {
		std::string title;
		unsigned id;
		my_timepoint date_added;
		my_timepoint last_modified;

		MyClass3( std::string Title, unsigned Id, int64_t DateAdded,
		          int64_t LastModified )
		  : title( Title )
		  , id( Id )
		  , date_added( seconds_since_epoch_to_tp( DateAdded ) )
		  , last_modified( seconds_since_epoch_to_tp( LastModified ) ) {}
	};

	bool operator==( MyClass3 const &lhs, MyClass3 const &rhs ) {
		return std::tie( lhs.title, lhs.id, lhs.date_added, lhs.last_modified ) ==
		       std::tie( rhs.title, rhs.id, rhs.date_added, rhs.last_modified );
	}
} // namespace daw::cookbook_dates3

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_dates3::MyClass3> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<
		  json_string<"title">, json_number<"id", unsigned>,
		  json_number<"dateAdded", int64_t, LiteralAsStringOpt::Always>,
		  json_number<"lastModified", int64_t>>;
#else
		static inline constexpr char const title[] = "title";
		static inline constexpr char const id[] = "id";
		static inline constexpr char const dateAdded[] = "dateAdded";
		static inline constexpr char const lastModified[] = "lastModified";
		using type = json_member_list<
		  json_string<title>, json_number<id, unsigned>,
		  json_number<dateAdded, int64_t, LiteralAsStringOpt::Always>,
		  json_number<lastModified, int64_t>>;
#endif
		static inline auto to_json_data( daw::cookbook_dates3::MyClass3 const &v ) {
			auto const date_added =
			  std::chrono::floor<std::chrono::seconds>( v.date_added )
			    .time_since_epoch( )
			    .count( );
			auto const last_modified =
			  std::chrono::floor<std::chrono::seconds>( v.last_modified )
			    .time_since_epoch( )
			    .count( );
			return std::tuple( std::as_const( v.title ), v.id, date_added,
			                   last_modified );
		}
	};
} // namespace daw::json

int main( int argc, char **argv ) try {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_dates3.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );

	daw::cookbook_dates3::MyClass3 const cls =
	  daw::json::from_json<daw::cookbook_dates3::MyClass3>(
	    std::string_view( data.data( ), data.size( ) ) );

	daw_json_assert( cls.title == "The Title", "Unexpected value" );

	std::string const str = daw::json::to_json( cls );
	puts( str.c_str( ) );

	daw::cookbook_dates3::MyClass3 const cls2 =
	  daw::json::from_json<daw::cookbook_dates3::MyClass3>( str );

	daw_json_assert( cls == cls2, "Unexpected round trip error" );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
