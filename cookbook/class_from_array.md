### Classes from array's

Sometimes the members of a class are encoded as an array in JSON.  GeoJSON is an example of this.  

```json
[ 34.3434, 134.3434 ]
```

The above is the encoding of a Point in GeoJSON.

```c++
struct Point {
	double x;
	double y;
};

namespace daw::json {
  template<>
  struct json_data_contract<Point> {
    using type = json_ordered_member_list<double, double>;

    static constexpr auto to_json_data( Point const &p ) {
      return std::forward_as_tuple( Point.x, Point.y );
    }
  };
}
```
