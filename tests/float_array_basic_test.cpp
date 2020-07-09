// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_do_n.h>
#include <daw/daw_random.h>
#include <daw/iterator/daw_back_inserter.h>

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

struct Number {
	float a{};
};
#ifdef __cpp_nontype_template_parameter_class
[[maybe_unused]] static constexpr auto
json_data_contract_for( Number ) noexcept {
	using namespace daw::json;
	return json_member_list<json_number<"a", float>>{};
}
#else
namespace symbols_Number {
	static inline constexpr char const a[] = "a";
}

[[maybe_unused]] static constexpr auto
json_data_contract_for( Number ) noexcept {
	using namespace daw::json;
	return json_member_list<json_number<symbols_Number::a, float>>{};
}
#endif

template<typename Float>
Float rand_float( ) {
	auto f0 = daw::randint<intmax_t>( daw::numeric_limits<intmax_t>::min( ),
	                                  daw::numeric_limits<intmax_t>::max( ) );
	if( f0 == 0 ) {
		f0 = 1;
	}
	auto const f1 =
	  daw::randint<intmax_t>( daw::numeric_limits<intmax_t>::min( ),
	                          daw::numeric_limits<intmax_t>::max( ) );

	return static_cast<Float>( f1 ) / static_cast<Float>( f0 );
}

template<size_t NUMVALUES>
void test_func( ) {
	using namespace daw::json;
	using iterator_t = daw::json::json_array_iterator<float>;

	std::string json_data3 = [] {
		std::string result = "[";
		result.reserve( NUMVALUES * 23 + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += std::to_string( rand_float<float>( ) ) + ',';
		} );
		result.back( ) = ']';
		return result;
	}( );

	daw::string_view json_sv{json_data3.data( ), json_data3.size( )};
	auto data2 = std::unique_ptr<float[]>( new float[NUMVALUES] );
	{
		auto const count3 = *daw::bench_n_test_mbs<100>(
		  "float", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
			  daw::do_not_optimize( data2 );
			  return ptr - data2.get( );
		  },
		  json_sv );

		std::cout << "float parse count: " << count3 << '\n';
	}
}

int main( int argc, char ** ) try {
	if( argc > 1 ) {
		test_func<1'000'000ULL>( );
	} else {
		test_func<1'000ULL>( );
	}
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
