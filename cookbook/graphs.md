# Graphs

JSON does not natively support graph storage. There are some formats, such as [JGF](https://github.com/jsongraph/json-graph-specification) JSON Graph Format, to provide a common encoding.

The following is a JGF graph encoded in JSON.
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


The code below uses a `json_array_range` to iterate over the nodes and edges separately to populate the internal graph.  The `"id"` members are an example of parsing strings as numbers with the `LiteralAsString` option.
The JSON document does not directly represent the in memory representation.
To see a working example using this code, refer to [cookbook_graphs1_test.cpp](../tests/cookbook_graphs1_test.cpp) 
```cpp
struct Metadata {
  int member0;
  std::string member1;
  bool member2;
};

struct GraphNode {
  size_t id;
  Metadata metadata;
};

struct GraphEdge {
  size_t source;
  size_t target;
};

namespace daw::json {
  template<>
  struct json_data_contract<Metadata> {
    using type = json_member_list<
      json_number<"member0", int>,
      json_string<"member1">, 
      json_bool<"member2">>;
  };

  template<>
  struct json_data_contract<GraphNode> {
    using type = json_member_list<
      json_number<"id", size_t, LiteralAsStringOpt::Always>,
      json_class<"metadata", Metadata>>;
  };

  template<>
  struct json_data_contract<GraphEdge> {
    using type = json_member_list<
      json_number<"source", size_t, LiteralAsStringOpt::Always>,
      json_number<"target", size_t, LiteralAsStringOpt::Always>>;
  };
} 

struct Node {
  size_t id;
  int member0;
  std::string member1;
  bool member2;
};

std::string_view json_sv = load_json_data( );

daw::graph_t<Node> g{};


using node_range_t = daw::json::json_array_range<GraphNode>;
// Create a json_array_range of GraphNode's and have it 
// iterate over the "nodes" JSON member
for( auto node : node_range_t( json_sv, "nodes" ) ) {
  g.add_node( node.id, node.metadata.member0, node.metadata.member1,
    node.metadata.member2 );
}

// Helper to lookup nodes we just added by their id member
auto const find_node_id = [&g]( size_t id ) -> std::optional<daw::node_id_t> {
  auto result =
    g.find( [id]( auto const &node ) { return node.value( ).id == id; } );
  if( result.empty( ) ) {
    return {};
  }
  return result.front( );
};

// Create a json_array_range of GraphEdge's and have it 
// iterate over the "edges" JSON member
using edge_range_t = daw::json::json_array_range<GraphEdge>;
for( auto edge : edge_range_t( json_sv, "edges" ) ) {
  auto source_id = *find_node_id( edge.source );
  auto target_id = *find_node_id( edge.target );
  // Add a directed edge from the source node id to the target node id
  g.add_directed_edge( source_id, target_id );
}
```