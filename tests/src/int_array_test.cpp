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
#include <daw/iterator/daw_back_inserter.h>

#include <fstream>
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
	intmax_t a{ };
};
namespace daw::json {
	template<>
	struct json_data_contract<Number> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"a", intmax_t>>;
#else
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<a, intmax_t>>;
#endif
	};
} // namespace daw::json

template<size_t N, typename T>
static std::vector<T> const &make_int_array( ) {
	static std::vector<T> const data = [] {
		auto result = std::vector<T>( );
		result.reserve( N );
		for( size_t n = 0; n < N; ++n ) {
			result.push_back( daw::randint<T>( daw::numeric_limits<T>::min( ),
			                                   daw::numeric_limits<T>::max( ) ) );
		}
		return result;
	}( );
	return data;
}

template<size_t N, typename T>
static std::string_view make_int_array_data( ) {
	static std::string const json_data = [] {
		std::string result = "[";
		result.reserve( N * 23 + 8 );
		for( size_t n = 0; n < N; ++n ) {
			result +=
			  std::to_string( daw::randint<T>( daw::numeric_limits<T>::min( ),
			                                   daw::numeric_limits<T>::max( ) ) ) +
			  ',';
		}
		result.back( ) = ']';
		// result.shrink_to_fit( );
		// Allow SSE Modes to have enough room
		result.resize( result.size( ) + 256U );
		return result;
	}( );
	return { json_data.data( ), json_data.size( ) };
}

template<size_t NUMVALUES>
void test_func( ) {

	using namespace daw::json;
	auto const json_sv_intmax = make_int_array_data<NUMVALUES, intmax_t>( );

	std::string const json_data = [] {
		std::string result = "[";

		// 23 is what I calculated as the string size of the serialized class.
		// It may be incorrect but that is ok, it is close and should reduce
		// allocations
		result.reserve( NUMVALUES * 23 + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += "{\"a\":" +
			          std::to_string( daw::randint<intmax_t>(
			            daw::numeric_limits<intmax_t>::min( ),
			            daw::numeric_limits<intmax_t>::max( ) ) ) +
			          "},";
		} );
		result.back( ) = ']';
		return result;
	}( );

	std::cout << "Unchecked\n";
	{ // Class of ints
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "array of class with single intmax_t element: from_json_array",
		  json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<Number, std::vector<Number>,
			                    NoCommentSkippingPolicyUnchecked>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );
		daw::do_not_optimize( count );
		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<Number, NoCommentSkippingPolicyUnchecked>;

		auto data = std::vector<Number>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "array of class with single intmax_t element: json_array_iterator",
		  json_sv.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy( iterator_t( sv ), iterator_t( ),
			             daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count2 << '\n';
	}
	{ // just ints
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "array of intmax_t: from_json_array", json_sv_intmax.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<intmax_t, std::vector<intmax_t>,
			                    NoCommentSkippingPolicyUnchecked>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv_intmax );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<intmax_t,
		                                 NoCommentSkippingPolicyUnchecked>;

		auto data = std::vector<intmax_t>( );
		data.resize( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "array of intmax_t: json_array_iterator copy to presized vector",
		  json_sv_intmax.size( ),
		  [&]( std::string_view sv ) noexcept {
			  std::copy( iterator_t( sv ), iterator_t( ), data.data( ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv_intmax );

		std::cout << "element count: " << count2 << '\n';

		{
			auto to_json_str = std::string( );
			to_json_str.resize(
			  static_cast<std::size_t>( ( json_sv_intmax.size( ) * 15 ) ) / 10 );
			auto result = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "array of intmax_t: to_json presized", data.size( ) * sizeof( data[0] ),
			  [&]( std::vector<intmax_t> const &v ) noexcept {
				  daw::json::to_json_array( v, to_json_str.data( ) );
				  daw::do_not_optimize( to_json_str.data( ) );
			  },
			  data );

			std::cout << "element count: " << data.size( ) << '\n';
		}
		{
			auto to_json_str = std::string( );
			to_json_str.resize( make_int_array_data<NUMVALUES, int32_t>( ).size( ) *
			                    2 );
			auto result = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "array of int32_t: to_json presized", ( NUMVALUES * sizeof( int32_t ) ),
			  [&]( std::vector<int32_t> const &v ) noexcept {
				  daw::json::to_json_array( v, to_json_str.data( ) );
				  daw::do_not_optimize( to_json_str.data( ) );
			  },
			  make_int_array<NUMVALUES, int32_t>( ) );

			std::cout << "element count: " << NUMVALUES << '\n';
		}
		{
			auto to_json_str = std::string( );
			to_json_str.resize( make_int_array_data<NUMVALUES, uint32_t>( ).size( ) *
			                    2 );
			auto result = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "array of uint32_t: to_json presized",
			  ( NUMVALUES * sizeof( uint32_t ) ),
			  [&]( std::vector<uint32_t> const &v ) noexcept {
				  daw::json::to_json_array( v, to_json_str.data( ) );
				  daw::do_not_optimize( to_json_str.data( ) );
			  },
			  make_int_array<NUMVALUES, uint32_t>( ) );

			std::cout << "element count: " << NUMVALUES << '\n';
		}
		{
			auto to_json_str = std::string( );
			to_json_str.resize( make_int_array_data<NUMVALUES, uintmax_t>( ).size( ) *
			                    2 );
			auto result = daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "array of uintmax_t: to_json presized",
			  ( NUMVALUES * sizeof( uintmax_t ) ),
			  [&]( std::vector<uintmax_t> const &v ) noexcept {
				  daw::json::to_json_array( v, to_json_str.data( ) );
				  daw::do_not_optimize( to_json_str.data( ) );
			  },
			  make_int_array<NUMVALUES, uintmax_t>( ) );

			std::cout << "element count: " << NUMVALUES << '\n';
		}
	}

	std::cout << "Checked\n";
	{ // Class of ints
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "int parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<Number>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator<Number>;

		auto data = std::vector<Number>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "int parsing 2", json_sv.size( ),
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
		std::cout << "p2. Processing " << json_sv_intmax.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv_intmax.size( ) )
		          << '\n';
		auto const count = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "int parsing 1", json_sv_intmax.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<json_checked_number<no_name, intmax_t>>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv_intmax );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_checked_number<no_name, intmax_t>>;

		auto data = std::vector<intmax_t>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
		  "p2. int parsing 2", json_sv_intmax.size( ),
		  [&]( auto &&sv ) noexcept {
			  data.clear( );
			  std::copy( iterator_t( sv ), iterator_t( ),
			             daw::back_inserter( data ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv_intmax );

		std::cout << "element count 2: " << count2 << '\n';

		{
			auto data2 = std::unique_ptr<intmax_t[]>( new intmax_t[NUMVALUES] );
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p3. int parsing 3", json_sv_intmax.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv_intmax );

			std::cout << "element count 3: " << count3 << '\n';
		}
	}

	std::cout << "Unchecked unsigned\n";
	{
		// Unsigned
		using iterator_t =
		  daw::json::json_array_iterator<uintmax_t,
		                                 NoCommentSkippingPolicyUnchecked>;

		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		auto data2 = std::unique_ptr<intmax_t[]>( new intmax_t[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p4. parsing", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "unsigned parse count: " << count3 << '\n';
		}
	}
	{
		// Unsigned
		using int_type = uint32_t;
		auto const json_sv = make_int_array_data<NUMVALUES, int_type>( );

		{
			auto const count4 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p5. parsing", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array<
				    int_type, daw::bounded_vector_t<int_type, NUMVALUES>,
				    NoCommentSkippingPolicyUnchecked>( sv );

				  daw::do_not_optimize( result );
				  return result.size( );
			  },
			  json_sv );

			std::cout << "unsigned parse count: " << count4 << '\n';
		}
	}
	std::cout << "Checked unsigned\n";
	{
		// Unsigned
		using iterator_t = daw::json::json_array_iterator<uintmax_t>;

		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		auto data2 = std::unique_ptr<intmax_t[]>( new intmax_t[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p4. parsing", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "unsigned parse count: " << count3 << '\n';
		}
	}
	{
		// Unsigned
		using int_type = uint32_t;
		auto const json_sv = make_int_array_data<NUMVALUES, int_type>( );

		{
			auto const count4 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p5. parsing", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array<
				    int_type, daw::bounded_vector_t<int_type, NUMVALUES>>( sv );

				  daw::do_not_optimize( result );
				  return result.size( );
			  },
			  json_sv );

			std::cout << "unsigned parse count: " << count4 << '\n';
		}
	}

