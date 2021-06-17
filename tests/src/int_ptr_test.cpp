// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_span.h>
#include <daw/json/daw_json_link.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

/***
 * This example shows how one might encode a int * array type where another
 * member has the size. In the future there will be a mapping type for this to
 * make it explicit and declarative
 */

struct Foo {
	std::size_t n = 0;
	int *data = nullptr;
};

template<JSONNAMETYPE, typename T>
struct ArrayPointerConstructor {
	inline static thread_local std::size_t size =
	  std::numeric_limits<std::size_t>::max( );

	struct SizeCtor {
		inline std::size_t operator( )( std::size_t v ) const noexcept {
			size = v;
			return v;
		}
	};

	struct ArrayCtor {
		template<typename Iterator>
		T *operator( )( Iterator first, Iterator last ) const {
			auto result = std::unique_ptr<T[]>( new T[size] );
			T *ptr = result.get( );
			while( first != last ) {
				*ptr = *first;
				++ptr;
				++first;
			}
			return result.release( );
		}
	};
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const member_n[] = "n";
		static constexpr char const member_data[] = "data";

		using type = json_member_list<
		  json_number<member_n, std::size_t, number_opts_def,
		              ArrayPointerConstructor<member_data, int>::SizeCtor>,
		  json_array<member_data, int, int *,
		             ArrayPointerConstructor<member_data, int>::ArrayCtor>>;

		static constexpr auto to_json_data( Foo const &f ) {
			return std::tuple{ f.n, daw::span<int const>( f.data, f.n ) };
		}
	};
} // namespace daw::json

int main( ) {
	std::string_view jdoc = R"({"n":5,"data":[1,2,3,4,5]})";
	auto foo = daw::json::from_json<Foo>( jdoc );
	std::cout << foo.n << '\n';

	auto jdoc2 = daw::json::to_json( foo );
	std::cout << jdoc2 << '\n';
	delete[] foo.data;
}
