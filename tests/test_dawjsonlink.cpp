#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_iterator.h>
#include <daw/daw_memory_mapped_file.h>

#include <iostream>
#include <libsocket/inetclientstream.hpp>

void notify(const std::string& msg) {
  try {
    libsocket::inet_stream sock("localhost", "9001", LIBSOCKET_IPv4);
    sock << msg;
  } catch (...) {
    // standalone usage
  }
}

struct coordinate_t {
	double x;
	double y;
	double z;
	// ignore string name
	// ignore object opts
};

struct coordinates_t {
	std::vector<coordinate_t> coordinates;
};

namespace daw::json {
	template<>
	struct json_data_contract<coordinate_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"x">, json_number<"y">, json_number<"z">>;
#else
		constexpr inline static char const x[] = "x";
		constexpr inline static char const y[] = "y";
		constexpr inline static char const z[] = "z";
		using type =
		  json_member_list<json_number<x>, json_number<y>, json_number<z>>;
#endif
	};

	template<>
	struct json_data_contract<coordinates_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_array<"coordinates", coordinate_t>>;
#else
		constexpr inline static char const coordinates[] = "coordinates";
		using type = json_member_list<json_array<coordinates, coordinate_t>>;
#endif
	};
} // namespace daw::json


int main(int argc, char *argv[]) {
  auto const text = daw::filesystem::memory_mapped_file_t<>( "/tmp/1.json" );

  std::stringstream ostr;
  ostr << "C++ DAW JSON Link\t" << getpid();
  notify(ostr.str());

	auto const json_sv = std::string_view( text.data( ), text.size( ) );

	using range_t = daw::json::json_array_range<coordinate_t, true>;

  double x = 0, y = 0, z = 0;
  int len = 0;

	for( auto c : range_t( json_sv, "coordinates" ) ) {
		++len;
		x += c.x;
		y += c.y;
		z += c.z;
	}

  std::cout << x / len << std::endl;
  std::cout << y / len << std::endl;
  std::cout << z / len << std::endl;

  notify("stop");
  return EXIT_SUCCESS;
}
