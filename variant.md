# Variant Types

Variant or sum types are where the json member can have more than one type(like string, number, class, or array).
JSON Link supports several forms of variant types

* An undiscriminated variant that is limited to a set of alternatives where there is up to one of each of the basic JSON types(array, object, string, number, bool, null).
* A discriminated variant where the discriminator is another member of the same class. e.g. ```json { "type": 1, "value": [] }``` where `"value"`'s type is determined by `"type"`
* A discriminated variant where the discriminator is a submember of the class type being parsed. e.g ```json { "obj": { "type": 1 } }``` where "type" determines how the class itself will be parsed

Take the following JSON array:

```json
[
  {
    "member0": 5,
    "member1": "hello"
  },
  {
    "member0": "world",
    "member1": true
  }
]
```

Here we have an array of a class that has two members. A variant of types number and string, member0; and a variant of
type string and bool, member1.

Too see a working example using this code, refer
to [cookbook_variant1_test.cpp](../tests/src/cookbook_variant1_test.cpp)

The following C++ can provide a mapping. It, also, highlights that the types bool, integers, floating point, std::
string, and previously mapped types can be default mapped to elements that do not require a name. Such as variant,
array, and some key_value's.

```c++
struct MyVariantStuff1 {
  std::variant<int, std::string> member0;
  std::variant<std::string, bool> member1;
};

namespace daw::json {
  template<>
  struct json_data_contract<MyVariantStuff1> {
    using type = json_member_list<
      json_variant<"member0", std::variant<int, std::string>,
        json_variant_type_list<int,
          json_string_no_name<>>>,
      json_variant<
        "member1", std::variant<std::string, bool>,
        json_variant_type_list<std::string, json_bool_no_name<>>>>;

    static inline auto
    to_json_data( MyVariantStuff1 const &value ) {
      return std::forward_as_tuple( value.member0, value.member1 );
    }
  };
} 
```

## Important note:

The elements in the `json_variant_type_list` must have matching types in the variant alternatives. (e.g. std::string ->
json_string, bool -> json_bool )

## Recursive variants

A variant can use pointer indirection to describe recursive data such as an
expression or nested value:

```c++
struct Variant {
  using value_t =
    std::variant<int, bool, std::shared_ptr<Variant>>;

  value_t value;
};
```

The recursive alternative is represented by another JSON object. For example,
the following array contains an integer, a boolean, and two recursively nested
values:

```json
[
  { "value": 5 },
  { "value": true },
  { "value": { "value": false } },
  { "value": { "value": { "value": 42 } } }
]
```

To see a working example, including serialization and round-trip parsing, refer
to [cookbook_variant6_test.cpp](../../tests/src/cookbook_variant6_test.cpp).

A direct mapping containing
`json_class_null_no_name<std::shared_ptr<Variant>>` would require the `Variant`
data contract while that same contract is still being defined. A `json_raw`
member delays parsing until the contract is complete. Its constructor examines
the fundamental JSON type and recursively calls `from_json<Variant>` only when
the value is an object.

```c++
struct VariantValueConstructor {
  Variant::value_t
  operator()( char const *ptr, std::size_t size ) const;
};

namespace daw::json {
  template<>
  struct json_data_contract<Variant> {
    using type = json_member_list<
      json_raw<"value", Variant::value_t, VariantValueConstructor>
    >;

    static std::tuple<std::string>
    to_json_data( Variant const &value );
  };
}

Variant::value_t VariantValueConstructor::operator()(
  char const *ptr, std::size_t size ) const {
  auto const raw_json = std::string_view( ptr, size );
  auto const value = daw::json::json_value( raw_json );

  switch( value.type() ) {
  case daw::json::JsonBaseParseTypes::Number:
    return daw::json::from_json<int>( value );
  case daw::json::JsonBaseParseTypes::Bool:
    return daw::json::from_json<bool>( value );
  case daw::json::JsonBaseParseTypes::Class:
    return std::make_shared<Variant>(
      daw::json::from_json<Variant>( raw_json ) );
  default:
    std::abort();
  }
}

std::tuple<std::string>
daw::json::json_data_contract<Variant>::to_json_data(
  Variant const &value ) {
  auto raw_json = std::visit(
    []( auto const &item ) -> std::string {
      using item_t = std::decay_t<decltype( item )>;
      if constexpr(
        std::is_same_v<item_t, std::shared_ptr<Variant>> ) {
        return daw::json::to_json( *item );
      } else {
        return daw::json::to_json( item );
      }
    },
    value.value );
  return { std::move( raw_json ) };
}
```

The serializer returns complete JSON text for the mapped `json_raw` member, so
it is emitted verbatim. This example assumes that the `shared_ptr` alternative
is non-null. Nested JSON must also remain acyclic; cyclic graphs should be
represented with IDs and edges as described in the [graphs cookbook](graphs.md).

## Tagged Variants

It is common to have a tag discriminator in JSON data. The `json_tagged_variant` member type allows using another parsed
member to return an index in a member list to parse. This allows any number of types to be inside the variant.

Below is a JSON array, containing a variant element where the `"type"` member determines the type of the `"value"`
member. In many JSON documents, the discriminator will be a string.

```json
[
  {
    "type": 0,
    "name": "string value",
    "value": "hello"
  },
  {
    "type": 1,
    "name": "int value",
    "value": 5
  },
  {
    "type": 2,
    "name": "bool value",
    "value": false
  }
]
```

