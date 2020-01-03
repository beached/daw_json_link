# JSON Link ![](https://github.com/beached/daw_json_link/workflows/MacOS/badge.svg) ![](https://github.com/beached/daw_json_link/workflows/Ubuntu/badge.svg) ![](https://github.com/beached/daw_json_link/workflows/Windows/badge.svg)

This library provides serialization/deserialization of JSON documents with a known structure into a C++ typed structure.

The library is [MIT](LICENCE) licensed so its free to use, just have to give credit.

Because the struct of the document is known, parsing is like the following 
```cpp
MyThing thing = from_json<MyThing>( data );
```
or for array documents 
```cpp
std::vector<MyThing> things = from_json_array<MyThing>( data2 );
```
## Code Examples
* The  [Cookbook](cookbook/) section has precanned tasks and working code examples
* [Tests](tests/) provide another source of working code samples. 
* Small samples below

## Content 
* [Intro](#intro)
* [API](api.md) - Member mapping classes
* [Cookbook](cookbook/)
  * [Arrays](cookbook/array.md)
  * [Classes](cookbook/class.md)
  * [Enums](cookbook/enum.md)
  * [Graphs](cookbook/graphs.md)
  * [Key Values](cookbook/key_values.md) - Map and Dictionary like things 
  * [Optional/Nullable Values](cookbook/optional_values.md)
  * [Strings](cookbook/strings.md)
  * [Variant](cookbook/variant.md)
  * [Automatic Code Generation](cookbook/automated_code_generation.md)
* [Installing](#installing)
* [Performance considerations](#performance-considerations)
* [Escaping/Unescaping of member names](#escaping/unescaping-of-member-names)
* [Differences between C++17 and C++20](#differences-between-c++17-and-c++20)
* [C++ 17 Naming of members](#c++-17-naming-of-members)
* [C++ 20 Enhanced member naming](#c++-20-enhanced-member-naming)
* [Using data types](#using-data-types)
* [Error Handling](#error-handling)
  * [Parsing call](#parsing-call)
  * [Global](#global)
* [Deserializing/Parsing](#deserializing/parsing)
  * [Serialization](#serialization)
* [Installing and Requirements](#installing-and-requirements)
* [Requirements](#requirements)
  * [For building tests](#for-building-tests)

## Intro
JSON Link allows serializing and deserializing of arbitrary C++ data types to JSON.   The underlying premise is that C++ data structures can be constructed from the values in the JSON object. 
Mapping of data structures is done by specializing ```daw::json::json_data_contract``` for type ```T```.
There are two parts to the trait `json_data_contract`, first is a type alias named ```type``` that maps JSON object members by name to the argumenets of the C++ data structures constructor. Second, an optional, static method with a signatures like ```static tuple<MemberTypes> to_json_data( T const & )``` which returns a tuple of calculated for referenced members corresponding to the previous mapping.  `to_json_data` is only required if serialization is wanted. 

For example a `json_data_contract` for a `Coordinate` class could look like 
```cpp
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
```cpp
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
The return of to_json_data does not have to return a reference to the existing object, but can return calculated values too.
## Installing

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
The order of the members in the data structures should generally match that of the JSON data.  The parser is faster if it doesn't have to back track for values.  Optional values, when missing in the JSON data, can slow down the parsing too.  If possible have them sent as null.  The parser does not allocate.  The parsed to data types may and this allows one to use custom allocators or a mix as their data structures will do the allocation.  The defaults for arrays is to use the std::vector<T> and if this isn't desireable, you must supply the type.

## Escaping/Unescaping of member names
The library, currently, does not escape or unescape the member names.  This is a design desision as the current architecture would make it difficult.  Post C++20 this may be doable as one can construct the string as a NTTP and encode it there.  In addition, one can put the escaped name as the name manually.

## Differences between C++17 and C++20
# C++ 17 Naming of members
```cpp
namespace daw::json {
  template<>
  struct json_data_contract<MyType> {
    static constexpr char const member_name[] = "memberName";
    using type = json_member_list<json_number<member_name>>;
  };
}
```
# C++ 20 Enhanced member naming
```cpp
namespace daw::json {
  template<>
  struct json_data_contract<MyType> {
    using type = json_member_list<json_number<"member_name">>;
  };
}
```

# Using mapped data types
Once a data type has been described, you can easily construct an object from a string or string_view.

```cpp
MyClass my_class = from_json<MyClass>( json_str );
```
Alternatively, if the input is trusted you can called the less checked version
```cpp
MyClass my_class = from_json_unchecked<MyClass>( json_str );
```

Or one can create a collection of object's from a JSON array

```cpp
std::vector<MyClass> my_data = from_json_array<MyClass>( json_str );
```
Alternatively, if the input is trusted you can called the less checked version
```cpp
std::vector<MyClass> my_data = from_json_array_unchecked<MyClass>( json_str );
```

If you want to work from JSON array data you can get an iterator and use the std algorithms too

```cpp
using iterator_t = json_array_iterator<MyClass>;
auto pos = std::find( iterator_t( json_str ), iterator_t( ), MyClass( ... ) );
```
Alternatively, if the input is trusted you can called the less checked version
```cpp
using iterator_t = daw::json::json_array_iterator_trusted<MyClass>;
auto pos = std::find( iterator_t( json_str ), iterator_t( ), MyClass( ... ) );
```

If you want to serialize to JSON 

```cpp
std::string my_json_data = to_json( MyClass{} );
```

Or serialize a collection of things
```cpp
std::vector<MyClass> arry = ...;
std::string my_json_data = to_json_array( arry );
```
# Error Handling
## Parsing call
With error checking enabled globally, you can now designate a parsing call as trusted by calling the _unchecked variant.  `from_json_unchecked`, `from_json_array_unchecked`, and `json_array_iterator_trusted`.  These paths are unchecked beyond missing non-nullable members.  The performance difference is from around 5%-15% in my testing.
## Global
There are two possible ways of handling errors.  The first, `abort( );` on an error in data.  Or the, second, throw a `daw::json::json_exception`.  json_exception has a member function `std::string_view reason( ) const`.  You can control which method is used by defining `DAW_JSON_DONT_USE_EXCEPTIONS` to make code noexcept.  In addition, you can control if the checks are only done in only debug mode `DAW_JSON_CHECK_DEBUG_ONLY`. In some cases, exporting strings the underlying libraries may throw too. However, the codebase is designed to work around -fno-exceptions and current will abort on error in those cases 
# Deserializing/Parsing
This can be accomplished by writing a function called json_data_contract_for with a single arugment that is your type.  The library is only concerned with it's return value. For example:

```cpp
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

```cpp
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
```cpp
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

```cpp
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
```cpp

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

## Serialization
To enable serialization on must create an additional free function called ```to_json_data( JsonClass );``` It will provide a mapping from your type to the arguments provided in the class description.  To serialize to a JSON string, one calls ```to_json( value );``` where value is a registered type.  The result of  to_json_data( JsonClass ) is a tuple who's arguments match that of the order in json_data_contract_for. Using the exmaple above lets add that

```cpp
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

### Installing and Requirements
## Requirements
* C++ 17 compiler, C++ 20 for enhanced names
* GCC(8/9)/Clang(7/8/9) have been tested.  
* MSVC 19.21 has been tested. 
# For building tests
* git
* cmake

Darrell Wright

json_link@dawdevel.ca
