# JSON Link
I wrote this library to allow me to describe the mappings between a JSON class and that of C++ with the least amount of effort or intrusion.  The end result is easy serialization and deserialization with C++ and JSON.  One just needs to describe the mapping of JSON class member names to the types needed to construct a C++ class.


### Installing and Requirements
## Requirements
* C++ 17 compiler, C++ 20 for enhanced names
* git
* cmake
* GCC(8/9)/Clang(7) have been tested.  Have not yet tested on MSVC

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
## Generating classes automatically
Currently I have a another project https://github.com/beached/json_to_cpp that can output the data structures used in json data along with the code to use this library.

## Performance considerations
The order of the data in the data structures should generally match that of the json data.  The parser is much faster if it doesn't have to back track for values.  Optional values where they are missing in the json data can slow down the parsing too.  If possible have them sent as null.

## Differences between C++17 and C++20
# C++ 17 Naming of members
```cpp
static constexpr char const name_a[] = "name_a";

...json_number<name_a>
```
# C++ 20 Enhanced member naming
```cpp
...json_number<"member_name">
```

## Code Examples

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
    std::string json_data = R"({
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

    TestClass test_class = daw::json::from_json<TestClass>( json_data );
    std::vector<TestClass> arry_of_test_class = daw::json::from_json_array<json_class<no_name, TestClass>>( json_data );
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
inline auto describe_json_class( AggClass ) {
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
To enable serialization on must create an additional free function called ```to_json_data( T );``` It will provide a mapping from your type to the arguments provided in the class description.  To serialize to a json string, one calls ```to_json( value );``` where value is a registered type. Using the exmaple above lets add that

```cpp
#include <daw/json/daw_json_link.h>
#include <tuple>

struct AggClass {
	int a{};
	double b{};
};
inline auto describe_json_class( AggClass ) {
	using namespace daw::json;
	return class_description_t<
		json_number<"a", int>,
		json_number<"b">
	>{};
}
inline auto to_json_data( AggClass const & ) {
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

### json_nullable
```cpp
template<typename JsonMember>
struct json_nullable
```
Marks a json member(see below) as nullable.  This includes being missing or having a json value of null

### json_number
```cpp
template<JSONNAMETYPE Name, 
    typename T = double, 
	LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
    typename Constructor = daw::construct_a<T>>
struct json_number
``` 
The defaults for json_number will construct a ```double``` with the supplied name.  However, there are some optimization with using an exact match arithmetic type like a float or int.  Integer parsing is faster than floating point if you know you will always get whole numbers.
```LiteralAsString``` allow specifying if the number is stored as a string.  Values are ```LiteralAsStringOpt::never```, ```LiteralAsStringOpt::maybe```, and ```LiteralAsStringOpt::always```.
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_bool
```cpp
template<JSONNAMETYPE Name, 
    typename T = bool, 
    typename Constructor = daw::construct_a<T>>
struct json_bool
``` 
The defaults for json_bool will construct a ```bool``` with the supplied name.  The resulting type T must be constructable from a bool.
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_string
```cpp
template<JSONNAMETYPE Name, 
    typename T = std::string, 
    typename Constructor = daw::construct_a<T>,
    bool EmptyStringNull = false>
struct json_string
``` 
The defaults for json_string will construct a ```std::string``` with the supplied name.  The resulting type T must be constructable from two arguments(a ```char const *``` and a ```size_t```).
```Constructor``` the default is almost always correct here but this will constuct your type.
```EmptyStringNull``` treat an empty JSON value ```""``` as a null value.

### json_date
```cpp
template<JSONNAMETYPE Name, 
    typename T = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>, 
    typename Constructor = daw::construct_a<T>>
struct json_date
``` 
json_date is a special class for when a json string fields is known to have timestamp data.
The defaults for json_date will construct a ```std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>``` with the supplied name.  The resulting type T must be constructable from two arguments(a ```char const *``` and a ```size_t```).
```Constructor``` the default parses javascript timestamps.  Supplying a functor that takes the ptr, size combo will allow other result types and date formats.

### json_custom
```cpp
template<JSONNAMETYPE Name, 
    typename T,
    typename FromConverter = custom_from_converter_t<T>,
	typename ToConverter = custom_to_converter_t<T>>
struct json_custom
``` 
```json_enum``` allows one to map unusual types.  The FromConverter is fed the raw value that is in the json and returns a T.  The ToConverter outputs a string from the T value.
```FromConverter``` A class who's operator( ) take a string_view and returns a T;  The default calls from_string( daw::tag_t<T>, T ).
```ToConverter``` A class who's operator( ) is takes a T value and returns a string like type.  The default uses to_string( T ).


### json_class
```cpp
template<JSONNAMETYPE Name, 
    typename T,
    typename Constructor = daw::construct_a<T>>
struct json_class
``` 
```json_class``` requires a type argument T that is a type that has an overload of the ```describe_json_class( T )```.
```Constructor``` the default is almost always correct here but this will constuct your type.

### json_array
```cpp
template<JSONNAMETYPE Name, 
    typename Container, 
    typename JsonElement,
    typename Constructor = daw::construct_a<Container>,
    typename Appender = impl::basic_appender<Container>>
struct json_array 
```
```json_array```'s are unique in the types because they have more info that is needed.

```Container``` is the type of container the ```JsonElement```'s are placed into, such as ```std::vector```

```JsonElement``` one of the above json types with daw::json::no_name or "" as it's name(the name will not be used).  This is the item type in the sequence

```Constructor``` the default is almost always correct here but this will constuct your type.

```Appender``` the default is almost always correct here but this will append via insert( Value ) on the container

### json_key_value
```cpp
template<JSONNAMETYPE Name, 
    typename Container, 
    typename JsonValueType,
    typename JsonKeyType = json_string<no_name>,
    typename Constructor = daw::construct_a<Container>,
    typename Appender = impl::basic_kv_appender<Container>>
struct json_key_value
``` 
```json_key_value``` Is like both an array and a class. All the keys/member names are json strings.

```Container``` The result type to place all the KV pairs into.  By default must behave like a Map.

```JsonValueType``` One of the above json types with daw::json::no_name or "" as it's name(the name will not be used).  This is the value type in the KV pairs 

```JsonKeyType``` The underlying json value is string, so the json_ type must account for that in parsing, with daw::json::no_name or "" as it's name(the name will not be used).  This is the key type in the KV pairs 

```Constructor``` the default is almost always correct here but this will constuct your Continer.

```Appender``` the default is almost always correct here but this will append via insert( std::pair<Key, Value> ) on the container


