## Json Data Types
The json types match those of the underlying JSON data types from the standard.

### json_number
```cpp
template<JSONNAMETYPE Name, 
  typename T = double,
  LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
  typename Constructor = daw::construct_a_t<T>,
  JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
  JsonNullable Nullable = JsonNullable::Never>
struct json_number
``` 
The defaults for ```json_number``` will construct a ```double``` with the supplied name.  However, there are some optimization with using an exact match arithmetic type like a float or int.  Integer parsing is faster than floating point if you know you will always get whole numbers.
- ```LiteralAsString``` allow specifying if the number is stored as a string.  Values are ```LiteralAsStringOpt::Never```, ```LiteralAsStringOpt::Maybe```, and ```LiteralAsStringOpt::Always```.
- ```Constructor``` the default is almost always correct here but this will constuct your type. 
- ```RangeCheck``` ensures that the number isn't narrowed when stored as your type. Values are ```JsonRangeCheck::Never```, and ```JsonRangeCheck::CheckForNarrowing```
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_number_null``` alias.
Or the alias ```json_checked_number``` that is always checked for narrowing, but has the Nullable option.

### json_bool
```cpp
template<JSONNAMETYPE Name, 
  typename T = bool,
  LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
  typename Constructor = daw::construct_a_t<T>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_bool
``` 
The defaults for ```json_bool``` will construct a ```bool``` with the supplied name.  The resulting type JsonClass must be constructable from a bool.
- ```LiteralAsString``` allow specifying if the number is stored as a string.  Values are ```LiteralAsStringOpt::Never```, ```LiteralAsStringOpt::Maybe```, and ```LiteralAsStringOpt::Always```.
- ```Constructor``` the default is almost always correct here but this will constuct your type.
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_bool_null``` alias.

### json_string
```cpp
template<JSONNAMETYPE Name, 
  typename String = std::string,
  typename Constructor = daw::construct_a_t<String>,
  typename Appender = impl::basic_appender<String>,
  JsonNullable EmptyStringNull = JsonNullable::Never,
  EightBitModes EightBitMode = EightBitModes::AllowFull,
  JsonNullable Nullable = JsonNullable::Never>
struct json_string
``` 
```json_string``` is fully processed and unescapes during deserialization and escapes while serializing
The defaults for json_string will construct a ```std::string``` with the supplied name.  The resulting type String must be default constructable if EmptyIsNull is JsonNullalbe::Nullable, otherwise the resulting type String must be constructable from two arguments(a ```char const *``` and a ```size_t```).

- ```Constructor``` the default is almost always correct here but this will constuct your type.  Appender will append each character to the string as
they are processed via push_back or insert( end, ... ), depending on which method is available.
- ```EmptyStringNull``` treat an empty JSON value ```""``` as a null value.
- ```EightBitMode``` allow filtering of characters with MSB set. Values are ```EightBitModes::AllowFull```, and ```EightBitModes::DisallowHigh```
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_string_null``` alias.

- See also the [cookbook](cookbook/strings.md) item for strings 

### json_string_raw
```cpp
template<JSONNAMETYPE Name, 
  typename String = std::string,
  typename Constructor = daw::construct_a_t<String>,
  JsonNullable EmptyStringNull = JsonNullable::Never,
  EightBitModes EightBitMode = EightBitModes::AllowFull,
  JsonNullable Nullable = JsonNullable::Never>
struct json_string_raw
``` 
```json_string_raw``` is unescaped and the exact bytes that are between the quotes in the string passed from json 
The defaults for json_string_raw will construct a ```std::string``` with the supplied name.  The resulting type String must be default constructable if EmptyIsNull is JsonNullalbe::Nullable, otherwise the resulting type String must be constructable from two arguments(a ```char const *``` and a ```size_t```).
- ```Constructor``` the default is almost always correct here but this will constuct your type.
- ```EmptyStringNull``` treat an empty JSON value ```""``` as a null value.
- ```EightBitMode``` allow filtering of characters with MSB set. Values are ```EightBitModes::AllowFull```, and ```EightBitModes::DisallowHigh```
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_string_raw_null``` alias.

- See also the [cookbook](cookbook/strings.md) item for strings 

### json_date
```cpp
template<JSONNAMETYPE Name,
  typename T = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>,
  typename Constructor = parse_js_date<JsonNullable::Never>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_date
``` 
```json_date``` is a special class for when a json string fields is known to have timestamp data.
The defaults for json_date will construct a ```std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>``` with the supplied name.  The resulting type T must be constructable from two arguments(a ```char const *``` and a ```size_t```).
- ```Constructor``` the default parses iso8601 timestamps.  Supplying a functor that takes the ptr, size combo will allow other result types and date formats.
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_date_null``` alias.

### json_class
```cpp
template<JSONNAMETYPE Name, 
  typename T,
  typename Constructor = daw::construct_a_t<T>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_class
``` 
```json_class``` requires a type argument `T` that is a type that has an overload of the ```json_data_contract_for( T )```.
- ```Constructor``` the default is almost always correct here but this will constuct your type.
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_class_null``` alias.

- See also the [cookbook](cookbook/class.md) item for classes

