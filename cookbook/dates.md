# Dates and Times

Timestmaps are common in JSON datasets. DAW JSON Link has a convienience type to serialize/deserialize ISO8601 timestamps.

## ISO 8601 Timestamps 

```json
{
  "name": "Toronto",
  "timestamp": "2020-01-02T01:34:34.443Z"
}
```

The above JSON object consists of a 2 string members, where the second stores an [ISO8601](https://en.wikipedia.org/wiki/ISO_8601#Combined_date_and_time_representations) combinded representation timestamp.

Too see a working example using this code, refer to [cookbook_dates1_test.cpp](../tests/cookbook_dates1_test.cpp) 

Below is code that code serialize/deserialize the above JSON using the `json_date` class.

```c++
struct MyClass {
	std::string name;
  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> timestamp;
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

## Custom string formats

```json
{
  "name": "Toronto",
  "timestamp": "Sun Aug 31 00:29:15 +0000 2014"
}
```

The above json is like the first example, but the timestamp format is not iso8601.  It is the date format used in some twitter JSON apis.  The string member will be a `json_string` and the timestmap will use `json_custom`.  This example is more involved and also outlines using `json_custom` to work with data that does not fit the other mappings.

`json_custom` requires a callable FromConverter and a callable ToConverter type to convert to `T` from a `std::string_view` and to a string like type from a `T`.

The To and From Converters can be the same type with different overloads for `operator()`.  T will be a `std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>`.

Too see a working example using this code, refer to [cookbook_dates2_test.cpp](../tests/cookbook_dates2_test.cpp) 

```c++
struct TimestampConverter {
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
	    operator( )( std::string_view sv ) const {
      // ... See linked code
    }

	std::string_view operator( )( std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp ) const {
      // ... See linked code
    }
};

// Same class as above, but the JSON encoding differs
struct MyClass2 {
  std::string name;
  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> timestamp;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass> {
    using type = json_member_list<
      json_string<"name">,
      json_custom<"timestamp", 
          std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>, 
          TimestampConverter, 
          TimestampConverter
      >
    >;

    static inline auto to_json_data( MyClass const & v ) {
      return std::forward_as_tuple( v.name, v.timestamp );
    }   
  };
}
```

