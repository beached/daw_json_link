### Classes from array's

Sometimes the members of a class are encoded as a JSON array.  GeoJSON is an example of this.  

```json
[ 34.3434, 134.3434 ]
```

The above is the encoding of a Point in GeoJSON.
Too see a working example using this code, refer to [cookbook_class_from_array1_test.cpp](../tests/src/cookbook_class_from_array1_test.cpp) 

Below is a `Point` structure with two double members.  The mapping uses a `json_ordered_member_list` and can accept unnamed member items.  Below describes a mapping to an array's first and second elements.  Any subsequent members are ignored.
```c++
struct Point {
	double x;
	double y;
};

namespace daw::json {
  template<>
  struct json_data_contract<Point> {
    using type = json_ordered_member_list<double, double>;

    static constexpr auto to_json_data( Point const &p ) {
      return std::forward_as_tuple( Point.x, Point.y );
    }
  };
}
```

Sometimes the members to extract are not contiguous and a specific index into the array is required.  For this a member can be wrapped in a `ordered_json_member<size_t, JsonMember>` type. 

```json
[
  34.3434,
  "a",
  34343,
  {
    "a": 1244,
    "b": [
      1,
      2,
      3,
      4
    ]
  },
  [
    "a",
    5,
    "4"
  ],
  134.3434,
  "a"
]
```

With the previous `Point` struct, the mapping here will be to parse the first element as a double, and the sixth element as a double. 

Too see a working example using this code, refer to [cookbook_class_from_array2_test.cpp](../tests/src/cookbook_class_from_array2_test.cpp) 

```c++
struct Point {
	double x;
	double y;
};

namespace daw::json {
  template<>
  struct json_data_contract<Point> {
    using type = json_ordered_member_list<double, ordered_json_member<5,json_number<no_name>>>;

    static constexpr auto to_json_data( Point const &p ) {
      return std::forward_as_tuple( Point.x, Point.y );
    }
  };
}
```
