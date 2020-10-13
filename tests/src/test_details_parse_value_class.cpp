// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_iterator_range.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

struct Empty {};

namespace daw::json {
	template<>
	struct json_data_contract<Empty> {
		using type = json_member_list<>;

		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( Empty const & ) {
			return std::tuple( );
		}
	};
} // namespace daw::json

bool empty_class_empty_json_class( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = "{}";
	daw::do_not_optimize( sv );
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_class<no_name, Empty>>(
	  ParseTag<JsonParseTypes::Class>{ }, rng );
	daw::do_not_optimize( v );
	return true;
}

bool empty_class_nonempty_json_class( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = R"({ "a": 12345, "b": {} })";
	daw::do_not_optimize( sv );
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_class<no_name, Empty>>(
	  ParseTag<JsonParseTypes::Class>{ }, rng );
	daw::do_not_optimize( v );
	return true;
}

bool missing_members_fail( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = "{}";
	daw::do_not_optimize( sv );
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	static constexpr char const member0[] = "member0";
	using class_t = tuple_json_mapping<json_number<member0, unsigned>>;
	auto v = parse_value<json_class<no_name, class_t>>(
	  ParseTag<JsonParseTypes::Class>{ }, rng );
	daw::do_not_optimize( v );
	return true;
}

bool wrong_member_type_fail( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = R"({ "member0": "this isn't a number" })";
	daw::do_not_optimize( sv );
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	static constexpr char const member0[] = "member0";
	using class_t = tuple_json_mapping<json_number<member0, unsigned>>;
	auto v = parse_value<json_class<no_name, class_t>>(
	  ParseTag<JsonParseTypes::Class>{ }, rng );
	daw::do_not_optimize( v );
	return true;
}

bool wrong_member_number_type_fail( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = R"({ "member0": -123 })";
	daw::do_not_optimize( sv );
	auto rng = DefaultParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	static constexpr char const member0[] = "member0";
	using class_t = tuple_json_mapping<json_number<member0, unsigned>>;
	auto v = parse_value<json_class<no_name, class_t>>(
	  ParseTag<JsonParseTypes::Class>{ }, rng );
	daw::do_not_optimize( v );
	return true;
}

#define do_test( ... )                                                         \
	try {                                                                        \
		daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );                    \
	} catch( daw::json::json_exception const &jex ) {                            \
		std::cerr << "Unexpected exception thrown by parser in test '"             \
		          << "" #__VA_ARGS__ << "': " << jex.reason( ) << std::endl;       \
	}                                                                            \
	do {                                                                         \
	} while( false )

#define do_fail_test( ... )                                                    \
	do {                                                                         \
		try {                                                                      \
			daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );                  \
		} catch( daw::json::json_exception const & ) { break; }                    \
		std::cerr << "Expected exception, but none thrown in '"                    \
		          << "" #__VA_ARGS__ << "'\n";                                     \
	} while( false )

int main( int, char ** )
#ifdef DAW_USE_JSON_EXCEPTIONS
try
#endif
{
	do_test( empty_class_empty_json_class( ) );
	do_test( empty_class_nonempty_json_class( ) );
	do_fail_test( missing_members_fail( ) );
	do_fail_test( wrong_member_type_fail( ) );
	do_fail_test( wrong_member_number_type_fail( ) );
} catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