### json_array
```cpp
template<JSONNAMETYPE Name, 
  typename JsonElement,
  typename Container = std::vector<typename impl::unnamed_default_type_mapping<JsonElement>::parse_to_t>,
  typename Constructor = daw::construct_a_t<Container>,
  typename Appender = impl::basic_appender<Container>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_array
```
```json_array```  are sequences of the same type.
- ```Container``` is the type of container the ```JsonElement```'s are placed into. The default is ```std::vector```.
- ```JsonElement``` one of the a valid json type described on this page with daw::json::no_name as it's name.  This is the item type in the sequence.  You can also specify a previously mapped type or basic types like numbers, boolean, and std::string's
- ```Constructor``` the default is almost always correct here but this will constuct your type.
- ```Appender``` the default is almost always correct here but this will append via insert( Value ) on the container
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_array_null``` alias.
- See also the [cookbook](cookbook/array.md) item for arrays

### json_key_value
```cpp
template<JSONNAMETYPE Name, 
  typename Container, 
  typename JsonValueType,
  typename JsonKeyType = json_string<no_name>,
  typename Constructor = daw::construct_a_t<Container>,
  typename Appender = impl::basic_kv_appender<Container>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_key_value
``` 
```json_key_value``` It a JSON class where the key's are strings and member names and the values are any valid type
- ```Container``` The result type to place all the KV pairs into.  By default must behave like a Map.
- ```JsonValueType``` One of the json types on this page with daw::json::no_name as it's name.  This is the value type in the KV pairs. You can also specify a previously mapped type or basic types like numbers, boolean, and std::string's.
- ```JsonKeyType``` The key type, as with value, of the json types on this page with daw::json::no_name as it's name.  This is the key type in the KV pairs.   You can also specify a previously mapped type or basic types like numbers, boolean, and std::string's. The underlying JSON data is a string.
- ```Constructor``` the default is  and the values are any valid typelmost always correct here but this will constuct your Continer.
- ```Appender``` the default is almost always correct here but this will append via `insert( std::pair<Key, Value> )` on the container
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_key_value_null``` alias.

- See also the [cookbook](cookbook/key_values.md) item for key/values 

### json_key_value_array
```cpp
template<JSONNAMETYPE Name, 
  typename Container, 
  typename JsonValueType,
  typename JsonKeyType,
  typename Constructor = daw::construct_a_t<Container>,
  typename Appender = impl::basic_kv_appender<Container>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_key_value_array
``` 
```json_key_value_array``` Is like both an array and a class. All the keys/member names are json strings.
- ```Container``` The result type to place all the KV pairs into.  By default must behave like a Map.
- ```JsonValueType``` One of the above json types with a specified name that matches the value member name as it's name.  This is the value type in the KV pairs   You can also specify a previously mapped type or basic types like numbers, boolean, and std::string's where the default member name is `"value"`
- ```JsonKeyType``` One of the above json types with a specified name that matches the key member name as it's name.  This is the key type in the KV pairs.   You can also specify a previously mapped type or basic types like numbers, boolean, and std::string's where the default member name is `"key"`
- ```Constructor``` the default is almost always correct here but this will constuct your Continer.
- ```Appender``` the default is almost always correct here but this will append via insert( std::pair<Key, Value> ) on the container
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_key_value_array_null``` alias.

- See also the [cookbook](cookbook/key_values.md) item for key/values 

### json_variant
```cpp
template<JSONNAMETYPE Name, 
  typename T, 
  typename JsonElements,
  typename Constructor = daw::construct_a_t<T>,
  JsonNullable Nullable = JsonNullable::Never>
struct json_variant
```
```json_variant``` allows a member's value to map to up to one of the basic json types (string, bool, number, array, object). The type `T` is a ```std::variant``` like type that can be constructed from any of the specified types
- ```JsonElements``` is a `json_variant_type_list<JsonMembers...>` where up to 5 json types from this page can be specified, one for each basic JSON type
- ```Constructor``` the default is almost always correct here but this will constuct your type.
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_variant_null``` alias.

- See also the [cookbook](cookbook/variant.md) item for variants 

### json_custom
```cpp
template<JSONNAMETYPE Name, typename T,
  typename FromConverter = custom_from_converter_t<T>,
  typename ToConverter = custom_to_converter_t<T>,
  CustomJsonTypes CustomJsonType = CustomJsonTypes::String,
  JsonNullable Nullable = JsonNullable::Never>
struct json_custom
``` 
```json_custom``` allows one to map unusual types.  The FromConverter is fed the raw value that is in the json and returns a `T`.  The ToConverter outputs a string from the `T` value.
- ```FromConverter``` A class who's `operator( )` take a `std::string_view` and returns an instance of `T`;  The default calls `from_string( daw::tag_t<T>, T )`.
- ```ToConverter``` A class who's `operator( )` is takes an instance of `T`  value and returns a string like type.  The default uses `to_string( T )`.
- ```CustomJsonType``` - The JSON data will be a literal or a string.  Values are `CustomJsonTypes::Literal`, and `CustomJsonTypes::String`
- ```Nullable``` is the value optional/nullable.  Valules are ```JsonNullable::Never```, and ```JsonNullable::Nullable``` 

As above but without the Nullable option, it is set to Nullable, is the ```json_custom_null``` alias.

