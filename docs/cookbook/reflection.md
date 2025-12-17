# C++ 26 Reflection

JSON Link has support for C++26 reflection. The API may still change, but is close to done. Until the API is finalized one must include `<daw/json/daw_json_reflection.h>` in their code that uses it.

Because reflection helps with mappings, the calls to (de)serialized are still `daw::json::from_json<Type>( json_document )` and `daw::json::to_json( value )`. Like explicit mappings with `daw::json::json_data_contract` already mapped, reflection or not, types just work.

## Basic example

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    int age;
    std::string name;
    Colour eyeColour;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": 42,
 "name": "Jane Smith",
 "eyeColour": 0
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    assert( p.eyeColour == Colour::blue );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"name":"Jane Smith","eyeColour":0}
```

Adding the `[[= daw::json::reflect ]]` annotation to a class will map any type with all public non-static data members. Another way is to specialize the variable template `daw::json::enable_reflection_for` for your type. This is useful for types that you do not control the source for.

```c++
template<>
inline constexpr bool daw::json::enable_reflection_for<Foo> = true;
```

## Renaming members

Let's take the previous example and rename a member with member annotations.

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    int age;
    
    [[= reflect.rename<"fullName"> ]]
    std::string name;
    Colour eyeColour;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": 42,
 "fullName": "Jane Smith",
 "eyeColour": 0
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    assert( p.eyeColour == Colour::blue );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"fullName":"Jane Smith","eyeColour":0}
```

## Explicitly setting the member mapping

Continuing the previous example and allow the age to sometimes be quoted.

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    [[= reflect.map_as<
        daw::json::json_number<
            "age", 
            int, 
            daw::json::options::number_opt(
		            daw::json::options::LiteralAsStringOpt::Maybe )>> ]]
    int age;
    
    [[= reflect.rename<"fullName"> ]]
    std::string name;
    Colour eyeColour;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": "42",
 "fullName": "Jane Smith"
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"fullName":"Jane Smith"}
```

This allows full control of individual member mappings as if it was a `daw::json::json_data_contract`.

### Note

One cannot use `rename` and `map_as` on the same member.

## Enumerations as strings

By default, enum members are mapped as numbers. If one wants to map them as strings they can with the `daw::json::reflect.enum_string` annotion. This uses the default options for a `json_custom` mapping. If one needs to pass options they can use `daw::json::reflect.enum_string_with_opt`.

With the previous example

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    [[= reflect.map_as<
        daw::json::json_number<
            "age", 
            int, 
            daw::json::options::number_opt(
		            daw::json::options::LiteralAsStringOpt::Maybe )>> ]]
    int age;
    
    [[= reflect.rename<"fullName"> ]]
    std::string name;
    
    [[= reflect.enum_string ]]
    Colour eyeColour;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": "42",
 "fullName": "Jane Smith"
 "eyeColour": "blue"
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    assert( p.eyeColour == Colour::blue );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"fullName":"Jane Smith","eyeColour":"blue"}
```

## Ignoring members

We can ignore specific members individually and they will use the default constructed value based on it's type(e.g. `T{}`) or a supplied value. Currently, member initialializers are not used as C++26 reflection does not expose them.

With the previous example, lets add an ingnored member

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    [[= reflect.map_as<
        daw::json::json_number<
            "age", 
            int, 
            daw::json::options::number_opt(
		            daw::json::options::LiteralAsStringOpt::Maybe )>> ]]
    int age;
    
    [[= reflect.rename<"fullName"> ]]
    std::string name;
    
    [[= reflect.enum_string ]]
    Colour eyeColour;
    [[= reflect.ignored ]]
    std::size_t code;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": "42",
 "fullName": "Jane Smith"
 "eyeColour": "blue"
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    assert( p.eyeColour == Colour::blue );
    assert( p.code == 0 );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"fullName":"Jane Smith","eyeColour":"blue"}
```

`daw::json::reflect.ignored` will only work for default constructible members without a default value or callable specified.

### Ignored with default value

Let's add a default value to `code` so that it does not default to `std::size_t{}`/`0`.

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    [[= reflect.map_as<
        daw::json::json_number<
            "age", 
            int, 
            daw::json::options::number_opt(
		            daw::json::options::LiteralAsStringOpt::Maybe )>> ]]
    int age;
    
    [[= reflect.rename<"fullName"> ]]
    std::string name;
    
    [[= reflect.enum_string ]]
    Colour eyeColour;
    [[= reflect.ignored(55U) ]]
    std::size_t code;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": "42",
 "fullName": "Jane Smith"
 "eyeColour": "blue"
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    assert( p.eyeColour == Colour::blue );
    assert( p.code == 55 );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"fullName":"Jane Smith","eyeColour":"blue"}
```

### Ignored with a default callable

Lets change the default for `code` to use a lambda to set the value.

```c++
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_reflection.h>
#include <cassert>
#include <print>
#include <string>
#include <string_view>

using daw::json::reflect;

enum class Colour { blue, green, brown };

struct [[= reflect ]] Person {
    [[= reflect.map_as<
        daw::json::json_number<
            "age", 
            int, 
            daw::json::options::number_opt(
		            daw::json::options::LiteralAsStringOpt::Maybe )>> ]]
    int age;
    
    [[= reflect.rename<"fullName"> ]]
    std::string name;
    
    [[= reflect.enum_string ]]
    Colour eyeColour;
    [[= reflect.ignored( []{ return 5555U; } ) ]]
    std::size_t code;
};

int main( ) {
    std::string_view json_doc = R"json(
{
 "age": "42",
 "fullName": "Jane Smith"
 "eyeColour": "blue"
}
)json";
    auto p = daw::json::from_json<Person>( json_doc );
    assert( p.age == 42 );
    assert( p.name == "Jane Smith" );
    assert( p.eyeColour == Colour::blue );
    assert( p.code == 5555 );
    std::println( "{}", daw::json::to_json( p ) ); 
}
```

The program will output

```
{"age":42,"fullName":"Jane Smith","eyeColour":"blue"}
```