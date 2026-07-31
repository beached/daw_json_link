# Incremental JSON Writer

`daw::json::json_writer` builds one JSON document incrementally. It is useful
when the document structure is known while writing, but constructing an
intermediate C++ object or container would be inconvenient.

Include the incremental writer header and create a writer over any supported
writable output. This includes string-like containers, C++ output streams, and
C `FILE *` outputs:

```cpp
#include <daw/json/daw_json_writer.h>

#include <string>

auto result = std::string{ };
auto writer = daw::json::json_writer( result );
```

For example, a writer can write directly to standard output:

```cpp
auto stream_writer = daw::json::json_writer( std::cout );
auto file_writer = daw::json::json_writer( stdout );
```

Values passed to `write_value` and `write_key_value` use the same serialization
support as `daw::json::to_json`, including JSON data contracts.

## Writing an Object

Use `write_key_value` when a member's value is immediately available:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );
  writer.write_key_value( "name", "Ada" );
  writer.write_key_value( "active", true );
  writer.write_key_value( "score", 42 );
  writer.close_object( );
}
```

The result is:

```json
{"name":"Ada","active":true,"score":42}
```

Use `add_key` when the value will be written separately. This is particularly
useful when the value is an array or object:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );

  writer.add_key( "values" );
  writer.open_array( );
  writer.write_value( 1 );
  writer.write_value( 2 );
  writer.write_value( 3 );
  writer.close_array( );

  writer.close_object( );
}
```

The result is:

```json
{"values":[1,2,3]}
```

Inside an object, `write_value` must follow `add_key`. Calling `add_key` again
before writing the previous key's value writes `null` for the previous value.

## Writing an Array

An array can be the root document:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_array( );
  writer.write_value( 1 );
  writer.write_value( "two" );
  writer.write_value( true );
  writer.close_array( );
}
```

The result is:

```json
[1,"two",true]
```

`write_array_values` appends an initializer list, a container-like range, or a
heterogeneous argument list to an open array:

```cpp
#include <vector>

auto values = std::vector<int>{ 1, 2, 3 };
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_array( );
  writer.write_array_values( values );
  writer.write_array_values( { 4, 5 } );
  writer.write_array_values( 6, "seven", false );
  writer.close_array( );
}
```

The result is:

```json
[1,2,3,4,5,6,"seven",false]
```

`write_value` also accepts an initializer list or multiple values and writes
them as a complete JSON array. This is useful at the root or after `add_key`:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );
  writer.add_key( "values" );
  writer.write_value( 1, "two", false );
  writer.close_object( );
}
```

The result is:

```json
{"values":[1,"two",false]}
```

Likewise, passing an initializer list or multiple values to
`write_key_value` writes the member value as an array:

```cpp
writer.write_key_value( "numbers", { 1, 2, 3 } );
writer.write_key_value( "mixed", 42, "Hello", 44 );
```

## Nesting Objects and Arrays

Objects and arrays can be nested to any depth supported by the writer's state
stack:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );
  writer.write_key_value( "kind", "example" );

  writer.add_key( "items" );
  writer.open_array( );

  writer.open_object( );
  writer.write_key_value( "id", 1 );
  writer.close_object( );

  writer.open_object( );
  writer.write_key_value( "id", 2 );
  writer.close_object( );

  writer.close_array( );
  writer.close_object( );
}
```

The result is:

```json
{"kind":"example","items":[{"id":1},{"id":2}]}
```

## Writing a Single Root Value

A scalar can be written as the complete document without opening a container:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.write_value( 42 );
}
```

Only one root value, object, or array can be written before the writer is
reset.

## Writing Values with an Explicit JSON Type

Writer functions normally deduce the JSON representation from the C++ value.
The following functions accept an optional JSON mapping type as their first
template argument:

* `write_value<JsonClass>(value)`
* `write_number<JsonClass>(value)`
* `write_string<JsonClass>(value)`
* `write_key_value<JsonClass>(key, value)`
* `write_array_values<JsonClass>(range)`
* `write_array_values<JsonClass>({ values... })`

