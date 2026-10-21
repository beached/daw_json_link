# Parser Options

## `ExecModeTypes`

There are 3 levels of execution modes; compile time, runtime, and simd. The default and currently supported mode is '
compile_time'. The others are often not faster or as well tested.

### Values

* `compile_time` - This option allows constexpr parsing. In the future the other modes maybe
  faster and better supported. It may use builtins and runtime only methods when appropriate when detection of the
  current evaluation mode is available (e.g `is_constant_evaluated`)
* `runtime` - This mode includes `compile_time` methods along with using methods only available at runtime (
  e.g `memchr`).
* `simd` - This mode includes `runtime` methods along with some simd enhanced methods (e.g. in number parsing).

### Default

* `compile_time`

## `ZeroTerminatedString`

The string data passed to `from_json` is zero terminated. This allows some potential
optimizations around bounds checking. If the type passed to `from_json` has a specialization
of `daw::json::is_zero_terminated_string` it will be assumed to be zero terminated. By default, std::basic_string
evaluates to being a zero terminated string.

### Values

* `no` - Bounds checking does not take advantage of assuming a terminated string
* `yes` - The string passed to `from_json` is zero terminated

### Default

* `no` or the value of the `daw::json::is_zero_terminated_string` specialization for the String input type

## `PolicyCommentTypes`

Are comments in whitespace allowed(defaults to no) and, if so, what kind

### Values

* `none` - Comments are not allowed. This is conformant with JSON and the fastest
* `cpp` - Allow C++ style comments, both `/* comment */` and `// comment until newline` are allowed in places where
  whitespace is allowed/required
* `hash` - Allow `# comment until newline` hash style line comments (e.g `# comment`)

### Default

* `none`

## `CheckedParseMode`

Do a checked parse or not. If the data is known to be trustworthy and generated correctly, one can
disable checking of a parse and gain performance(measured 15% in some documents).

**Warning:** `no` is a memory-safety contract, not just a speed/strictness knob. Disabled checks
include bounds checks on the parse cursor (e.g. scanning for the next member no longer verifies the
cursor is still within the buffer), so malformed, truncated, or adversarial input can cause
out-of-bounds reads (undefined behavior) rather than a catchable parse error. Only use `no` with
input you fully trust and control, such as data your own code just serialized — never with input
received from an external or untrusted source.

### Values

* `yes` - All checks are performed
* `no` - Disable many parse time checks, assumes perfect, trusted input; malformed input is undefined behavior, not just a wrong result

### Default

* `yes`

## `MinifiedDocument`

Assume the document in minified and there is no whitespace. This may offer performance benefits(
measured 5% in some minified documents). This option is incompatible with comments.

### Values

* `no` - Assumes there is whitespace in document.
* `yes` - Does not skip whitespace in documents, assumes a minified document.

### Default

* `no`

## `UseExactMappingsByDefault`

* 'no' - Extra members in JSON document are not an error by default
* `yes` - By default, Any non-mapped member is an error

### Default

* 'no'
## `ArrayReserve`

How many elements to reserve up front when parsing a JSON array into a `std::vector` whose size is not known yet.
Reserving avoids the first few reallocations as the vector grows, at the cost of unused capacity for small arrays. The
amount is in elements, not bytes, and applies to every such array in the parse.

When an array was skipped before being parsed, e.g. a member that appears out of order in the document, its element
count is already known and exactly that many are reserved instead.

This only affects the default constructor for `std::vector`. Custom constructors are not changed.

### Values

* `none` - Do not reserve, the vector grows as elements are parsed
* `elements_128` - Reserve 128 elements
* `elements_512` - Reserve 512 elements
* `elements_1024` - Reserve 1024 elements

### Default

* `none`

## `AllowLeadingZeroPlus`

Accept skipped numbers that start with a `+` or have leading zeros in their whole part, e.g. `+1`, `007` and
`-00.5`. These are not valid JSON. Skipped numbers are those of members that are not mapped and those inside skipped
classes and arrays. Mapped numbers always follow the JSON grammar.

### Values

* `yes` - A leading `+` and leading zeros are allowed in skipped numbers
* `no` - Skipped numbers follow the JSON grammar

### Default

* `yes`
