// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>
#include <unordered_map>

#include <daw/daw_benchmark.h>
#include <daw/daw_graph.h>
#include <daw/daw_memory_mapped_file.h>
#include <daw/daw_optional.h>

#include "daw/json/daw_json_iterator.h"
#include "daw/json/daw_json_link.h"

namespace daw::cookbook_graphs1 {
	struct Metadata {
		int member0;
		std::string member1;
		bool member2;
	};

#ifdef __cpp_nontype_template_parameter_class
	auto describe_json_class( Metadata const & ) {
		using namespace daw::json;
		return class_description_t<json_number<"member0", int>,
		                           json_string<"member1">, json_bool<"member2">>{};
	}
#else
	namespace symbols_Metadata {
		static inline constexpr char const member0[] = "member0";
		static inline constexpr char const member1[] = "member1";
		static inline constexpr char const member2[] = "member2";
	} // namespace symbols_Metadata

	auto describe_json_class( Metadata const & ) {
		using namespace daw::json;
		return class_description_t<json_number<symbols_Metadata::member0, int>,
		                           json_string<symbols_Metadata::member1>,
		                           json_bool<symbols_Metadata::member2>>{};
	}
#endif

	struct GraphNode {
		size_t id;
		Metadata metadata;
	};

#ifdef __cpp_nontype_template_parameter_class
	auto describe_json_class( GraphNode const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_number<"id", size_t, LiteralAsStringOpt::always>,
		  json_class<"metadata", Metadata>>{};
	}
#else
	namespace symbols_GraphNode {
		static inline constexpr char const id[] = "id";
		static inline constexpr char const metadata[] = "metadata";
	} // namespace symbols_GraphNode
	auto describe_json_class( GraphNode const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_number<symbols_GraphNode::id, size_t, LiteralAsStringOpt::always>,
		  json_class<symbols_GraphNode::metadata, Metadata>>{};
	}
#endif

	struct GraphEdge {
		size_t source;
		size_t target;
	};

#ifdef __cpp_nontype_template_parameter_class
	auto describe_json_class( GraphEdge const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_number<"source", size_t, LiteralAsStringOpt::always>,
		  json_number<"target", size_t, LiteralAsStringOpt::always>>{};
	}
#else
	namespace symbols_GraphEdge {
		static inline constexpr char const source[] = "source";
		static inline constexpr char const target[] = "target";
	} // namespace symbols_GraphEdge
	auto describe_json_class( GraphEdge const & ) {
		using namespace daw::json;
		return class_description_t<json_number<symbols_GraphEdge::source, size_t,
		                                       LiteralAsStringOpt::always>,
		                           json_number<symbols_GraphEdge::target, size_t,
		                                       LiteralAsStringOpt::always>>{};
	}
#endif
} // namespace daw::cookbook_graphs1

struct Node {
	size_t id;
	int member0;
	std::string member1;
	bool member2;
};

int main( int argc, char **argv ) {
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_graphs1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = daw::filesystem::memory_mapped_file_t<>( argv[1] );
	auto json_sv = std::string_view( data.data( ), data.size( ) );

	daw::graph_t<Node> g{};
	using namespace daw::json;

	using node_range_t = json_array_range<daw::cookbook_graphs1::GraphNode>;
	for( auto node : node_range_t( json_sv, "nodes" ) ) {
		g.add_node( node.id, node.metadata.member0, node.metadata.member1,
		            node.metadata.member2 );
	}

	auto const find_node_id = [&g]( size_t id ) -> std::optional<daw::node_id_t> {
		auto result =
		  g.find( [id]( auto const &node ) { return node.value( ).id == id; } );
		if( result.empty( ) ) {
			return {};
		}
		daw::expecting( result.size( ) == 1 );
		return result.front( );
	};
	auto const find_node = [&]( size_t id ) {
		auto result = find_node_id( id );
		daw_json_assert( result, "Expected a result" );
		return g.get_node( *result );
	};

	using edge_range_t = json_array_range<daw::cookbook_graphs1::GraphEdge>;
	for( auto edge : edge_range_t( json_sv, "edges" ) ) {
		auto source_id = *find_node_id( edge.source );
		auto target_id = *find_node_id( edge.target );

		g.add_directed_edge( source_id, target_id );
	}
	daw::do_not_optimize( g );

	daw_json_assert( g.size( ) == 3U, "Expected graph to have 3 nodes" );
	auto nid_0 = find_node_id( 0 );
	daw_json_assert( nid_0, "Expecting a node with id 0" );
	auto nid_1 = find_node_id( 1 );
	daw_json_assert( nid_1, "Expecting a node with id 1" );
	auto nid_2 = find_node_id( 2 );
	daw_json_assert( nid_2, "Expecting a node with id 2" );

	auto const &n0 = find_node( 0 );

	daw_json_assert( n0.outgoing_edges( ).size( ) == 1U,
	                 "Node id 0 should only have 1 outgoing edge" );
	daw_json_assert(
	  n0.outgoing_edges( ).count( *nid_1 ) == 1U,
	  "Node id 0 should only have 2 incoming edge from node id 1" );

	daw_json_assert( n0.incoming_edges( ).size( ) == 2U,
	                 "Node id 0 should 2 incoming edges" );
	daw_json_assert( n0.incoming_edges( ).count( *nid_1 ) == 1U,
	                 "Node id 0 should have 1 incoming edge from node id 1" );

	daw_json_assert( n0.incoming_edges( ).count( *nid_2 ) == 1U,
	                 "Node id 0 should have 1 incoming edge from node id 2" );

	return 0;
}
