# Optional Values

null or optional values are supported by appending the type name with `_null`.  The requirement of the underlying type is that it will be default constructable for null values.

Take the following JSON
```json
[
  {
    "member0": 5,
    "member1": "hello"
  },
  {
    "member1": "world",
    "member2": true
  }
]
```

The JSON document is an array of an object with two members, an optional int, a string, and an optional bool.

Too see a working example using this code, refer to [cookbook_optional_values1_test.cpp](../tests/src/cookbook_optional_values1_test.cpp). 

```c++
struct MyOptionalStuff1 {
  std::optional<int> member0;
  std::string member1;
  std::unique_ptr<bool> member2;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyOptionalStuff1> {
    using type = json_member_list<
      json_number_null<"member0", std::optional<int>>, 
      json_string<"member1">,
      json_bool_null<
        "member2", 
        std::unique_ptr<bool>, 
        LiteralAsStringOpt::NotBeforeDblQuote, 
        UniquePtrConstructor<bool>
      >
    >;

    static inline auto to_json_data( MyOptionalStuff1 const &value ) {
      return std::forward_as_tuple( value.member0, value.member1, value.member2 );
    }
  };
}
```
