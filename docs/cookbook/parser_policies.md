# Parser Options

## Parser option presets

The default parser policy is validating and disallows comments.  The `Unchecked` variants will skip most error checks and can give a good performance boost; this is useful when you can trust and know the data will be free of malicious errors.

The predefined policies are:
* `daw::json::NoCommentSkippingPolicyChecked` (default)
* `daw::json::NoCommentSkippingPolicyUnchecked`

Hash comments, a `#` in whitespace until end of line is comment.
* `daw::json::HashCommentSkippingPolicyChecked`
* `daw::json::HashCommentSkippingPolicyUnchecked`
```json
{
  "a": 234, # This is comment
  "b": "This is not a comment"
}
```
C++ comments, either a `//` in whitespace until end of line, or between `/*` and `*/` in whitespace, are commments.
* `daw::json::CppCommentSkippingPolicyChecked`
* `daw::json::CppCommentSkippingPolicyUnchecked`
```json
{
  "a": 234, // This is comment
  "b": "This is not a comment", /* but these
       are part of the same comment */ "c": "value over here"
}
```
To change the policy from the default one for parsing, the following types/methods have a template argument for the `PolicyType` or an equivalent.

* `daw::json::json_value` -> `daw::json::basic_json_value<PolicyType>`
* `daw::json::json_pair` -> `daw::json::basic_json_pair<PolicyType>`
* `daw::json::from_json<T>` -> `daw::json::from_json<T, PolicyType>`
* `daw::json::from_json_array<ElementType>` -> `daw::json::from_json_array<ElementType, ContainerType, PolicyType>`

An example of parsing with C++ comments could be:
```c++
MyType value = daw::json::from_json<
		MyType, 
		daw::json::CppCommentSkippingPolicyChecked
	>( json_string );
```

## Individual parser options

`ExecModeTypes` - There are 3 paths here, with only `compile_time` being fully supported.  It is also the fastest generally
  * `ExecModeTypes::compile_time` - This option provides constexpr parsing and is the normal path.  In the future the other modes maybe faster and better supported.  It may use builtins and runtime only methods when appropriate and one can detect if the current evaluaton is constexpr.
  * `ExecModeTypes::runtime` - This mode includes baseline methods that cannot run during compiletime.  
  * `ExecModeTypes::simd` - This mode will utilize SIMD enhanced methods for certain functions

`ZeroTerminatedString` - The string data passed to `from_json` is zero terminated.  This allows some potential optimizations around bounds checking.  If the type passed to `from_json` has a specialization of `daw::json::is_zero_terminated_string` it will be assumed to be zero terminated.  By default std::basic_string evaluates to being a zero terminated string.
  * `ZeroTerminatedString::no` - Default for non-specialized types of `daw::json::is_zero_terminated_string`
  * `ZeroTerminatedString::yes` - The string passed to `from_json` is zero terminated

`PolicyCommentTypes`