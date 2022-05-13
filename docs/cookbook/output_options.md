# Output Options

`to_json` allows optional policy flags to control the output. The default's are specified below under Format Policy
Flags.

## Example
The example below shows output with the defaults and with Pretty formatting enabled.
To see a working example using this code, refer
to [cookbook_output_flags1_test.cpp](https://raw.githubusercontent.com/beached/daw_json_link/v3/tests/src/cookbook_output_flags1_test.cpp).

```cpp
struct OutputFlags1 {
  std::string s;
  std::vector<int> ary;
};

namespace daw::json {
  template<>
  struct json_data_contract<OutputFlags1> {
    static constexpr char const s[] = "s";
    static constexpr char const ary[] = "ary";
    using type = json_member_list<json_string<s>, json_array<ary, int>>;

    static inline auto to_json_data( OutputFlags1 const &v ) {
      return std::forward_as_tuple( v.s, v.ary );
    }
  };
} // namespace daw::json

int main( ) {
  auto const v = OutputFlags1{ "Hello", { 1, 2, 3 } };
  puts( daw::json::to_json( v ).c_str( ) );
  puts( daw::json::to_json(
    v, daw::json::output_flags<daw::json::SerializationFormat::Pretty> ).c_str( ) );
}
```

# Format Policy Flags

## SerializationFormat

Enable or disable formatted output

### Values

    * Minified - Output the minimal JSON document
    * Pretty - Use newlines and indentation to format JSON document

### Default

    * Minified

## IndentationType

Control how the file is indented

### Values

    * Tab - use tab character for indentation
    * Space1 - Indent 1 space
    * Space2 - Indent 2 spaces
    * Space3 - Indent 3 spaces
    * Space4 - Indent 4 spaces
    * Space5 - Indent 5 spaces
    * Space8 - Indent 8 spaces
    * Space10 - Indent 10 spaces

### Default

    * Space2

## RestrictedStringOutput

Allow for a JSON document that is ASCII only and each byte has a value <= 0x7F

### Values

    * None - Do not restrict to 7bit ASCII
    * OnlyAllow7bitStrings - Restrict all string member values and all member names to 7bits, escaping all values >= 0x7F.

### Default

    * None

## NewLineDelimiter

    Change how newlines are delimited

### Values

    * n - Use \n to delimit newlines
    * rn - Use \r\n to delimit newlines

### Default

    * n

## OutputTrailingComma

    With arrays/classes output optionally output a trailing comma

### Values

    * No - Do not output trailing comma's. This is conformant
    * Yes - Add non-conformant trailing commas to array elements and class members

### Default

    * No