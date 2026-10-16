Many types can be deduced without being explicitly mapped to a JSON type
(string, number, bool, object, or array). This allows one to use them directly
in places that don't require a member name (e.g. a `json_array` element type) or
with the `json_link<Name, Type>` mapping type. The first matching mapping is
used, in the following order:

* Types with an explicit `json_data_contract` specialization. An explicit
  mapping always takes precedence over any deduced mapping, including the
  default mapping for empty types.
* Well known types

  | Type                  | Mapped To        | Notes                     |
  |-----------------------|------------------|---------------------------|
  | std:string_view       |json_string_raw   | Escaped when serialized without a contract, see below |
  | std::string           |json_string       |                           |
  | bool                  |json_bool         |                           |
  | Integer               |json_number       | Uses std::numeric_limits. json_number optimized for Signed/Unsigned integers|
  | Enum                  |json_number       | Used std::is_enum and std::underlying_type|
  | Floating point        |json_number       | Uses std::numeric_limits|
  | Associative Container |json_key_value_map| Has begin()/end()/key_type/mapped_type and constructable with two iterators|
  | readable values       |                  | The value_type in the readable mapping of T with a json_null wrapped around T's deduced mapping. See the readable value cookbook item |
  | Containers            |json_array        | Excluding associative containers. Uses value_type as the type for each element|
  | Empty default-constructible types | Empty JSON object | Used only when no explicit mapping exists; serializes as `{}` |

* Containers - map to json_array with the element type as the detected type of the value_type. Must have the methods
  begin(), end(), type alias value_type, and can be constructed with two iterators. Same Iterator requirements as std::
  vector's two iterator constructor.

## Serialization without a contract

`to_json`, `to_json_array`, and `json_writer` never parse the value they are
given, so a raw string mapping would write unescaped text. When they deduce a
mapping, either because no mapping type was given or because a C++ type was
given in its place, string-like types are mapped to escaped strings instead.
This also applies to the elements of deduced containers, key/value types,
nullable types, and tuples. An explicit JSON mapping type, such as
`json_string_raw_no_name<daw::string_view>`, is used as given.

| Type                         | In a `json_data_contract` | `to_json`, `to_json_array`, `json_writer` |
|------------------------------|---------------------------|--------------------------------------------|
| `std::string_view`, `daw::string_view` | `json_string_raw` | escaped string                      |
| `std::string`                | `json_string`             | escaped string                             |
| `char`                       | `json_number`             | one character escaped string               |
| `signed char`, `unsigned char` | `json_number`           | `json_number`                              |
| `char[N]`, `char const[N]`   | not deduced               | escaped string of length `N - 1`           |
| `char *`, `char const *`     | nullable `char`           | escaped string, `nullptr` is `null`; inside a container, nullable `char` |

Character pointers are converted where the value is passed to `to_json` or the
writer, not by deduction, so they are only treated as strings when passed
directly. See [Strings](strings.md#serializing-strings-without-a-mapping)
for examples and their output.

