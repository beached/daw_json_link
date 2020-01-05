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

Now we will have an executable called json_to_cpp/json_to_cpp.exe that will allow us to convert a json file to C++ structures along with the descript_json_class/to_json_data methods.

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

namespace symbols_a_t {
	static constexpr char const member0[] = "member0";
	static constexpr char const member1[] = "member1";
	static constexpr char const member2[] = "member2";
}

inline auto json_data_contract_for( a_t ) {
	using namespace daw::json;
	return daw::json::json_member_list<
		json_string<symbols_a_t::member0>
		,json_number<symbols_a_t::member1, int64_t>
		,json_bool<symbols_a_t::member2>
	>{};
}

inline auto to_json_data( a_t const & value ) {
	return std::forward_as_tuple( value.member0, value.member1, value.member2 );
}

struct root_object_t {
	a_t a;
	int64_t b;
};	// root_object_t

namespace symbols_root_object_t {
	static constexpr char const a[] = "a";
	static constexpr char const b[] = "b";
}

inline auto json_data_contract_for( root_object_t ) {
	using namespace daw::json;
	return daw::json::json_member_list<
		json_class<symbols_root_object_t::a, a_t>
		,json_number<symbols_root_object_t::b, int64_t>
	>{};
}

inline auto to_json_data( root_object_t const & value ) {
	return std::forward_as_tuple( value.a, value.b );
}
```

As you can see it is pretty close to the types specified, except in this case for integrals it defaults to intmax_t as it cannot know that it was an unsigned.

We can make it prettier as the default will use root_object_t as the type of the root class and the other objects are the name of their members suffixed with `_t`

Let's give the root object a name
```bash
json_to_cpp --root_object=MyClass2 --infile=cookbook_class2.json
```

The tool was used to generate the citm, twitter, and canada json examples as the json data is large.

One other option is we can specify members that are kv class pairs where the keys are the member names and values the associated values.  This cleans up the generated code substantially in many cases.  The path is specified as `member.submember.subsubmember...`
 

