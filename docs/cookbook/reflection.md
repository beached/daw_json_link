# Reflection and reflection-like mappings

JSON Link can derive object mappings from C++26 static reflection. It also
provides separate adapters for Boost.PFR and Boost.Describe on toolchains that
do not provide standard reflection.

All three approaches create the mapping used by the normal API:

```cpp
auto value = daw::json::from_json<Type>( json_document );
auto json = daw::json::to_json( value );
```

An explicit `daw::json::json_data_contract<Type>` remains authoritative and is
used instead of a derived mapping.

## C++26 static reflection

### Requirements

The standard-reflection integration is available when the compiler defines
`__cpp_lib_reflection` with a value of at least `202506L`. Include the main
JSON Link header:

```cpp
#include <daw/json/daw_json_link.h>
```

The repository's reflection test configuration uses C++26 and a compiler flag
that enables P2996 reflection. With CMake, enable the dedicated test using:

```text
-DCMAKE_CXX_STANDARD=26 -DDAW_JSON_USE_REFLECTION=ON
```

The exact compiler option needed to enable reflection is compiler-specific.
`DAW_JSON_USE_REFLECTION` enables the repository's reflection test; the public
headers detect support through the standard feature-test macro.

### Automatic mapping

When no explicit mapping exists, a class that can be constructed from its
public non-static data members can use reflection as a fallback. Member names
become JSON object keys, and the normal deduced mapping is used for each member.

```cpp
#include <daw/json/daw_json_link.h>

#include <cassert>
#include <string>
#include <string_view>

struct Person {
  int age;
  std::string name;
};

int main( ) {
  std::string_view const json_document =
    R"json({"age":42,"name":"Jane Smith"})json";

  auto const person = daw::json::from_json<Person>( json_document );
  assert( person.age == 42 );
  assert( person.name == "Jane Smith" );
  assert( daw::json::to_json( person ) == json_document );
}
```

Reflected members may themselves use any deduced JSON Link mapping, including
other reflected classes, containers, maps, nullable types, and smart pointers.

### Explicitly enabling reflection

Annotate a class when reflection should be explicitly enabled:

```cpp
using daw::json::reflect;

struct [[= reflect ]] Person {
  int age;
  std::string name;
};
```

For a type whose source cannot be changed, specialize
`enable_reflection_for`:

```cpp
struct ExternalType {
  int first;
  int second;
};

template<>
inline constexpr bool
  daw::json::enable_reflection_for<ExternalType> = true;
```

Member annotations also enable reflection for their containing class, so a
class-level `[[= reflect ]]` annotation is not required when a member already
has `rename`, `map_as`, `enum_string`, or `ignored`.

### Renaming a member

Use `reflect.rename` when the JSON key differs from the C++ member name:

```cpp
using daw::json::reflect;

struct Person {
  int age;

  [[= reflect.rename<"fullName"> ]]
  std::string name;
};
```

This maps `name` to `"fullName"` for both parsing and serialization.

### Overriding a member mapping

Use `reflect.map_as` to provide the same mapping type that would otherwise
appear in a `json_data_contract`. The mapping must include the JSON member name:

```cpp
using daw::json::reflect;

struct Person {
  [[= reflect.map_as<
    daw::json::json_number<
      "age",
      int,
      daw::json::options::number_opt(
        daw::json::options::LiteralAsStringOpt::Maybe )>> ]]
  int age;

  std::string name;
};
```

This accepts either `{"age":42,"name":"Jane"}` or
`{"age":"42","name":"Jane"}`. Do not put `rename` and `map_as` on the
same member because `map_as` already supplies the name.

### Enumerations as strings

Reflected enumerations use their underlying numeric representation by default.
Annotate an enum member to use its enumerator name instead:

```cpp
using daw::json::reflect;

enum class Colour { blue, green, brown };

struct Person {
  std::string name;

  [[= reflect.enum_string ]]
  Colour eye_colour;
};
```

