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

## Sized arrays

Use `json_sized_array` when an array's container needs a size supplied by
another member of the same JSON object. This is useful for containers such as
`std::unique_ptr<T[]>` that do not store their size.

```json
{
  "size": 3,
  "values": [1, 2, 3]
}
```

The size mapping is passed to `json_sized_array` as its third template
argument. Its constructor receives the parsed element range followed by that
size. To see a working example, refer to
[test_json_sized_array.cpp](../../tests/src/test_json_sized_array.cpp).

```c++
#include <daw/daw_span.h>
#include <daw/json/daw_json_link.h>

#include <memory>
#include <stdexcept>

struct Stuff {
  std::size_t size;
  std::unique_ptr<int[]> values;
};

template<typename T>
struct UniquePtrArrayCtor {
  template<typename Iterator>
  std::unique_ptr<T[]> operator()( Iterator first, Iterator last,
                                   std::size_t size ) const {
    if( size > 1024 ) {
      throw std::length_error( "array is too large" );
    }

    auto result = std::make_unique<T[]>( size );
    std::size_t count = 0;
    while( first != last ) {
      if( count == size ) {
        throw std::length_error( "array size does not match size member" );
      }
      result[count++] = *first;
      ++first;
    }
    if( count != size ) {
      throw std::length_error( "array size does not match size member" );
    }
    return result;
  }
};

namespace daw::json {
  template<>
  struct json_data_contract<Stuff> {
    using size_member = json_number<"size", std::size_t>;
    using type = json_member_list<
      size_member,
      json_sized_array<"values", int, size_member,
                       std::unique_ptr<int[]>, UniquePtrArrayCtor<int>>
    >;

    static auto to_json_data( Stuff const &value ) {
      return std::tuple{
        value.size,
        daw::span<int const>( value.values.get(), value.size )
      };
    }
  };
}
```

The size member must be a named, non-nullable mapping in the same contract.
Because input can request an arbitrarily large allocation or provide a size
that disagrees with the array, custom constructors should enforce suitable
resource limits and validate the number of parsed elements before returning.
For serialization, expose pointer-like storage as a sized range such as
`daw::span`; a pointer alone does not provide an end iterator.

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
`TreeNode` contract while that contract is still being defined. The
`json_recursive_class_no_name` element mapping exposes the JSON object type
without eagerly instantiating that contract. When parsing reaches a child, the
outer contract is complete and can be used normally.

```c++
struct TreeNode {
  std::string name;
  std::vector<TreeNode> children;
};

namespace daw::json {
  template<>
  struct json_data_contract<TreeNode> {
    using type = json_member_list<
      json_string<"name">,
      json_array<
        "children",
        json_recursive_class_no_name<TreeNode>,
        std::vector<TreeNode>
      >
    >;

    static auto to_json_data( TreeNode const &node ) {
      return std::forward_as_tuple( node.name, node.children );
    }
  };
}
```

The empty children array supplies the recursion's base case, so no nullable or
pointer wrapper is required. Parsing and serialization use the ordinary
`std::vector<TreeNode>` value. As with any nested JSON representation, the
structure must be acyclic.

### Pointer like arrays

For dealing with pointer like arrays(T *, has element_type type alias) see [int_ptr_test](../../tests/src/int_ptr_test.cpp)

## Serializing C arrays and strings with to_json_array

`to_json_array` writes its argument as a JSON array. C arrays are accepted
along with containers. A `char` array is treated as a `std::string_view` of
length `N - 1`, so each character is written as a one character string.
Pointers have no length and are a compile error; pass a container or a
`daw::span` instead.

Given

```c++
char buffer[] = "Hi\"";
int ints[3] = { 1, 2, 3 };
std::vector<char const *> ptrs{ "A\"da", nullptr };
```

| Expression                                                   | Output             |
|--------------------------------------------------------------|--------------------|
| `to_json_array( "Hi\"" )`                                    | `["H","i","\""]`   |
| `to_json_array( buffer )`                                    | `["H","i","\""]`   |
| `to_json_array( "Hi\""sv )`                                  | `["H","i","\""]`   |
| `to_json_array( ints )`                                      | `[1,2,3]`          |
| `to_json_array( std::vector<std::string_view>{ "A\"da" } )`  | `["A\"da"]`        |
| `to_json_array( ptrs )`                                      | `[65,null]`        |
| `to_json_array( ptr )` with `char const *ptr`                | compile error: pointers have no length |

As with `to_json`, character pointers inside a container are deduced as a
nullable `char`. To write them as strings, give the element mapping explicitly:

```c++
using char_ptr_string = daw::json::json_nullable_no_name<
  char const *, daw::json::json_string_no_name<daw::string_view>>;

to_json_array<char_ptr_string>( ptrs ); // ["A\"da",null]
```

See [Containers of character pointers](strings.md#containers-of-character-pointers).
