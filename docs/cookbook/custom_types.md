# Custom Types and Output

`json_custom` maps a JSON value to a C++ type by calling user-provided
conversion objects. It is useful when a type does not naturally map to one of
the standard JSON mappings, or when its JSON representation needs custom
formatting.

A custom mapping has the following form:

```cpp
json_custom<
  "member_name",
  value_type,
  from_json_converter,
  to_json_converter,
  options
>
```

The from-JSON converter accepts a `std::string_view` and returns the mapped
C++ value. The to-JSON converter accepts the C++ value and returns a
string-like value containing its serialized representation.

## String output

`JsonCustomTypes::String` is the default. The parser expects a JSON string and
passes its unquoted contents to the converter. During serialization,
`json_custom` surrounds the converter's output with double quotes.

```cpp
#include <daw/json/daw_json_link.h>

#include <string>
#include <string_view>

struct Identifier {
  int value;
};

struct IdentifierFromJson {
  Identifier operator( )( std::string_view sv ) const {
    return Identifier{ std::stoi( std::string( sv ) ) };
  }
};

struct IdentifierToJson {
  std::string operator( )( Identifier const &id ) const {
    return std::to_string( id.value );
  }
};

using identifier_json = daw::json::json_custom_no_name<
  Identifier,
  IdentifierFromJson,
  IdentifierToJson
>;

auto id = daw::json::from_json<identifier_json>( R"("42")" );
auto json = daw::json::to_json<identifier_json>( id ); // "42"
```

Because the converter's output is copied directly between the quotes, the
converter must escape any characters that require JSON escaping.

The option can also be written explicitly:

```cpp
using identifier_json = daw::json::json_custom_no_name<
  Identifier,
  IdentifierFromJson,
  IdentifierToJson,
  daw::json::options::json_custom_opt(
    daw::json::options::JsonCustomTypes::String )
>;
```

## Literal output

Use `JsonCustomTypes::Literal` when the converter produces an unquoted JSON
number, boolean, or `null`. The parser passes the literal text to the converter,
and serialization writes the converter's output without adding quotes.

```cpp
using identifier_literal_json = daw::json::json_custom_no_name<
  Identifier,
  IdentifierFromJson,
  IdentifierToJson,
  daw::json::options::json_custom_opt(
    daw::json::options::JsonCustomTypes::Literal )
>;

auto id = daw::json::from_json<identifier_literal_json>( "42" );
auto json = daw::json::to_json<identifier_literal_json>( id ); // 42
```

`json_custom_lit` and `json_custom_lit_no_name` are convenience aliases that
select `JsonCustomTypes::Literal`:

```cpp
using identifier_literal_json = daw::json::json_custom_lit_no_name<
  Identifier,
  IdentifierFromJson,
  IdentifierToJson
>;
```

The to-JSON converter is responsible for returning a valid JSON literal.

## Accepting any JSON value

`JsonCustomTypes::Any` is experimental. It lets the parser pass any complete
JSON value to the from-JSON converter. Unlike `String`, a JSON string is passed
with its surrounding quotes. This is useful for constructing a `json_value` or
performing ad hoc parsing when `json_raw` is not suitable.

```cpp
struct CopyJsonText {
  std::string operator( )( std::string_view sv ) const {
    return std::string( sv );
  }
};

struct EmitJsonText {
  std::string operator( )( std::string const &value ) const {
    return value;
  }
};

using any_json = daw::json::json_custom_no_name<
  std::string,
  CopyJsonText,
  EmitJsonText,
  daw::json::options::json_custom_opt(
    daw::json::options::JsonCustomTypes::Any )
>;

auto object_text = daw::json::from_json<any_json>( R"({"answer":42})" );
auto string_text = daw::json::from_json<any_json>( R"("answer")" );
// object_text == R"({"answer":42})"
// string_text == R"("answer")"
```

`Any` changes the accepted input. For output it has the same quoted behavior as
`String`. Its to-JSON converter must therefore return escaped JSON string
contents, not a raw object or array. Use `Literal` when the converter's result
must be emitted without quotes.

## Options

Build the options argument with
`daw::json::options::json_custom_opt( ... )` and one of these values:

| `JsonCustomTypes` value | Accepted input | Serialization treatment |
|---|---|---|
| `String` (default) | JSON string | Surrounded with double quotes |
| `Literal` | JSON number, boolean, or `null` | Written without quotes |
| `Any` (experimental) | Any JSON value | Surrounded with double quotes |

Named, unnamed, and nullable forms are available:

```cpp
json_custom<"value", T, FromJson, ToJson>
json_custom_no_name<T, FromJson, ToJson>
json_custom_null<"value", std::optional<T>, FromJson, ToJson>

json_custom_lit<"value", T, FromJson, ToJson>
json_custom_lit_no_name<T, FromJson, ToJson>
json_custom_lit_null<"value", std::optional<T>, FromJson, ToJson>
```

See [Member Options](member_options.md#jsoncustomtypes) for the option
reference and [Dates and Times](dates.md#custom-string-formats) for a complete
class contract using a custom string representation. To exchange complete raw
JSON values with another library, see
[Inserting and Extracting Raw JSON](inserting_extracting_raw_json.md#interoperating-with-another-json-library).
