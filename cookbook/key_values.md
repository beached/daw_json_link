# Key Values

Dictionaries and Maps are not directly represented in JSON.  There are several ways that they are commonly encoded.

## As Class
Keys are stored as member names and Values as their value.  This is very common, but limits the Key type to strings in JSON.  However, numbers and other literals are encodable as strings.

```json
{ 
  "kv":  {
    "key0": 353434,
    "key1": 314159 
  }
}
```

Too see a working example using this code, refer to [cookbook_kv1_test.cpp](../tests/cookbook_kv1_test.cpp) 
Below the JSON object is mapped to a `std::unordered_map<std::string, int>`
```cpp
struct MyKeyValue {
  std::map<std::string, int> kv;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyKeyValue1> {
    using type = json_member_list<
      json_key_value<"kv", std::unordered_map<std::string, int>, int>
    >;

    static inline auto
    to_json_data( MyKeyValue1 const &value ) {
      return std::forward_as_tuple( value.kv );
    }
  };
}
```


## As Array
Key/Values are stored as JSON objects in an array.  Generally the key member's name is `"key"` and the value members name is `"value"`

```json
{
  "kv": [
    { "key": 0, "value": "test_001" },
    { "key": 1, "value": "test_002" }
  ]
}
```
The above JSON document an array member named `"kv"` whose element is an object with int keys, named `"key"` and string values named `"value"`.

Below it is mapped to a `std::unorderd_map<intmax_t, std::string>`
Too see a working example using this code, refer to [cookbook_kv2_test.cpp](../tests/cookbook_kv2_test.cpp) 
```cpp
struct MyKeyValue2 {
  std::unordered_map<intmax_t, std::string> kv;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyKeyValue2> {
    using type = json_member_list<
      json_key_value_array<"kv", std::unordered_map<intmax_t, std::string>,
        json_string_raw<"value">, intmax_t>>;

    static inline auto to_json_data( MyKeyValue2 const &v ) {
      return std::forward_as_tuple( v.kv );
    }
  };
} 
```

