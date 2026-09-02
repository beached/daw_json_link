# json_nullable types

nullable types are supported by using the `json_nullable` mapping type or appending the type name with `_null` to use the convenience wrapper. The requirement of the underlying type is that it will be default constructable for null values or support the [Nullable Concept](nullable_value_concept.md).

Named `_null` mappings accept a `JsonNullable` template argument that controls
how an empty value is serialized when it is a class member:

- `JsonNullable::Nullable` (the default) permits the entire member to be omitted.
- `JsonNullable::NullVisible` always emits the member, using `null` for an empty
  value.

## Custom null tests

When a nullable mapping is serialized, its null state is detected in this
order:

1. If an `IsNull` type was specified, a default-constructed `IsNull` is called
   with the mapped value.
2. Otherwise, if the value can be converted to `bool`, `false` means null.
3. Otherwise, `nullable_value_traits<T>::has_value` is used for types satisfying
   the [Nullable Concept](nullable_value_concept.md).

The optional `IsNull` template argument is useful when neither of the default
tests represents the type's null state. It is the last argument of both
mappings:

```c++
json_nullable<
  Name, T, JsonMember, NullableType, Constructor, IsNull
>

json_nullable_no_name<
  T, JsonMember, Constructor, IsNull
>
```

For example, a `std::variant` can reserve `std::monostate` as its null state:

```c++
using Value = std::variant<std::monostate, int, std::string>;

struct IsVariantNull {
  constexpr bool operator( )( Value const &value ) const {
    return value.index( ) == 0;
  }
};

struct MyValue {
  Value member;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyValue> {
    using type = json_member_list<json_nullable<
      "member",
      Value,
      json_variant_no_name<Value>,
      JsonNullable::NullVisible,
      daw::use_default,
      IsVariantNull
    >>;

    static auto to_json_data( MyValue const &value ) {
      return std::forward_as_tuple( value.member );
    }
  };
}
```

Here, a variant holding `std::monostate` serializes as `{"member":null}`.
`IsNull` controls null detection during serialization; it does not construct a
value when parsing JSON `null`. Parsing continues to use `Constructor`, and
`daw::use_default` creates a default-constructed variant containing
`std::monostate`.

For a named class member with a custom null test, use
`JsonNullable::NullVisible` as above so the null member is emitted. For an
unnamed value, such as an array element or root value,
`json_nullable_no_name` always emits the JSON literal `null` for a value that
passes its null test.

For complete named and unnamed mappings, see
[json_nullable_is_null_test.cpp](../../tests/src/json_nullable_is_null_test.cpp).

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

To see a working example using this code, refer to [cookbook_optional_values1_test.cpp](../../tests/src/cookbook_optional_values1_test.cpp).

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
        options::bool_opt( options::LiteralAsStringOpt::Never ),
        JsonNullable::Nullable,
        UniquePtrConstructor<bool>
      >
    >;

    static inline auto to_json_data( MyOptionalStuff1 const &value ) {
      return std::forward_as_tuple( value.member0, value.member1, value.member2 );
    }
  };
}
```
