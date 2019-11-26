# JSON Link ![](https://github.com/beached/daw_json_link/workflows/MacOS/badge.svg) ![](https://github.com/beached/daw_json_link/workflows/Ubuntu/badge.svg) ![](https://github.com/beached/daw_json_link/workflows/Windows/badge.svg)

This library provides serialization/deserialization of JSON documents with a known structure into a C++ typed structure.

The library is [MIT](LICENCE) licensed so its free to use, just have to give credit.

By using the know data structures in the JSON data we will be able to parse json as simply as 
```cpp
MyThing thing = from_json<MyThing>( data );
```
or
```cpp
std::vector<MyThing> things = from_json_array<json_class<no_name, MyThing>>( data2 );
```
## Code Examples
* The  [Cookbook](cookbook/) section has precanned tasks and working code examples
* [Tests](tests/) provide another source of working code samples. 
* Small samples below

## Api and Data Structures
See the [api.md](api.md) document

## Installing

The following will build and run the tests.  Windows is close but uses `md` instead of `mkdir` to make the build folder
```
git clone https://github.com/beached/daw_json_link
cd daw_json_link
mkdir build
cd build
cmake ..
cmake --build . --target full -j 2
ctest -C Debug
```
After the build there the examples can be tested.  ```city_test_bin``` requires the path to the cities json file.
```
./city_test_bin ../test_data/cities.json
```

## Performance considerations
The order of the data in the data structures should generally match that of the json data.  The parser is much faster if it doesn't have to back track for values.  Optional values where they are missing in the json data can slow down the parsing too.  If possible have them sent as null.

## Escaping/Unescaping of member names
The library will not escape or unescape the member names.  This is a design desision as the current architecture would make it difficult.  Post C++20 this may be doable as one can construct the string as a NTTP and encode it there.  In addition, one can put the escaped name as the name manually.

## Differences between C++17 and C++20
# C++ 17 Naming of members
```cpp
static constexpr char const member_name[] = "memberName";

...json_number<member_name>
```
# C++ 20 Enhanced member naming
```cpp
...json_number<"memberName">
```

# Using data types
Once a data type has been described, you can easily construct an object from a string or string_view.

```C++
auto my_class = from_json<MyClass>( json_str );
```
Alternatively, if the input is trusted you can called the less checked version
```C++
auto my_class = from_json_trusted<MyClass>( json_str );
```

Or one can create a collection of your object from a JSON array

```C++
auto my_data = from_json_array<json_class<no_name, MyClass>>( json_str );
```
Alternatively, if the input is trusted you can called the less checked version
```C++
auto my_data = from_json_array_trusted<json_class<no_name, MyClass>>( json_str );
```

If you want to work from JSON array data you can get an iterator and use the std algorithms too

```C++
using iterator_t = daw::json::json_array_iterator<json_class<no_name, MyClass>>;
auto pos = std::find( iterator_t( json_str ), iterator_t( ), MyClass( ... ) );
```
Alternatively, if the input is trusted you can called the less checked version
```C++
using iterator_t = daw::json::json_array_iterator_trusted<json_class<no_name, MyClass>>;
auto pos = std::find( iterator_t( json_str ), iterator_t( ), MyClass( ... ) );
```

If you want to serialize to JSON 

```C++
std::string my_json_data = to_json( MyClass{} );
```

Or serialize a collection of things
```C++
std::vector<MyClass> arry = ...;
std::string my_json_data = to_json_array( arry );
```
# Error Handling
## Parsing call
With error checking enabled globally, you can now designate a parsing call as trusted by calling the _trusted variant.  `from_json_trusted`, `from_json_array_trusted`, and `json_array_iterator_trusted`.  These paths are unchecked beyond missing non-nullable members.  The performance difference is from around 5%-15% in my testing.
## Global
There are two possible ways of handling errors.  The first, `abort( );` on an error in data.  Or the, second, throw a `daw::json::json_exception`.  json_exception has a member function `std::string_view reason( ) const`.  You can control which method is used by defining `DAW_JSON_DONT_USE_EXCEPTIONS` to make code noexcept.  In addition, you can control if the checks are only done in only debug mode `DAW_JSON_CHECK_DEBUG_ONLY`. In some cases, exporting strings the underlying libraries may throw too. However, the codebase is designed to work around -fno-exceptions and current will abort on error in those cases 
# Deserializing/Parsing
This can be accomplished by writing a function called describe_json_class with a single arugment that is your type.  The library is only concerned with it's return value. For example:

```C++
#include <daw/json/daw_json_link.h>

struct TestClass {
	int i = 0;
	double d = 0.0;
	bool b = false;
	daw::string_view s{};
	std::vector<int> y{};

	TestClass( int Int, double Double, bool Bool, daw::string_view S,
	            std::vector<int> Y ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , s( S )
	  , y( Y ) {}
};

auto describe_json_class( TestClass ) {
	using namespace daw::json;
	return class_description_t<
		json_number<"i", int>,
		json_number<"d">,
		json_bool<"b">,
		json_string<"s", daw::string_view>,
		json_array<"y", std::vector<int>, json_number<no_name, int>>
 	>{};
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
    std::vector<TestClass> arry_of_test_class = daw::json::from_json_array<json_class<no_name, TestClass>>( test_001_t_json_data );
}
```
Both aggregate and normal construction is supported.  The description provides the values needed to construct your type and the order.  The order specified is the order they are placed into the constructor.  There are customization points to provide a way of constructing your type too(TODO discuss customization points)  A class like:

```cpp
#include <daw/json/daw_json_link.h>

struct AggClass {
	int a{};
	double b{};
};
auto describe_json_class( AggClass ) {
	using namespace daw::json;
	return class_description_t<
		json_number<"a", int>,
		json_number<"b">
	>{};
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
auto describe_json_class( AggClass ) {
	using namespace daw::json;
	static constexpr char const a[] = "a";
	static constexpr char const b[] = "b";
	return class_description_t<
		json_number<a, int>,
		json_number<b>
	>{};
}
```
The class descriptions are recursive with their submembers.  Using the previous `AggClass` one can include it as a member of another class
```cpp
// See above for AggClass
struct MyClass {
    AggClass other;
    std::string_view some_name;
};
auto describe_json_class( MyClass ) {
    using namespace daw::json;
    return class_description_t<
        json_class<"other", AggClass>,
        json_string<"id", std::string_view>
    >{};
}
```
The above maps a class MyClass that has another class that is described AggClass.  Also, you can see that the member names of the C++ class do not have to match that of the mapped json names and that strings can use `std::string_view` as the result type.  This is an important performance enhancement if you can guarantee the buffer containing the json file will exist as long as the class does.

Iterating over JSON arrays.  The input iterator ```daw::json::json_array_iterator<JsonElement>``` allows one to iterator over the array of JSON elements.  It is technically an input iterator but can be stored and reused like a forward iterator.  It does not return a reference but a value.
```cpp

#include <daw/json/daw_json_link.h>

struct AggClass {
	int a{};
	double b{};
};
auto describe_json_class( AggClass ) {
	using namespace daw::json;
	return class_description_t<
		json_number<"a", int>,
		json_number<"b">
	>{};

int main( ) {
	std::string json_array_data = R"([
	        {"a":5,"b":2.2},
	        {"a":5,"b":3.14},
	        {"a":5,"b":0.122e44},
	        {"a":5334,"b":34342.2}
	       ])";
	using iterator_t = daw::json::json_array_iterator<JsonElement<no_name, AggClass>>;
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
To enable serialization on must create an additional free function called ```to_json_data( T );``` It will provide a mapping from your type to the arguments provided in the class description.  To serialize to a json string, one calls ```to_json( value );``` where value is a registered type.  The result of  to_json_data( T ) is a tuple who's arguments match that of the order in describe_json_class. Using the exmaple above lets add that

```cpp
#include <daw/json/daw_json_link.h>
#include <tuple>

struct AggClass {
	int a{};
	double b{};
};
auto describe_json_class( AggClass ) {
	using namespace daw::json;
	return class_description_t<
		json_number<"a", int>,
		json_number<"b">
	>{};
}
auto to_json_data( AggClass const & ) {
    return std::forward_as_tuple( c.a, c.b );
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
* GCC(8/9)/Clang(7/8) have been tested.  
* MSVC 19.21 has been tested. Must copy `glean.cmake.renamewin` to `glean.cmake` prior to running cmake.  
# For building tests
* git
* cmake

Darrell Wright

json_link@dawdevel.ca