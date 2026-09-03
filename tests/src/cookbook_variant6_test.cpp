// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_read_file.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace daw::cookbook_variant6 {
	struct Variant {
		using value_t = std::variant<int, bool, std::shared_ptr<Variant>>;

		value_t value;
	};

	struct VariantValueConstructor {
		Variant::value_t operator( )( char const *ptr, std::size_t size ) const;
	};

	bool operator==( Variant const &lhs, Variant const &rhs ) {
		if( lhs.value.index( ) != rhs.value.index( ) ) {
			return false;
		}
		switch( lhs.value.index( ) ) {
		case 0:
			return std::get<int>( lhs.value ) == std::get<int>( rhs.value );
		case 1:
			return std::get<bool>( lhs.value ) == std::get<bool>( rhs.value );
		case 2: {
			auto const &lhs_ptr = std::get<std::shared_ptr<Variant>>( lhs.value );
			auto const &rhs_ptr = std::get<std::shared_ptr<Variant>>( rhs.value );
			if( not lhs_ptr or not rhs_ptr ) {
				return not lhs_ptr and not rhs_ptr;
			}
			return *lhs_ptr == *rhs_ptr;
		}
		default:
			std::abort( );
		}
	}
} // namespace daw::cookbook_variant6

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_variant6::Variant> {
		static constexpr char const value[] = "value";
		using type = json_member_list<
		  json_raw<value, daw::cookbook_variant6::Variant::value_t,
		           daw::cookbook_variant6::VariantValueConstructor>>;

		static std::tuple<std::string>
		to_json_data( daw::cookbook_variant6::Variant const &v );
	};
} // namespace daw::json

namespace daw::cookbook_variant6 {
	Variant::value_t VariantValueConstructor::operator( )( char const *ptr,
	                                                       std::size_t size ) const {
		auto const raw_json = std::string_view( ptr, size );
		auto const value = daw::json::json_value( raw_json );
		switch( value.type( ) ) {
		case daw::json::JsonBaseParseTypes::Number:
			return daw::json::from_json<int>( value );
		case daw::json::JsonBaseParseTypes::Bool:
			return daw::json::from_json<bool>( value );
		case daw::json::JsonBaseParseTypes::Class:
			return std::make_shared<Variant>(
			  daw::json::from_json<Variant>( raw_json ) );
		case daw::json::JsonBaseParseTypes::String:
		case daw::json::JsonBaseParseTypes::Array:
		case daw::json::JsonBaseParseTypes::Null:
		case daw::json::JsonBaseParseTypes::None:
			std::abort( );
		}
	}
} // namespace daw::cookbook_variant6

std::tuple<std::string>
daw::json::json_data_contract<daw::cookbook_variant6::Variant>::to_json_data(
  daw::cookbook_variant6::Variant const &v ) {
	auto raw_json = std::visit(
	  []( auto const &item ) -> std::string {
		  using item_t = daw::remove_cvref_t<decltype( item )>;
		  if constexpr( std::is_same_v<
		                  item_t,
		                  std::shared_ptr<daw::cookbook_variant6::Variant>> ) {
			  return item ? daw::json::to_json( *item ) : "null";
		  } else {
			  return daw::json::to_json( item );
		  }
	  },
	  v.value );
	return std::make_tuple( std::move( raw_json ) );
}

int main( int argc, char **argv )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_variant6.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = daw::read_file( argv[1] ).value( );
	auto const values =
	  daw::json::from_json_array<daw::cookbook_variant6::Variant>( data );

	test_assert( values.size( ) == 4, "Unexpected size" );
	test_assert( std::get<int>( values[0].value ) == 5, "Unexpected integer" );
	test_assert( std::get<bool>( values[1].value ), "Unexpected boolean" );

	auto const &nested =
	  std::get<std::shared_ptr<daw::cookbook_variant6::Variant>>(
	    values[2].value );
	test_assert( nested and not std::get<bool>( nested->value ),
	             "Unexpected nested boolean" );

	auto const &nested_twice =
	  std::get<std::shared_ptr<daw::cookbook_variant6::Variant>>(
	    values[3].value );
	test_assert( nested_twice, "Unexpected null nested value" );
	auto const &nested_integer =
	  std::get<std::shared_ptr<daw::cookbook_variant6::Variant>>(
	    nested_twice->value );
	test_assert( nested_integer and std::get<int>( nested_integer->value ) == 42,
	             "Unexpected nested integer" );

	auto const json = daw::json::to_json_array( values );
	auto const round_tripped =
	  daw::json::from_json_array<daw::cookbook_variant6::Variant>( json );
	test_assert( values == round_tripped, "Error in round tripping" );
}
#if defined( DAW_USE_EXCEPTIONS )
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
