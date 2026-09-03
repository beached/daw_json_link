# Array's

Parsing arrays is common and it is a fundamental data structure in JSON. It can be both the root object or a member.

## Simple Array of int

```json
[
  1,
  2,
  3,
  4,
  5
]
```

The above JSON document contains an array of integers as the root object.

The C++ to parse this could look like

```c++
std::vector<int> vec = from_json_array<int>( json_str );
```

To see a working example of the following, refer to [cookbook_array1_test.cpp](../../tests/src/cookbook_array1_test.cpp)
The previous is a simple example, had the element type of the array been more complicated, such as a class, a data mapping would be required.

## Array of a class

```json
[
  {
    "a": "Hello World",
    "b": 1234,
    "c": 1.23,
    "d": false
  },
  {
    "a": "Goodbye World",
    "b": 4321,
    "c": 123,
    "d": true
  }
]
```

Here we hae a JSON array containing a class with members of type string, unsigned, float, and boolean.

The C++ data structures and the mapping could look like the following
To see a working example using this code, refer to [cookbook_array2_test.cpp](../../tests/src/cookbook_array2_test.cpp)

```c++
struct MyClass4 {
  std::string a;
  unsigned b;
  float c;
  bool d;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass4> {
  using type = json_member_list<
    json_string<"a">, 
    json_number<"b", unsigned>,
    json_number<"c", float>, 
    json_bool<"d">
  >;

  static inline auto to_json_data( MyClass4 const &value ) {
    return std::forward_as_tuple( 
      value.a, 
      value.b, 
      value.c, 
      value.d );
    }
  };
} 

std::vector<MyClass4> v = from_json_array<MyClass4>( str );
```

The above would construct MyClass4 with arguments of types `std::string, unsigned, float, bool`

## Array's as members

Use the `json_array` member type in the member list to describe a member that is an array type.

To see a working example using this code, refer to [cookbook_array3_test.cpp](../../tests/src/cookbook_array3_test.cpp)

```json
{
  "member0": 5,
  "member1": [
    1,
    2,
    3,
    4,
    5
  ],
  "member2": [
    "hello",
    "world"
  ]
}
```

The above JSON document, has an object root, with int, array of int, and an array of string members

The C++ data structures could look like the following

```c++
struct MyArrayClass1 {
  int member0;
  std::vector<int> member1;
  std::vector<std::string> member2;
};
```

The `json_data_contract` specialization as follows

```c++
namespace daw::json {
  template<>
  struct json_data_contract<MyArrayClass1> {
    using type = json_member_list<
      json_number<"member0", int>,
      json_array<"member1", int>,
      json_array<"member2", std::string>>;

    static inline auto
    to_json_data( MyArrayClass1 const &value ) {
      return std::forward_as_tuple( 
        value.member0, 
        value.member1,
        value.member2 );
    }
  };
}
```

## Recursive arrays

Arrays often provide the recursive edge in an n-ary tree:

```json
{
  "name": "root",
  "children": [
    {
      "name": "left",
      "children": []
    },
    {
      "name": "right",
      "children": [
        {
          "name": "right.left",
          "children": []
        }
      ]
    }
  ]
}
```

To see a working example, including serialization and round-trip parsing, refer
to [cookbook_array4_test.cpp](../../tests/src/cookbook_array4_test.cpp).

A direct `json_array<"children", TreeNode>` mapping would require the
`TreeNode` contract while that contract is still being defined. Mapping the
array as raw JSON delays its parsing until the contract is complete. The custom
constructor then parses the captured value with `from_json_array`.

```c++
struct TreeNode {
  std::string name;
  std::vector<TreeNode> children;
};

struct TreeChildrenConstructor {
  std::vector<TreeNode>
  operator()( char const *ptr, std::size_t size ) const;
};

namespace daw::json {
  template<>
  struct json_data_contract<TreeNode> {
    using type = json_member_list<
      json_string<"name">,
      json_raw<
        "children", std::vector<TreeNode>, TreeChildrenConstructor>
    >;

    static std::tuple<std::string, std::string>
    to_json_data( TreeNode const &node );
  };
}

std::vector<TreeNode>
TreeChildrenConstructor::operator()( char const *ptr,
                                     std::size_t size ) const {
  return daw::json::from_json_array<TreeNode>(
    std::string_view( ptr, size ) );
}

std::tuple<std::string, std::string>
daw::json::json_data_contract<TreeNode>::to_json_data(
  TreeNode const &node ) {
  return {
    node.name,
    daw::json::to_json_array( node.children )
  };
}
```

The serialized children array is returned as complete JSON text because a
`json_raw` member writes its value verbatim. As with any nested JSON
representation, the structure must be acyclic.

### Pointer like arrays

For dealing with pointer like arrays(T *, has element_type type alias) see [int_ptr_test](../../tests/src/int_ptr_test.cpp)
