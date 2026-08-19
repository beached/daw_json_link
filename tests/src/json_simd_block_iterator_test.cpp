// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/impl/daw_json_simd_iterator.h>
#include <daw/json/daw_json_link_types.h>

#include <daw/daw_ensure.h>

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

namespace {
	using block = daw::json::json_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::Number>;
	using bool_block = daw::json::json_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::Bool>;
	using string_block = daw::json::json_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::String>;

	template<typename T>
	concept has_number_start = requires( T value ) { value.number_start; };
	template<typename T>
	concept has_boolean_start = requires( T value ) { value.boolean_start; };
	template<typename T>
	concept has_string_start = requires( T value ) { value.string_start; };
	template<typename T>
	concept has_true_start = requires( T value ) { value.true_start; };
	using iterator = daw::json::experimental::json_simd_block_iterator<
	  daw::json::json_number_no_name<double>>;
	using bool_iterator = daw::json::experimental::json_simd_block_iterator<
	  daw::json::json_bool_no_name<bool>>;
	using string_iterator = daw::json::experimental::json_simd_block_iterator<
	  daw::json::json_string_raw_no_name<std::string>>;

	struct parsed_number {
		double value;
	};

	struct parsed_number_constructor {
		[[nodiscard]] parsed_number operator( )( double value ) const {
			return { value };
		}
	};

	using constructed_number = daw::json::json_number_no_name<
	  double, daw::json::number_opts_def, parsed_number_constructor>;
	using constructed_iterator =
	  daw::json::experimental::json_simd_block_iterator<constructed_number>;

	struct parsed_bool {
		bool value;
	};

	struct parsed_bool_constructor {
		[[nodiscard]] parsed_bool operator( )( bool value ) const {
			return { value };
		}
	};

	using constructed_bool = daw::json::json_bool_no_name<
	  bool, daw::json::bool_opts_def, parsed_bool_constructor>;
	using constructed_bool_iterator =
	  daw::json::experimental::json_simd_block_iterator<constructed_bool>;

	void test_iterator_semantics( ) {
		auto empty = iterator( std::string_view{ } );
		daw_ensure( not empty );
		daw_ensure( empty.begin( ) == empty.end( ) );
		daw_ensure( empty == iterator{ } );

		auto const document = std::string{ "[1.25, -2.5, 3.75]" };
		auto range = iterator( document );
		auto first = range.begin( );
		auto copy = first;
		daw_ensure( first );
		daw_ensure( first == copy );
		daw_ensure( first != range.end( ) );

		auto const first_read = *first;
		auto const second_read = *first;
		daw_ensure( first_read == 1.25 );
		daw_ensure( second_read == first_read );
		daw_ensure( first == copy );

		first++;
		daw_ensure( first != copy );
		daw_ensure( *first == -2.5 );
		++first;
		daw_ensure( *first == 3.75 );
		++first;
		daw_ensure( not first );
		daw_ensure( first == range.end( ) );
	}

	void test_json_member_result_type( ) {
		auto values = constructed_iterator( "[4.5, -1.25]" );
		auto first = values.begin( );
		daw_ensure( ( *first ).value == 4.5 );
		++first;
		daw_ensure( ( *first ).value == -1.25 );
		++first;
		daw_ensure( first == values.end( ) );

		auto bool_values = constructed_bool_iterator( "[true, false]" );
		auto bool_first = bool_values.begin( );
		daw_ensure( ( *bool_first ).value );
		++bool_first;
		daw_ensure( not( *bool_first ).value );
	}

	void test_separate_base_type_paths( ) {
		auto bool_values = bool_iterator( "[true, false]" );
		auto bool_first = bool_values.begin( );
		daw_ensure( *bool_first );
		++bool_first;
		daw_ensure( not *bool_first );
		++bool_first;
		daw_ensure( bool_first == bool_values.end( ) );

		auto string_values = string_iterator( R"json(["alpha", "beta"])json" );
		auto string_first = string_values.begin( );
		daw_ensure( *string_first == "alpha" );
		++string_first;
		daw_ensure( *string_first == "beta" );
		++string_first;
		daw_ensure( string_first == string_values.end( ) );

#if defined( DAW_USE_EXCEPTIONS )
		auto rejected_wrong_type = false;
		try {
			(void)iterator( "[1.0, true, 2.0]" );
		} catch( daw::json::json_exception const & ) {
			rejected_wrong_type = true;
		}
		daw_ensure( rejected_wrong_type );

		auto rejected_bad_literal = false;
		try {
			auto bad_values = bool_iterator( "[true, falsx]" );
			auto bad_first = bad_values.begin( );
			++bad_first;
			(void)*bad_first;
		} catch( daw::json::json_exception const & ) {
			rejected_bad_literal = true;
		}
		daw_ensure( rejected_bad_literal );
#endif
	}

} // namespace

int main( ) {
	static_assert( std::is_same_v<iterator::reference, iterator::value_type> );
	static_assert( std::is_same_v<iterator::value_type, double> );
	static_assert(
	  std::is_same_v<constructed_iterator::value_type, parsed_number> );
	static_assert( block::block_size > 1U );
	static_assert( block::block_size <= 64U );
	static_assert( has_number_start<block> );
	static_assert( not has_boolean_start<block> );
	static_assert( not has_string_start<block> );
	static_assert( has_boolean_start<bool_block> );
	static_assert( has_true_start<bool_block> );
	static_assert( not has_number_start<bool_block> );
	static_assert( not has_string_start<bool_block> );
	static_assert( has_string_start<string_block> );
	static_assert( not has_true_start<string_block> );
	static_assert( not has_number_start<string_block> );
	static_assert( not has_boolean_start<string_block> );

	test_iterator_semantics( );
	test_json_member_result_type( );
	test_separate_base_type_paths( );
}

#else

int main( ) {}

#endif
