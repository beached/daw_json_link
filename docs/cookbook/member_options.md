::# Member Options/Type Options

Many of the member types have options that can change how they are parsed. The options and their setters are located in
the `daw::json::options` namespace and are placed into `Options` template parameter via a setter function that encodes
the options into a `json_options_t`.

___

# `json_number` and floating-point mappings

To set number options use the `daw::json::options::number_opt( Flags... )` method.

## `LiteralAsStringOpt`

Controls the ability to parse numbers that are encoded as strings.

### Values

* `Never` - Never allow parsing this member as a string. It is a parser error if this member is encoded as a string
* `Maybe` - Allow parsing this member as a string or number.
* `Always` - Only allow parsing this member as a string. It is an error for this member to not be encoded as a string.

### Default

* `Never`

## `JsonRangeCheck`

### Values

* `Never` - Do not attempt to check for narrowing when parsing integral types
* `CheckForNarrowing` - Attempt to check for narrowing when parsing integral types

### Default

* `Never`

## `JsonNumberErrors`

When outputting floating point numbers, control whether Inf/NaN values can be parsed/serialized. This requires that the
`LiteralAsString` option be set to `Maybe` or `Always`

### Values

* `None` - Do not allow serialization/parsing of NaN/Inf
* `AllowNaN` - Allow NaN to be expressed/parsed if number can be a string
* `AllowInf` - Allow Inf/-Inf to be expressed/parsed if number can be a string
* `AllowNanInf` - Allow NaN/Inf/-Inf to be expressed/parsed if number can be a string

### Default

* `None`

## `FPOutputFormat`

Control the floating-point output format used by `json_fp` and the
`json_checked_fp` mappings. These mappings are useful when the serialized
representation needs a defined precision, rather than the shortest default
representation.

### Values

* `Auto` - Round to `Precision` significant digits and use decimal notation for
  decimal-point positions from -4 through 6; use scientific notation outside
  that range.
* `Scientific` - Use exponent notation `<whole>[.fraction]e<exponent>`. Its
  `Precision` is the number of digits after the decimal point.
* `Decimal` - Use fixed-point notation `<whole>[.fraction]`. Its `Precision`
  is the number of digits after the decimal point.
* `Minimum` - Round to `Precision` significant digits and use the shortest
  unpadded decimal representation.

For `Auto` and `Minimum`, `Precision` counts significant digits. For `Decimal`
and `Scientific`, it counts digits after the decimal point. The default
`daw::max_value<unsigned>` precision means no explicit digit limit; `Auto` and
`Minimum` then preserve the shortest representation, while `Decimal` and
`Scientific` still apply their selected notation. Rounding uses nearest-even
rounding.

### `json_fp` examples

`json_fp` is the floating-point equivalent of `json_number` with explicit
format and precision template parameters:

```cpp
using decimal_amount = daw::json::json_fp_no_name<
  double, daw::json::options::FPOutputFormat::Decimal, 2>;
using scientific_amount = daw::json::json_fp_no_name<
  double, daw::json::options::FPOutputFormat::Scientific, 3>;
using general_amount = daw::json::json_fp_no_name<
  double, daw::json::options::FPOutputFormat::Auto, 4>;

daw::json::to_json<decimal_amount>( 12.345 );     // "12.35"
daw::json::to_json<scientific_amount>( 12.345 );  // "1.235e1"
daw::json::to_json<general_amount>( 12.345 );     // "12.35"
```

Named, nullable, and narrowing-checking variants use the same `Format` and
`Precision` parameters:

```cpp
using amount = daw::json::json_fp<
  "amount", double, daw::json::options::FPOutputFormat::Decimal, 2>;
using optional_amount = daw::json::json_fp_null<
  "amount", std::optional<double>,
  daw::json::options::FPOutputFormat::Decimal, 2>;
using checked_amount = daw::json::json_checked_fp<
  "amount", double, daw::json::options::FPOutputFormat::Decimal, 2>;
```

### `json_number` versus `json_fp`

For floating-point values, `json_number` also reads
`JsonMember::fp_output_format`, which is set through `number_opt`:

```cpp
using number_decimal = daw::json::json_number_no_name<double,
  daw::json::options::number_opt(
    daw::json::options::FPOutputFormat::Decimal )>;
```

The distinction is that `json_number` selects the output format through its
encoded options and uses the default precision. `json_fp` selects the format
through its `Format` template parameter and additionally provides an explicit
`Precision` template parameter. Internally, both mappings expose the selected
format as `JsonMember::fp_output_format`; `json_fp` also exposes its precision
to the serializer.

### Default

* `Auto`

___

# `json_bool`

To set bool options use the `daw::json::options::bool_opt( Flags... )` method.

## `LiteralAsStringOpt`

Controls the ability to parse booleans that are encoded as strings.

### Values

* `Never` - Never allow parsing this member as a string. It is a parser error if this member is encoded as a string
* `Maybe` - Allow parsing this member as a string or boolean literal.
* `Always` - Only allow parsing this member as a string. It is an error for this member to not be encoded as a string.

### Default

* `Never`

___

# `json_string`

To set string options use the `daw::json::options::string_opt( Flags... )` method.

## `EightBitModes`

Controls whether any string character has the high bit set. If restricted, the member will escape any character with the
high bit set and when parsing will throw if the high bit is encountered. This allows 7bit JSON encoding.

### Values

* `DisallowHigh` - Escape any character with the high bit set and throw when encountered
  during parse
* `AllowFull` - Allow the full 8bits in output without escaping

### Default

* `AllowFull`

___

# `json_string_raw`

To set raw string options use the `daw::json::options::string_raw_opt( Flags... )` method.

## `EightBitModes`

Controls whether any string character has the high bit set. If restricted, the member will escape any character with the
high bit set and when parsing will throw if the high bit is encountered. This allows 7bit JSON encoding.

### Values

* `DisallowHigh` - Escape any character with the high bit set and throw when encountered
  during parse
* `AllowFull` - Allow the full 8bits in output without escaping

### Default

* `AllowFull`

### `AllowEscapeCharacter`

In RAW String processing, if we know that there are no escaped double quotes `\"` we can stop at the first double quote.
This allows faster string parsing

### Values

* `Allow` - Full string processing to skip escaped characters
* `NoEscapedDblQuote` - There will never be a \" sequence inside the string. This allows faster parsing

### Default

* `Allow`

___

# `json_custom`

To set json_custom options use the `daw::json::options::json_custom_opt( Flags... )` method.
See [Custom Types and Output](custom_types.md) for complete converter and
serialization examples.

## `JsonCustomTypes`

Custom JSON types can be Strings(default), unquoted Literals, or a mix.

### Values

* `String` - Parser always expects a JSON string. Will surround serialized value with double quotes
* `Literal` - Parser will expect a valid JSON literal number, bool, null
* `Any` (Experimental) - Parser will return any valid JSON value excluding leading whitespace. Strings remain quoted.
  `Any` is suitable for constructing a `json_value` to allow adhoc parsing if `json_raw` is not suitable

### Default

* `String`
