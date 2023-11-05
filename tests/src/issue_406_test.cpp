// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include <daw/json/daw_json_link.h>

#include <variant>

namespace daw::json {
	template<>
	struct json_data_contract<std::monostate> {
		using type = json_member_list<>;

		static constexpr std::tuple<> to_json_data( std::monostate ) noexcept {
			return { };
		}
	};
} // namespace daw::json

struct SomeEmpty {};

int main( ) {
	using type = daw::json::json_variant_null_no_name<
	  std::variant<std::monostate, double, std::string_view>>;
	auto a = daw::json::from_json<type>( R"("hello")" );
	ensure( a.index( ) == 2 );

	using type2 = daw::json::json_variant_null_no_name<
	  std::variant<SomeEmpty, double, std::string_view>>;
	auto a2 = daw::json::from_json<type2>( R"("hello")" );
	ensure( a2.index( ) == 2 );
	auto b2 = daw::json::from_json<type2>( "null" );
	ensure( b2.index( ) == 0 );
}
