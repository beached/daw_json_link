# Mapping Types

The highlighted mappings below are in the `daw::json` namespace.

### `json_link`

Uses deduced type mappings to determine the mapping type to use. This is also useful in systems with generic types or reflection(future or simulated). The `daw_json_link_describe.h`(Uses Boost.Describe for class members) and
`daw_json_boost_pfr_mapping.h`(Uses Boost PFR library to get type members) use this mapping type.

### `json_array`

Maps JSON arrays to C++ container types.

### `json_sized_array`

Maps JSON arrays/size member to C++ container types. This allows easier mapping to types like `unique_ptr`. Precautions should be taken that the input is trusted or validated in the Constructor type to prevent resource exhaustion.

### `json_bool`

Maps JSON boolean values to C++ boolean/integral types.

### `json_class`

Maps JSON objects/types to C++ types with a `daw::json::json_data_contract` mapping

### `json_custom`

Maps JSON strings/literals to C++ types that don't fit.
See [Custom Types and Output](custom_types.md) for converter and option
examples.

### `json_custom_lit`

A helper for `json_custom` for mapping to the non-string types

### `json_date`

Maps JSON strings in iso8601 timestamp to values to C++ `std::chrono::time_point` types.

### `json_key_value`

Maps JSON objects to C++ containers like maps or vector of pairs.

### `json_key_value_array`

Maps JSON array of objects with key/value members to C++ containers

### `json_nullable`

Maps JSON values that can be null to Nullable C++ std::optional or pointer like types. The `_null` variants of other mappings can help with this. Types are required to be Nullable via `daw::json::concepts::nullable_value_traits<T>`.

### `json_number`

Maps JSON number values to C++ arithmetic/bool/enum types.

### `json_checked_number`

Maps JSON number values to C++ arithmetic/bool/enum types but the value is checked for narrowing.

### `json_raw`

Maps and JSON value to a C++ type.  The constructor must take a character/size range and to_json_data returns raw JSON.

### `json_string`

Maps JSON string values to C++ string types. The string type will need to own the character range they construct(no string_view).

### `json_string_raw`

Maps JSON string values to C++ string types without processing. This allows for `string_view` mappings.

### `json_submember`

Maps a C++ member to a value below a named JSON object or array member. The
relative path is searched while parsing. Serialization reconstructs the minimal
path and supports only array index `[0]`. See [Submember Mappings](submember.md)
for its invariants and error behavior.

### `json_tuple`

Maps JSON tuples/arrays/heterogeneous arrays to C++ class types.  This requires that the order is significant.  This is useful to save space because member names are not needed.

### `json_value`

General-purpose mapping for JSON values where the type may not be predetermined.

### `json_variant`

Maps JSON values that may be one of several types to C++ type(s). Because there is no logic specified to determine the type, only one each of JSON string, boolean, object, number, or array are allowed.

### `json_tagged_variant`

Maps JSON values that may be one of serveral types to C++ type(s). A switcher type that inspects another member to determine which alternative is required.

### `json_intrusive_variant`

Maps JSON values that may be one of serveral types to C++ type(s). A switcher type that inspects a sub member to determine which alternative is required.

## Mapping Template Parameters

Named mappings use the JSON member name as their first template parameter.
The remaining parameters are summarized below; parameters shown with `= ...`
have defaults. `use_default` asks the library to deduce the mapping or use its
standard constructor.

| Mapping | Template parameters after `Name` |
| --- | --- |
| `json_class` | `T, Constructor = use_default` |
| `json_recursive_class` | `T, Constructor = use_default` |
| `json_number` | `T = double, Options = number_opts_def, Constructor = use_default` |
| `json_checked_number` | `T = double, Options = number_opts_def, Constructor = use_default` |
| `json_fp` | `T = double, Format = FPOutputFormat::Auto, Precision = max_value<unsigned>, Options = fp_opts_def, Constructor = use_default` |
| `json_checked_fp` | `T, Format = FPOutputFormat::Auto, Precision = max_value<unsigned>, Options = fp_opts_def, Constructor = use_default` |
| `json_bool` | `T = bool, Options = bool_opts_def, Constructor = use_default` |
| `json_string` | `String = std::string, Options = string_opts_def, Constructor = use_default` |
| `json_string_raw` | `String = std::string, Options = string_raw_opts_def, Constructor = use_default` |
| `json_date` | `T = system_clock time_point, Constructor = use_default` |
| `json_array` | `JsonElement, Container = use_default, Constructor = use_default` |
| `json_sized_array` | `JsonElement, SizeMember, Container = use_default, Constructor = use_default` |
| `json_key_value` | `Container, JsonValueType = use_default, JsonKeyType = use_default, Constructor = use_default` |
| `json_key_value_array` | `Container, JsonValueType = use_default, JsonKeyType = use_default, Constructor = use_default` |
| `json_tuple` | `Tuple, JsonTupleTypesList = use_default, Constructor = use_default` |
| `json_variant` | `Variant, JsonElements = use_default, Constructor = use_default` |
| `json_tagged_variant` | `T, TagMember, Switcher, JsonElements = use_default, Constructor = use_default` |
| `json_intrusive_variant` | `Variant, TagMember, Switcher, JsonElements = use_default, Constructor = use_default` |
| `json_custom` | `T, FromJsonConverter = use_default, ToJsonConverter = use_default, Options = json_custom_opts_def` |
| `json_raw` | `T = json_value, Constructor = use_default` |
| `json_nullable` | `T, JsonMember = use_default, NullableType = JsonNullable::Nullable, Constructor = use_default` |

`Options` is an encoded `json_options_t` value. Build it with the setter for
the mapping family: `options::number_opt`, `fp_opt`, `bool_opt`, `string_opt`,
`string_raw_opt`, or `json_custom_opt`. The available flags and defaults are
listed in [Member Options/Type Options](member_options.md).

`JsonElement`, `JsonValueType`, `JsonKeyType`, `JsonTupleTypesList`, and
`JsonElements` accept explicit mappings when deduction is not appropriate.
Array elements and object keys/values use unnamed mappings such as
`json_number_no_name<int>`.

`Constructor` is a callable used in place of the standard construction step.
For scalar mappings it accepts the parsed scalar. Container and class mappings
use the arguments described by that mapping's declaration. Nullable mappings
use it to construct the outer nullable result.

### Nullable convenience mappings

Named aliases ending in `_null` add this parameter before `Constructor`:

```cpp
JsonNullable NullableType = JsonNullable::Nullable
```

Their value type is the wrapped/nullable type, such as `std::optional<int>`.
`JsonNullable::Nullable` permits an empty class member to be omitted;
`JsonNullable::NullVisible` emits that member as `null`. See
[Nullable JSON Values](json_nullable.md).

Unnamed nullable aliases ending in `_null_no_name` always preserve an empty
value as the JSON literal `null`, because an array element or root value cannot
be omitted. They therefore do not expose a `JsonNullable` parameter.

## Understanding _null and _no_name Variants

### Nullable mappings end in `_null`

The `_null` variant allows the mapped value to be nullable.
Useful when the JSON field may or may not be present.
The wrapped C++ type is supplied explicitly and may be `std::optional`, a smart
pointer, or another type satisfying the nullable-value requirements.

### Unnamed mappings end in `_no_name`

The `_no_name` variant implies the JSON field does not have a named key in the serialized output.
Typically used for JSON arrays or when the field names are not necessary.
These mappings provide robust and flexible ways to handle different JSON structures and types seamlessly within C++ using daw_json_link.

### Unnamed nullable mappings `_null_no_name`

The `_null_no_name` variants combine a nullable type without a name. Empty
values serialize as `null`.
