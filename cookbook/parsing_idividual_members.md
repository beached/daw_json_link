# Parsing Individual Members
Parsing single members from the JSON data is common.  This can be to discriminate on a single member or when only a single member is needed.

```json
{
  "member0": 55,
  "member1": [1,2,3,4,5],
  "member2": {
    "a": 123.4,
    "b": "found me"
  }
}
```

With the previous JSON data, the next example will extract the member `"a.b"`, the `"b"` member of the `"a"` member of the root object.

Too see a working example using this code, refer to [cookbook_parsing_individual_members1_test.cpp](../tests/cookbook_parsing_individual_members1_test.cpp). 
```c++
std::string b_value = daw::json::from_json<std::string>( json_data, "a.b" );
```

`b_value` should have a value of `"found me"`


Below shows when member is known to be an array and full control of the resulting collection is needed.

Too see a working example using this code, refer to [cookbook_parsing_individual_members2_test.cpp](../tests/cookbook_parsing_individual_members2_test.cpp). 
```c++
std::vector<int> values = daw::json::from_json_array<int>( json_data, "member1" );
```

## Specifying array indexes

In the member path array indexes are specified inside square brackets `[` and `]`.  Below the 3rd item of `"member1"` will be extracted 

Too see a working example using this code, refer to [cookbook_parsing_individual_members3_test.cpp](../tests/cookbook_parsing_individual_members3_test.cpp). 
```c++
int third_value = daw::json::from_json<int>( json_data, "member1[2]" );
```