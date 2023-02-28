// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace impl {
	template<typename T, daw::json::json_options_t Options, typename Constructor>
	struct FromConverter {
		constexpr T operator( )( std::string_view sv ) {

			using namespace daw::json;
			using parse_to_t =
			  typename daw::json::json_number_null_no_name<T>::base_type;
			using member_type =
			  typename daw::json::json_number_null_no_name<T>::member_type;

			constexpr auto opts =
			  member_type::literal_as_string == options::LiteralAsStringOpt::Never
			    ? json_details::number_opts_set<Options,
			                                    options::LiteralAsStringOpt::Maybe>
			    : Options;

			using parse_type =
			  daw::json::json_number_no_name<parse_to_t, opts, Constructor>;

			auto jv = json_value( sv );
			if( jv.is_string( ) and jv.template as<std::string_view>( ).empty( ) ) {
				return { };
			}
			return from_json<parse_type>( jv );
		}
	};

	template<typename T>
	struct ToConverter {
		inline T operator( )( T const &value ) {
			return daw::json::to_json( value );
		}
	};
} // namespace impl

/**
 * The member is a nullable number
 * @tparam Name name of json member
 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
 * Constructor
 * @tparam LiteralAsString Could this number be embedded in a string
 * @tparam Constructor Callable used to construct result
 * @tparam RangeCheck Check if the value will fit in the result
 */
template<JSONNAMETYPE Name, typename T = std::optional<double>,
         daw::json::json_options_t Options = daw::json::number_opts_def,
         daw::json::JsonNullable NullableType =
           daw::json::JsonNullable::Nullable,
         typename Constructor = daw::use_default>
using json_empty_nullable_number = daw::json::json_custom_lit_null<
  Name, T, impl::FromConverter<T, Options, Constructor>, impl::ToConverter<T>,
  daw::json::json_details::json_custom_opts_set<
    Options, daw::json::json_custom_opts_def, NullableType>>;
//************************************

struct Thing {
	std::optional<int> value;
};

namespace daw::json {
	template<>
	struct json_data_contract<Thing> {
		static constexpr char const name[] = "value";
		using type =
		  json_member_list<json_empty_nullable_number<name, std::optional<int>>>;

		static constexpr auto to_json_data( Thing const &value ) {
			return std::forward_as_tuple( value.value );
		}
	};
} // namespace daw::json

int main( )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	constexpr std::string_view json_doc = R"json(
[
  { "value": "5" },
  { "value": "" },
  { "value": null },
  { "value": "6" }
]
)json";
	auto things = daw::json::from_json_array<Thing>( json_doc );
	ensure( things.size( ) == 4 );
	ensure( things[0].value );
	ensure( not things[1].value );
	ensure( not things[2].value );
	ensure( things[3].value );
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