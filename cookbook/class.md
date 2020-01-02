# Classes

## Single Class
```json
{ 
  "member0": "this is a test",
  "member1": 314159,
  "member2": true 
}
```
The above JSON describes a class with three members, a string named `member0`, an integer named `member1`, and a boolean named `member2`

The C++ to contain and parse this could look like
To see a working example using this code, look at the [cookbook_class1_test.cpp](../tests/cookbook_class1_test.cpp) test in tests
```cpp
struct MyClass1 {
  std::string member_0;
  int member_1;
  bool member_2;
};

namespace daw::json {
  template<>
  struct json_data_contract<daw::cookbook_class1::MyClass1> {
    using type = json_member_list<
      json_string<"member0">, 
      json_number<"member1", int>,
      json_bool<"member2">>;

    static inline auto
    to_json_data( daw::cookbook_class1::MyClass1 const &value ) {
      return std::forward_as_tuple( value.member_0, value.member_1,
                                    value.member_2 );
    }
  };
}
```
As you can see the local c++ member names do not have to match the json member names.

## Class in a Class
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

Here we have a JSON object that has a member `a` that is a class of type MyClass1, and a second member that is an unsigned
To see a working example using this code, look at the [cookbook_class2_test.cpp](../tests/cookbook_class2_test.cpp) test in tests
```cpp
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

As you can see, we only had to say that member `"a"` is of type MyClass1

## Selective mapping

One does not need to map all the members in the JSON class to use it, this is often useful as we only need a subset of the members in our C++ code.  Starting from the previous `MyClass2` example, lets remove the `b` member
To see a working example using this code, look at the [cookbook_class3_test.cpp](../tests/cookbook_class3_test.cpp) test in tests
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

```cpp
// Code from previous MyClass1 example

struct MyClass3 {
  MyClass1 a;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass3> {
    using type = json_member_list<
      json_class<
        "a",
        daw::cookbook_class3::MyClass1>
      >;

    static inline auto
    to_json_data( MyClass3 const &value ) {
      return std::forward_as_tuple( value.a );
    }
  };
}
```



