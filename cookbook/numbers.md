# Numbers

There is direct support for numbers in JSON of the form `integer`[.`fraction`][e`exponent`] .  Often we know the number we are parsing is more limited in form such as an unsigned integer, a signed integer, or a real number.

## Default, full parsing of JSON numbers (eg real numbers).
```json
{
  "member0": 1.23,
  "member1": 1,
  "member2": 1.2e3,
  "member3": 3e3
}
```
The above JSON object has 4 members, each of which are numbers.

Below is the C++ data structure and trait to map the structure to the JSON object.
To see a working example using this code, refer to [cookbook_numbers1_test.cpp](../tests/src/cookbook_numbers1_test.cpp) 
```c++
struct MyClass1 {
  double member0;
  double member1;
  double member2;
  double member3;
};
namespace daw::json {
  template<>
  struct json_data_contract<MyClass1> {
    using type = json_member_list<
      json_number<"member0">, 
      json_number<"member1">, 
      json_number<"member2">,
      json_number<"member3">
    >;

    static inline auto to_json_data( MyClass1 const &value ) {
      return std::forward_as_tuple( 
        value.member0, 
        value.member1,
        value.member2,
        value.member3 );
    }
  };
}
```

## Specific numeric type parsing
The parser supports parsing to specific types such as int32_t and uin64_t with range checking, if possible.

```json
{
  "member_unsigned": 12345,
  "member_signed": -12345
}
```
The above JSON object has 2 members, both numbers but one is expected to always be an unsigned, the other a signed integer

Below is the C++ data structure and trait to map the structure to the JSON object.
To see a working example using this code, refer to [cookbook_numbers1_test.cpp](../tests/src/cookbook_numbers2_test.cpp) 
```
struct MyClass2 {
  unsigned member_unsigned;
  signed member_signed;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass2> {
    using type = json_member_list<
      json_number<"member_unsigned", unsigned>,
      json_number<"member_signed", signed>
    >;

    static inline auto to_json_data( MyClass1 const &value ) {
      return std::forward_as_tuple( 
        value.member_unsigned, 
        value.member_signed );
    }
  };
}
```

