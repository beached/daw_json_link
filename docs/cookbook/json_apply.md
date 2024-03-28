# json_apply

`json_apply` parses the document to parameter types specified in signature, then passes them to
the Callable for evaluation. This is similar to `std::apply` but using a JSON documents instead of tuples.

## Uses

`json_apply` is useful in IPC/RPC situations where the data is (de)serialized from/to JSON, such as JSON RPC or IPC with web views.

## Example

```c++
struct NumberX {
    int x;
};
// ...
auto x = daw::json::json_apply( 
  R"json({"x":10})json", 
  []( NumberX nx ) {
	  return nx.x;
  } );
  assert( x == 10 );
```
