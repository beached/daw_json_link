# Classes
Arbitrary C++ data structures are supported and can be described using the trait `json_data_contract`.

## Basic class mapping
Below is an ordinary JSON object
```json
{ 
  "member0": "this is a test",
  "member1": 314159,
  "member2": true 
}
```
The above JSON describes a class with three members, a string named `member0`, an integer named `member1`, and a boolean named `member2`

Below is the C++ data structure and the trait to map the members to that of the JSON object.  Note that the names of the C++ data members do not have to be the same as the JSON object's.
To see a working example using this code, refer to [cookbook_class1_test.cpp](../tests/cookbook_class1_test.cpp) 
```c++
struct MyClass1 {
  std::string member_0;
  int member_1;
  bool member_2;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass1> {
    using type = json_member_list<
      json_string<"member0">, 
      json_number<"member1", int>,
      json_bool<"member2">
    >;

    static inline auto to_json_data( MyClass1 const &value ) {
      return std::forward_as_tuple( 
        value.member_0, 
        value.member_1,
        value.member_2 );
    }
  };
}
```
The above `json_data_contract` trait maps the JSON members to the constructor of `MyClass1` in the order specified.  The arguments of type `std::string, int, bool` will be passed.

## Class as a member
The serializing and deserializing is recursive.  So if a class contains another class, the requirement is that that class has been mapped already.  Assuming we already have the mapping above, lets embed that into another class

```json
{
  "a": {
    "member0": "this is a test",
    "member1": 314159,
    "member2": true
  },
  "b": 1234
}
```

The JSON object that has a member `"a"` that matches `MyClass1`, and a second member is an `unsigned`
To see a working example using this code, refer to [cookbook_class2_test.cpp](../tests/cookbook_class2_test.cpp) 
```c++
// Code from previous MyClass1 example

struct MyClass2 {
  MyClass1 a;
  unsigned b;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass2> {
    using type = json_member_list<
      json_class<"a", MyClass1>,
      json_number<"b", unsigned>
    >;

    static inline auto
    to_json_data( MyClass2 const &value ) {
      return std::forward_as_tuple( value.a, value.b );
    }
  };
} // namespace daw::json
```

## Selective mapping

Not all of the JSON objects members need to be mapped. Below is the same JSON object as in the `MyClass2` example above.
To see a working example using this code, refer to [cookbook_class3_test.cpp](../tests/cookbook_class3_test.cpp) 
```json
{
  "a": {
    "member0": "this is a test",
    "member1": 314159,
    "member2": true
  },
  "b": 1234
}
```

```c++
// Code from previous MyClass1 example

struct MyClass3 {
  MyClass1 a;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass3> {
    using type = json_member_list<json_class<"a", MyClass1>>;

    static inline auto
    to_json_data( MyClass3 const &value ) {
      return std::forward_as_tuple( value.a );
    }
  };
}
```

Only the `"a"` member is mapped, the `"b"` member of the JSON object is ignored.



