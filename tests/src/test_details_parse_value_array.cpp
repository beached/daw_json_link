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

bool empty_array_empty_json_array( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	DAW_CONSTEXPR std::string_view sv = "[]";
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_array_no_name<int>>(
	  rng, ParseTag<JsonParseTypes::Array>{ } );
	return v.empty( );
}

bool int_array_json_string_array_fail( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = R"([ "this is strange" ])";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_array_no_name<int>>(
	  rng, ParseTag<JsonParseTypes::Array>{ } );
	daw::do_not_optimize( v );
	return true;
}

template<typename... Members>
struct InlineClass {
	std::tuple<typename Members::parse_to_t...> members;

	template<typename... Ts>
	requires( not daw::traits::is_first_type_v<InlineClass, Ts...> and
	          ( std::is_convertible_v<Ts, Members> and ... ) ) //
	inline DAW_CONSTEXPR InlineClass( Ts &&...values )
	  : members{ std::forward<Ts>( values )... } {}
};

namespace daw::json {
	template<typename... Members>
	struct json_data_contract<InlineClass<Members...>> {
		using type = json_member_list<Members...>;
	};

	template<typename... Members>
	[[nodiscard]] static inline auto const &
	to_json_data( InlineClass<Members...> const &value ) {
		return value.members;
	}
} // namespace daw::json

bool array_with_closing_class_fail( ) {
	using namespace daw::json;
	using namespace daw::json::json_details;

	std::string_view sv = R"([ {}}, ])";
	daw::do_not_optimize( sv );
	auto rng = BasicParsePolicy( sv.data( ), sv.data( ) + sv.size( ) );
	auto v = parse_value<json_array_no_name<InlineClass<>>>(
	  rng, ParseTag<JsonParseTypes::Array>{ } );
	daw::do_not_optimize( v );
	return true;
}

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

int main( int, char ** )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	do_test( empty_array_empty_json_array( ) );
	do_fail_test( int_array_json_string_array_fail( ) );
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