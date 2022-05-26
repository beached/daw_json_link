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

### Values

* `yes` - All checks are performed
* `no` - Disable many parse time checks, assumes perfect input

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