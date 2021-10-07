// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"
#include "defines.h"

#include <daw/daw_read_file.h>

#include <string>
#include <string_view>
#include <tuple>

struct Foo {
	std::tuple<int, std::string_view, bool, int> a;

	inline bool operator==( Foo const &rhs ) const {
		return a == rhs.a;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const a[] = "a";
		using type = json_member_list<
		  json_tuple<a, std::tuple<int, std::string_view, bool, int>>>;

		static inline auto to_json_data( Foo const &f ) {
			return std::forward_as_tuple( f.a );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{

	// This works around a bug in MSVC and an error about
	// '__builtin_array_init_helper' not being defined or constexpr during
	// constexpr evaluation
	// https://developercommunity.visualstudio.com/t/predefined-c-types-compiler-internal-warning-c4834/1491646
#if defined( _MSC_VER ) and not defined( __clang__ ) and DAW_CXX_STANDARD == 20
#if _MS_VER >= 1429
#define NO_CX_TEST
#endif
#endif

#if not defined( NO_CX_TEST )
	static constexpr std::string_view json_data = R"(
{
	"a":[1,"Hi ♥️ ", true, 55]
}
)";
#if defined( _MSVC_LANG ) and _MSVC_LANG > 201703L
	// MSVC has a bug that can prevent constexpr evaluation
	// https://developercommunity.visualstudio.com/t/error-C3615:-constexpr-function-__builti/1546452
	static auto const cxf = daw::json::from_json<Foo>( json_data );
#else
	static constexpr auto const cxf = daw::json::from_json<Foo>( json_data );
#endif

	if( not( std::get<3>( cxf.a ) == 55 ) ) {
		assert( std::get<3>( cxf.a ) == 55 );
	}
	Foo f = daw::json::from_json<Foo>(
	  json_data ); // This does not evaluate the pack in the correct order during
	               // runtime evaluation but does work at compile time when cxf
	               // is evaluated
	if( not( cxf == f ) ) {
		assert( cxf == f );
	}
#endif
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_tuple1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::read_file( argv[1] );
	assert( data and not data->empty( ) );
	Foo foo1 = daw::json::from_json<Foo>( *data );
	(void)foo1;
	std::string data2 = daw::json::to_json( foo1 );
	Foo foo2 = daw::json::from_json<Foo>( data2 );
	(void)foo2;
	assert( foo1 == foo2 );
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
