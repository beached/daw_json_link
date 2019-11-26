# Array's

## Simple Array
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


## Class Elements in Array

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

auto describe_json_class( MyClass4 const & ) {
    using namespace daw::json;
    return class_description_t<
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


