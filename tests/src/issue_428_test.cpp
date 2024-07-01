// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <iostream>
#include <type_traits>
#include <vector>

template<class Key, class T>
struct vector_map {
	using value_type = std::pair<Key, T>;
	using vector_type = std::vector<value_type>;
	using reference = typename vector_type::reference;
	using const_reference = typename vector_type::const_reference;
	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;

private:
	vector_type data_;

public:
	vector_map( ) = default;

	template<typename Container,
	         std::enable_if_t<
	           not std::is_same_v<vector_map, daw::remove_cvref_t<Container>>,
	           std::nullptr_t> = nullptr>
	vector_map( Container &&values ) {
		for( auto &&v : values ) {}
	}

	template<class... Args>
	std::pair<iterator, bool> emplace( Args &&...args ) {
		data_.emplace_back( std::make_pair( std::forward<Args>( args )... ) );
		return std::make_pair( data_.end( ) - 1, true );
	}
};

template<typename class_type>
struct map_builder {
	constexpr class_type operator( )( ) const {
		static_assert( std::is_same_v<int, class_type> );
		return class_type{ };
	}

	template<typename T>
	constexpr class_type operator( )( T &&v ) const {
		static_assert( std::is_same_v<int, class_type> );
		return class_type( v );
	}

	template<typename Iter>
	constexpr class_type operator( )( Iter first, Iter last ) const {
		static_assert( std::is_same_v<int, class_type> );
		class_type ans;
		while( first != last ) {
			auto [k, v] = *first;
			ans.emplace( k, v );
			++first;
		}
		return ans;
	}
};

namespace daw::json {
	template<class Key, class T>
	struct json_data_contract<vector_map<Key, T>> {
		using class_type = vector_map<Key, T>;

		//using constructor_t = map_builder<class_type>;
		using type = json_type_alias<
		  json_key_value_no_name<class_type, Key, T, map_builder<class_type>>>;
	};
} // namespace daw::json

void json_test( std::string_view data_view ) {
	auto obj = daw::json::from_json<vector_map<std::string, int>>( data_view );
	auto obj_json = daw::json::to_json(
	  obj, daw::json::options::output_flags<
	         daw::json::options::SerializationFormat::Pretty> );

	std::cout << obj_json.c_str( ) << std::endl;
}
