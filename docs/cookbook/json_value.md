# Json Value

 The `json_value` type is a lazy pull parser. It provides lookup and extraction mechanisms for when the regular mappings cannot work or doing data exploration.  One such example is the [variant_on_members.cpp](../../tests/src/variant_on_members.cpp) where the structure of the member is used to determine which parser to use.  For types that use a tag or member to discriminate this isn't necessary as there are `json_variant_...` mappings for that.  The use of `json_value` is not encouraged except when in conjunction with the mappings such as the `json_raw` mapping when existing mappings do not work, or data discovery.


The general interface to `json_value` is via `operator[]`.  This can be used with a JSON Path to extract a new `json_value`.  e.g 

```cpp
auto type_jv = jv["value.type"];
assert( type_kv );
auto type = as<int>( type_jv );
```

The example extracts the "type" member of "value" and returns a new `json_value`.  If it is not found, the `operator bool` will evaluate to false.  It is also false if the "type" member had a value of `null`.  If one wants to know if it was found or not, `has_more( )` will return false as the return is a default constructed `json_value`.  Following that, one can use the `as` function to parse the value.  `from_json` will, also, work here.

One can also check the type of the current value via several members such as `is_class`, `is_null`, `is_array`, `is_number`, `is_string`, and `is_bool`.  There is also `type( )` member that returns an enum of the basic JSON types that can be switched over.

The `json_value` type is a range and one can iterate over classes or arrays.  The value_type is a `json_pair` type that has a `name` and a `value` member.  The name is an `optional<string_view>`.  When iterating over an array, the name will be `nullopt`.  The value member is a `json_value`.  The `json_pair`, also, fulfills the tuple protocol and can get used with structured bindings.

```cpp
for( auto [name,value] jp: my_jv_of_class ) {
    std::cout << "name:" << *name << " value: " << value.get_string_view( ) << '\n';
}
```

The following is equivalent 

```cpp
auto jv = other_jv[5]["a"]["b"][2];
```

```cpp
auto jv = other_jv["[5].a.b[2]"];
```