

#include "defines.h"

#include <daw/json/daw_json_iostream.h>

#include <deque>
#include <list>
#include <vector>

struct NumberX {
	int x;
};

namespace daw::json {
	template<>
	struct json_data_contract<NumberX> {
		// Having this type alias, any type on the RHS works, says that operator<<(
		// ostream &, NumberX const & ) is available.  When used, it will output the
		// JSON serialization
		using opt_into_iostreams = void;
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"x", int>>;
#else
		static constexpr char const x[] = "x";
		using type = json_member_list<json_number<x, int>>;
#endif
		static constexpr auto to_json_data( NumberX const &v ) {
			return std::forward_as_tuple( v.x );
		}
	};
} // namespace daw::json

static_assert(
  daw::json::json_details::is_opted_into_json_iostreams_v<NumberX> );
static_assert(
  daw::json::json_details::is_container_opted_into_json_iostreams_v<
    std::vector<NumberX>> );

int main( ) {
	DAW_CONSTEXPR std::string_view const single_numberx = R"({"x":123})";
	DAW_CONSTEXPR auto const nx = daw::json::from_json<NumberX>( single_numberx );

	DAW_CONSTEXPR std::string_view const numberx_in_json_array =
	  R"([{"x":1},{"x":2},{"x":3}])";
	std::vector<NumberX> vec_nx =
	  daw::json::from_json_array<NumberX>( numberx_in_json_array );
	std::deque<NumberX> deq_nx =
	  daw::json::from_json_array<NumberX, std::deque<NumberX>>(
	    numberx_in_json_array );
	std::list<NumberX> lst_nx =
	  daw::json::from_json_array<NumberX, std::list<NumberX>>(
	    numberx_in_json_array );

	std::cout << nx << '\n';
	std::cout << vec_nx << '\n';
	std::cout << deq_nx << '\n';
	std::cout << lst_nx << '\n';
}
