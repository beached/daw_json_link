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

auto describe_json_class( MyClass1 const & ) {
    using namespace daw::json;
    return class_description_t<
			json_string<"member0">, 
			json_number<"member1", int>,
			json_bool<"member2">>{};
}

auto to_json_data( MyClass1 const & value ) {
	return std::forward_as_tuple( value.member_0, value.member_1, value.member_2 );
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

auto describe_json_class( MyClass2 const & ) {
    using namespace daw::json;
    return class_description_t<
			json_class<"a", MyClass1>, 
			json_number<"b", unsigned>>>{};
}

auto to_json_data( MyClass2 const & value ) {
	return std::forward_as_tuple( value.a, value.b );
}
```

As you can see, we only had to say that member `"a"` is of type MyClass1