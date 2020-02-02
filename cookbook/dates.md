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

### Unix time encoding

A time_point can be encoded into JSON a the number of seconds since epoch(Jan 1, 1970 @ 12:00:00am ).

```json
{
 "title":"The Title",
 "id":24,
 "dateAdded":"1356753810000000",
 "lastModified":1356753810000000
}
```

This example will use the constructor of the class to construct the `time_point` from the integer.  Otherwise, a `json_custom` type could be used here too.  It demonstrates using a constructor to do the data conversions, along with `to_json_data` to reverse the conversion.  Another alternative is to use the `Constructor` template argument to do the conversion of the integer to the `time_point`.  The `dateAdded` member shows parsing strings into numbers.  This is often done as numbers in JSON are double and can only hold integers as large as 2^53.

```c++
using my_timepoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

my_timepoint seconds_since_epoch_to_tp( int64_t seconds ) {
  return my_timepoint{} + std::chrono::seconds( seconds );
}

struct MyClass3 {
  std::string title;
  unsigned id;
  my_timepoint date_added;
  my_timepoint last_modified;

  MyClass3( std::string Title, unsigned Id, int64_t DateAdded,
    int64_t LastModified )
    : title( Title )
    , id( Id )
    , date_added( seconds_since_epoch_to_tp( DateAdded ) )
    , last_modified( seconds_since_epoch_to_tp( LastModified ) ) {}
};

bool operator==( MyClass3 const &lhs, MyClass3 const &rhs ) {
  return std::tie( lhs.title, lhs.id, lhs.date_added, lhs.last_modified ) == std::tie( rhs.title, rhs.id, rhs.date_added, rhs.last_modified );
}

namespace daw::json {
  template<>
  struct json_data_contract<daw::cookbook_dates3::MyClass3> {
    using type = json_member_list<
      json_string<"title">, 
      json_number<"id", unsigned>,
    json_number<"dateAdded", int64_t, LiteralAsStringOpt::Always>,
    json_number<"lastModified", int64_t>>;

  static inline auto to_json_data( daw::cookbook_dates3::MyClass3 const &v ) {
    auto const date_added = std::chrono::floor<std::chrono::seconds>( v.date_added ).time_since_epoch( ).count( );
    auto const last_modified = std::chrono::floor<std::chrono::seconds>( v.last_modified ).time_since_epoch( ).count( );
    return std::tuple( std::as_const( v.title ), v.id, date_added, last_modified );
  };
} // namespace daw::json
```
