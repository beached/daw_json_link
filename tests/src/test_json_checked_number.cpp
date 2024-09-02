// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <cstddef>
#include <exception>
#include <optional>
#include <tuple>

struct Byte {
	std::byte x;
};

namespace daw::json {
	template<>
	struct json_data_contract<Byte> {
		static constexpr char const x[] = "x";
		using type = json_member_list<json_checked_number<x, std::byte>>;

		static constexpr auto to_json_data( Byte const &v ) {
			return std::forward_as_tuple( v.x );
		}
	};
} // namespace daw::json

struct NullableByte {
	std::optional<std::byte> x;
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableByte> {
		static constexpr char const x[] = "x";
		using type =
		  json_member_list<json_checked_number_null<x, std::optional<std::byte>>>;

		static constexpr auto to_json_data( NullableByte const &v ) {
			return std::forward_as_tuple( v.x );
		}
	};
} // namespace daw::json

static_assert( daw::json::from_json<Byte>( R"json({"x":42})json" ).x ==
               std::byte{ 42 } );
static_assert( not daw::json::from_json<NullableByte>( R"json({})json" ).x );
static_assert( daw::json::from_json<NullableByte>( R"json({"x":42})json" ).x ==
               std::byte{ 42 } );

int main( ) {
#if defined( DAW_USE_EXCEPTIONS )
	bool success = false;
	try {
		(void)daw::json::from_json<Byte>( R"json({"x":1024})json" );
		success = false;
	} catch( std::exception const & ) { success = true; }
	try {
		(void)daw::json::from_json<NullableByte>( R"json({"x":1024})json" );
		success = false;
	} catch( std::exception const & ) { success = true; }
	daw_ensure( success );
#endif
}
