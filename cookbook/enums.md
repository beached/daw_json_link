# Enums

Enums are generally transmitted as number of strings.

## As String 
```json
{
  "member0": [ "red", "green", "blue", "black" ]
}
```
The above JSON describes a class with an array, ```member0``` of strings
In order to store as json strings we need as to_string and from_string method as below
The C++ to contain and parse this could look like
To see a working example using this code, look at the [cookbook_enums1_test.cpp](../tests/cookbook_enums1_test.cpp) test in tests
```cpp
enum class Colours : uint8_t { red, green, blue, black };

constexpr std::string_view to_string( Colours c ) {
    switch( c ) {
        case Colours::red: return "red";
        case Colours::green: return "green";
        case Colours::blue: return "blue";
        case Colours::black: return "black";
    }
    std::abort( );
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
    std::abort( );
}

struct MyClass1 {
    std::vector<Colours> member0;
};

auto describe_json_class( MyClass1 const & ) {
  using namespace daw::json;
  return class_description_t<
    json_array<"member0", std::vector<Colours>,
      json_custom<no_name, Colours>
    >
  >{};
}

auto to_json_data( MyClass1 const &value ) {
    return std::forward_as_tuple( value.member0 );
}
```

## As Number

We can use enums just like json numbers.

With the enum above 
```cpp
enum class Colours : uint8_t { red, green, blue, black };
```
We have an enum where `red = 0`, `green=1`,...

The json above would look like 
```json
{
  "member0": [ 0, 1, 2, 3 ]
}
```
The C++ to contain and parse this could look like
To see a working example using this code, look at the [cookbook_enums2_test.cpp](../tests/cookbook_enums2_test.cpp) test in tests
```cpp
enum class Colours : uint8_t { red, green, blue, black };

struct MyClass1 {
    std::vector<Colours> member0;
};

auto describe_json_class( MyClass1 const & ) {
  using namespace daw::json;
  return class_description_t<
    json_array<"member0", std::vector<Colours>,
      json_number<no_name, Colours>
    >
  >{};
}

auto to_json_data( MyClass1 const &value ) {
  return std::forward_as_tuple( value.member0 );
}
```
