// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_writer.h"

#include <daw/daw_ensure.h>

#include <string>

int main( ) {
		auto out = std::string{ };
		auto w = daw::json::json_writer( out );
		using json_fp_t = daw::json::json_number_no_name<
			double,
			daw::json::options::number_opt(
				daw::json::options::FPOutputFormat::Decimal )>;
		w.write_number<json_fp_t>( 10.0 );
		w.finalize( );
		daw_ensure( out == "10.0" );
}

