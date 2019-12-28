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

Here we are going to use the array iterator tool to populate our graph so that the structure is easier to build in situ. In addition we are going to tell the system that the id members are just numbers wrapped in quotes with the LiteralAsString option.

```cpp
namespace daw::cookbook_graphs1 {
	struct Metadata {
		int member0;
		std::string member1;
		bool member2;
	};

	auto json_data_contract_for( Metadata const & ) {
		using namespace daw::json;
		return json_data_contract<json_number<"member0", int>,
		               json_string<"member1">, json_bool<"member2">>{};
	}

	struct GraphNode {
		size_t id;
		Metadata metadata;
	};

	auto json_data_contract_for( GraphNode const & ) {
		using namespace daw::json;
		return json_data_contract<
		  json_number<"id", size_t, LiteralAsStringOpt::Always>,
		  json_class<"metadata", Metadata>>{};
	}

	struct GraphEdge {
		size_t source;
		size_t target;
	};

	auto json_data_contract_for( GraphEdge const & ) {
		using namespace daw::json;
		return json_data_contract<
		  json_number<"source", size_t, LiteralAsStringOpt::Always>,
		  json_number<"target", size_t, LiteralAsStringOpt::Always>>{};
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
    return result.front( );
};

using edge_range_t = json_array_range<daw::cookbook_graphs1::GraphEdge>;
for( auto edge : edge_range_t( json_sv, "edges" ) ) {
    auto source_id = *find_node_id( edge.source );
    auto target_id = *find_node_id( edge.target );

    g.add_directed_edge( source_id, target_id );
}
```