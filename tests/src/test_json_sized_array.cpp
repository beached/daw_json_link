// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/daw_ensure.h>

#include <cassert>
#include <memory>
#include <tuple>

struct Stuff {
	std::size_t size;
	std::unique_ptr<int[]> values;
};

template<typename T>
struct UniquePtrArrayCtor {
	constexpr std::unique_ptr<T[]> operator( )( ) const {
		return { };
	}

	template<typename Iterator>
	inline std::unique_ptr<T[]> operator( )( Iterator first, Iterator last,
	                                         std::size_t sz ) const {
		assert( sz <= 1024 ); // ensure size isn't too large
		auto result = std::unique_ptr<T[]>( new T[static_cast<std::size_t>( sz )] );
		auto out_last = std::copy( first, last, result.get( ) );
		(void)out_last;
		auto const elements_copied = out_last - result.get( );
		(void)elements_copied;
		assert( elements_copied >= 0 );
		assert( static_cast<std::size_t>( elements_copied ) == sz );

		return result;
	}
};

namespace daw::json {
	template<>
	struct json_data_contract<Stuff> {
		static constexpr const char values[] = "values";
		static constexpr const char size[] = "size";
		using type = json_member_list<
		  json_number<size, unsigned>,
		  json_sized_array<values, int, json_number<size, unsigned>,
		                   std::unique_ptr<int[]>, UniquePtrArrayCtor<int>>>;

		static constexpr auto to_json_data( Stuff const &s ) {
			return std::forward_as_tuple( s.size, s.values );
		}
	};
} // namespace daw::json

int main( ) {
	static constexpr daw::string_view json_doc = R"json(
{
	"size": 3,
	"values": [1,2,3]
}
)json";

	auto my_stuff = daw::json::from_json<Stuff>( json_doc );
	daw_ensure( my_stuff.size == 3 );
	daw_ensure( my_stuff.values[0] == 1 );
	daw_ensure( my_stuff.values[1] == 2 );
	daw_ensure( my_stuff.values[2] == 3 );
}
