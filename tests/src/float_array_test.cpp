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

#include <daw/daw_benchmark.h>
#include <daw/daw_do_n.h>
#include <daw/daw_random.h>

#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

#if not defined( DAW_NUM_RUNS )
#if not defined( DEBUG ) or defined( NDEBUG )
static inline constexpr std::size_t DAW_NUM_RUNS = 250;
#else
static inline constexpr std::size_t DAW_NUM_RUNS = 2;
#endif
#endif
static_assert( DAW_NUM_RUNS > 0 );

struct Number {
	float a{ };
};

struct Number2 {
	float a{ };
};

namespace daw::json {
	template<>
	struct json_data_contract<Number> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"a", float>>;
#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, float>>;
#endif
	};

	template<>
	struct json_data_contract<Number2> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"a", float>>;
#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, float>>;
#endif
	};
} // namespace daw::json

template<typename Float>
Float rand_float( ) {
	static DAW_CONSTEXPR Float fmin = 0;
	static DAW_CONSTEXPR Float fmax = 1;
	static auto e = std::default_random_engine( );
	static auto dis = std::uniform_real_distribution<Float>( fmin, fmax );
	return dis( e );
}

template<size_t NUMVALUES>
void test_func( ) {
	using namespace daw::json;
	std::string const json_data = [] {
		std::string result = "[";

		// 23 is what I calculated as the string size of the serialized class.
		// It may be incorrect but that is ok, it is close and should reduce
		// allocations
		result.reserve( NUMVALUES * 23U + 8U + 256U );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += "{\"a\":" + std::to_string( rand_float<float>( ) ) + "},";
		} );
		result.back( ) = ']';
		result.resize( result.size( ) + 16U );
		/*
		if constexpr( NUMVALUES < 1500 ) {
		  std::cout << "Class data: " << result << '\n';
		}
		*/
		return result;
	}( );

	std::string json_data2 = [] {
		std::string result = "[";
		result.reserve( NUMVALUES * 23 + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += std::to_string( rand_float<float>( ) ) + ',';
		} );
		result.back( ) = ']';
		result.resize( result.size( ) + 16U );
		/*
		if constexpr( NUMVALUES < 1500 ) {
		  std::cout << "Class data: " << result << '\n';
		}
		*/
		return result;
	}( );

	// Normal
	{
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<Number>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );
		daw::do_not_optimize( count );
		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_class<no_name, Number>>;

		auto data = std::vector<Number>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy_n( iterator_t( sv ), NUMVALUES, daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';
	}
	{ // just ints
		auto json_sv = std::string_view( json_data2 );
		std::cout << "p2. Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<float>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_number<no_name, float>>;

		auto data = std::vector<float>( );
		data.resize( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "p2. float parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  std::copy_n( iterator_t( sv ), NUMVALUES, data.data( ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';
	}

	std::cout << "Checked\n";
	{ // Class of ints
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<Number>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_class<no_name, Number>>;

		auto data = std::vector<Number>( );
		data.resize( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  std::copy_n( iterator_t( sv ), NUMVALUES, data.data( ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';
	}
	{ // just ints
		auto json_sv = std::string_view( json_data2 );
		std::cout << "p2. Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<json_checked_number<no_name, float>>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_checked_number<no_name, float>>;

		auto data = std::vector<float>( );
		data.resize( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "p2. float parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  std::copy_n( iterator_t( sv ), NUMVALUES, data.data( ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';

		{
			auto data2 = std::unique_ptr<float[]>( new float[NUMVALUES] );
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p3. float parsing 3", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy_n( iterator_t( sv ), NUMVALUES, data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "element count 3: " << count3 << '\n';
		}
	}

	{
		std::cout << "double parsing\n**********************\n";
		using iterator_t = daw::json::json_array_iterator<double>;

		std::string json_data3 = [] {
			std::string result = "[";
			result.reserve( NUMVALUES * 23 + 8 );
			daw::algorithm::do_n( NUMVALUES, [&result] {
				result += std::to_string( rand_float<double>( ) ) + ',';
			} );
			result.back( ) = ']';
			// Allow for SSE to have room to look ahead
			result.resize( result.size( ) + 16U );
			return result;
		}( );
		auto json_sv3 = daw::string_view( json_data3.data( ), json_data3.size( ) );
		auto data2 = std::unique_ptr<double[]>( new double[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p4. parsing", json_sv3.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy_n( iterator_t( sv ), NUMVALUES, data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv3 );

			std::cout << "double parse count: " << count3 << '\n';
		}
	}
#ifdef DAW_ALLOW_SSE42
	// ***********************************************
	// SSE42
	std::cout << "**********************\nSSE42 Processing\n";
	{
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float sse3 parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<Number2, std::vector<Number2>,
			                    SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>(
			      sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );
		daw::do_not_optimize( count );
		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator<
		  json_class<no_name, Number2>,
		  SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>;

		auto data = std::vector<Number2>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float  sse3parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy( iterator_t( sv ), iterator_t( ),
			             daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';
	}
	{ // just floats
		auto json_sv = std::string_view( json_data2 );
		std::cout << "p2. Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float  sse3parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<json_number<no_name, float>, std::vector<float>,
			                    SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>(
			      sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator<
		  json_number<no_name, float>,
		  SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>;

		auto data = std::vector<float>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "p2. float sse3 parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy( iterator_t( sv ), iterator_t( ),
			             daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';
	}

	std::cout << "Checked\n";
	{ // Class of floats
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float sse3 parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<Number2, std::vector<Number2>,
			                    SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>(
			      sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator<
		  json_class<no_name, Number2>,
		  SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>;

		auto data = std::vector<Number2>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float sse3 parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy( iterator_t( sv ), iterator_t( ),
			             daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';
	}
	{ // just ints
		auto json_sv = std::string_view( json_data2 );
		std::cout << "p2. Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "float sse3 parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<json_checked_number<no_name, float>,
			                    std::vector<float>,
			                    SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>(
			      sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator<
		  json_checked_number<no_name, float>,
		  SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>;

		auto data = std::vector<float>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "p2. float sse3 parsing 2", json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy( iterator_t( sv ), iterator_t( ),
			             daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count 2: " << count2 << '\n';

		{
			auto data2 = std::unique_ptr<float[]>( new float[NUMVALUES] );
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p3. float sse3 parsing 3", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "element count 3: " << count3 << '\n';
		}
	}

	std::cout << "double sse3 parsing\n";
	{
		using iterator_t = daw::json::json_array_iterator<
		  json_number<no_name, double>,
		  SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>;

		std::string json_data3 = [] {
			std::string result = "[";
			result.reserve( NUMVALUES * 23 + 8 );
			daw::algorithm::do_n( NUMVALUES, [&result] {
				result += std::to_string( rand_float<double>( ) ) + ',';
			} );
			result.back( ) = ']';
			result.resize( result.size( ) + 16U );
			return result;
		}( );
		auto json_sv3 = daw::string_view( json_data3.data( ), json_data3.size( ) );
		auto data2 = std::unique_ptr<double[]>( new double[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p4. parsing", json_sv3.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv3 );

			std::cout << "double sse3 parse count: " << count3 << '\n';
		}
	}
#endif
}

int main( int argc, char ** )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc > 1 ) {
		test_func<1'000'000ULL>( );
	} else {
		test_func<1'000ULL>( );
	}
}
#ifdef DAW_USE_JSON_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
#endif
