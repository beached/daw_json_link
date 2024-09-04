# Mapping Types

The highlighted mappings below are in the `daw::json` namespace.

### `json_link`

Uses deduced type mappings to determine the mapping type to use. This is also useful in systems with generic types or reflection(future or simulated). The `daw_json_link_describe.h`(Uses Boost.Describe for class members) and
`daw_json_boost_pfr_mapping.h`(Uses Boost PFR library to get type members) use this mapping type.

### `json_array`

Maps JSON arrays to C++ container types.

### `json_sized_array`

Maps JSON arrays/size member to C++ container types. This allows easier mapping to types like `unique_ptr`. Precautions should be taken that the input is trusted or validated in the Constructor type to prevent resource exhaustion.

### `json_boolean`

Maps JSON boolean values to C++ boolean/integral types.

### `json_class`

Maps JSON objects/types to C++ types with a `daw::json::json_data_contract` mapping

### `json_custom`

Maps JSON strings/literals to C++ types that don't fit.

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

## Understanding _null and _no_name Variants

### Nullable mappings end in `_null`

The `_null` variant allows the mapped value to be nullable.
Useful when the JSON field may or may not be present.
Maps to std::optional in C++, effectively allowing an absence of the value.

### Unnamed mappings end in `_no_name`

The `_no_name` variant implies the JSON field does not have a named key in the serialized output.
Typically used for JSON arrays or when the field names are not necessary.
These mappings provide robust and flexible ways to handle different JSON structures and types seamlessly within C++ using daw_json_link.

### Unnamed nullable mappings `_null_no_name`

The `_null_no_name` variants combine a nullable type without a name.