This maps `Colour::blue` to and from `"blue"`. The configurable form is
`reflect.enum_string_with_options<Options>`, where `Options` is a
`json_custom` option value. Do not combine `enum_string` and `map_as` on the
same member.

### Ignoring members

An ignored member is absent from both input and output. By default it is
constructed as `T{}`:

```cpp
using daw::json::reflect;

struct Request {
  int id;

  [[= reflect.ignored ]]
  std::size_t internal_code;
};
```

A value or a nullary callable can supply another default:

```cpp
struct RequestWithValue {
  int id;

  [[= reflect.ignored( 55U )]]
  std::size_t internal_code;
};

struct RequestWithFactory {
  int id;

  [[= reflect.ignored( [] { return 5555U; } )]]
  std::size_t internal_code;
};
```

The supplied value, or the callable's result, must be convertible to the member
type. A bare `reflect.ignored` therefore requires a default-constructible member
type. Member initializers are not used as ignored-member defaults.

### Including non-public members

`reflect` normally maps public non-static data members only. Use
`reflect.unchecked` to include all non-static data members:

```cpp
using daw::json::reflect;

class [[= reflect.unchecked ]] PrivateValue {
  int value;

public:
  constexpr explicit PrivateValue( int v )
    : value( v ) {}
};
```

This deliberately exposes private representation through JSON and should only
be used when that is part of the type's intended wire format. The type must be
constructible in reflected member order.

The executable coverage for these features is in
[daw_json_link_reflection_test.cpp](../../tests/src/daw_json_link_reflection_test.cpp).

## Boost.PFR adapter

Boost.PFR support is independent of C++26 reflection. It derives member names
and values for a non-empty aggregate and installs a JSON Link mapping when the
type explicitly opts in.

```cpp
#include <daw/json/daw_json_boost_pfr_mapping.h>

#include <cassert>
#include <string>
#include <vector>

struct Person {
  int id;
  std::string name;
  std::vector<int> scores;
};

template<>
inline constexpr bool daw::json::use_boost_pfr<Person> = true;

int main( ) {
  auto const person = daw::json::from_json<Person>(
    R"json({"id":42,"name":"Jane","scores":[10,20]})json" );

  assert( person.id == 42 );
  assert( person.name == "Jane" );
  assert( person.scores == std::vector<int>( { 10, 20 } ) );
  assert( daw::json::to_json( person ) ==
          R"json({"id":42,"name":"Jane","scores":[10,20]})json" );
}
```

The adapter requires `<boost/pfr.hpp>` and Boost.PFR member-name support. The
repository enables its functional test only when that header is available and
builds the test as C++20. See
[daw_json_link_boost_pfr_test.cpp](../../tests/src/daw_json_link_boost_pfr_test.cpp).

## Boost.Describe adapter

Boost.Describe support is also independent of standard reflection. Describe
the public members and include the adapter; described types are enabled by
default.

```cpp
#include <daw/json/daw_json_link_describe.h>

#include <boost/describe.hpp>

#include <cassert>
#include <string>

struct Person {
  int id;
  std::string name;
};

BOOST_DESCRIBE_STRUCT( Person, ( ), ( id, name ) );

int main( ) {
  auto const person = daw::json::from_json<Person>(
    R"json({"id":42,"name":"Jane"})json" );

  assert( person.id == 42 );
  assert( person.name == "Jane" );
  assert( daw::json::to_json( person ) ==
          R"json({"id":42,"name":"Jane"})json" );
}
```

Only public described data members are supported. A type with private or
protected described data members must use a manual mapping. To replace the
derived mapping for one described type, opt it out and provide a
`json_data_contract`:

```cpp
template<>
inline constexpr bool daw::json::use_boost_describe_v<Person> = false;
```

See
[daw_json_link_describe_test.cpp](../../tests/src/daw_json_link_describe_test.cpp)
for nested objects, containers, nullable values, smart pointers, and round-trip
serialization.

Do not enable both adapters for the same type. If a Boost-described aggregate
is intentionally mapped with Boost.PFR, set `use_boost_describe_v<T>` to
`false` before enabling `use_boost_pfr<T>`.
