// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <optional>
#include <tuple>

struct Tuple {
	std::tuple<int, bool, long long> a;

	constexpr bool operator==( Tuple const &rhs ) const {
		return a == rhs.a;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Tuple> {
		static constexpr char const a[] = "a";
		using type =
		  json_member_list<json_tuple<a, std::tuple<int, bool, long long>>>;

		static constexpr auto to_json_data( Tuple const &t ) {
			return std::forward_as_tuple( t.a );
		}
	};
} // namespace daw::json

constexpr auto tp0 =
  daw::json::from_json<Tuple>( R"json({"a":[5,false,-42]})json" );
static_assert( std::get<0>( tp0.a ) == 5 );
static_assert( std::get<1>( tp0.a ) == false );
static_assert( std::get<2>( tp0.a ) == -42 );
//
struct NullableTuple {
	std::optional<std::tuple<int, bool, long long>> a;

	constexpr bool operator==( NullableTuple const &rhs ) const {
		return a == rhs.a;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<NullableTuple> {
		static constexpr char const a[] = "a";
		using type = json_member_list<
		  json_tuple_null<a, std::optional<std::tuple<int, bool, long long>>>>;

		static constexpr auto to_json_data( NullableTuple const &t ) {
			return std::forward_as_tuple( t.a );
		}
	};
} // namespace daw::json

// optional's copy ctor is only constexpr in C++20
#if defined( __cpp_lib_optional )
#if __cpp_lib_optional >= 202106L
#define DAW_HAS_OPTIONAL_CX_COPY
#endif
#endif

#if defined( DAW_HAS_OPTIONAL_CX_COPY )
constexpr auto tp1 =
  daw::json::from_json<NullableTuple>( R"json({"a":[5,false,-42]})json" );
static_assert( tp1.a );
static_assert( std::get<0>( *tp1.a ) == 5 );
static_assert( std::get<1>( *tp1.a ) == false );
static_assert( std::get<2>( *tp1.a ) == -42 );
#endif

static_assert( not daw::json::from_json<NullableTuple>( R"json({})json" ).a );

int main( ) {
#if not defined( DAW_HAS_OPTIONAL_CX_COPY )
	auto tp =
	  daw::json::from_json<NullableTuple>( R"json({"a":[5,false,-42]})json" );
	daw_ensure( tp.a );
	daw_ensure( std::get<0>( *tp.a ) == 5 );
	daw_ensure( std::get<1>( *tp.a ) == false );
	daw_ensure( std::get<2>( *tp.a ) == -42 );
#endif
}
