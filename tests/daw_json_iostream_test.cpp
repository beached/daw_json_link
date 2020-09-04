

#include <daw/json/daw_json_iostream.h>

#include <vector>

struct NumberX {
	int x;
};

namespace daw::json {
	template<>
	struct json_data_contract<NumberX> {
		using opt_into_iostreams = void;
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"x", int>>;
#else
		static constexpr char const x[] = "x";
		using type = json_member_list<json_number<x, int>>;
#endif
		static inline auto to_json_data( NumberX const &v ) {
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
	constexpr std::string_view const json_data3 = R"([{"x":1},{"x":2},{"x":3}])";

	std::vector<NumberX> nx = daw::json::from_json_array<NumberX>( json_data3 );

	std::cout << nx << '\n';
}
