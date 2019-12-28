# Array's

## Simple Array of int
```json
[1,2,3,4,5]
```
The above JSON describes an array of integers

The C++ to contain and parse this could look like
To see a working example using this code, look at the [cookbook_array1_test.cpp](../tests/cookbook_array1_test.cpp) test in tests
```cpp
auto vec = from_json_array<int>( json_str );
```

This is a simple example, had the element type of the array been more complicated, like a class we would have to describe that.


## Array of a class

```json
[
	{
		"a": "Hello World", 
		"b": 1234, 
		"c": 1.23, 
		"d": false
	}, 
	{
		"a": "Goodbye World",
		"b": 4321,
		"c": 123,
		"d": true
	}
]
```

Here we hae a JSON array containing a class with members of type string, unsigned, float, and boolean.

The C++ to contain and parse this could look like
To see a working example using this code, look at the [cookbook_array2_test.cpp](../tests/cookbook_array2_test.cpp) test in tests

```C++
struct MyClass4 {
	std::string a;
	unsigned b;
	float c;
	bool d;
};

auto json_data_contract_for( MyClass4 const & ) {
    using namespace daw::json;
    return json_data_contract<
			json_string_raw<"a">,
            json_number<"b", unsigned>,
            json_number<"c", float>,
            json_bool<"d">>{};
}

auto to_json_data( MyClass4 const & value ) {
	return std::forward_as_tuple( value.a, value.b, value.c, value.d );
}

std::vector<MyClass4> v = from_json_array<MyClass4>( str );
```

## Array's as members
The `json_array` member type allows you to easily map a member variable to a vector of JsonClass. If the container is different, you can specify it too.

To see a working example using this code, look at the [cookbook_array3_test.cpp](../tests/cookbook_array3_test.cpp) test in tests
Take the following json

```json
{
  "member0": 5,
  "member1": [ 1,2,3,4,5 ],
  "member2": [ "hello", "world" ]
}
```

Here the JSON class has an int member, an array of int member, and an array of string member

In C++ it could be represented like

```cpp
struct MyArrayClass1 {
    int member0;
    std::vector<int> member1;
    std::vector<std::string> member2;
};
```
With the following functions defined to allow mapping of JSON
```cpp
auto json_data_contract_for( MyArrayClass1 const & ) {
    using namespace daw::json;
    return json_data_contract<
            json_number<"member0", int>,
            json_array<"member1", int>,
            json_array<"member2", std::string>>{};
}

auto to_json_data( MyArrayClass1 const & value ) {
	return std::forward_as_tuple( value.member0, value.member1, value.member2 );
}
```

