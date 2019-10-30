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

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string_view>
#include <vector>

#include <daw/daw_benchmark.h>
#include <daw/daw_do_n.h>
#include <daw/daw_random.h>
#include <daw/iterator/daw_back_inserter.h>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

struct Number {
	intmax_t a{};
};
#ifdef __cpp_nontype_template_parameter_class
[[maybe_unused]] static constexpr auto describe_json_class( Number ) noexcept {
	using namespace daw::json;
	return class_description_t<json_number<"a", intmax_t>>{};
}
#else
namespace symbols_Number {
	static inline constexpr char const a[] = "a";
}

[[maybe_unused]] static constexpr auto describe_json_class( Number ) noexcept {
	using namespace daw::json;
	return class_description_t<json_number<symbols_Number::a, intmax_t>>{};
}
#endif

#ifndef NDEBUG
static constexpr size_t const NUMVALUES = 1'000ULL;
#else
static constexpr size_t const NUMVALUES = 1'000'000ULL;
#endif
int main( ) {
	using namespace daw::json;
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

	std::string json_data2 = [] {
		std::string result = "[";
		result.reserve( NUMVALUES * 23 + 8 );
		daw::algorithm::do_n( NUMVALUES, [&result] {
			result += std::to_string( daw::randint<intmax_t>(
			            std::numeric_limits<intmax_t>::min( ),
			            std::numeric_limits<intmax_t>::max( ) ) ) +
			          ',';
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
		  "int parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<json_class<no_name, Number>>( sv );
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
		auto json_sv = std::string_view( json_data2 );
		std::cout << "p2. Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<100>(
		  "int parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<json_number<no_name, intmax_t>>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_number<no_name, intmax_t>>;

		auto data = std::vector<intmax_t>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<100>(
		  "p2. int parsing 2", json_sv.size( ),
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
	{ // Class of ints
		auto json_sv = std::string_view( json_data );
		std::cout << "Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<100>(
		  "int parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data = from_json_array<json_class<no_name, Number>>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_class<no_name, Number>>;

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
		auto json_sv = std::string_view( json_data2 );
		std::cout << "p2. Processing " << json_sv.size( ) << " bytes "
		          << daw::utility::to_bytes_per_second( json_sv.size( ) ) << '\n';
		auto const count = *daw::bench_n_test_mbs<100>(
		  "int parsing 1", json_sv.size( ),
		  []( auto &&sv ) noexcept {
			  auto const data =
			    from_json_array<json_checked_number<no_name, intmax_t>>( sv );
			  daw::do_not_optimize( data );
			  return data.size( );
		  },
		  json_sv );

		std::cout << "element count: " << count << '\n';
		using iterator_t =
		  daw::json::json_array_iterator<json_checked_number<no_name, intmax_t>>;

		auto data = std::vector<intmax_t>( );
		data.reserve( NUMVALUES );

		auto const count2 = *daw::bench_n_test_mbs<100>(
		  "p2. int parsing 2", json_sv.size( ),
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
			auto data2 = std::unique_ptr<intmax_t[]>( new intmax_t[NUMVALUES] );
			auto const count3 = *daw::bench_n_test_mbs<100>(
			  "p3. int parsing 3", json_sv.size( ),
			  [&]( auto &&sv ) noexcept {
				  auto ptr = std::copy( iterator_t( sv ), iterator_t( ), data2.get( ) );
				  daw::do_not_optimize( data2 );
				  return ptr - data2.get( );
			  },
			  json_sv );

			std::cout << "element count 3: " << count3 << '\n';
		}
	}

	{
		// Unsigned
		using iterator_t =
		  daw::json::json_array_iterator<json_number<no_name, uintmax_t>>;

		std::string json_data3 = [] {
			std::string result = "[";
			result.reserve( NUMVALUES * 23 + 8 );
			daw::algorithm::do_n( NUMVALUES, [&result] {
				result += std::to_string( daw::randint<uintmax_t>(
				            std::numeric_limits<uintmax_t>::min( ),
				            std::numeric_limits<uintmax_t>::max( ) ) ) +
				          ',';
			} );
			result.back( ) = ']';
			return result;
		}( );

		daw::string_view json_sv{json_data3.data( ), json_data3.size( )};
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
}
