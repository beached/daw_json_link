# Variant Types 

Variant or sum types are where the json member can have more than one type(like string, number, class, or array).

Take the following JSON
```json
[
  {
    "member0": 5,
    "member1": "hello"
  },
  {
    "member0": "world",
    "member1": true
  }
]
```

Here we have an array of a class that has two members.  A variant of types number and string, member0; and a variant of type string and bool, member1.

Too see a working example using this code, refer to [cookbook_variant1_test.cpp](../tests/cookbook_variant1_test.cpp) 

The following C++ can provide a mapping.  It, also, highlights that the types bool, integers, floating point, std::string, and previously mapped types can be default mapped to elements that do not require a name.  Such as variant, array, and some key_value's.

```c++
struct MyVariantStuff1 {
  std::variant<int, std::string> member0;
  std::variant<std::string, bool> member1;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyVariantStuff1> {
    using type = json_member_list<
      json_variant<"member0", std::variant<int, std::string>,
        json_variant_type_list<int,
          json_string<no_name>>>,
      json_variant<
        "member1", std::variant<std::string, bool>,
        json_variant_type_list<std::string, json_bool<no_name>>>>;

    static inline auto
    to_json_data( MyVariantStuff1 const &value ) {
      return std::forward_as_tuple( value.member0, value.member1 );
    }
  };
} 
```

## Important note:
The elements in the `json_variant_type_list` must have matching types in the variant alternatives. (e.g. std::string -> json_string, bool -> json_bool )

## Tagged Variants
It is common to have a tag discriminator in JSON data.  The `json_tagged_variant` member type allows using another parsed member to return an index in a member list to parse.  This allows any number of types to be inside the variant.

Below is a JSON array, containing a variant element where the `"type"` member determines the type of the `"value"` member.  In many JSON documents, the discriminator will be a string.
```json
[
  {
    "type": 0,
    "name": "string value",
    "value": "hello"
  },
  {
    "type": 1,
    "name": "int value",
    "value": 5
  },
  {
    "type": 2,
    "name": "bool value",
    "value": false
  }
]
```

A member name and a callable are needed to tell the parser which type will parsed.

Too see a working example using this code, refer to [cookbook_variant2_test.cpp](../tests/cookbook_variant2_test.cpp) 
```c++
struct MyClass {
  std::string name;
  std::variant<std::string, int, bool> value;  
};

struct MyClassSwitcher {
  // Convert JSON tag member to type index
  constexpr size_t operator( )( int type ) const {
    return (size_t)type;
  }     
  // Get value for Tag from class value
  int operator( )( MyClass const & v ) const {
    return (int)v.index( );
  }
};

template<>
struct daw::json::json_data_contract<MyClass> {
  using type = json_member_list<
    json_string<"name">,
    json_tagged_variant<
      "value", 
      std::variant<std::string, int, bool>,
      json_number<"type", int>,
      MyClassSwitcher,
      json_tagged_variant_type_list<std::string, int, bool>>
    >;

  static constexpr inline to_json_data( MyClass const & v ) {
    return std::forward_as_tuple( v.name, v.value );
  }
};
```

In the above example, two members are mapped to construct MyClass, `"name"` and `"value"`.  The variant uses the JSON `"type"` member to determine the index of the parser to use for the variant value.
