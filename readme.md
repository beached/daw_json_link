# daw_json_link v3

For now v3 is focussed on deserialization.  This can be accomplished by writing a function called describe_json_class with a single arugment that is your type.  The library is only concerned with it's return value.  For example:

```C++
struct TestClass {
	int i = 0;
	double d = 0.0;
	bool b = false;
	daw::string_view s{};
	std::vector<int> y{};

	TestClass( int Int, double Double, bool Bool, daw::string_view S,
	            std::vector<int> Y ) noexcept
	  : i( Int )
	  , d( Double )
	  , b( Bool )
	  , s( S )
	  , y( Y ) {}
};

auto describe_json_class( TestClass ) noexcept {
	using namespace daw::json;
	return json_parser_t<
		json_number<"i", int>,
		json_number<"d">,
		json_bool<"b">,
		json_string<"s", daw::string_view>,
		json_array<"y", std::vector<int>, json_number<"", int>>
 	>{};
}

std::string json_data = ...;
std::string json_array_data = ...;

auto test_class = daw::json::from_json_t<test_001_t>( json_data );
auto arry_of_test_class = daw::json::from_json_array_t<json_class<"", City>>( json_data );
```
Both aggregate and normal construction is supported.  A class like:

```cpp
struct AggClass {
	int a{};
	double b{};
};
auto describe_json_class( AggClass ) noexcept {
	using namespace daw::json;
	return json_parser_t<
		json_number<"a", int>,
		json_number<"b">
	>{};
}
```
Works too.
