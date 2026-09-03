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
#include <optional>
#include <string>
#include <tuple>

namespace daw::cookbook_class4 {
	struct LinkedNode {
		int value;
		std::shared_ptr<LinkedNode> next;
	};

	struct LinkedNodeConstructor {
		LinkedNode operator( )( char const *ptr, std::size_t size ) const;
	};

	bool operator==( LinkedNode const &lhs, LinkedNode const &rhs ) {
		if( lhs.value != rhs.value ) {
			return false;
		}
		if( not lhs.next or not rhs.next ) {
			return not lhs.next and not rhs.next;
		}
		return *lhs.next == *rhs.next;
	}
} // namespace daw::cookbook_class4

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_class4::LinkedNode> {
		static constexpr char const value[] = "value";
		static constexpr char const next[] = "next";
		using type = json_member_list<
		  json_number<value, int>,
		  json_nullable<
		    next, std::shared_ptr<daw::cookbook_class4::LinkedNode>,
		    json_raw_no_name<daw::cookbook_class4::LinkedNode,
		                     daw::cookbook_class4::LinkedNodeConstructor>>>;

		static std::tuple<int, std::optional<std::string>>
		to_json_data( daw::cookbook_class4::LinkedNode const &node );
	};
} // namespace daw::json

namespace daw::cookbook_class4 {
	LinkedNode LinkedNodeConstructor::operator( )( char const *ptr,
	                                               std::size_t size ) const {
		return daw::json::from_json<LinkedNode>( std::string_view( ptr, size ) );
	}
} // namespace daw::cookbook_class4

std::tuple<int, std::optional<std::string>>
daw::json::json_data_contract<daw::cookbook_class4::LinkedNode>::to_json_data(
  daw::cookbook_class4::LinkedNode const &node ) {
	if( not node.next ) {
		return std::make_tuple( node.value, std::optional<std::string>{ } );
	}
	return std::make_tuple(
	  node.value, std::optional<std::string>{ daw::json::to_json( *node.next ) } );
}

int main( int argc, char **argv )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_class4.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = daw::read_file( argv[1] ).value( );
	auto const node =
	  daw::json::from_json<daw::cookbook_class4::LinkedNode>( data );

	test_assert( node.value == 1, "Unexpected root value" );
	test_assert( node.next and node.next->value == 2,
	             "Unexpected first nested value" );
	test_assert( node.next->next and node.next->next->value == 3,
	             "Unexpected second nested value" );
	test_assert( not node.next->next->next, "Expected the list to end" );

	auto const json = daw::json::to_json( node );
	auto const round_tripped =
	  daw::json::from_json<daw::cookbook_class4::LinkedNode>( json );
	test_assert( node == round_tripped, "Error in round tripping" );
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