A member name and a callable are needed to tell the parser which type will parsed.

Too see a working example using this code, refer
to [cookbook_variant2_test.cpp](../tests/src/cookbook_variant2_test.cpp)

```c++
struct MyClass {
  std::string name;
  std::variant<std::string, int, bool> value;  
};

struct MyClassSwitcher {
  // Convert JSON tag member to type index
  constexpr size_t operator( )( int type ) const {
    return (size_t)type;
  }     
  // Get value for Tag from class value
  int operator( )( MyClass const & v ) const {
    return (int)v.value.index( );
  }
};

template<>
struct daw::json::json_data_contract<MyClass> {
  using type = json_member_list<
    json_string<"name">,
    json_tagged_variant<
      "value", 
      std::variant<std::string, int, bool>,
      json_number<"type", int>,
      MyClassSwitcher>
    >;

  static constexpr inline to_json_data( MyClass const & v ) {
    return std::forward_as_tuple( v.name, v.value );
  }
};
```

In the above example, two members are mapped to construct MyClass, `"name"` and `"value"`. The variant uses the
JSON `"type"` member to determine the index of the parser to use for the variant value.

Extending the previous example, it auto detected the `std::string`, `int`, and `bool` types and supplied the parser
descriptions for them. Lets do it manually.

Below is a JSON array, containing a variant element where the `"type"` member determines the type of the `"value"`
member. In many JSON documents, the discriminator will be a string.

 ```json
 [
  {
    "type": 0,
    "name": "string value",
    "value": "hello"
  },
  {
    "type": 1,
    "name": "int value",
    "value": 5
  },
  {
    "type": 2,
    "name": "bool value",
    "value": false
  }
]
 ```

A member name and a Callable are needed to tell the parser which type will parsed.

Too see a working example using this code, refer
to [cookbook_variant3_test.cpp](../tests/src/cookbook_variant3_test.cpp)

 ```c++
 struct MyClass {
   std::string name;
   std::variant<std::string, int, bool> value;  
 };
 
 struct MyClassSwitcher {
   // Convert JSON tag member to type index
   constexpr size_t operator( )( int type ) const {
     return (size_t)type;
   }     
   // Get value for Tag from class value
   int operator( )( MyClass const & v ) const {
     return (int)v.index( );
   }
 };
 
 template<>
 struct daw::json::json_data_contract<MyClass> {
   using type = json_member_list<
     json_string<"name">,
     json_tagged_variant<
       "value", 
       std::variant<std::string, int, bool>,
       json_number<"type", int>,
       MyClassSwitcher,
       json_tagged_variant_type_list<
         std::string,
         json_number_no_name<int>,
         json_bool_no_name<>    
       >
     >
   >;
 
   static constexpr to_json_data( MyClass const & v ) {
     return std::forward_as_tuple( v.name, v.value );
   }
 };
```

As you can see, the json_variant_type_list can use terse type names for some, or the full names.

## Submember as tag for tagged_variant

There are cases where a classes structure is determined by one of it's submembers. This comes up with file versioning.

In our example we have two versions of a config file. The tag member `"version"` determines the layout of the other
members in the example.

```json
{
  "version": 1,
  "name": "what is the answer to the ultimate question?",
  "value": 42,
  "next question": "what is earth"
}
```

```json
{
  "version": 2,
  "config_options": [
    {
      "name": "bob",
      "value": 42
    }
  ],
  "option2": 5
}
```

The above example shows two distinct JSON objects that both have a `"version"` member that is a discriminator for the
expected data structure.

```cpp
namespace version1 {
struct Config {
	int version;
	std::string name;
	int value;
	std::string next_question;
};
}

namespace version2 {
struct Config {
	int version;
	std::map<std::string, int> config_options;
	int option2;
};
}
using configs_t = std::variant<version1::Config, version2::Config>;

struct Switcher {
	// Convert JSON tag member to type index
	size_t operator( )( int type ) const {
		return static_cast<std::size_t>( type - 1 );
	}

	// Get value for Tag from class value
	std::size_t operator( )( configs_t const &v ) const {
		return static_cast<std::size_t>( v.index( ) );
	}
};

namespace daw::json {
  template<>
  struct json_data_contract<version1::Config> {
    using type = json_member_list<
      json_number<"version", int>,  
      json_string<"name">,
      json_number<"value", int>,
      json_string<"next question">
    >;
    
    static constexpr auto to_json_data( version1::Config const & v ) {
    	return std::forward_as_tuple( v.version, v.name, v.value, v.next_question );
    }
  };
  
  template<>
  struct json_data_contract<version2::Config> {
    using type = json_member_list<
      json_number<"version", int>,  
      json_key_value_array<"config_options", 
        std::map<std::string, int>,
        json_number<"value", int>, 
        json_string<"name">
      >,
      json_number<"option2", int>
    >;
    
    static constexpr auto to_json_data( version2::Config const & v ) {
    	return std::forward_as_tuple( v.version, v.config_options, v.option2 );
    }
  };
  
  template<>
  struct json_data_contract<std::variant<version1::Config, version2::Config>> {
      using type = json_submember_tagged_variant<
        json_number<"version", int>,
        Switcher,   
        version1::Config,
        version2::Config
      >;  
  };
}
```
