# Optional Values

null or optional values are supported by appending the type name with `_null`.  The requirement of the underlying type is that it will be default constructable.

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

Here we have an array of a class that has two members.  An optional int, a string, and an optional bool.

Too see a working example using this code, look at the [cookbook_optional_values1_test.cpp](../tests/cookbook_optional_values1_test.cpp) test in tests

The following C++ can provide a mapping

```cpp
struct MyOptionalStuff1 {
	std::optional<int> member0;
	std::string member1;
    std::unique_ptr<bool> member2;
};

auto describe_json_class( MyOptionalStuff1 const & ) {
	using namespace daw::json;
	return class_description_t<
        json_number_null<"member0", int>,
        json_string<"member1">,
        json_bool_null<"member2", 
            std::unique_ptr<bool>, 
            LiteralAsStringOpt::Never, 
            UniquePtrConstructor<bool>
        >>{};
}

auto to_json_data( MyOptionalStuff1 const &value ) {
	return std::forward_as_tuple( value.member0, value.member1, value.member2 );
}
```
