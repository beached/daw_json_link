# Parser Options

The default parser policy is validating and disallows comments.  The `Unchecked` variants will skip most error checks and can give a good performance boost; this is useful when you can trust and know the data will be free of malicious errors.

The predefined policies are:
* `daw::json::NoCommentSkippingPolicyChecked` (default)
* `daw::json::NoCommentSkippingPolicyUnchecked`

Hash comments, a `#` in whitespace until end of line is comment.
* `daw::json::HashCommentSkippingPolicyChecked`
* `daw::json::HashCommentSkippingPolicyUnchecked`
```
{
  "a": 234, # This is comment
  "b": "This is not a comment"
}
```
C++ comments, either a `//` in whitespace until end of line, or between `/*` and `*/` in whitespace, are comments.
* `daw::json::CppCommentSkippingPolicyChecked`
* `daw::json::CppCommentSkippingPolicyUnchecked`
```
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
MyType value = daw::json::from_json<MyType, daw::json::CppCommentSkippingPolicyChecked>( json_string );
```