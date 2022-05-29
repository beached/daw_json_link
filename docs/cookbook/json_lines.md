# JSON Lines Support

[JSON Lines](https://jsonlines.org/) is in the form of an iterator `daw::json::json_lines_iterator` and a range `daw::json::json_lines_range` like object. They allow iteration over each lines in the JSON Lines file. 

## Parsing JSON Lines
A working example can be seen at [json_lines_test.cpp](https://raw.githubusercontent.com/beached/daw_json_link/release/tests/src/json_lines_test.cpp)

```json lines
{
  "a": 1,
  "b": false
}
{
  "a": 2,
  "b": true
}
```

The code to iterate over these elements can look like

```cpp
struct Element {
	int a;
	bool b;
};

namespace daw::json {
	template<>
	struct json_data_contract<Element> {
		static constexpr char const a[] = "a";
		static constexpr char const b[] = "b";
		using type = json_member_list<json_link<a, int>, json_link<b, bool>>;
	};
} // namespace daw::json

int main( ) {
	auto jl_range = daw::json::json_lines_range<Element>( json_lines_doc );
  for( Element e : jl_range ) {
      std::cout << e.a << ", " << e.b << '\n';
  }  
}
```

## Serializing to JSON Lines

Staring with the `Element` type in the previous example, one can output to a JSON Line document as follows.

```cpp
  std::vector<Element> elements = get_element_vector( );
  auto result = std::string( );
  auto it = std::back_inserter( result );
  for( auto const & e : elements ) {
      daw::json::to_json( e, it );
      *it = '\n';
  }
```