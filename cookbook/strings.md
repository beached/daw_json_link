# Strings

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
The above JSON describes a class with three members, a string named `member0`, an integer named `member1`, and a boolean named `member2`

The C++ `vector<string>` of the above json should be equivilent to:
```cpp
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

The C++ to contain and parse this could look like
To see a working example using this code, look at the [cookbook_strings1_test.cpp](../tests/cookbook_strings1_test.cpp) test in tests
```cpp
struct WebData {
	std::vector<std::string> uris;
};

auto describe_json_class( WebData const & ) {
    using namespace daw::json;
    return class_description_t<
			json_array<"uris", json_string<no_name>>>{};
}

auto to_json_data( WebData const & value ) {
	return std::forward_as_tuple( value.uris );
}
```

## Raw strings
Raw strings are useful where we don't want to process the strings, we know they will never be escaped, or there are non-conformant items encoded.  A raw string, is also simpler in that it only requires a constructor that requires a pointer and size, like a string_view.  

Using the json above and contrasting with the code above, all we need to do is change the `json_string` to `json_string_raw`

```cpp
struct WebData {
	std::vector<std::string> uris;
};

auto describe_json_class( WebData const & ) {
    using namespace daw::json;
    return class_description_t<
			json_array<"uris", json_string_raw<no_name>>>{};
}

auto to_json_data( WebData const & value ) {
	return std::forward_as_tuple( value.uris );
}
```

Now the C++ `std::vector<string>` will parse as 
```cpp
std::vector<string> uris = {
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
