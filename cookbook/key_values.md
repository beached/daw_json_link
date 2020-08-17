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
```c++
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
```c++
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

# MultiMap
It is possible to parse/serialize Multimap likes types.  A JSON document, such as 

```json
{
  "kv": { 
    "firstName": "Jane",
    "firstName": "John"
  }
}
```

When mapped to a C++ class, it will parse the first `"firstName"` it finds, skipping the rest.  To parse this as a `json_key_value`, the following code will work if it is a submember of the root, or subsequent submember's.

Too see a working example using this code, refer to [cookbook_kv3_test.cpp](../tests/cookbook_kv3_test.cpp) 

```c++
struct MyKeyValue3 {
  std::multimap<std::string, std::string> kv;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyKeyValue3> {
    using type = json_member_list<
      json_key_value<
        "kv", 
        std::multimap<std::string, std::string>, 
        std::string>
      >;

    static inline auto to_json_data( MyKeyValue3 const &v ) {
      return std::forward_as_tuple( v.kv );
    }
  };
} 
```
Alternatively, a type like `std::vector<std::pair<Key, Value>>` would work in the above example too.

If the root object has duplicate keys, one needs to use the `json_value` interface and then use `from_json` to parse it.
```json
{
  "firstName": "Jane",
  "firstName": "John"
}
```

Too see a working example using this code, refer to [cookbook_kv4_test.cpp](../tests/cookbook_kv4_test.cpp) 

```c++
std::string_view json_data = ...;
auto val = daw::json::json_value( json_data );
std::multimap<std::string, std::string kv = 
  daw::json::from_json<
    json_key_value<
      no_name, 
      std::multimap<std::string, std::string>,
      std::string
    >
  >( val );
```