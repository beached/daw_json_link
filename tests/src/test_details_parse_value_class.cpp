// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>
#include <daw/json/impl/daw_json_parse_common.h>

#include <daw/daw_benchmark.h>

#include <iostream>
#include <optional>
#include <string_view>

#define do_test( ... )                                                   \
	try {                                                                  \
		daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ );              \
	} catch( daw::json::json_exception const &jex ) {                      \
		std::cerr << "Unexpected exception thrown by parser in test '"       \
		          << "" #__VA_ARGS__ << "': " << jex.reason( ) << std::endl; \
	}                                                                      \
	do {                                                                   \
	} while( false )

#define do_fail_test( ... )                                   \
	do {                                                        \
		try {                                                     \
			daw::expecting_message( __VA_ARGS__, "" #__VA_ARGS__ ); \
		} catch( daw::json::json_exception const & ) { break; }   \
		std::cerr << "Expected exception, but none thrown in '"   \
		          << "" #__VA_ARGS__ << "'\n";                    \
	} while( false )

namespace daw::json {
	template<>
	struct json_data_contract<daw::Empty> {
		using type = json_member_list<>;

		[[nodiscard]] static inline auto to_json_data( daw::Empty const & ) {
			return std::tuple<>( );
		}
	};
} // namespace daw::json

using namespace daw::json;
using namespace daw::json::json_details;

bool empty_class_empty_json_class( ) {
	std::string_view sv = "{}";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_class_no_name<daw::Empty>>(
	  rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

bool empty_class_nonempty_json_class( ) {
	std::string_view sv = R"({ "a": 12345, "b": {} })";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_class_no_name<daw::Empty>>(
	  rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

// putting member names/type alias into struct so that MSVC doesn't ICE
struct missing_members_fail_t {
	static constexpr char const member0[] = "member0";
	using class_t =
	  daw::json::tuple_json_mapping<daw::json::json_number<member0, unsigned>>;
};
bool missing_members_fail( ) {
	std::string_view sv = "{}";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );

	auto v = parse_value<json_class_no_name<missing_members_fail_t::class_t>>(
	  rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

struct wrong_member_type_fail_t {
	static constexpr char const member0[] = "member0";
	using class_t =
	  daw::json::tuple_json_mapping<daw::json::json_number<member0, unsigned>>;
};
bool wrong_member_type_fail( ) {
	std::string_view sv = R"({ "member0": "this isn't a number" })";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );

	auto v = parse_value<json_class_no_name<wrong_member_type_fail_t::class_t>>(
	  rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

struct wrong_member_number_type_fail_t {
	static constexpr char const member0[] = "member0";
	using class_t =
	  daw::json::tuple_json_mapping<daw::json::json_number<member0, unsigned>>;
};
bool wrong_member_number_type_fail( ) {
	std::string_view sv = R"({ "member0": -123 })";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );

	auto v =
	  parse_value<json_class_no_name<wrong_member_number_type_fail_t::class_t>>(
	    rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

struct unexpected_eof_in_class1_fail_t {
	static constexpr char const member0[] = "member0";
	using class_t = tuple_json_mapping<json_number<member0>>;
};
bool unexpected_eof_in_class1_fail( ) {
	std::string_view sv = R"({ "member0": 123 )";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );

	auto v = parse_value<json_class_no_name<unexpected_eof_in_class1_fail_t::class_t>>(
	  rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

struct wrong_member_stored_pos_fail_t {
	static constexpr char const member0[] = "member0";
	static constexpr char const member1[] = "member1";
	using class_t =
	  daw::json::tuple_json_mapping<daw::json::json_number<member0>,
	                                daw::json::json_number<member1>>;
};
bool wrong_member_stored_pos_fail( ) {
	std::string_view sv = R"({ "member1": 1, "member0": 2,)";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );

	auto v =
	  parse_value<json_class_no_name<wrong_member_stored_pos_fail_t::class_t>>(
	    rng, ParseTag<JsonParseTypes::Class>{ } );
	daw::do_not_optimize( v );
	return true;
}

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	do_test( empty_class_empty_json_class( ) );
	do_test( empty_class_nonempty_json_class( ) );
#if( not defined( _MSC_VER ) or __cpp_constexpr > 201700L ) or \
  defined( __clang__ )
#endif
	do_fail_test( missing_members_fail( ) );
	do_fail_test( wrong_member_type_fail( ) );
	do_fail_test( wrong_member_number_type_fail( ) );
	do_fail_test( unexpected_eof_in_class1_fail( ) );
	do_fail_test( wrong_member_stored_pos_fail( ) );
}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif
