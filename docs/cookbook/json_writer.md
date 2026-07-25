# Incremental JSON Writer

`daw::json::json_writer` builds one JSON document incrementally. It is useful
when the document structure is known while writing, but constructing an
intermediate C++ object or container would be inconvenient.

Include the incremental writer header and create a writer over any supported
writable output:

```cpp
#include <daw/json/daw_json_writer.h>

#include <string>

auto result = std::string{ };
auto writer = daw::json::json_writer( result );
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

`write_values` can append an initializer list, a container-like range, or a
heterogeneous argument list:

```cpp
#include <vector>

auto values = std::vector<int>{ 1, 2, 3 };
auto result = std::string{ };
{
  auto writer = daw::json::json_writer( result );
  writer.open_array( );
  writer.write_values( values );
  writer.write_values( { 4, 5 } );
  writer.write_values( 6, "seven", false );
  writer.close_array( );
}
```

The result is:

```json
[1,2,3,4,5,6,"seven",false]
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

Only one root value, object, or array can be written by a writer.

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
  writer.write_values( { 1, 2, 3 } );
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

## Operation Rules

* `add_key` and `write_key_value` are valid only inside an object.
* An object value written separately must follow `add_key`.
* `write_value` writes an array element when inside an array.
* `write_values` is valid only inside an array.
* `close_object` and `close_array` must match the currently open container.
* A writer produces exactly one root JSON value.
* The writer is neither copyable nor movable and must not outlive its output.
