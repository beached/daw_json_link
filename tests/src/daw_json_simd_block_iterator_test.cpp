// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link_types.h>
#include <daw/json/impl/daw_json_simd_iterator.h>

#include <daw/daw_ensure.h>

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

namespace {
	using block = daw::json::json_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::Number, char>;
	using bool_block = daw::json::json_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::Bool, char>;
	using string_block = daw::json::json_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::String, char>;

	template<typename T>
	concept has_number_start = requires( T value ) {
		value.number_start;
	};
	template<typename T>
	concept has_boolean_start = requires( T value ) {
		value.boolean_start;
	};
	template<typename T>
	concept has_string_start = requires( T value ) {
		value.string_start;
	};
	template<typename T>
	concept has_true_start = requires( T value ) {
		value.true_start;
	};
	using iterator = daw::json::experimental::json_simd_block_iterator<
	  daw::json::json_number_no_name<double>>;
	using bool_iterator = daw::json::experimental::json_simd_block_iterator<
	  daw::json::json_bool_no_name<bool>>;
	using string_iterator = daw::json::experimental::json_simd_block_iterator<
	  daw::json::json_string_no_name<std::string>>;

	struct parsed_number {
		double value;
	};

	struct parsed_number_constructor {
		[[nodiscard]] constexpr parsed_number operator( )( double value ) const {
			return { value };
		}
	};

	using constructed_number =
	  daw::json::json_number_no_name<double, daw::json::number_opts_def,
	                                 parsed_number_constructor>;
	using constructed_iterator =
	  daw::json::experimental::json_simd_block_iterator<constructed_number>;

	struct parsed_bool {
		bool value;
	};

	struct parsed_bool_constructor {
		[[nodiscard]] constexpr parsed_bool operator( )( bool value ) const {
			return { value };
		}
	};

	using constructed_bool =
	  daw::json::json_bool_no_name<bool, daw::json::bool_opts_def,
	                               parsed_bool_constructor>;
	using constructed_bool_iterator =
	  daw::json::experimental::json_simd_block_iterator<constructed_bool>;

	[[nodiscard]] constexpr std::size_t
	padding_to_last_lane( std::size_t prefix_size,
	                      std::size_t block_size ) noexcept {
		return ( block_size - 1U - prefix_size % block_size + block_size ) %
		       block_size;
	}

	[[nodiscard]] constexpr bool test_constexpr_number_iterator( ) {
		auto empty = iterator{ };
		if( empty or empty.begin( ) != empty.end( ) ) {
			return false;
		}

		auto values = iterator( "[1.25, -2.5e1, 3]" );
		auto first = values.begin( );
		auto copy = first;
		if( not first or first != copy or *first != 1.25 ) {
			return false;
		}
		first++;
		if( *first != -25.0 ) {
			return false;
		}
		++first;
		if( *first != 3.0 ) {
			return false;
		}
		++first;
		return first == values.end( );
	}

	[[nodiscard]] constexpr bool test_constexpr_bool_iterator( ) {
		auto values = bool_iterator( "[true, false, true, false]" );
		if( not *values ) {
			return false;
		}
		values++;
		if( *values ) {
			return false;
		}
		++values;
		if( not *values ) {
			return false;
		}
		++values;
		if( *values ) {
			return false;
		}
		++values;
		return values == values.end( );
	}

	[[nodiscard]] constexpr bool test_constexpr_string_iterator( ) {
		auto values = string_iterator( R"json(["alpha", "beta"])json" );
		if( *values != "alpha" ) {
			return false;
		}
		values++;
		if( *values != "beta" ) {
			return false;
		}
		++values;
		return values == values.end( );
	}

	[[nodiscard]] constexpr bool test_constexpr_custom_constructors( ) {
		auto numbers = constructed_iterator( "[4.5]" );
		auto booleans = constructed_bool_iterator( "[true]" );
		return ( *numbers ).value == 4.5 and ( *booleans ).value;
	}

