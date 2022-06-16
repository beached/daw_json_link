Many types can be deduced and not explicitly mapped to a JSON type(string, number, bool, object, array)
This allows one to use them directly in places that don't require a member name(e.g. json_array's element type) or with
the `json_link<Name, Type>` mapping type. The order of deduction is as follows

* types with existing json_data_contract's specialized for them
* Well known types

  | Type                  | Mapped To        | Notes |
-----------------------|---------------------|------------------|-------|
  | std:string_view       |json_string_raw   |
  | std::string           |json_string       |
  | bool                  |json_bool         |
  | Integer               |json_number       |Uses std::numeric_limits. json_number optimized for Signed/Unsigned integers|
  | Enum                  |json_number       |Used std::is_enum and std::underlying_type
  | Floating point        |json_number       |Uses std::numeric_limits
  | Associative Container |json_key_value_map|Has begin()/end()/key_type/mapped_type and constructable with two iterators
  | readable values       |                  |The value_type in the readable mapping of T with a json_null wrapped around T's deduced mapping. See the readable value cookbook item
  | Containers            |json_array        |Excluding associative containers. Uses value_type as the type for each element

* Containers - map to json_array with the element type as the detected type of the value_type. Must have the methods
  begin(), end(), type alias value_type, and can be constructed with two iterators. Same Iterator requirements as std::
  vector's two iterator constructor.

  
