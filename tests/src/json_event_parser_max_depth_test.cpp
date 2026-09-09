// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_event_parser.h>
#include <daw/json/daw_json_link.h>

#if defined( DAW_USE_EXCEPTIONS )
#include <cstddef>
#include <string>

namespace {
	struct null_handler {};

	[[nodiscard]] std::string nested_array_doc( std::size_t depth ) {
		std::string doc;
		doc.reserve( depth * 2 + 1 );
		for( std::size_t n = 0; n < depth; ++n ) {
			doc += '[';
		}
		doc += '1';
		for( std::size_t n = 0; n < depth; ++n ) {
			doc += ']';
		}
		return doc;
	}
} // namespace

int main( ) {
	constexpr std::size_t depth = 50;
	auto const doc = nested_array_doc( depth );

	// Default MaxDepth is unlimited: deeply nested input parses fine.
	{
		null_handler h;
		daw::json::json_event_parser( doc, h );
	}

	// A limit below the actual nesting depth throws MaxDepthExceeded.
	{
		null_handler h;
		bool threw_expected_reason = false;
		try {
			daw::json::json_event_parser<daw::use_default, 10>( doc, h );
		} catch( daw::json::json_exception const &e ) {
			threw_expected_reason =
			  e.reason_type( ) == daw::json::ErrorReason::MaxDepthExceeded;
		}
		daw_ensure( threw_expected_reason );
	}

	// A limit exactly matching the actual nesting depth still succeeds.
	{
		null_handler h;
		daw::json::json_event_parser<daw::use_default, depth>( doc, h );
	}
}
#else
int main( ) {}
#endif
