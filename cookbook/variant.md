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

Too see a working example using this code, look at the [cookbook_variant1_test.cpp](../tests/cookbook_variant1_test.cpp) test in tests

The following C++ can provide a mapping

```cpp
struct MyVariantStuff1 {
  std::variant<int, std::string> member0;
  std::variant<std::string, bool> member1;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyVariantStuff1> {
    using type = json_member_list<
      json_variant<"member0", std::variant<int, std::string>,
        json_variant_type_list<
          json_number<no_name, int>,
          json_string<no_name>>>,
      json_variant<
        "member1", std::variant<std::string, bool>,
        json_variant_type_list<json_string<no_name>, json_bool<no_name>>>>;

    static inline auto
    to_json_data( MyVariantStuff1 const &value ) {
      return std::forward_as_tuple( value.member0, value.member1 );
    }
  };
} 
```

## Important note:
The elements in the json_variant_type_list must have matching types in the variant alternatives. (e.g. std::string -> json_string, bool -> json_bool )
