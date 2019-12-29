// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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
	intmax_t a{};
};
#ifdef __cpp_nontype_template_parameter_class
[[maybe_unused]] static constexpr auto
json_data_contract_for( Number ) noexcept {
	using namespace daw::json;
	return json_data_contract<json_number<"a", intmax_t>>{};
}
#else
namespace symbols_Number {
	static inline constexpr char const a[] = "a";
}

[[maybe_unused]] static constexpr auto
json_data_contract_for( Number ) noexcept {
	using namespace daw::json;
	return json_data_contract<json_number<symbols_Number::a, intmax_t>>{};
}
#endif

#ifndef NDEBUG
static constexpr size_t const NUMVALUES = 1'000ULL;
#else
static constexpr size_t const NUMVALUES = 1'000'000ULL;
#endif

template<size_t N, typename T>
static std::string_view make_int_array_data( ) {
	static std::string const json_data = [] {
		std::string result = "[";
		result.reserve( N * 23 + 8 );
		for( size_t n = 0; n < N; ++n ) {
			result +=
			  std::to_string( daw::randint<T>( std::numeric_limits<T>::min( ),
			                                   std::numeric_limits<T>::max( ) ) ) +
			  ',';
		}
		result.back( ) = ']';
		// Allow SSE Modes to have enough room
		result += "                                                     ";
		result.shrink_to_fit( );
		return result;
	}( );
	return {json_data.data( ), json_data.size( )};
}

int main( ) {
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
			            std::numeric_limits<intmax_t>::min( ),
			            std::numeric_limits<intmax_t>::max( ) ) ) +
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
		auto const count = *daw::bench_n_test_mbs<100>(
		  "array of class with single intmax_t element: from_json_array",
		  json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array_trusted<Number>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );
		daw::do_not_optimize( count );
		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator_trusted<Number>;

		auto data = std::vector<Number>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<100>(
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
		auto const count = *daw::bench_n_test_mbs<100>(
		  "array of intmax_t: from_json_array", json_sv_intmax.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array_trusted<intmax_t>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv_intmax );

		std::cout << "element count: " << count << '\n';
		using iterator_t = daw::json::json_array_iterator_trusted<intmax_t>;

		auto data = std::vector<intmax_t>( );
		data.resize( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<100>(
		  "array of intmax_t: json_array_iterator copy to presized vector",
		  json_sv_intmax.size( ),
		  [&]( std::string_view sv ) noexcept {
			  std::copy( iterator_t( sv ), iterator_t( ), data.data( ) );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv_intmax );

		std::cout << "element count: " << count2 << '\n';
	}

	std::cout << "Checked\n";
	{ // Class of ints
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<100>(
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

		auto const count2 = *daw::bench_n_test_mbs<100>(
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
		auto const count = *daw::bench_n_test_mbs<100>(
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

		auto const count2 = *daw::bench_n_test_mbs<100>(
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
			auto const count3 = *daw::bench_n_test_mbs<100>(
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
		using iterator_t = daw::json::json_array_iterator_trusted<uintmax_t>;

		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		auto data2 = std::unique_ptr<intmax_t[]>( new intmax_t[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<100>(
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
			auto const count4 = *daw::bench_n_test_mbs<100>(
			  "p5. parsing", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array_trusted<
				    int_type, daw::bounded_vector_t<int_type, NUMVALUES>>( sv );

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
			auto const count3 = *daw::bench_n_test_mbs<100>(
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
			auto const count4 = *daw::bench_n_test_mbs<100>(
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
	{
		// Unsigned SSE2
		using uint_type = json_number_sse2<no_name, uintmax_t>;
		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		{
			auto const count4 = *daw::bench_n_test_mbs<100>(
			  "p5. parsing sse2", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array_trusted<
				    uint_type, daw::bounded_vector_t<uintmax_t, NUMVALUES>>( sv );

				  daw::do_not_optimize( result );
				  return result.size( );
			  },
			  json_sv );

			std::cout << "unsigned sse2 parse count: " << count4 << '\n';
		}
	}
	std::cout << "Checked unsigned sse2\n";
	{
		// Unsigned SSE2
		using uint_type = json_number_sse2<no_name, uintmax_t>;
		using iterator_t = daw::json::json_array_iterator<uint_type>;

		auto const json_sv = make_int_array_data<NUMVALUES, uintmax_t>( );

		auto data2 = std::unique_ptr<uintmax_t[]>( new uintmax_t[NUMVALUES] );
		{
			auto const count3 = *daw::bench_n_test_mbs<100>(
			  "p4. parsing sse2", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "unsigned sse2 parse count: " << count3 << '\n';
		}
	}
	{
		// Unsigned SSE2
		using uint_type = json_number_sse2<no_name, uint32_t>;
		auto const json_sv = make_int_array_data<NUMVALUES, uint32_t>( );

		{
			auto const count4 = *daw::bench_n_test_mbs<100>(
			  "p5. parsing sse2", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto result = daw::json::from_json_array<
				    uint_type, daw::bounded_vector_t<uint32_t, NUMVALUES>>( sv );

				  daw::do_not_optimize( result );
				  return result.size( );
			  },
			  json_sv );

			std::cout << "unsigned sse2 parse count: " << count4 << '\n';
		}
	}
}
