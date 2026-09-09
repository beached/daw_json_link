// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_event_parser.h>
#include <daw/json/daw_json_link.h>

#include <vector>

namespace {
	// Returns SkipClassArray on the value `2`: events for the rest of that
	// array's elements must not fire, but the array's own end event still
	// must, and parsing must resume normally with the next sibling member.
	struct SkipArrayHandler {
		std::vector<double> seen_numbers{};
		int array_ends = 0;
		int class_ends = 0;

		daw::json::json_parse_handler_result handle_on_number( double d ) {
			seen_numbers.push_back( d );
			if( d == 2.0 ) {
				return daw::json::json_parse_handler_result::SkipClassArray;
			}
			return daw::json::json_parse_handler_result::Continue;
		}
		daw::json::json_parse_handler_result handle_on_array_end( ) {
			++array_ends;
			return daw::json::json_parse_handler_result::Continue;
		}
		daw::json::json_parse_handler_result handle_on_class_end( ) {
			++class_ends;
			return daw::json::json_parse_handler_result::Continue;
		}
	};

	// Returns SkipClassArray on the first top-level member: the rest of the
	// root object's own members must be skipped, but its class-end event
	// still must fire.
	struct SkipRootHandler {
		int numbers_seen = 0;
		int class_ends = 0;

		daw::json::json_parse_handler_result handle_on_number( double ) {
			++numbers_seen;
			return daw::json::json_parse_handler_result::SkipClassArray;
		}
		daw::json::json_parse_handler_result handle_on_class_end( ) {
			++class_ends;
			return daw::json::json_parse_handler_result::Continue;
		}
	};

	// Returns Complete at the top level: parsing stops cleanly with no error.
	struct CompleteAtRootHandler {
		bool called = false;

		daw::json::json_parse_handler_result handle_on_number( double ) {
			called = true;
			return daw::json::json_parse_handler_result::Complete;
		}
	};

	// Returns Complete from a number two levels deep, inside an open array
	// inside the root object: parsing must stop cleanly with no error, even
	// though the array and the root object are never actually closed.
	struct CompleteNestedHandler {
		std::vector<double> seen_numbers{};

		daw::json::json_parse_handler_result handle_on_number( double d ) {
			seen_numbers.push_back( d );
			if( d == 5.0 ) {
				return daw::json::json_parse_handler_result::Complete;
			}
			return daw::json::json_parse_handler_result::Continue;
		}
	};
} // namespace

int main( ) {
	{
		SkipArrayHandler h;
		daw::json::json_event_parser( R"({"a":[1,2,3],"b":[4,5,6],"c":7})", h );
		std::vector<double> const expected{ 1, 2, 4, 5, 6, 7 };
		daw_ensure( h.seen_numbers == expected );
		daw_ensure( h.array_ends == 2 );
		daw_ensure( h.class_ends == 1 );
	}

	{
		SkipRootHandler h;
		daw::json::json_event_parser( R"({"a":1,"b":2,"c":3})", h );
		daw_ensure( h.numbers_seen == 1 );
		daw_ensure( h.class_ends == 1 );
	}

	{
		CompleteAtRootHandler h;
		daw::json::json_event_parser( "42", h );
		daw_ensure( h.called );
	}

	{
		CompleteNestedHandler h;
		daw::json::json_event_parser( R"({"a":[1,2,3],"b":[4,5,6],"c":7})", h );
		std::vector<double> const expected{ 1, 2, 3, 4, 5 };
		daw_ensure( h.seen_numbers == expected );
	}
}
