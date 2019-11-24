# Graphs

```json
{
  "nodes": [
    {
      "id": "0",
      "metadata": {
        "member0": 44,
        "member1": "value1",
        "member2": true
      }
    },
    {
      "id": "1",
      "metadata": {
        "member0": 2323,
        "member1": "value4",
        "member2": false
      }
    },
    {
      "id": "2",
      "metadata": {
        "member0": 1234,
        "member1": "value7",
        "member2": false
      }
    }
  ],
  "edges": [
    {
      "source": "0",
      "target": "1"
    },
    {
      "source": "1",
      "target": "0"
    },
    {
      "source": "2",
      "target": "0"
    },
    {
      "source": "1",
      "target": "2"
    }
  ]
}

```

Here we have a graph in [JGF](https://github.com/jsongraph/json-graph-specification) JSON Graph Format.

Here we are going to use the array iterator tool to populate our graph so that the structure is easier to build in situ.

```cpp
namespace daw::cookbook_graphs1 {
	struct Metadata {
		int member0;
		std::string member1;
		bool member2;
	};

	auto describe_json_class( Metadata const & ) {
		using namespace daw::json;
		return class_description_t<json_number<"member0", int>,
		               json_string<"member1">, json_bool<"member2">>{};
	}

	struct GraphNode {
		size_t id;
		Metadata metadata;
	};

	auto describe_json_class( GraphNode const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_number<"id", size_t, LiteralAsStringOpt::always>,
		  json_class<"metadata", Metadata>>{};
	}

	struct GraphEdge {
		size_t source;
		size_t target;
	};

	auto describe_json_class( GraphEdge const & ) {
		using namespace daw::json;
		return class_description_t<
		  json_number<"source", size_t, LiteralAsStringOpt::always>,
		  json_number<"target", size_t, LiteralAsStringOpt::always>>{};
	}
} // namespace daw::cookbook_graphs1

struct Node {
	size_t id;
	int member0;
	std::string member1;
	bool member2;
};

using namespace daw::json;
json_sv = ...;

Graph<Node> g{};
using node_iterator_t = daw::json::json_array_iterator<
  json_class<no_name, daw::cookbook_graphs1::GraphNode>>;

auto const last_node = node_iterator_t( );
for( auto it = node_iterator_t( json_sv, "nodes" );
   it != last; 
   ++it ) {
  auto node = *it;  // json_array_iterator will parse on operator*
  g.add_node( node.id, node.metadata.member0, node.metadata.member1, node.metadata.member2 ) );  
}

auto find_node_id = [&g]( size_t id ) {
  return g.find( [id]( auto const &node ) { 
    return node.value( ).id == id; 
  } ).front( );
};

using edge_iterator_t =
  json_array_iterator<json_class<no_name, daw::cookbook_graphs1::GraphEdge>>;
auto const last_edge = edge_iterator_t( );
for( auto it = edge_iterator_t( json_sv, "edges" );
   it != last; 
   ++it ) {
  auto edge = *it;
  g.add_directed_edge( find_node_id( edge.source ), find_node_id( edge.target ) );  
}
```