#ifdef DAW_ALLOW_SSE42
	{
		// Unsigned SSE42
		using uint_type = json_number<no_name, uintmax_t>;
		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		{
			auto const count4 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p5. parsing sse3", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array<
				    uint_type, daw::bounded_vector_t<uintmax_t, NUMVALUES>,
				    SIMDNoCommentSkippingPolicyUnchecked<sse42_exec_tag>>( sv );

				  daw::do_not_optimize( result );
				  return result.size( );
			  },
			  json_sv );

			std::cout << "unsigned sse3 parse count: " << count4 << '\n';
		}
	}
	std::cout << "Checked unsigned sse3\n";
	{
		// Unsigned SSE42
		using uint_type = json_number<no_name, uintmax_t>;
		using iterator_t = daw::json::json_array_iterator<
		  uint_type, SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>;

		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		auto data2 = std::unique_ptr<uintmax_t[]>( new uintmax_t[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p4. parsing sse3", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "unsigned sse3 parse count: " << count3 << '\n';
		}
	}
	{
		// Unsigned SSE42
		using uint_type = json_number<no_name, uint32_t>;
		auto const json_sv = make_int_array_data<NUMVALUES, uint32_t>( );

		{
			auto const count4 = *daw::bench_n_test_mbs<DAW_NUM_RUNS>(
			  "p5. parsing sse3", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array<
				    uint_type, daw::bounded_vector_t<uint32_t, NUMVALUES>,
				    SIMDNoCommentSkippingPolicyChecked<sse42_exec_tag>>( sv );

				  daw::do_not_optimize( result );
				  return result.size( );
			  },
			  json_sv );

			std::cout << "unsigned sse3 parse count: " << count4 << '\n';
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
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