	[[nodiscard]] constexpr bool test_constexpr_classifiers( ) {
		using number_classifier = daw::json::json_details::
		  simd_json_classifier<daw::json::JsonBaseParseTypes::Number, char>;
		using bool_classifier = daw::json::json_details::
		  simd_json_classifier<daw::json::JsonBaseParseTypes::Bool, char>;
		using string_classifier = daw::json::json_details::
		  simd_json_classifier<daw::json::JsonBaseParseTypes::String, char>;

		constexpr auto document =
		  std::string_view{ R"json([1.5e2, true, "x"])json" };
		auto number_state = number_classifier::state_type{ };
		auto bool_state = bool_classifier::state_type{ };
		auto string_state = string_classifier::state_type{ };
		auto const number_block = number_classifier::classify_number(
		  document.data( ), document.size( ), number_state );
		auto const boolean_block = bool_classifier::classify_bool(
		  document.data( ), document.size( ), bool_state );
		auto const text_block = string_classifier::classify_string(
		  document.data( ), document.size( ), string_state );

		return number_block.number_start != 0 and
		       number_block.decimal_points != 0 and
		       number_block.exponent_markers != 0 and
		       boolean_block.true_start != 0 and text_block.string_start != 0;
	}

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

	void test_classified_number_parts_across_blocks( ) {
		auto document = std::string{ "[" };
		document.append(
		  padding_to_last_lane( document.size( ), block::block_size ), ' ' );
		document += "-1.25e+3, 6.02e2]";

		auto values = iterator( document );
		auto first = values.begin( );
		daw_ensure( *first == -1250.0 );
		++first;
		daw_ensure( *first == 602.0 );
		++first;
		daw_ensure( first == values.end( ) );
	}

	void test_values_across_native_blocks( ) {
		auto bool_document = std::string{ "[" };
		bool_document.append(
		  padding_to_last_lane( bool_document.size( ), bool_block::block_size ),
		  ' ' );
		bool_document += "true, false]";
		auto bool_values = bool_iterator( bool_document );
		daw_ensure( *bool_values );
		++bool_values;
		daw_ensure( not *bool_values );
		++bool_values;
		daw_ensure( bool_values == bool_values.end( ) );

		auto string_document = std::string{ "[" };
		// Place the escape character in the final lane of a block.
		constexpr std::size_t prefix_through_string_text = 10U;
		string_document.append( padding_to_last_lane( prefix_through_string_text,
		                                              string_block::block_size ),
		                        ' ' );
		string_document += R"json("escaped \" quote", "tail"])json";
		auto string_values = string_iterator( string_document );
		daw_ensure( *string_values == R"json(escaped " quote)json" );
		++string_values;
		daw_ensure( *string_values == "tail" );
		++string_values;
		daw_ensure( string_values == string_values.end( ) );
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
		} catch( daw::json::json_exception const & ) { rejected_wrong_type = true; }
		daw_ensure( rejected_wrong_type );

		auto rejected_bad_literal = false;
		try {
			(void)bool_iterator( "[true, falsx]" );
		} catch( daw::json::json_exception const & ) {
			rejected_bad_literal = true;
		}
		daw_ensure( rejected_bad_literal );
#endif
	}

} // namespace

int main( ) {
	static_assert( test_constexpr_number_iterator( ) );
	static_assert( test_constexpr_bool_iterator( ) );
	static_assert( test_constexpr_string_iterator( ) );
	static_assert( test_constexpr_custom_constructors( ) );
	static_assert( test_constexpr_classifiers( ) );

	static_assert( std::is_same_v<iterator::reference, iterator::value_type> );
	static_assert( std::is_same_v<iterator::value_type, double> );
	static_assert(
	  std::is_same_v<constructed_iterator::value_type, parsed_number> );
	static_assert( block::block_size > 0U );
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
	test_classified_number_parts_across_blocks( );
	test_values_across_native_blocks( );
	test_json_member_result_type( );
	test_separate_base_type_paths( );
}

#else

int main( ) {}

#endif
