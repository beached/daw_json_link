### json_type_alias

It is sometimes desirable to map a class to be parsed like that of another type of mapping or another class. This, can
allow parsing the json data as a number and constructing a class.

For instance, we want to store a GNU MP Rational type `mpq_class::mpq_class` as a string and use it's constructor to
parse the data.

```cpp
namespace daw::json {
  template<>
  struct json_data_contract<mpq_class::mpq_class> {
    using type = json_type_alias<std::string>;
    
    static inline auto to_json_data( mpq_class::mpq_class const & v ) {
        return std::tuple{ return v.get_str( ); }
    }
  };
}
```

The data for this mapping could look like "12345/56789"

The
file [cookbook_aliases1_test.cpp](https://raw.githubusercontent.com/beached/daw_json_link/release/tests/src/cookbook_aliases1_test.cpp)
shows an example share a type `MyClass` has a single member of type `std::string` that is serialized and deserialized as
a string.