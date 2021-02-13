# Automated Code Generation

## This is currently out of date

What we need to begin
We need the [json_to_cpp](https://github.com/beached/json_to_cpp) tool. 

To build it
```bash
git clone https://github.com/beached/json_to_cpp.git
cd json_to_cpp
mkdir build
cd build
cmake ..
cmake --build . 
```

Now we will have an executable called `json_to_cpp` or `json_to_cpp.exe` that will allow us to convert a json file to C++ structures along with the JSON Link mappings.

For example, let's use [cookbook_class2.json](../test_data/cookbook_class2.json) from the [class.md](class.md) cookbook

```bash
json_to_cpp --infile=cookbook_class2.json
```

The program will output the following C++ code
```c++
#include <tuple>
#include <cstdint>
#include <string>
#include <daw/json/daw_json_link.h>

struct a_t {
	std::string member0;
	int64_t member1;
	bool member2;
};	// a_t

struct root_object_t {
	a_t a;
	int64_t b;
};	// root_object_t

namespace daw::json {
  template<>
  struct json_data_contract<a_t> {
    static constexpr char const mem_member0[] = "member0";
    static constexpr char const mem_member1[] = "member1";
    static constexpr char const mem_member2[] = "member2";
    using type = json_member_list<
        json_string<mem_member0>,
        json_number<mem_member1, int64_t>,
        json_bool<mem_member2>>;

    static inline auto to_json_data( a_t const &value ) {
      return std::forward_as_tuple( value.member0, value.member1,
                                    value.member2 );
    }
  };
}

namespace daw::json {
  template<>
  struct json_data_contract<root_object_t> {
  	static constexpr char const mem_a[] = "a";
  	static constexpr char const mem_b[] = "b";
  	using type =
  	  json_member_list<
  	    json_class<mem_a, a_t>, 
  	    json_number<mem_b, int64_t>>;

  	static inline auto to_json_data( root_object_t const &value ) {
  	  return std::forward_as_tuple( value.a, value.b );
  	}
  };
}
```

As you can see it is pretty close to the types specified, except in this case for integrals it defaults to int64_t as it cannot know that it was an unsigned.

We can make it prettier as the default will use root_object_t as the type of the root class and the other objects are the name of their members suffixed with `_t`

Let's give the root object a name
```bash
json_to_cpp --root_object=MyClass2 --infile=cookbook_class2.json
```

The tool was used to generate the citm, twitter, and canada json examples as the json data is large.

One other option is we can specify members that are kv class pairs where the keys are the member names and values the associated values.  This cleans up the generated code substantially in many cases.  The path is specified as `member.submember.subsubmember...`
 

