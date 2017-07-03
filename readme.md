# daw_json_link 

This library lets you serialize and deserialize C++ structures to and from JSON text.  It will generate a parser/generator for each member you link.

```C++
struct A {
	int a;
	std::string b;

	static void json_link_map( ) {
		link_json_integer( "a", a );
		link_json_string( "b", b );
	}
};

std::string some_json_string = ...

auto a = A::from_json_string( some_json_string.begin( ), some_json_string.end( ) );
```

