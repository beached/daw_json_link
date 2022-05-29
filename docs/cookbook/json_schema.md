# Exporting JSON Schema

One can export the JSON Schema to validate the mapped types.  `#include <daw/json/daw_json_schema.h>` and
call `daw::json::to_json_schema<MyType>( "identifier", "title" );` to generate a `std::string`. There is an output
iterator version too.  
Refer
to [daw_json_schema_test.cpp](https://raw.githubusercontent.com/beached/daw_json_link/release/tests/src/daw_json_schema_test.cpp)
for a working example.