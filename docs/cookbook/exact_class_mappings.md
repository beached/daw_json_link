By default unmapped members in a JSON document are skipped.  There are a few ways to turn this into an error.

# Global parser option

One can change the default for all classes as a parser option.  For example with `from_json` it would loook similar to:

```c++
daw::json::from_json<Foo>( 
   json_doc1, 
   daw::json::options::parse_flags<
       daw::json::options::UseExactMappingsByDefault::yes>
   );
```

When the value is set to `yes`, you can exclude a class from this by adding a type alias to it's class mapping called `ignore_unknown_members`. For example:

```c++
namespace daw::json {
    template<>
    struct json_data_contract<Foo> {
        using ignore_unknown_members = void;
        
        using type = json_member_list<json_number<"bar", int>>;
        
        //...
    };
}
```

# Enabling per class 

To enable errors for unknown members per class, add a type alias to the mapping called `exact_class_mapping` 

```c++
namespace daw::json {
    template<>
    struct json_data_contract<Zaz> {
        using exact_class_mapping = void;
        
        using type = json_member_list<json_number<"foo", int>>;
        //...
    };
}
```