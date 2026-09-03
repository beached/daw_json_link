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

## Creating reusable mapping aliases

A mapping can be wrapped in an alias template when the same C++ type and
converters are used for several JSON members. Only the member name needs to
remain a template parameter:

```cpp
template<JSONNAMETYPE Name>
using json_identifier = daw::json::json_custom<
  Name,
  Identifier,
  IdentifierFromJson,
  IdentifierToJson
>;
```

`JSONNAMETYPE` provides compatibility across language versions. In C++17 it
accepts a `char const *` member name, while in C++20 and later it uses
`daw::json::json_name`, allowing a string literal to be supplied directly.

The alias can then be used like any other member mapping. In C++20:

```cpp
using type = daw::json::json_member_list<
  json_identifier<"primary_id">,
  json_identifier<"secondary_id">
>;
```

For C++17-compatible code, give the member names static storage:

```cpp
static constexpr char primary_id[] = "primary_id";
static constexpr char secondary_id[] = "secondary_id";

using type = daw::json::json_member_list<
  json_identifier<primary_id>,
  json_identifier<secondary_id>
>;
```

When an alias only needs to support C++20 and later, the parameter can instead
be written explicitly:

```cpp
template<daw::json::json_name Name>
using json_identifier = daw::json::json_custom<
  Name,
  Identifier,
  IdentifierFromJson,
  IdentifierToJson
>;
```

The same technique can be used with other mappings, such as `json_string`,
`json_number`, and `json_class`, to give frequently used mapping configurations
a descriptive name.

### Hiding converters behind an alias

The converter types can be kept as implementation details while the public
aliases describe how the application type appears in a JSON contract:

```cpp
#include <daw/json/daw_json_link.h>

#include <optional>
#include <string>
#include <string_view>

struct Identifier {
  int value;
};

namespace identifier_json_details {
  struct FromJson {
    Identifier operator( )( std::string_view sv ) const {
      return Identifier{ std::stoi( std::string( sv ) ) };
    }
  };

  struct ToJson {
    std::string operator( )( Identifier const &value ) const {
      return std::to_string( value.value );
    }
  };
} // namespace identifier_json_details

template<JSONNAMETYPE Name>
using json_identifier = daw::json::json_custom<
  Name,
  Identifier,
  identifier_json_details::FromJson,
  identifier_json_details::ToJson
>;

template<JSONNAMETYPE Name>
using json_nullable_identifier = daw::json::json_custom_null<
  Name,
  std::optional<Identifier>,
  identifier_json_details::FromJson,
  identifier_json_details::ToJson
>;
```

Code defining a data contract no longer needs to know which converters implement
the mapping:

```cpp
using type = daw::json::json_member_list<
  json_identifier<"primary_id">,
  json_nullable_identifier<"secondary_id">
>;
```

`json_identifier` requires the member to contain a JSON string.
`json_nullable_identifier` additionally accepts `null` and maps it to an empty
`std::optional<Identifier>`.

The nullable alias can also be expressed directly with `json_nullable`. Its
inner mapping must be unnamed:

```cpp
template<JSONNAMETYPE Name>
using json_nullable_identifier = daw::json::json_nullable<
  Name,
  std::optional<Identifier>,
  daw::json::json_custom_no_name<
    Identifier,
    identifier_json_details::FromJson,
    identifier_json_details::ToJson
  >
>;
```

The `json_custom_null` form is the shorter convenience alias; the explicit
`json_nullable` form is useful when building a reusable nullable wrapper around
another unnamed mapping.

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

For output, `Any` treats the to-JSON converter's result as a complete JSON
value and emits it verbatim, without adding quotes, escaping characters, or
validating the result. To produce a valid JSON document, the converter must
therefore return exactly one valid JSON value: `null` produces a JSON null,
while `"null"` (including the quotes) produces a JSON string.

On input, `Any` uses the normal JSON value-skipping parser to determine the
range passed to the from-JSON converter. Syntax checking follows the selected
parse policy: the default checked mode performs the normal parser checks,
whereas `CheckedParseMode::no` assumes the input is well-formed. This parse-time
checking does not apply to text returned by the to-JSON converter.

## Options

Build the options argument with
`daw::json::options::json_custom_opt( ... )` and one of these values:

| `JsonCustomTypes` value | Accepted input | Serialization treatment |
|---|---|---|
| `String` (default) | JSON string; converter receives the contents without quotes | Converter output is copied between added double quotes; it is not escaped or validated |
| `Literal` | JSON number, boolean, or `null` | Converter output is copied verbatim without validation |
| `Any` (experimental) | Any JSON value; strings retain their quotes | Converter output is copied verbatim without validation |

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
