// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <iostream>
#include <string>
#include <variant>

#include <daw/json/daw_json_link.h>

struct MonostateNullVariantChecker {
	template<typename... Ts>
	constexpr bool operator( )( std::variant<Ts...> const &value ) const {
		return std::holds_alternative<std::monostate>( value );
	}
};

using MyVariant = std::variant<std::monostate, bool, float>;

struct VariableDescription {
	std::string name;
	MyVariant variant_field = { };
};

namespace daw::json {
	template<>
	struct json_data_contract<VariableDescription> {
		static constexpr char const name[] = "name";
		static constexpr char const variant_field[] = "variant_field";
		using type = json_member_list<
		  json_string<name>,
		  json_nullable<variant_field, MyVariant, json_variant_no_name<MyVariant>,
		                JsonNullable::Nullable, // NOTE HERE
		                daw::use_default, MonostateNullVariantChecker>>;

		static inline auto to_json_data( VariableDescription const &value ) {
			return std::forward_as_tuple( value.name, value.variant_field );
		}
	};
} // namespace daw::json

int main( ) {
	VariableDescription desc = {
	  "saymyname",
	  { },
	};

	constexpr std::string_view expected = R"json({"name":"saymyname"})json";
	std::string output = daw::json::to_json( desc );
	daw_ensure( output == expected );
}
