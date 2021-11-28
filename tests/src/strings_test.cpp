// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 2500;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

template<typename Container>
constexpr void clear( Container &c ) {
	for( auto &v : c ) {
		v = { };
	}
}

template<typename T>
using can_ostream_test = decltype( std::declval<std::ostream &>( ) << std::declval<T>( ) );

template<typename T>
inline constexpr bool can_ostream_v = daw::is_detected_v<can_ostream_test, T>;

template<typename T, typename A>
auto operator<<( std::ostream &os, std::vector<T, A> const &rhs )
  -> std::enable_if_t<can_ostream_v<T>, std::ostream &> {
	os << '[';
	if( not rhs.empty( ) ) {
		os << rhs.front( );
		for( std::size_t n = 1; n < rhs.size( ); ++n ) {
			os << ", " << rhs[n];
		}
	}
	os << ']';
	return os;
}

template<typename T, typename U>
inline constexpr void test_equal( T const &lhs, U const &rhs, std::string_view msg ) {
	if( lhs != rhs ) {
		std::cerr << msg << '\n';
		if constexpr( can_ostream_v<T> ) {
			std::cerr << "LHS: " << lhs << '\n';
		}
		if constexpr( can_ostream_v<U> ) {
			std::cerr << "RHS: " << rhs << '\n';
		}
		exit( 1 );
	}
}

template<typename ExecTag>
std::size_t test( std::string_view json_data ) {
	std::cout << "Using " << ExecTag::name << " exec model\n";
	using namespace daw::json;
	using JString = json_string_raw_no_name<std::string_view,
	                                        string_raw_opt( JsonNullable::MustExist,
	                                                        EightBitModes::AllowFull,
	                                                        AllowEscapeCharacter::NotBeforeDblQuote ),
	                                        daw::construct_a_t<std::string_view>>;

	std::vector<std::string_view> values =
	  from_json_array<JString,
	                  std::vector<std::string_view>,
	                  daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( json_data );
	auto const v2 = values;
	clear( values );
	auto checked_tst = []( auto sv, auto ptr ) {
		auto range =
		  json_array_range<JString, daw::json::SIMDNoCommentSkippingPolicyChecked<ExecTag>>( sv );
		for( auto v : range ) {
			*ptr++ = v;
		}
	};
	checked_tst( json_data, values.data( ) );
	test_equal( v2, values, "Expected them to parse the same" );
	auto const h0 = std::accumulate( values.begin( ),
	                                 values.end( ),
	                                 std::size_t{ 0 },
	                                 []( auto old, auto current ) {
		                                 return old += std::hash<std::string>{ }(
		                                          static_cast<std::string>( current ) );
	                                 } );
	std::vector<std::string_view> values2 =
	  from_json_array<JString,
	                  std::vector<std::string_view>,
	                  daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( json_data );

	auto unchecked_tst = []( auto sv, auto ptr ) {
		auto range =
		  json_array_range<JString, daw::json::SIMDNoCommentSkippingPolicyUnchecked<ExecTag>>( sv );
		for( auto v : range ) {
			*ptr++ = v;
		}
	};
	unchecked_tst( json_data, values2.data( ) );
	auto const h1 = std::accumulate( values2.begin( ),
	                                 values2.end( ),
	                                 std::size_t{ 0 },
	                                 []( auto old, auto current ) {
		                                 return old += std::hash<std::string>{ }(
		                                          static_cast<std::string>( current ) );
	                                 } );
	test_equal( values, values2, "Parses don't match" );
	test_equal( h0, h1, "Hashes don't match" );
	return h1;
}

int main( int argc, char **argv )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to strings.json file\n" );
		exit( EXIT_FAILURE );
	}
	std::string const json_string = [argv] {
		auto const data = *daw::read_file( argv[1] );
		return std::string( data.data( ), data.size( ) );
	}( );
	auto const h0 = test<daw::json::constexpr_exec_tag>( json_string );
	auto const h1 = test<daw::json::runtime_exec_tag>( json_string );
	test_equal( h0, h1, "constexpr/runtime exec model hashes do not match" );
	if constexpr( not std::is_same_v<daw::json::simd_exec_tag, daw::json::runtime_exec_tag> ) {
		auto const h2 = test<daw::json::simd_exec_tag>( json_string );
		test_equal( h0, h2, "constexpr/fast exec model hashes do not match" );
	}
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( ) << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif