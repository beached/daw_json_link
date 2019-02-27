# JSON Link v3

## Note: this requires C++20 which isn't released yet and is still early testing.
## Note2: Just backported to allow C++17.  Names must be static constexpr string literals with names, instead of bare string literals  e.g.
```cpp
static constexpr char const name_a[] = "name_a";

...json_number<name_a>
```

### Installing and Requirements
## Requirements
* C++ 17 compiler, C++ 20 for enhanced names
* git
* cmake
* GCC(8/9)/Clang(7) have been tested.  Have not yet tested on MSVC
* Curl for date library

## Installing(Linux/Mac commandline)

The following will build the tests
```
git clone https://github.com/beached/daw_json_link
cd ./daw_json_link
mkdir build
cd ./build
cmake ..
make full 
```
After the build there the examples can be tested.  ```city_test_bin``` requires the path to the cities json file.
```
./city_test_bin ../tests/cities.json
```

## Code Examples

#Deserializing/Parsing
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
		json_array<"y", std::vector<int>, json_number<"", int>>
 	>{};
}

std::string json_data = R"({
    "i":5,
    "d":2.2e4,
    "b":false,
    "s":"hello world",
    "y":[1,2,3,4] 
    })";
std::string json_array_data = ...;

TestClass test_class = daw::json::from_json<TestClass>( json_data );
std::vector<TestClass> arry_of_test_class = daw::json::from_json_array<json_class<"", TestClass>>( json_data );
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
	std::string json_array_data = ...;
	using iterator_t = daw::json::json_array_iterator<JsonElement<"", AggClass>>;
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
To enable serialization on must create an additional free function called ```to_json_data( T );``` It will provide a mapping from your type to the arguments provided in the class description.  To serialize to a json string, one calls ```to_json( value );``` where value is a registered type. Using the exmaple above lets add that

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
std::string json_data = to_json( value );

// or
std::vector<AggData> values = //...;
std::string json_array_data = to_json_array( values );
```

## Json Data Types
The json types match those of the underlying JSON data types from the standard.

### json_number
```cpp
template<JSONNAMETYPE Name, 
    typename T = double, 
    NullValueOpt Nullable = NullValueOpt::never,
    typename Constructor = daw::construct_a<T>>
struct json_number
``` 
The defaults for json_number will construct a ```double``` with the supplied name.  However, there are some optimization with using an exact match arithmetic type like a float or int.  Integer parsing is faster than floating point if you know you will always get whole numbers.
```NullValueOpt``` controls is nulls are allowed(this includes missing values)
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_bool
```cpp
template<JSONNAMETYPE Name, 
    typename T = bool, 
    NullValueOpt Nullable = NullValueOpt::never,
    typename Constructor = daw::construct_a<T>>
struct json_bool
``` 
The defaults for json_bool will construct a ```bool``` with the supplied name.  The resulting type T must be constructable from a bool.
```NullValueOpt``` controls is nulls are allowed(this includes missing values)
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_string
```cpp
template<JSONNAMETYPE Name, 
    typename T = std::string, 
    NullValueOpt Nullable = NullValueOpt::never,
    typename Constructor = daw::construct_a<T>>
struct json_string
``` 
The defaults for json_string will construct a ```std::string``` with the supplied name.  The resulting type T must be constructable from two arguments(a ```char const *``` and a ```size_t```).
```NullValueOpt``` controls is nulls are allowed(this includes missing values)
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_date
```cpp
template<JSONNAMETYPE Name, 
    typename T = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>, 
    NullValueOpt Nullable = NullValueOpt::never,
    typename Constructor = daw::construct_a<T>>
struct json_date
``` 
json_date is a special class for when a json string fields is known to have timestamp data.
The defaults for json_date will construct a ```std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>``` with the supplied name.  The resulting type T must be constructable from two arguments(a ```char const *``` and a ```size_t```).
```NullValueOpt``` controls is nulls are allowed(this includes missing values)
```Constructor``` the default parses javascript timestamps.  Supplying a functor that takes the ptr, size combo will allow other result types and date formats.

### json_class
```cpp
template<JSONNAMETYPE Name, 
    typename T,
    NullValueOpt Nullable = NullValueOpt::never,
    typename Constructor = daw::construct_a<T>>
struct json_date
``` 
```json_class``` requires a type argument T that is a type that has an overload of the ```describe_json_class( T )```.
```NullValueOpt``` controls is nulls are allowed(this includes missing values)
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_array
```cpp
template<JSONNAMETYPE Name, 
    typename Container, 
    typename JsonElement,
    NullValueOpt Nullable = NullValueOpt::never,
    typename Constructor = daw::construct_a<Container>,
    typename Appender = impl::basic_appender<Container>>
struct json_array 
```
```json_array```'s are unique in the types because they have more info that is needed.
```Container``` is the type of container the ```JsonElement```'s are placed into, such as ```std::vector```
```JsonElement``` one of the above json types with daw::json::no_name or "" as it's name(actually does not matter as it is ignored).  This is the item type in the sequence
```NullValueOpt``` controls is nulls are allowed(this includes missing values)
```Constructor``` the default is almost always correct here but this will constuct your type.


