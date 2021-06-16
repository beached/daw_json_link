# Enums

There is no direct support for enum's in JSON.  However, they are often encoded as a number or a string.

## As StringRaw
```json
{
  "member0": [ "red", "green", "blue", "black" ]
}
```
The above JSON object has a member, `"member0"`, that is an array of strings.
In order to store the enum as a JSON string, an overload for `to_string` and `from_string` are required. `to_string` is required to return a `Container` where the `begin( )` iterator's `value_type` is `char, not necessarily a `std::string`.

To see a working example using this code, refer to [cookbook_enums1_test.cpp](../tests/src/cookbook_enums1_test.cpp)
```c++
enum class Colours : uint8_t { red, green, blue, black };

constexpr std::string_view to_string( Colours c ) {
  switch( c ) {
    case Colours::red: return "red";
    case Colours::green: return "green";
    case Colours::blue: return "blue";
    case Colours::black: return "black";
  }
}

constexpr Colours from_string( daw::tag_t<Colours>, std::string_view sv ) {
  if( sv == "red" ) {
    return Colours::red;
  }
  if( sv == "green" ) {
    return Colours::green;
  }
  if( sv == "blue" ) {
    return Colours::blue;
  }
  if( sv == "black" ) {
    return Colours::black;
  }
}

struct MyClass1 {
  std::vector<Colours> member0;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass1> {
    using type = json_member_list<
      json_array<"member0", json_custom<no_name, Colours>>
    >;

    static inline auto
    to_json_data( MyClass1 const &value ) {
      return std::forward_as_tuple( value.member0 );
    }
  };
}
```

## As Number

Enums can, also, be stored as numbers.  The underlying value is used.

```json
{
  "member0": [ 0, 1, 2, 3 ]
}
```

With a C++ enum of
```c++
enum class Colours : uint8_t { red = 0, green = 1, blue = 2, black = 3 };
```

The enum type is encoded as the type in the `json_number` member type.
To see a working example using this code, refer to [cookbook_enums2_test.cpp](../tests/src/cookbook_enums2_test.cpp)
```c++
enum class Colours : uint8_t { red, green, blue, black };

struct MyClass1 {
  std::vector<Colours> member0;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass1> {
    using type = json_member_list<
      json_array<"member0", Colours>
    >;

    static inline auto
    to_json_data( MyClass1 const &value ) {
      return std::forward_as_tuple( value.member0 );
    }
  };
}
```
