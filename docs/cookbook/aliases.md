### json_type_alias

It is sometimes desirable to map a class to be parsed like that of another type of mapping or another class. This, can
allow parsing the json data as a number and constructing a class. There are two parts, first the type alias describes what parser to use that will allow construction of the type. The `to_json_data` static function will return a value of the type that was parsed to.

```cpp
// A basic type that wraps an integer
class MyType {
    int value;

   public:
    MyType( int i ) : value( i ) {}

    int const& get() const { return value; }
};

namespace daw::json {
template <>
struct json_data_contract<MyType> {
    // Tell JSON Link that MyType will be constructible from and parsed as if it was an int.  This could also be json_number_no_name<int>
    using type = json_type_alias<int>;

    // Extract the int so that it can be serialized as if it was the type being serialized
    static inline int to_json_data(MyType const& v) {
        return v.get();
    }
};
}  // namespace daw::json
```

Another example is we want to store a GNU MP Rational type `mpq_class::mpq_class` as a string and use it's constructor to
parse the data.

```cpp
namespace daw::json {
  template<>
  struct json_data_contract<mpq_class::mpq_class> {
    using type = json_type_alias<std::string>;
    
    static inline auto to_json_data( mpq_class::mpq_class const & v ) {
        return return v.get_str( );
    }
  };
}
```

The data for this mapping could look like "12345/56789"

The
file [cookbook_aliases1_test.cpp](../../tests/src/cookbook_aliases1_test.cpp)
shows an example share a type `MyClass` has a single member of type `std::string` that is serialized and deserialized as
a string.