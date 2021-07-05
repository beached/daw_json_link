To parse/serialize raw JSON data, such as data from other libraries, one can use the `json_delayed` mapping type. The type requirement for mapping to `json_delayed` is constructable with a `char const *` and a `std::size_t`, along with supporting `std::begin`/`std::end`.

```c++
struct Foo {
  std::string bar;
  std::string raw_json;
};

namespace daw::json {
  template<>
  struct json_data_contract<Foo> {
    using type = json_member_list<
      json_link<"bar", std::string>, 
      json_delayed<"raw_json", std::string>
    >;
  
    static auto to_json_data( Foo const & v ) {
      return std::forward_as_tuple( v.bar, v.raw_json );
    }
  };
}
```
