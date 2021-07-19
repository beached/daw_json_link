# Dates and Times

Timestamps are common in JSON datasets. DAW JSON Link has a convenience type to serialize/deserialize ISO8601 timestamps.

## ISO 8601 Timestamps

```json
{
  "name": "Toronto",
  "timestamp": "2020-01-02T01:34:34.443Z"
}
```

The above JSON object consists of a 2 string members, where the second stores an [ISO8601](https://en.wikipedia.org/wiki/ISO_8601#Combined_date_and_time_representations) combined representation timestamp.

Too see a working example using this code, refer to [cookbook_dates1_test.cpp](../tests/src/cookbook_dates1_test.cpp)

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

The above json is like the first example, but the timestamp format is not iso8601.  It is the date format used in some twitter JSON apis.  The string member will be a `json_string` and the timestamp will use `json_custom`.  This example is more involved and also outlines using `json_custom` to work with data that does not fit the other mappings.

`json_custom` requires a callable FromJsonConverter and a callable ToJsonConverter type to convert to `T` from a `std::string_view` and to a string like type from a `T`.

The To and From Converters can be the same type with different overloads for `operator()`.  T will be a `std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>`.

Too see a working example using this code, refer to [cookbook_dates2_test.cpp](../tests/src/cookbook_dates2_test.cpp)

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
  struct json_data_contract<MyClass2> {
    using type = json_member_list<
      json_string<"name">,
      json_custom<"timestamp",
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>,
        TimestampConverter,
        TimestampConverter
      >
    >;

    static inline auto to_json_data( MyClass2 const & v ) {
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

Too see a working example using this code, refer to [cookbook_dates3_test.cpp](../tests/src/cookbook_dates3_test.cpp)

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
  struct json_data_contract<MyClass3> {
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

### Microsoft AJAX DateTime.

In some older MS implementations of AJAX, dates were encoded like
```json
"/Date(725842800000+0100)/"
```
where the first number is the number of seconds since UNIX epoch, and then, optionally, followed by an offset.

Taking the first example, modified the time to use the MS AJAX format, the timestamp only has 1s resolution and is a number.

```json
{
  "name": "Toronto",
  "timestamp": "/DATE(1577928874)/"
}
```

When parsing the `timestamp` member we need to skip the first 6 characters, optionally verifying it says `"/DATE(`. Then parse the number and add it to a `std::chrono` timestamp with the time of Unix Epoc, Midight January 1, 1970.

Too see a working example using this code, refer to [cookbook_dates4_test.cpp](../tests/src/cookbook_dates4_test.cpp)

```c++
using timepoint_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
struct TimestampConverter {
 timepoint_t operator( )( std::string_view sv ) const {
    auto sv2 = daw::string_view( sv.data( ), sv.size( ) );
    daw::string_view const prefix = "/DATE(";
    daw::string_view const suffix = ")/";
    daw_json_assert( sv2.starts_with( prefix ), "Unexpected date format" );
    daw_json_assert( sv2.ends_with( suffix ), "Unexpected date format" );
    sv2.remove_prefix( prefix.size( ) );
    sv2.remove_suffix( suffix.size( ) );

    int64_t val =
      daw::json::from_json<int64_t, daw::json::NoCommentSkippingPolicyChecked,
                           true>( sv2 );
    const auto epoch =
      daw::json::datetime::civil_to_time_point( 1970, 1, 1, 0, 0, 0, 0 );

    return epoch + std::chrono::seconds( val );
}

// Same class as above, but the JSON encoding differs
struct MyClass4 {
  std::string name;
  timepoint_t timestamp;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass4> {
    using type = json_member_list<
      json_string<"name">,
      json_custom<"timestamp",
        timepoint_t,
        TimestampConverter,
        TimestampConverter
      >
    >;

    static inline auto to_json_data( MyClass4 const & v ) {
      return std::forward_as_tuple( v.name, v.timestamp );
    }
  };
}
// ...
timepoint_t my_timepoint = daw::json::from_json<MyClass4>( json_document );
```
