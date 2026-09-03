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
#include <string>
#include <tuple>
#include <vector>

namespace daw::cookbook_array4 {
	struct TreeNode {
		std::string name;
		std::vector<TreeNode> children;
	};

	struct TreeChildrenConstructor {
		std::vector<TreeNode> operator( )( char const *ptr, std::size_t size ) const;
	};

	bool operator==( TreeNode const &lhs, TreeNode const &rhs ) {
		return lhs.name == rhs.name and lhs.children == rhs.children;
	}
} // namespace daw::cookbook_array4

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_array4::TreeNode> {
		static constexpr char const name[] = "name";
		static constexpr char const children[] = "children";
		using type = json_member_list<
		  json_string<name>,
		  json_raw<children, std::vector<daw::cookbook_array4::TreeNode>,
		           daw::cookbook_array4::TreeChildrenConstructor>>;

		static std::tuple<std::string, std::string>
		to_json_data( daw::cookbook_array4::TreeNode const &node );
	};
} // namespace daw::json

namespace daw::cookbook_array4 {
	std::vector<TreeNode>
	TreeChildrenConstructor::operator( )( char const *ptr,
	                                      std::size_t size ) const {
		return daw::json::from_json_array<TreeNode>( std::string_view( ptr, size ) );
	}
} // namespace daw::cookbook_array4

std::tuple<std::string, std::string>
daw::json::json_data_contract<daw::cookbook_array4::TreeNode>::to_json_data(
  daw::cookbook_array4::TreeNode const &node ) {
	return std::make_tuple( node.name, daw::json::to_json_array( node.children ) );
}

int main( int argc, char **argv )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_array4.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto const data = daw::read_file( argv[1] ).value( );
	auto const tree =
	  daw::json::from_json<daw::cookbook_array4::TreeNode>( data );

	test_assert( tree.name == "root", "Unexpected root name" );
	test_assert( tree.children.size( ) == 2, "Unexpected root child count" );
	test_assert( tree.children[0].name == "left", "Unexpected child name" );
	test_assert( tree.children[0].children.empty( ),
	             "Expected a leaf node" );
	test_assert( tree.children[1].children.size( ) == 1,
	             "Unexpected nested child count" );
	test_assert( tree.children[1].children[0].name == "right.left",
	             "Unexpected nested child name" );

	auto const json = daw::json::to_json( tree );
	auto const round_tripped =
	  daw::json::from_json<daw::cookbook_array4::TreeNode>( json );
	test_assert( tree == round_tripped, "Error in round tripping" );
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
