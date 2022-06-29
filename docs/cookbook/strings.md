# Strings
JSON natively encodes strings.  There are two options for mapping them to C++ data structures, fully processed and raw strings. Raw strings only find the end of the string and do not change the underlying values.

## Escaped
```json
{
  "uris": [
    "example.com",
    "B\u00FCcher.ch",
    "happy.cn",
    "happy\u5FEB\u4E50.cn",
    "\u5FEB\u4E50.cn",
    "happy.\u4E2D\u56FD",
    "\u5FEB\u4E50.\u4E2D\u56FD",
    "www.\u30CF\u30F3\u30C9\u30DC\u30FC\u30EB\u30B5\u30E0\u30BA.com",
    "\uD83E\uDD84.com"
  ]
}
```
The above JSON document consists of an array member named `"uris"` with an element type of string.

The C++ `std::vector<std::string>` of the above json should be equivalent to:
```c++
std::vector<string> uris = {
  "example.com", 
  "Bücher.ch",
  "happy.cn", 
  "happy快乐.cn",
  "快乐.cn", 
  "happy.中国", 
  "快乐.中国", 
  "www.ハンドボールサムズ.com", 
  "🦄.com"};
```

To see a working example using this code, refer to [cookbook_escaped_strings1_test.cpp](../tests/src/cookbook_escaped_strings1_test.cpp). 

Below the mapping of the JSON object with C++ data structures.
```c++
struct WebData {
  std::vector<std::string> uris;
};

namespace daw::json {
  template<>
  struct json_data_contract<WebData> {
    using type = json_member_list<
      json_array<"uris", std::string>
    >;

    static inline auto
    to_json_data( WebData const &value ) {
      return std::forward_as_tuple( value.uris );
    }
  };
} 
```

## Raw strings
Raw strings are useful where we don't want to process the strings, we know they will never be escaped, or we do not require processing.  A raw string, is also simpler in that it only requires a constructor that requires a pointer and size, like `std::string_view` or `std::string`.  
```json
{
  "uris": [
    "example.com",
    "B\u00FCcher.ch",
    "happy.cn",
    "happy\u5FEB\u4E50.cn",
    "\u5FEB\u4E50.cn",
    "happy.\u4E2D\u56FD",
    "\u5FEB\u4E50.\u4E2D\u56FD",
    "www.\u30CF\u30F3\u30C9\u30DC\u30FC\u30EB\u30B5\u30E0\u30BA.com",
    "\uD83E\uDD84.com"
  ]
}
```
The above JSON document consists of an array member named `"uris"` with an element type of string.

The C++ `std::vector<std::string>` of the above json should be equivalent to when used as `json_string_raw`:
```c++
std::vector<std::string> uris = {
  "example.com",
  "B\u00FCcher.ch",
  "happy.cn",
  "happy\u5FEB\u4E50.cn",
  "\u5FEB\u4E50.cn",
  "happy.\u4E2D\u56FD",
  "\u5FEB\u4E50.\u4E2D\u56FD",
  "www.\u30CF\u30F3\u30C9\u30DC\u30FC\u30EB\u30B5\u30E0\u30BA.com",
  "\uD83E\uDD84.com"
};
```

Contrasting with the [Escaped]("#Raw Strings") example, the difference will be that the array element type is `json_string_raw`

```c++
struct WebData {
	std::vector<std::string> uris;
};

namespace daw::json {
  template<>  
  json_data_contract<WebData> {
    using type = json_member_list<
      json_array<"uris", json_string_raw<no_name>>
    >;

    static inline auto to_json_data( WebData const & value ) {
      return std::forward_as_tuple( value.uris );
    }
  };
}
```
