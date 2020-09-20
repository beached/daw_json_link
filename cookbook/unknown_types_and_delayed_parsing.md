#Unknown Types and Delayed Parsing

# Unknown JSON Data
When the data structure is unknown or partially known it can be parsed as a `json_delayed` type. This also, allows for skipping the parsing of certain members and returning later. 

```json
["a", 2, "b", 3]
``` 
The above JSON describes an array of mixed types, string and number.

Below is the C++ code to parse and use it.
```c++
std::string_view json_data = ...;
auto val = daw::json::json_value( json_data );
for( auto member : val ) {
  if( member.name ) {
    std::cout << *member.name << " is a ";
  } else {
    std::cout << "Array element is a ";
  }
  std::cout << to_string( member.value.type( ) ) << " with a value of "
            << member.value.get_string_view( ) << '\n';
}
```

# Delayed Parsing
It is possible to delay the parsing of json members until a later time.  The `json_delayed` member type will store the character range and allow full parsing later.  The cost of doing so is similar to that of skipping the value and is already paid for when parsing JSON data.

```json
{
  "member0": 5,
  "member_later": { "a": 1, "b":  true },
  "member1": "Hello world"
}
```

The above JSON class has 3 members, a number, a class to parse later, and a string.

Below is the C++ code to parse and use it.
```c++
struct MyClass2 {
  int member0;
  daw::json::json_delayed_value member_later;
  std::string member1;
};


struct MyDelayedClass {
  int a;
  bool b;	
};

namespace daw::json {	
  template<>
  struct json_data_contract<MyClass2> {
    using type = json_member_list<
      json_number<"member0", int>,
      json_delayed<"member_later">,
      json_string<"member1">
    >;
 
    static inline auto
    to_json_data( MyClass2 const &value ) {
      return std::forward_as_tuple( value.member0, value.member_later, value.member1 );
    }
  }; 

  // This isn't necessary to parse the values as but allows for directly constructing the object
  template<>
  struct json_data_contract<MyDelayedClass> {
    using type = json_member_list<
      json_number<"a", int>,
      json_bool<"b">
    >;
 
    static inline auto
    to_json_data( MyDelayedClass const &value ) {
      return std::forward_as_tuple( value.a, value.b );
    }
  }; 
}

std::string_view json_data = ...;
MyClass2 val = from_json<MyClass2>( json_data );

MyDelayedClass delayed_val = cls2.member_layer.template parse<MyDelayedClass>( );
```
