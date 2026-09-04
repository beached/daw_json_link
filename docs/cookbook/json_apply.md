# `json_apply`

`daw::json::json_apply` parses JSON values into the parameters of a callable
and invokes the callable with those values. It is similar to `std::apply`, but
the arguments come from a JSON document instead of a tuple.

Include either the main JSON Link header or the dedicated header:

```cpp
#include <daw/json/daw_json_link.h>
// or
#include <daw/json/daw_json_apply.h>
```

## Basic use

When the callable takes one parameter, the complete JSON value is parsed as
that parameter's type:

```cpp
auto const length = daw::json::json_apply(
  R"json("Hello")json",
  []( std::string_view value ) {
    return value.size( );
  } );

assert( length == 5 );
```

When the callable takes multiple parameters, the document is a JSON array and
its elements are parsed in parameter order:

```cpp
auto const result = daw::json::json_apply(
  R"json(["Hello", 52, true])json",
  []( std::string_view text, std::size_t count, bool use_text ) {
    return use_text ? text.size( ) : count;
  } );

assert( result == 5 );
```

The return value is the result of invoking the callable.

## Parameter types

By default, `json_apply` obtains the parameter types from the callable's
signature. The usual JSON Link mappings apply, so parameters may be primitive
types, containers, or mapped user-defined types.

CV and reference qualifiers do not affect how a parameter is parsed. For
example, parameters such as `std::size_t &&` and `std::string_view &&` are
parsed using their underlying value types before the callable is invoked.

A callable with no parameters is also supported:

```cpp
auto const result = daw::json::json_apply(
  R"json([])json",
  []( ) {
    return std::size_t{ 5 };
  } );

assert( result == 5 );
```

## Explicit signatures

The parameter types can be specified instead of inferred by passing a function
type as the `Signature` template argument:

```cpp
auto const result = daw::json::json_apply<std::size_t( std::string_view )>(
  R"json("Hello")json",
  []( std::string_view value ) -> std::size_t {
    return value.size( );
  } );

assert( result == 5 );
```

The explicit signature's parameter list determines the types used for parsing;
`json_apply` returns the callable's actual result. The callable must currently
have a single, non-template call operator; generic lambdas and types with
overloaded call operators do not satisfy `json_apply`'s callable constraint.

## Parsing at a JSON path

Pass a JSON path between the document and callable to apply the callable to a
value within the document:

```cpp
auto const length = daw::json::json_apply(
  R"json({ "name": "Hello" })json",
  "name",
  []( std::string_view value ) {
    return value.size( );
  } );

assert( length == 5 );
```

The value selected by the path is parsed according to the callable's
parameters. See [Parsing Individual Members](parsing_individual_members.md)
for more about JSON paths.

## Parser policies

Parser flags may be passed before the callable, with or without a JSON path:

```cpp
using namespace daw::json::options;

auto const length = daw::json::json_apply(
  R"json({ "name": "Hello" })json",
  "name",
  parse_flags<CheckedParseMode::no>,
  []( std::string_view value ) {
    return value.size( );
  } );

assert( length == 5 );
```

The overloads are:

```cpp
json_apply<Signature>( json_document, callable );
json_apply<Signature>( json_document, json_path, callable );
json_apply<Signature>( json_document, parse_flags, callable );
json_apply<Signature>( json_document, json_path, parse_flags, callable );
```

`Signature` is optional in every overload. See
[Parser Options](parser_policies.md) for the available parsing policies and
their safety implications.

## Parsed JSON values

The input does not have to be JSON text. An existing `json_value` can be passed
to avoid locating the value again:

```cpp
auto const value = daw::json::json_value( R"json("Hello")json" );
auto const length = daw::json::json_apply(
  value,
  []( std::string_view text ) {
    return text.size( );
  } );

assert( length == 5 );
```

As with other non-owning JSON Link values, ensure that the source JSON remains
alive while it is being referenced. See [JSON Value](json_value.md) for details
about inspecting and retaining parsed values.

## Typical uses

`json_apply` is particularly useful at IPC and RPC boundaries, where an array
or value received as JSON maps naturally to a handler's parameters. It keeps
the parsing types next to the code that consumes them and avoids a separate
intermediate tuple or request object.
