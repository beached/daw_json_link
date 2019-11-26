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
  typename Constructor = daw::construct_a<T>
>
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
				 typename String = std::string,
	       typename Constructor = daw::construct_a_t<String>,
	       typename Appender = impl::basic_appender<String>,
	       bool EmptyStringNull = false,
	       bool DisallowHighEightBit = false>
struct json_string
``` 
json_string is fully processed and unescaped or escaped.
The defaults for json_string will construct a ```std::string``` with the supplied name.  The resulting type String must be default constructable.
```Constructor``` the default is almost always correct here but this will constuct your type.  Appender will append each character to the string as
they are processed via push_back or insert( end, ... ), depending on which method is available.
```EmptyStringNull``` treat an empty JSON value ```""``` as a null value.
```DisallowHighEightBit``` error if deserializing a json string and there are values > 0x7F or < 0x20 

### json_string_raw
```cpp
template<JSONNAMETYPE Name, 
    typename T = std::string, 
    typename Constructor = daw::construct_a<T>,
    bool EmptyStringNull = false,
	  bool DisallowHighEightBit = false>
struct json_string_raw
``` 
json_string_raw is whatever bytes are between the quotes in the string passed from json
The defaults for json_string_raw will construct a ```std::string``` with the supplied name.  The resulting type T must be constructable from two arguments(a ```char const *``` and a ```size_t```).
```Constructor``` the default is almost always correct here but this will constuct your type.
```EmptyStringNull``` treat an empty JSON value ```""``` as a null value.
```DisallowHighEightBit``` error if deserializing/serializing a json string and there are values > 0x7F or < 0x20

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
```json_custom``` allows one to map unusual types.  The FromConverter is fed the raw value that is in the json and returns a T.  The ToConverter outputs a string from the T value.
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
    typename JsonKeyType = json_string_raw<no_name>,
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

### json_key_value
```cpp
template<JSONNAMETYPE Name, 
    typename Container, 
    typename JsonValueType,
    typename JsonKeyType,
    typename Constructor = daw::construct_a<Container>,
    typename Appender = impl::basic_kv_appender<Container>>
struct json_key_value_array
``` 
```json_key_value``` Is like both an array and a class. All the keys/member names are json strings.

```Container``` The result type to place all the KV pairs into.  By default must behave like a Map.

```JsonValueType``` One of the above json types with a specified name that matches the value member name as it's name.  This is the value type in the KV pairs 

```JsonKeyType``` One of the above json types with a specified name that matches the key member name as it's name.  This is the key type in the KV pairs 

```Constructor``` the default is almost always correct here but this will constuct your Continer.

```Appender``` the default is almost always correct here but this will append via insert( std::pair<Key, Value> ) on the container


