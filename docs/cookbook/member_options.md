# Member Options/Type Options

Many of the member types have options that can change how they are parsed. The options and their setters are located in
the `daw::json::options` namespace and are placed into `Options` template parameter via a setter function that encodes
the options into a `json_options_t`.

___

# `json_number`

To set number options use the `daw::json::options::number_opt( Flags... )` method.

## `LiteralAsStringOpt`

Controls the ability to parse numbers that are encoded as strings.

### Values

* `Never` - Never allow parsing this member as a string. It is a parser error if this member is encoded as a string
* `Maybe` - Allow parsing this member as a string or number.
* `Always` - Only allow parsing this member as a string. It is an error for this member to not be encoded as a string.

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

Control the floating point output format

### Values

* `Auto` - Automatically choose between decimal and scientific output formats.
* `Scientific` - Always format in terms of an exponent `<whole>[.fraction]e<exponent>`.
* `Decimal` - Always format in terms of an exponent `<whole>[.fraction]e<exponent>`.

### Default

* `Auto`

___

# `json_bool`

To set bool options use the `daw::json::options::bool_opt( Flags... )` method.

## `LiteralAsStringOpt`

Controls the ability to parse booleans that are encoded as strings.

### Values

* `Never` - Never allow parsing this member as a string. It is a parser error if this member is encoded as a string
* `Maybe` - Allow parsing this member as a string or number.
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

## `JsonCustomTypes`

Custom JSON types can be Strings(default), unquoted Literals, or a mix.

### Values

* `String` - Parser always expects a JSON string. Will surround serialized value with double quotes
* `Literal` - Parser will expect a valid JSON literal number, bool, null
* `Any` (Experimental) Parser will return any valid JSON value excluding leading whitespace. strings will be quoted.
  `Any` is suitable for constructing a `json_value` to allow adhoc parsing if `json_raw` is not suitable

### Default

* `String`