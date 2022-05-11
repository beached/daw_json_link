# Readable Values

A readable value is also called and option type or nullable type.  This includes std::optional, std::unique_ptr, std::shared_ptr, and pointers.  In order to use them without a custom Constructor type for each mapping, or in deduced situations (`json_link<...>`) they must be mapped via the `daw::readable_value_traits` type mapping.