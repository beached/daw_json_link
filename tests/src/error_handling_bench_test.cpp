// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw_json_benchmark.h"
#include "defines.h"

#include <daw/daw_random.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <chrono>
#include <csetjmp>

int main( ) {
	auto const numbers = daw::make_random_data<int>( 1024 );
	auto const numbers_str_good = daw::json::to_json( numbers );
	auto const numbers_str_bad = [&numbers_str_good] {
		auto result = numbers_str_good;
		result.back( ) = 'a'; //[512] = 'a';
		return result;
	}( );
	static constexpr std::size_t total = 7919;
	using time_type = std::chrono::time_point<std::chrono::steady_clock,
	                                       std::chrono::nanoseconds>;
	{
		auto total_time = std::chrono::nanoseconds{ };
		auto start_time = std::chrono::steady_clock::now( );
		daw::json::daw_json_error_handler_data = &start_time;
		daw::json::daw_json_error_handler =
		  +[]( daw::json::json_exception &&jex, void *st ) {
			  *reinterpret_cast<time_type *>( st ) = std::chrono::steady_clock::now( );
			  throw std::move( jex );
		  };
		for( std::size_t n = 0; n < total; ++n ) {
			daw::do_not_optimize( total );
			daw::do_not_optimize( numbers_str_bad );
			[&]( daw::string_view json_doc ) {
				try {
					std::intmax_t sum = 0;
					for( int num : daw::json::json_array_range<int>( json_doc ) ) {
						sum += num;
					}
					daw::do_not_optimize( sum );
				} catch( daw::json::json_exception const & ) {
					auto const end_time = std::chrono::steady_clock::now( );
					total_time += end_time - start_time;
				}
			}( numbers_str_bad );
		}
		total_time /= total;
		std::cout << "Average Exception Time: "
		          << daw::json::benchmark::ns_to_string( total_time, 2 ) << '\n';
	}

	struct state_t {
		time_type start_time;
		std::jmp_buf jmp_buff;
		std::optional<daw::json::json_exception> jex;
	};
	auto state = state_t{ };
	auto total_time = std::chrono::nanoseconds{ };
	daw::json::daw_json_error_handler_data = &state;

	daw::json::daw_json_error_handler =
	  +[]( daw::json::json_exception &&jex, void *st ) {
		  auto &s = *reinterpret_cast<state_t *>( st );
		  s.start_time = std::chrono::steady_clock::now( );
		  s.jex = std::move( jex );
		  std::longjmp( s.jmp_buff, true );
	  };
	for( std::size_t n = 0; n < total; ++n ) {
		daw::do_not_optimize( total );
		state.jex = std::nullopt;
		daw::do_not_optimize( numbers_str_bad );
		[&]( daw::string_view json_doc ) {
			if( not setjmp( state.jmp_buff ) ) {
				std::intmax_t sum = 0;
				for( int num : daw::json::json_array_range<int>( json_doc ) ) {
					sum += num;
				}
				daw::do_not_optimize( sum );
			}
			auto const end_time = std::chrono::steady_clock::now( );
			total_time += end_time - state.start_time;
			ensure( state.jex );
		}( numbers_str_bad );
	}
	total_time /= total;
	std::cout << "Average lngjmp Time: "
	          << daw::json::benchmark::ns_to_string( total_time, 2 ) << '\n';
}
