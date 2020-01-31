# Dates and Times

Timestmaps are common in JSON datasets. DAW JSON Link has a convienience type to serialize/deserialize ISO8601 timestamps.

## Using json_date

```json
{
  "name": "Toronto",
  "timestamp": "2020-01-02T01:34:34.443Z"
}
```

The above JSON object consists of a 2 string members, where the second stores an [ISO8601](https://en.wikipedia.org/wiki/ISO_8601#Combined_date_and_time_representations) combinded representation timestamp.

Too see a working example using this code, refer to [cookbook_dates1_test.cpp](../tests/cookbook_dates1_test.cpp) 

Below is code that code serialize/deserialize the above JSON

```c++
struct MyClass {
	std::string name;
  std::chrono::timepoint<std::chrono::system_clock, std::chrono::milliseconds> timestamp;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass> {
    using type = json_member_list<
      json_string<"name">,
      json_date<"timestamp">
    >;

    static inline auto to_json_data( MyClass const & v ) {
    return std::forward_as_tuple( v.name, v.timestamp );
    }   
  };
}
```

