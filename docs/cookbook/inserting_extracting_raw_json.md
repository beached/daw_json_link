# Inserting and Extracting Raw JSON

Use the `json_raw` mapping type to parse or serialize a complete JSON value as
raw text. This is useful for preserving a section of a document for later or
for passing that section to another JSON library.

## Storing raw JSON text

For a string-like destination, the type must be constructible from a
`char const *` and a `std::size_t`. For serialization, the supplied value must
support `std::begin` and `std::end`, with iterators whose value type is `char`.

```c++
struct Foo {
  std::string bar;
  std::string raw_json;
};

namespace daw::json {
  template<>
  struct json_data_contract<Foo> {
    using type = json_member_list<
      json_link<"bar", std::string>, 
      json_raw<"raw_json", std::string>
    >;
  
    static auto to_json_data( Foo const & v ) {
      return std::forward_as_tuple( v.bar, v.raw_json );
    }
  };
}
```

If `raw_json` contains `R"({"answer":42})"`, it is emitted as an object rather
than as an escaped JSON string:

```json
{
  "bar": "example",
  "raw_json": {"answer":42}
}
```

## Interoperating with another JSON library

Most JSON DOM types are not directly constructible from a character range and
do not iterate over their serialized representation. A custom constructor can
adapt parsing, while `to_json_data` can provide the other library's serialized
string for output.

The following example uses placeholder names for the other library's `parse`
and `dump` operations:

```cpp
#include <daw/json/daw_json_link.h>
#include <other_json_library.hpp>

#include <cstddef>
#include <string>
#include <tuple>

struct ParseOtherJson {
  other_json_library::value
  operator( )( char const *data, std::size_t size ) const {
    return other_json_library::parse( data, data + size );
  }
};

struct Message {
  std::string name;
  other_json_library::value payload;
};

namespace daw::json {
  template<>
  struct json_data_contract<Message> {
    using type = json_member_list<
      json_string<"name">,
      json_raw<
        "payload",
        other_json_library::value,
        ParseOtherJson
      >
    >;

    static auto to_json_data( Message const &message ) {
      // std::tuple owns the temporary string returned by dump.
      return std::tuple{
        message.name,
        other_json_library::dump( message.payload )
      };
    }
  };
}
```

When parsing the following document:

```json
{
  "name": "example",
  "payload": {"answer":42}
}
```

`ParseOtherJson` receives a pointer and size covering `{"answer":42}`. When
serializing, the string returned by `dump` is copied directly into the output,
without surrounding quotes or escaping.

Use an owning `std::tuple` when the other library returns a temporary string.
`std::forward_as_tuple` would store a reference to that temporary and leave a
dangling reference after `to_json_data` returns.

## Requirements and guarantees

### Parsing

* The constructor passed to `json_raw` must be callable with
  `(char const *, std::size_t)` and return the mapped C++ type.
* The character range contains one complete JSON value. It is not guaranteed
  to be null-terminated.
* Whitespace surrounding the JSON value may not be preserved.
* If the destination type keeps a non-owning view of the input, the original
  JSON buffer must outlive that destination value.

If the destination type is itself constructible from `(char const *,
std::size_t)`, omit the custom constructor:

```cpp
json_raw<"payload", RawJsonType>
```

### Serialization

* The value supplied by `to_json_data` must be a character range accepted by
  `std::begin` and `std::end`; its iterator value type must be `char`.
* The range is copied verbatim. DAW JSON Link does not add quotes or escape the
  contents.
* The supplied text is not validated during serialization and must therefore
  already be one valid JSON value.
* The character range must remain alive for the duration of serialization.

If the third-party type itself iterates over serialized JSON characters, it can
be returned directly from `to_json_data`. Most DOM types instead iterate over
array or object elements, so returning their `dump`, `serialize`, or equivalent
string is usually required.

`json_custom` configured with `options::JsonCustomTypes::Any` also accepts and
emits complete JSON values, but does so through user-supplied conversion
functions. Prefer `json_raw` when the mapped value is already a character range
containing serialized JSON. As with `json_raw`, serialized text is copied
without validation.
