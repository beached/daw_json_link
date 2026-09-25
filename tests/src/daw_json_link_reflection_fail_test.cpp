// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Each DAW_REFL_FAIL_CASE is expected to fail to compile with a specific
// static_assert message. See tests/CMakeLists.txt

#include <daw/json/daw_json_link.h>

using daw::json::reflect;
using namespace daw::json;

#if DAW_REFL_FAIL_CASE == 1
// reflect is for class types
enum class [[= reflect]] T { A };
#elif DAW_REFL_FAIL_CASE == 2
struct[[= reflect.rename<"t">]] T {
	int v;
};
#elif DAW_REFL_FAIL_CASE == 3
struct[[= reflect.ignored]] T {
	int v;
};
#elif DAW_REFL_FAIL_CASE == 4
struct[[= reflect.enum_string]] T {
	int v;
};
#elif DAW_REFL_FAIL_CASE == 5
enum class [[= reflect.enum_string]] [[= reflect.map_as<
  json_number_no_name<int>>]] T : int { A };
#elif DAW_REFL_FAIL_CASE == 6
// A type level map_as must be a no name JSON type
struct[[= reflect.map_as<json_number<"t", int>>]] T {
	int v;
};
#elif DAW_REFL_FAIL_CASE == 7
// A type level map_as must be a JSON type
struct[[= reflect.map_as<int>]] T {
	int v;
};
#else
#error "Unknown DAW_REFL_FAIL_CASE"
#endif

int main( ) {
	(void)to_json( T{ } );
}
