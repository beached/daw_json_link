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

## Trusted, already-escaped strings

By default, `json_string` validates UTF-8 and escapes the value during
serialization. When an application can guarantee that a value is valid UTF-8
and is already escaped as JSON string content, the
`EscapeValidUTF8::AssumeValid` mapping option writes those bytes directly:

```c++
struct WebData {
  std::string escaped_uri;
};

namespace daw::json {
  template<>
  struct json_data_contract<WebData> {
    using type = json_member_list<
      json_string<
        "escaped_uri",
        std::string,
        options::string_opt( options::EscapeValidUTF8::AssumeValid )>
    >;

    static inline auto to_json_data( WebData const &value ) {
      return std::forward_as_tuple( value.escaped_uri );
    }
  };
}
```

For example, a value containing the bytes `already escaped: \"quoted\"` is
safe for this mapping because its quotation marks already have JSON escape
characters. A value containing unescaped quotation marks is not safe and can
produce invalid JSON.

This option affects serialization only. `from_json` continues to decode and
unescape a `json_string` normally. Consequently, a value produced by parsing
must not automatically be treated as suitable for `AssumeValid`; the
application must establish that it is already escaped before serializing it.
The default `EscapeValidUTF8::Validate` behavior should be used whenever that
guarantee is unavailable.

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
      json_array<"uris", json_string_raw_no_name<>>
    >;

    static inline auto to_json_data( WebData const & value ) {
      return std::forward_as_tuple( value.uris );
    }
  };
}
```

## Serializing strings without a mapping

`to_json`, `to_json_array`, and `json_writer` can serialize values that have no
`json_data_contract`. Because these values did not come from parsing, string
values are always escaped. A `std::string_view` is deduced as a raw string only
inside a `json_data_contract`, where it holds the still-escaped text from the
parsed document (see [Mapping Deduction](mapping_deduction.md)).

Character types are handled before deduction:

* `char *` and `char const *` are nul terminated strings. `nullptr` is written as `null`.
* `char[N]` and `char const[N]`, including string literals, are strings of length `N - 1`.
* `char` is a one character string. `signed char` and `unsigned char`, including
  `std::int8_t` and `std::uint8_t`, are distinct types and remain numbers.

Given

```c++
char buffer[] = "Hi\"";
char const *ptr = "A\"da";
char const *null_ptr = nullptr;
std::vector<char const *> ptrs{ "A\"da", nullptr };
```

| Expression                                                                  | Output               |
|-----------------------------------------------------------------------------|----------------------|
| `to_json( "A\"da" )`                                                        | `"A\"da"`            |
| `to_json( buffer )`                                                         | `"Hi\""`             |
| `to_json( ptr )`                                                            | `"A\"da"`            |
| `to_json( null_ptr )`                                                       | `null`               |
| `to_json( "A\"da"s )`                                                       | `"A\"da"`            |
| `to_json( "A\"da"sv )`                                                      | `"A\"da"`            |
| `to_json( 'x' )`                                                            | `"x"`                |
| `to_json( std::int8_t{ 65 } )`                                              | `65`                 |
| `to_json( std::uint8_t{ 65 } )`                                             | `65`                 |
| `to_json( std::optional<std::string_view>( "A\"da" ) )`                     | `"A\"da"`            |
| `to_json( std::vector<std::string_view>{ "A\"da", "B\\b" } )`               | `["A\"da","B\\b"]`   |
| `to_json( std::map<std::string_view, std::string_view>{ { "na\"me", "A\"da" } } )` | `{"na\"me":"A\"da"}` |
| `to_json( std::tuple<int, std::string_view>{ 1, "A\"da" } )`                | `[1,"A\"da"]`        |
| `to_json( std::vector<char>{ 'a', 'b' } )`                                  | `["a","b"]`          |
| `to_json( ptrs )`                                                           | `[65,null]`          |

Character pointers are only converted when passed directly. Inside a container
they are deduced as a nullable `char`, so `to_json( ptrs )` writes the first
character's code. Use a container of `std::string_view` instead, or write the
elements with `json_writer::write_array_values`.

### Already escaped values

Passing an explicit raw mapping writes the value as-is. The value must already
be valid, escaped JSON string content.

```c++
using raw = json_string_raw_no_name<daw::string_view>;
```

| Expression                      | Output    |
|---------------------------------|-----------|
| `to_json<raw>( R"(A\"da)"sv )`  | `"A\"da"` |
| `to_json<raw>( R"(A\"da)" )`    | `"A\"da"` |

A `std::string_view` member of a `json_data_contract` is also raw. Given a
contract mapping `json_link<"value", std::string_view>` for `Member`,
`to_json( Member{ R"(A\"da)" } )` writes `{"value":"A\"da"}`. Likewise a `char`
member of a contract, `json_link<"c", char>`, remains a number:
`to_json( CharMember{ 'z' } )` writes `{"c":122}`.

### Invalid UTF-8

With the default `RestrictedStringOutput::ErrorInvalidUTF8` output option,
valid UTF-8 is written unchanged and invalid UTF-8 is an error.

| Expression                     | Output                                               |
|--------------------------------|------------------------------------------------------|
| `to_json( "caf\xC3\xA9"sv )`   | `"café"`                                             |
| `to_json( "\xC3"sv )`          | throws `json_exception` with `ErrorReason::InvalidUTFCodepoint` |

A lone byte above `0x7F` is not a complete UTF-8 character, so `to_json( char( 0xC3 ) )`
also throws.

See [Arrays](array.md#serializing-c-arrays-and-strings-with-to_json_array) for
`to_json_array` and [Incremental JSON Writer](json_writer.md#strings-characters-and-character-pointers)
for `json_writer`.
