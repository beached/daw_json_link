# Submember Mappings

`json_submember<Name, JsonPath, JsonMember>` maps one C++ value to a value below
a named JSON object or array member. `JsonPath` is relative to `Name` and uses
the same dot and square-bracket syntax as the path overloads of `from_json`.

For example, this JSON stores `reading` below two object members and the first
element of an array.

```json
{
  "payload": {
    "readings": [
      {
        "value": 42
      }
    ]
  }
}
```

The mapping selects only the integer at `payload.readings[0].value`.

```c++
#include <daw/json/daw_json_link.h>

#include <tuple>

struct Sample {
  int reading;
};

template<>
struct daw::json::json_data_contract<Sample> {
  using type = json_member_list<
    json_submember<"payload", "readings[0].value", int>>;

  static constexpr auto to_json_data( Sample const &sample ) {
    return std::forward_as_tuple( sample.reading );
  }
};
```

Parsing finds `payload`, follows the relative path, and passes the selected JSON
value to the mapping for `JsonMember`. `JsonMember` must be unnamed; it can be an
explicit mapping such as `json_string_no_name<std::string>` or a type such as
`int` for which an unnamed mapping can be deduced.

```c++
auto const sample = daw::json::from_json<Sample>( json_data );
// sample.reading == 42
```

Serialization reconstructs the minimal hierarchy described by the path.

```c++
auto const json = daw::json::to_json( Sample{ 42 } );
// json == R"({"payload":{"readings":[{"value":42}]}})"
```

This is a projection rather than preservation of the input document. Object
members and array elements not represented by the mapping are discarded during
parsing and cannot be recreated by `to_json`.

## Invariants and errors

The following invariants apply:

* `JsonPath` must not be empty.
* `JsonMember` must be an unnamed JSON mapping or have an unnamed deduced
  mapping.
* A member-name path step requires the current JSON value to be an object. An
  array-index step requires it to be an array.
* The outer member named by `Name` and the complete relative path must exist
  while parsing. A nullable `JsonMember` does not make a missing path optional.
* A `json_data_contract` must not contain multiple `json_submember` mappings
  with the same outer `Name`.
* Serialization can reconstruct only index `[0]` at every array step. Any other
  index would require inventing preceding elements and is rejected.

An invalid or mismatched container step reports `ErrorReason::InvalidJSONPath`.
A well-formed path that is absent reports `ErrorReason::JSONPathNotFound`.
Attempting to serialize a path containing an array index other than exactly
`[0]` reports `ErrorReason::OutputError` through the configured JSON error
handler.

In C++17, declare both names as static character arrays and pass those arrays as
the first two template arguments. C++20 and later permit the string-literal form
shown above.

A complete parsing, serialization, and error-handling example is exercised by
[test_json_submember.cpp](../../tests/src/test_json_submember.cpp).
