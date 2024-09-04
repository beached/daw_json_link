// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>
#include <daw/daw_string_view.h>
#include <daw/daw_visit.h>

#include <cassert>
#include <string>
#include <variant>

struct ConfigV1 {
	constexpr static int version = 1;
	std::string file;
	bool flag;

	[[nodiscard]] bool operator==( ConfigV1 const &rhs ) const {
		return std::tie( file, flag ) == std::tie( rhs.file, rhs.flag );
	}
};

struct ConfigV2 {
	constexpr static int version = 2;
	bool flag1;
	bool flag2;
	bool flag3;
	bool flag4;
	std::string path;

	[[nodiscard]] bool operator==( ConfigV2 const &rhs ) const {
		return std::tie( flag1, flag2, flag3, flag4, path ) ==
		       std::tie( rhs.flag1, rhs.flag2, rhs.flag3, rhs.flag4, rhs.path );
	}
};

using Config = std::variant<ConfigV1, ConfigV2>;

struct ConfigSwitcher {
	constexpr std::size_t operator( )( int n ) const {
		assert( n >= 1 );
		assert( n <= 2 );
		return static_cast<std::size_t>( n - 1 );
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<ConfigV1> {
		static constexpr char const file[] = "file";
		static constexpr char const flag[] = "flag";
		using type = json_member_list<json_string<file>, json_bool<flag>>;

		static inline auto to_json_data( ConfigV1 const &c ) {
			return std::forward_as_tuple( c.file, c.flag );
		}
	};

	template<>
	struct json_data_contract<ConfigV2> {
		static constexpr char const flag1[] = "flag1";
		static constexpr char const flag2[] = "flag2";
		static constexpr char const flag3[] = "flag3";
		static constexpr char const flag4[] = "flag4";
		static constexpr char const path[] = "path";
		using type =
		  json_member_list<json_bool<flag1>, json_bool<flag2>, json_bool<flag3>,
		                   json_bool<flag4>, json_string<path>>;

		static inline auto to_json_data( ConfigV2 const &c ) {
			return std::forward_as_tuple( c.flag1, c.flag2, c.flag3, c.flag4,
			                              c.path );
		}
	};
	template<>
	struct json_data_contract<Config> {
		static constexpr char const version[] = "version";
		using type = json_type_alias<json_intrusive_variant_no_name<
		  Config, json_number<version, int>, ConfigSwitcher>>;
	};
} // namespace daw::json

int main( ) {
	static constexpr daw::string_view json_doc_v1 = R"json(
{
	"version": 1,
	"file": "/path/to/thing",
	"flag": true
}
)json";

	static constexpr daw::string_view json_doc_v2 = R"json(
{
	"version": 2,
	"flag1": true,
	"flag2": false,
	"flag3": false,
	"flag4": true,
	"path": "/path/to/thing"
}
)json";

	auto const cv1 = daw::json::from_json<ConfigV1>( json_doc_v1 );
	auto const cv2 = daw::json::from_json<ConfigV2>( json_doc_v2 );
	auto const c1 = daw::json::from_json<Config>( json_doc_v1 );
	daw_ensure( c1.index( ) == 0 );
	daw_ensure( daw::visit(
	  c1,
	  []( ConfigV2 const & ) -> bool {
		  std::abort( );
	  },
	  [&cv1]( ConfigV1 const &a ) -> bool {
		  return a == cv1;
	  } ) );
	auto const c2 = daw::json::from_json<Config>( json_doc_v2 );
	daw_ensure( c2.index( ) == 1 );
	daw_ensure( daw::visit(
	  c2,
	  []( ConfigV1 const & ) -> bool {
		  std::abort( );
	  },
	  [&cv2]( ConfigV2 const &a ) -> bool {
		  return a == cv2;
	  } ) );
}
