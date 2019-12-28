# Key Values

## As Class
Stored in JSON objects and string/value pairs

```json
{ 
	"kv":	{
		"key0": 353434,
		"key1": 314159 
	}
}
```
The above JSON describes a class with a key value member that represents a map with a string key and a numeric value

The C++ to contain and parse this could look like
Too see a working example using this code, look at the [cookbook_kv1_test.cpp](../tests/cookbook_kv1_test.cpp) test in tests
```cpp
struct MyKeyValue {
	std::map<std::string, int> kv;
};

auto json_data_contract_for( MyKeyValue1 const & ) {
    using namespace daw::json;
    return json_data_contract<json_key_value<
      "kv", std::unordered_map<std::string, int>, int>>{};
}

auto to_json_data( MyKeyValue const & value ) {
	return std::forward_as_tuple( value.kv );
}
```


## As Array
Too see a working example using this code, look at the [cookbook_kv2_test.cpp](../tests/cookbook_kv2_test.cpp) test in tests
```json
{
	"kv": [
		{ "key": 0, "value": "test_001" },
		{ "key": 1, "value": "test_002" }
	]
}
```
The above JSON describes a class with a member that stores key valus as objects in an array.  In this case the key is integral and the value is a string

The C++ to contain and parse this could look like
```cpp
struct MyKeyValue2 {
	std::unordered_map<intmax_t, std::string> kv;
};

auto json_data_contract_for( MyKeyValue2 const & ) {
	using namespace daw::json;
	return json_data_contract<json_key_value_array<
		"kv", std::unordered_map<intmax_t, std::string>, json_string_raw<"value">, intmax_t>>{};
}

auto to_json_data( MyKeyValue2 const &value ) {
	return std::forward_as_tuple( value.kv );
}
```

