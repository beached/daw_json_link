![logo image](images/json_link_logo_128.png)
# JSON Link 

[![Build Status Macos](https://github.com/beached/daw_json_link/workflows/MacOS/badge.svg)](https://github.com/beached/daw_json_link/actions?query=workflow%3AMacOS)

[![Build Status Ubuntu](https://github.com/beached/daw_json_link/workflows/Ubuntu/badge.svg)](https://github.com/beached/daw_json_link/actions?query=workflow%3AUbuntu)

[![Build Status Windows](https://github.com/beached/daw_json_link/workflows/Windows/badge.svg)](https://github.com/beached/daw_json_link/actions?query=workflow%3AWindows)

[![Build Status Arm64 Linux](https://travis-ci.com/beached/daw_json_link.svg?branch=release)](https://travis-ci.com/beached/daw_json_link) - Arm64/PPC64LE/S390X Linux

This library provides serialization/deserialization of JSON documents with a known structure into a C++ object.   In addition, it supports iterating and browsing the document or delayed loading of members.

The library is using the [BSL](LICENSE) licensed

Because the structure of the JSON document is known, parsing is like the following 
```c++
MyThing thing = from_json<MyThing>( string );
```
or for array documents 
```c++
std::vector<MyThing> things = from_json_array<MyThing>( json_string2 );
```
If the structure of the JSON document is unknown, one can construct a `json_value` that acts as a container and allows iteration and parsing on demand.  The following is an example
```c++
json_value val = json_value( json_string );
```

## Code Examples
* The  [Cookbook](cookbook/readme.md) section has precanned tasks and working code examples
* [Tests](tests/) provide another source of working code samples. 
* Small samples below

## Content 
* [Intro](#intro)
  * [Default Mapping of Types](#default-mapping-of-types)
* [API](api.md) - Member mapping classes
* [Cookbook](cookbook/readme.md) Get cooking and putting it all together 
  * [Arrays](cookbook/array.md)
  * [Classes](cookbook/class.md)
  * [Class from Array](cookbook/class_from_array.md)
  * [Dates](cookbook/dates.md)
  * [Enums](cookbook/enums.md)
  * [Graphs](cookbook/graphs.md)
  * [Key Values](cookbook/key_values.md) - Map and Dictionary like things
  * [Numbers](cookbook/numbers.md)
  * [Optional/Nullable Values](cookbook/optional_values.md)
  * [Parsing Individual Members](cookbook/parsing_individual_members.md)
  * [Parser Policies](cookbook/parser_policies.md)
  * [Strings](cookbook/strings.md)
  * [Unknown JSON and Delayed Parsing](cookbook/unknown_types_and_delayed_parsing.md) - Browsing the JSON Document and delaying of parsing of specified members
  * [Variant](cookbook/variant.md)
  * [Automatic Code Generation](cookbook/automated_code_generation.md)
* [Intro](#intro)
* [Installing](#installing)
* [Performance considerations](#performance-considerations)
  * [Benchmarks](#benchmarks)
* [Escaping/Unescaping of member names](#escapingunescaping-of-member-names)
* [Differences between C++17 and C++20](#differences-between-c17-and-c20)
  * [C++ 17 Naming of members](#c-17-naming-of-json-members)
  * [C++ 20 Naming of members](#c-20-naming-of-json-members)
* [Using data types](#using-mapped-data-types)
* [Error Handling](#error-handling)
  * [Parsing call](#parsing-call)
  * [Global](#global)
* [Deserializing/Parsing](#deserializingparsing)
  * [Member Paths](#member-paths) 
* [Serialization](#serialization)
* [Build Configuration Points](#build-configuration-points)
* [Requirements](#requirements)
  * [For building tests](#for-building-tests)
* [Limitations](#limitations)

## Intro 
###### [Top](#content)

JSON Link allows serializing and deserializing of C++ data types and JSON using a predefined schema.   The underlying premise is the constructor of C++ data structures can be called with the JSON object's members.  The parsers goal isn't conformance.  It should be stricter in many ways, but somethings will not be checked(e.g. trailing commas).  The serializer is attempting to be conformant and generates minimal JSON. 
Mapping of data structures is done by specializing ```daw::json::json_data_contract``` for type ```T```.
There are two parts to the trait `json_data_contract`, first is a type alias named ```type``` that maps JSON object members by name to the argumenets of the C++ data structures constructor. Second, an optional, static method with a signatures like ```static tuple<MemberTypes> to_json_data( T const & )``` which returns a tuple of calculated for referenced members corresponding to the previous mapping.  `to_json_data` is only required if serialization is wanted. 
For example a `json_data_contract` for a `Coordinate` class could look like 
```c++
namespace daw::json {
  template<>
  struct json_data_contract<Coordinate> {
    using type = json_member_list<
      json_number<"lat">, 
      json_number<"lng">
    >;
  };
}
```
This says that there is a json object with members `lat` and `lng` that are numbers.  The default mapping for numbers is `double`, but one can benefit from specifying and restring that e.g. `int`.  The Coordinate class would need to be constructable from two double's.

To allow for serializing, the `to_json_data` method takes an existing C++ object and breaks out the values to the serializer.  
```c++
namespace daw::json {
  template<>
  struct json_data_contract<Coordinate> {
    // Same as previous example
    using type = json_member_list<
      json_number<"lat">, 
      json_number<"lng">
    >;

    static inline auto to_json_data( Coordinate const & c ) {
      return std::forward_as_tuple( c.latitude, c.longitude ); 
    }
  };
}
```
 * Note: The return type of `to_json_data` does not have to return references to the existing object members, but can return calculated values.

The parsers work by constructing each argument in place to the constructor of a class.  The individual argument parsers can be tuned for the specified circumstances of the data(e.g. floating point and integral numbers).  Then with our type trait defining the arguments needed to construct the c++ class and their order we are able to look at each member in the JSON.  Now we construct the value with the result of each parser; similar to `T{ parse<0, json_string<"name">>( data ), parse<1, json_number<"age", unsigned>>( data ), parse<json_number<2, "number>>( data )}`. For each member, the data stream will be moved forward until we find the member we need to parse, storing interested locations for later too.  This process allows us to parse other classes as members too via the `json_class<"member_name", Type>` mapping type.  So that each mapping trait only has to deal with it's specific members and not their details.
![general parsing flow](images/parse_flow.jpg)

## Default mapping of types
###### [Top](#content)

In unnamed contexts, array element types, some key value types, and variant element lists where the name would be `no_name`, one can use some native C++ data types instead of the the JSON member types.  This includes, integer, floating point, bool, std::string, and previously mapped types.  

For example, to map an array of string's.
```c++
template<>
struct daw::json::json_data_contract<MyType> {
  using type = json_member_list<json_array<"member_name", std::string>>;
};
```
 
## Installing
###### [Top](#content)

The following will build and run the tests. 
```bash
git clone https://github.com/beached/daw_json_link
cd daw_json_link
mkdir build
cd build
cmake ..
cmake --build . --target full -j 2
ctest -C Debug
```
After the build there the examples can be tested.  ```city_test_bin``` requires the path to the cities JSON file.
```bash
./city_test_bin ../test_data/cities.json
```

## Performance considerations
###### [Top](#content)

The order of the members in the data structures should generally match that of the JSON data.  The parser is faster if it doesn't have to back track for values.  Optional values, when missing in the JSON data, can slow down the parsing too.  If possible have them sent as null.  The parser does not allocate.  The parsed to data types may and this allows one to use custom allocators or a mix as their data structures will do the allocation.  The defaults for arrays is to use the std::vector<T> and if this isn't desireable, you must supply the type.

### Benchmarks
* [Kostya results](kostya_benchmark_results.md) using [test_dawjsonlink.cpp](tests/test_dawjsonlink.cpp)

![chart desribing kostya benmark results](images/kostya_bench_chart.png)

* [Parial NativeJson](partial_nativejson_benchmark.md)

## Escaping/Unescaping of member names
###### [Top](#content)

The library, currently, does not escape or unescape the member names.  This is a design desision as the current architecture would make it difficult.  Post C++20 this may be doable as one can construct the string as a NTTP and encode it there.  In addition, one can put the escaped name as the name manually.

## Differences between C++17 and C++20
###### [Top](#content)

There are slight differences between C++17 and C++20 
# C++ 17 Naming of JSON members
```c++
namespace daw::json {
  template<>
  struct json_data_contract<MyType> {
    static constexpr char const member_name[] = "memberName";
    using type = json_member_list<json_number<member_name>>;
  };
}
```
# C++ 20 Naming of JSON members
When compiled within C++20 compiler, in addition to passing a `char const *` as in C++17, the member names can be specified as string literals directly.
```c++
namespace daw::json {
  template<>
  struct json_data_contract<MyType> {
    using type = json_member_list<json_number<"member_name">>;
  };
}
```

# Using mapped data types
###### [Top](#content)

Once a data type has been mapped with a `json_data_contract`, the library provides methods to parse JSON to them

```c++
MyClass my_class = from_json<MyClass>( json_str );
```
Alternatively, if the input is trusted, the less checked version can be faster 
```c++
MyClass my_class = from_json_unchecked<MyClass>( json_str );
```

JSON documents with array root's use the `from_json_array` function to parse 
```c++
std::vector<MyClass> my_data = from_json_array<MyClass>( json_str );
```
Alternatively, if the input is trusted, the less checked version can be faster 
```c++
std::vector<MyClass> my_data = from_json_array_unchecked<MyClass>( json_str );
```

If you want to work from JSON array data you can get an iterator and use the std algorithms to
Iterating over array's in JSON data can be done via the `json_array_iterator`
```c++
using iterator_t = json_array_iterator<MyClass>;
auto pos = std::find( iterator_t( json_str ), iterator_t( ), MyClass( ... ) );
```
Alternatively, if the input is trusted you can called the less checked version
```c++
using iterator_t = daw::json::json_array_iterator_trusted<MyClass>;
auto pos = std::find( iterator_t( json_str ), iterator_t( ), MyClass( ... ) );
```

If you want to serialize to JSON 

```c++
std::string my_json_data = to_json( MyClass{} );
```

Or serialize a collection of things
```c++
std::vector<MyClass> arry = ...;
std::string my_json_data = to_json_array( arry );
```
# Error Handling
## Parsing call
###### [Top](#content)

With error checking enabled globally, you can now designate a parsing call as trusted by calling the _unchecked variant.  `from_json_unchecked`, `from_json_array_unchecked`, and `json_array_iterator_trusted`.  These paths are unchecked beyond missing non-nullable members.  The performance difference is from around 5%-15% in my testing.
## Global
###### [Top](#content)

There are two possible ways of handling errors.  The first, `abort( );` on an error in data.  Or the, second, throw a `daw::json::json_exception`.  json_exception has a member function `std::string_view reason( ) const`.  You can control which method is used by defining `DAW_JSON_DONT_USE_EXCEPTIONS` to make code noexcept.  In addition, you can control if the checks are only done in only debug mode `DAW_JSON_CHECK_DEBUG_ONLY`. In some cases, exporting strings the underlying libraries may throw too. However, the codebase is designed to work around -fno-exceptions and current will abort on error in those cases 
# Deserializing/Parsing
###### [Top](#content)

This can be accomplished by writing a function called json_data_contract_for with a single arugment that is your type.  The library is only concerned with it's return value. For example:

```c++
#include <daw/json/daw_json_link.h>

struct TestClass {
  int i = 0;
  double d = 0.0;
  bool b = false;
  daw::string_view s{};
  std::vector<int> y{};

  TestClass( int Int, double Double, bool Bool, daw::string_view S, std::vector<int> Y ) 
    : i( Int )
    , d( Double )
    , b( Bool )
    , s( S )
    , y( Y ) {}
};

namespace daw::json {
  template<>
  struct json_data_contract<TestClass> {
  using type = json_member_list<
    json_number<"i", int>,
    json_number<"d">,
    json_bool<"b">,
    json_string<"s", daw::string_view>,
    json_array<"y", int>
   >;
  };
}

int main( ) {
  std::string test_001_t_json_data = R"({
    "i":5,
    "d":2.2e4,
    "b":false,
    "s":"hello world",
    "y":[1,2,3,4] 
    })";
  std::string json_array_data = R"([{
    "i":5,
    "d":2.2e4,
    "b":false,
    "s":"hello world",
    "y":[1,2,3,4] 
    },{
    "i":4,
    "d":122e4,
    "b":true,
    "s":"goodbye world",
    "y":[4,3,1,4] 
    }])";

  TestClass test_class = daw::json::from_json<TestClass>( test_001_t_json_data );
  std::vector<TestClass> arry_of_test_class = daw::json::from_json_array<TestClass>( test_001_t_json_data );
}
```
Both aggregate and normal construction is supported.  The description provides the values needed to construct your type and the order.  The order specified is the order they are placed into the constructor.  There are customization points to provide a way of constructing your type too(TODO discuss customization points)  A class like:

```c++
#include <daw/json/daw_json_link.h>

struct AggClass {
  int a{};
  double b{};
};

namespace daw::json {
  template<>
  struct json_data_contract<AggClass> {
    using type = json_member_list<
      json_number<"a", int>,
      json_number<"b">
    >;
  };
}
```
Works too.
Same but C++17
```c++
#include <daw/json/daw_json_link.h>

struct AggClass {
  int a{};
  double b{};
};

namespace daw::json {
  template<>
  struct json_data_contract<AggClass> {
    static inline constexpr char const a[] = "a";
    static inline constexpr char const b[] = "b";
    using type = json_member_list<
      json_number<a, int>,
      json_number<b>
    >;
  };
}
```
The class descriptions are recursive with their submembers.  Using the previous `AggClass` one can include it as a member of another class

```c++
// See above for AggClass
struct MyClass {
  AggClass other;
  std::string_view some_name;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyClass> {
    using type = json_member_list<
      json_class<"other", AggClass>,
      json_string<"id", std::string_view>
    >;
  };
}
```

The above maps a class MyClass that has another class that is described AggClass.  Also, you can see that the member names of the C++ class do not have to match that of the mapped JSON names and that strings can use `std::string_view` as the result type.  This is an important performance enhancement if you can guarantee the buffer containing the JSON file will exist as long as the class does.

Iterating over JSON arrays.  The input iterator ```daw::json::json_array_iterator<JsonElement>``` allows one to iterator over the array of JSON elements.  It is technically an input iterator but can be stored and reused like a forward iterator.  It does not return a reference but a value.
```c++

#include <daw/json/daw_json_link.h>

struct AggClass {
  int a{};
  double b{};
};

namespace daw::json {
  template<>
  struct json_data_contract<AggClass> {
    using type = json_member_list<
      json_number<"a", int>,
      json_number<"b">
    >;
  };
}

int main( ) {
  std::string json_array_data = R"([
    {"a":5,"b":2.2},
    {"a":5,"b":3.14},
    {"a":5,"b":0.122e44},
    {"a":5334,"b":34342.2}
     ])";
  using iterator_t = daw::json::json_array_iterator<AggClass>;
  auto pos = std::find_if( iterator_t( json_array_data ), iterator_t( ),
    []( AggData const & element ) { return element.b > 1000.0; } );
  if( pos == iterator_t( ) ) {
    std::cout << "Not found\n";
  } else {
    std::cout << "Found\n";
  }
}
```

## Member Paths
Parsing can begin at a specific member.  An optional member path to `from_json_array`, `from_json_array_unchecked`, `from_json_array`, or `from_json_array_unchecked` can be specified.
The format is a dot separated list of member names and optionally an array index such as `member0.member1` or `member0[5].member1`.

## Comments
Comments are supported when the parser policy for them is used.  Currently there are two forms of comment policies.  C++ style `//` and `/* */`.  Comments can be placed anywhere there is whitespace allowed

* Hash style
```
{ # This is a comment
    "a" #this is also a comment
      : "a's value"
}
```

* C++ style
```
{ // This is a comment
    "a" /*this is also a comment*/: "a's value"
}
```
To change the parser policy, you add another argument to `from_json` and call like `from_json<MyType, CppCommentParsePolicy>( json_data )`

## Serialization
###### [Top](#content)

To enable serialization on must create an additional free function called ```to_json_data( JsonClass );``` It will provide a mapping from your type to the arguments provided in the class description.  To serialize to a JSON string, one calls ```to_json( value );``` where value is a registered type.  The result of  to_json_data( JsonClass ) is a tuple who's arguments match that of the order in json_data_contract_for. Using the exmaple above lets add that

```c++
#include <daw/json/daw_json_link.h>
#include <tuple>

struct AggClass {
  int a{};
  double b{};
};

namespace daw::json {
  template<>
  struct json_data_contract<AggClass> {
    using type = json_member_list<
      json_number<"a", int>,
      json_number<"b">
    >;

    static inline auto to_json_data( AggClass const & value ) {
      return std::forward_as_tuple( value.a, value.b );
    }
  };
}
//...
AggData value = //...;
std::string test_001_t_json_data = to_json( value );

// or
std::vector<AggData> values = //...;
std::string json_array_data = to_json_array( values );
```

Alternatively there is an optional `iostreams` interface.  In you types `json_data_constract`
add a type alias named `opt_into_iostreams` the type it aliases doesn't matter, and include `daw_json_iostream.h` .  For example  
```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_iostream.h>
#include <tuple>

struct AggClass {
  int a{};
  double b{};
};

namespace daw::json {
  template<>
  struct json_data_contract<AggClass> {
    using opt_into_iostreams = void;
    using type = json_member_list<
      json_number<"a", int>,
      json_number<"b">
    >;

    static inline auto to_json_data( AggClass const & value ) {
      return std::forward_as_tuple( value.a, value.b );
    }
  };
}
//...
AggData value = //...;
std::cout << value << '\n';

// or
std::vector<AggData> values = //...;
std::cout << values << '\n';
```
A working example can be found at [daw_json_iostream_test.cpp](tests/daw_json_iostream_test.cpp) 

## Build configuration points
There are a few defines that affect how JSON Link operates
* `DAW_JSON_DONT_USE_EXCEPTIONS` - Controls if exceptions are allowed.  If they are not, an `abort()` on errors will occur
* `DAW_JSON_CHECK_DEBUG_ONLY` - Most checks are disabled in release mode
* `DAW_ALLOW_SSE3` - Allow experimental SSE3 mode
* `DAW_JSON_NO_CONST_EXPR` - This can be used to allow classes without move/copy special members to be constructed from JSON data prior to C++ 20.  This mode does not work in a constant expression prior to C++20 when this flag is no longer needed. 

## Requirements
###### [Top](#content)

* C++ 17 compiler 
* GCC(8/9)/Clang(7/8/9/10) have been tested.  
* MSVC 19.21 has been tested. 

### For building tests
* git
* cmake

### Quickly pulling into a project.  

Two other header only libraries are needed.  They exist in the `.glean` subfolder of the project already.  Alternatively 
When making tests they are automatically pulled in.  However, having the include folder of the following two projects is needed
* https://github.com/beached/header_libraries - `git clone --depth 1 https://github.com/beached/header_libraries.git`
* https://github.com/beached/utf_range - `git clone --depth 1 https://github.com/beached/utf_range.git`

#### Contact
Darrell Wright
json_link@dawdevel.ca

# Limitations
* When parsing classes, the first member with a mapped name will be used.  If you want to parse a class that can have more than one of any member by name, either parse as a `json_value` see or as a `json_key_value` that is mapped to a `std::multimap` or a `std::vector` with a pair of key type(`string`) and value type(s). [Cookbook Key Values](cookbook/key_values.md) demonstrates these methods. If a `json_key_value` is used and the mapped data type does not support duplicate keys, it will insert for each key.  This may result in the last item being the value reflected after serializing.  If the duplicate member is the tag type in a `json_tagged_variant`, it is undefined what the behaviour for parsing is.