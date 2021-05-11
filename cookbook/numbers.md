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
To see a working example using this code, refer to [cookbook_numbers2_test.cpp](../tests/src/cookbook_numbers2_test.cpp) 
```c++
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

## Big Numbers, Rationals, ...
The parser supports parsing big numbers that model arithmetic types directly.  However, some types have built in serialization/deserialization and that is often more efficient.

The json_class_map lets us parse as if it was another json type.  Often this is json_string via std::string, but there is no restriction on it.  The result is passed to the constructor of the type being mapped.

For instance, if we have an array of large numbers we want to parse to boost::multiprecision::cpp_dec_float_100,
```json
[
	"54326789013892014531903492543267890138920145319034925432678901389201", 
	"7890138920145319034925432678907890138920145319034925432678901903492543267890"
]
```

To see a working example using this code, refer to [cookbook_numbers3_test.cpp](../tests/src/cookbook_numbers3_test.cpp)
The following code will map any boost multiprecision number to/from JSON strings.
```c++
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace daw::json {
  template<typename Backend, boost::multiprecision::expression_template_option
  ExpressionTemplates>
  struct json_data_contract<
  boost::multiprecision::number<Backend, ExpressionTemplates>> {

    using type = json_class_map<std::string>;

    static inline auto to_json_data( boost::multiprecision::number<Backend, ExpressionTemplates> const &value ) {
    	return value.str( );
    }
  };
} 

//...
std::vector<boost::multiprecision::cpp_dec_float_100> values = 
		daw::json::from_json_array<boost::multiprecision::cpp_dec_float_100>( json_string ); 
```


