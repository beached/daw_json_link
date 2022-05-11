# Readable Values

A readable value is also called and option type or nullable type.  This includes std::optional, std::unique_ptr, std::shared_ptr, and pointers.  In order to use them without a custom Constructor type for each mapping, or in deduced situations (`json_link<...>`) they must be mapped via the `daw::readable_value_traits` type mapping.

A specialization must have the following, assuming T is the specialized type(e.g `std::optional<int>`):
 * `value_type` type alias to the inner type. e.g ( `std::optional<U>` -> `U` )
 * `is_readable` A variable that must be `static constexpr bool is_readable = true` for readable types.  If false it will mark the type `T` as not readable
 * `static value_type const & read( T const & )` A function that reads the value from the readable type and returns a const ref to it.  A `value_type` return will work too
 * `T operator( )( daw::construct_readable_empty_t ) const` A member function that takes no args and returns an empty value.
 * `T operator( )( daw::construct_readable_value_t, Args... ) const` A member function that when passed appropriate args constructs a value.  It should be SFINAE'able and constrain on valid arguments.
 * `static bool has_value( T const & )` returns if the value is present or not.