The initializer-list overloads of `write_value` and `write_key_value` also
accept an explicit mapping and apply it to every array element.

For example, `FPOutputFormat::Decimal` can be used to preserve the decimal
form of a floating-point value:

```cpp
using decimal_number = daw::json::json_number_no_name<
  double,
  daw::json::options::number_opt(
    daw::json::options::FPOutputFormat::Decimal )>;
```

The mapping can be supplied to `write_number`:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.write_number<decimal_number>( 10.0 );
}
```

or to the more general `write_value`:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.write_value<decimal_number>( 10.0 );
}
```

Both examples produce:

```json
10.0
```

The same mapping can be used when writing an object member:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );
  writer.write_key_value<decimal_number>( "value", 10.0 );
  writer.close_object( );
}
```

The result is:

```json
{"value":10.0}
```

It can also be applied to a range or initializer list:

```cpp
auto values = std::array{ 10.0, 20.0 };

writer.write_array_values<decimal_number>( values );
writer.write_array_values<decimal_number>( { 30.0, 40.0 } );
```

An explicit mapping cannot be supplied to the variadic
`write_array_values(value, values...)` overload. Its potentially heterogeneous
values are mapped individually using their deduced types. The variadic
`write_value` and `write_key_value` forms use this behavior when producing
their arrays.

The mapping supplied to `write_number` must have a number or Boolean
underlying JSON type. The mapping supplied to `write_string` must have a
number, Boolean, or string underlying JSON type. `write_boolean` and
`write_null` have fixed representations and do not take a mapping type.

## Pretty Output

Serialization policy flags are template arguments to `json_writer`. For
example, enable pretty output as follows:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer<
    daw::json::options::SerializationFormat::Pretty>( result );

  writer.open_object( );
  writer.write_key_value( "answer", 42 );
  writer.add_key( "values" );
  writer.open_array( );
  writer.write_array_values( { 1, 2, 3 } );
  writer.close_array( );
  writer.close_object( );
}
```

The result is:

```json
{
  "answer": 42,
  "values": [
    1,
    2,
    3
  ]
}
```

See [Output Options](output_options.md) for the available serialization policy
flags.

## Automatic Completion and Missing Values

Explicitly closing every object and array makes the intended structure clear,
but the writer also closes open containers when it is destroyed:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );
  writer.write_key_value( "answer", 42 );
} // Completes the object.
```

If an object key has no value when another key is added, the object is closed,
or the writer is destroyed, its value is written as `null`:

```cpp
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_object( );
  writer.add_key( "missing" );
  writer.close_object( );
}
```

The result is:

```json
{"missing":null}
```

Call `finalize()` to perform the same completion explicitly before the writer
is destroyed. It writes `null` for a pending object key and closes all open
objects and arrays:

```cpp
auto result = std::string{ };
auto writer = daw::json::json_writer( result );
writer.open_object( );
writer.write_key_value( "answer", 42 );
writer.finalize( );
```

## Reusing a Writer

Call `reset()` to finish the current document and reset the writer state so
another root JSON value can be appended to the same output. `reset()` calls
`finalize()`, so it writes `null` for a pending object key and closes every
open object and array. It does not clear or otherwise modify output that has
already been written.

```cpp
auto result = std::string{ };
auto writer = daw::json::json_writer( result );

writer.open_object( );
writer.write_key_value( "answer", 42 );
writer.reset( ); // Completes the first document.

writer.write_value( true );
writer.finalize( );
```

Here, `result` is `{"answer":42}true`. Add any separator required by the
surrounding output format before writing the next root value.

## Operation Rules

* `add_key` and `write_key_value` are valid only inside an object.
* An object value written separately must follow `add_key`.
* The scalar write functions write an array element when inside an array.
* `write_array_values` is valid only inside an array.
* Passing multiple values or an initializer list to `write_value` or
  `write_key_value` writes those values as an array.
* `close_object` and `close_array` must match the currently open container.
* A writer produces one root JSON value between construction or `reset()` and
  the next `reset()`.
* `finalize()` completes the current document, and `reset()` finalizes it
  before resetting the writer state.
* The writer is neither copyable nor movable and must not outlive its output.
