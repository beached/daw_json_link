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

#if defined( DAW_JSON_HAS_SIMD )

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

namespace {
	using block = daw::json::json_details::simd_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::Number, char>;
	using bool_block = daw::json::json_details::simd_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::Bool, char>;
	using string_block = daw::json::json_details::simd_details::simd_json_block<
	  daw::json::JsonBaseParseTypes::String, char>;

	DAW_MAKE_REQ_TRAIT( has_number_start, std::declval<T>( ).number_start );
	DAW_MAKE_REQ_TRAIT( has_boolean_start, std::declval<T>( ).boolean_start );
	DAW_MAKE_REQ_TRAIT( has_string_start, std::declval<T>( ).string_start );
	DAW_MAKE_REQ_TRAIT( has_string_end, std::declval<T>( ).string_end );
	DAW_MAKE_REQ_TRAIT( has_escape_characters,
	                    std::declval<T>( ).escape_characters );
	DAW_MAKE_REQ_TRAIT( has_boolean_values, std::declval<T>( ).boolean_values );

	using iterator =
	  daw::json::json_simd_block_iterator<daw::json::json_number_no_name<double>>;
	using signed_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_number_no_name<std::int64_t>>;
	using unsigned_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_number_no_name<std::uint64_t>>;
	using unchecked_signed_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_number_no_name<std::int64_t>, char,
	  daw::json::options::CheckedParseMode::no,
	  daw::json::options::ExecModeTypes::compile_time>;
	using bool_iterator =
	  daw::json::json_simd_block_iterator<daw::json::json_bool_no_name<bool>>;
	using string_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_string_no_name<std::string>>;
	using unchecked_string_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_string_no_name<std::string>, char,
	  daw::json::options::CheckedParseMode::no,
	  daw::json::options::ExecModeTypes::compile_time>;
	using raw_string_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_string_raw_no_name<std::string>>;

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
	  daw::json::json_simd_block_iterator<constructed_number>;

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
	  daw::json::json_simd_block_iterator<constructed_bool>;

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

	[[nodiscard]] constexpr bool test_constexpr_integer_iterators( ) {
		auto signed_values = signed_iterator( "[-42, 0, 123456]" );
		if( *signed_values != -42 ) {
			return false;
		}
		++signed_values;
		if( *signed_values != 0 ) {
			return false;
		}
		++signed_values;
		if( *signed_values != 123456 ) {
			return false;
		}
		++signed_values;
		if( signed_values != signed_values.end( ) ) {
			return false;
		}

		auto unsigned_values = unsigned_iterator( "[0, 42, 123456]" );
		if( *unsigned_values != 0 ) {
			return false;
		}
		++unsigned_values;
		if( *unsigned_values != 42 ) {
			return false;
		}
		++unsigned_values;
		if( *unsigned_values != 123456 ) {
			return false;
		}
		++unsigned_values;
		return unsigned_values == unsigned_values.end( );
	}

	[[nodiscard]] constexpr bool test_constexpr_bool_iterator( ) {
		if( daw::simd_impl::compress_bits( 0b10010010U, 0b10010010U ) != 0b111U or
		    daw::simd_impl::compress_bits( 0b10000010U, 0b10010010U ) != 0b101U ) {
			return false;
		}
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

	[[nodiscard]] DAW_CPP20_CX_ALLOC bool test_constexpr_string_iterator( ) {
		auto values = string_iterator( R"json(["alpha", "beta"])json" );
		if( *values != "alpha" ) {
			return false;
		}
		values++;
		if( *values != "beta" ) {
			return false;
		}
		++values;
		if( values != values.end( ) ) {
			return false;
		}

		auto unchecked_values =
		  unchecked_string_iterator( R"json(["alpha", "beta"])json" );
		if( *unchecked_values != "alpha" ) {
			return false;
		}
		++unchecked_values;
		if( *unchecked_values != "beta" ) {
			return false;
		}
		++unchecked_values;
		return unchecked_values == unchecked_values.end( );
	}

	[[nodiscard]] constexpr bool test_constexpr_custom_constructors( ) {
		auto numbers = constructed_iterator( "[4.5]" );
		auto booleans = constructed_bool_iterator( "[true]" );
		return ( *numbers ).value == 4.5 and ( *booleans ).value;
	}

	[[nodiscard]] constexpr bool test_constexpr_classifiers( ) {
		using number_classifier = daw::json::json_details::simd_details::
		  simd_json_classifier<daw::json::JsonBaseParseTypes::Number, char>;
		using bool_classifier = daw::json::json_details::simd_details::
		  simd_json_classifier<daw::json::JsonBaseParseTypes::Bool, char>;
		using string_classifier = daw::json::json_details::simd_details::
		  simd_json_classifier<daw::json::JsonBaseParseTypes::String, char>;

		constexpr auto document =
		  std::string_view{ R"json([1.5e2, true, "x"])json" };
		auto const array_contents = document.substr( 1 );
		auto number_state = number_classifier::state_type{ };
		auto bool_state = bool_classifier::state_type{ };
		auto string_state = string_classifier::state_type{ };
		auto number_spans = std::array<daw::json::json_details::number_span,
		                               block::number_span_capacity>{ };
		auto pending_number = daw::json::json_details::pending_number_span{ };
		auto const number_block =
		  number_classifier::classify_number<daw::json::JsonParseTypes::Real>(
		    array_contents.data( ),
		    array_contents.size( ),
		    number_state,
		    number_spans,
		    pending_number );
		auto const boolean_block = bool_classifier::classify_bool(
		  array_contents.data( ), array_contents.size( ), bool_state );
		auto const text_block = string_classifier::classify_string(
		  array_contents.data( ), array_contents.size( ), string_state );

		auto const &number = number_spans[0];
		return number_block.number_start != 0 and
		       number_block.decimal_points != 0 and
		       number_block.exponent_markers != 0 and
		       number_block.number_span_count == 1 and
		       number.first == array_contents.data( ) and
		       number.last == array_contents.data( ) + 5 and
		       number.decimal_point == array_contents.data( ) + 1 and
		       number.exponent_marker == array_contents.data( ) + 3 and
		       boolean_block.boolean_values != 0 and
		       text_block.string_start != 0 and text_block.string_end != 0 and
		       text_block.escape_characters == 0;
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

	void test_bool_buffer_refill( ) {
		auto document = std::string{ "[" };
		for( std::size_t n = 0; n < 130U; ++n ) {
			if( n != 0 ) {
				document += ',';
			}
			document += n % 3U == 0 ? "true" : "false";
		}
		document += ']';

		auto values = bool_iterator( document );
		for( std::size_t n = 0; n < 130U; ++n ) {
			auto const expected = n % 3U == 0;
			daw_ensure( static_cast<bool>( values ) );
			daw_ensure( *values == expected );
			daw_ensure( *values == expected );
			auto const copy = values;
			daw_ensure( copy == values );
			++values;
			daw_ensure( copy != values );
		}
		daw_ensure( values == values.end( ) );
	}

	void test_many_string_values( ) {
		auto document = std::string{ "[" };
		for( std::size_t n = 0; n < 130U; ++n ) {
			if( n != 0 ) {
				document += ',';
			}
			switch( n % 3U ) {
			case 0:
				document += R"json("x")json";
				break;
			case 1:
				document += R"json("escaped \" quote")json";
				break;
			default:
				document += '"';
				document.append( 80U, 'a' );
				document += '"';
				break;
			}
		}
		document += ']';

		auto values = string_iterator( document );
		for( std::size_t n = 0; n < 130U; ++n ) {
			daw_ensure( static_cast<bool>( values ) );
			auto const expected = [n] {
				switch( n % 3U ) {
				case 0:
					return std::string{ "x" };
				case 1:
					return std::string{ R"json(escaped " quote)json" };
				default:
					return std::string( 80U, 'a' );
				}
			}( );
			daw_ensure( *values == expected );
			daw_ensure( *values == expected );
			auto const copy = values;
			++values;
			daw_ensure( copy != values );
		}
		daw_ensure( values == values.end( ) );
	}

	void test_number_span_buffer_refill( ) {
		auto document = std::string{ "[" };
		for( std::size_t n = 0; n < 100U; ++n ) {
			if( n != 0 ) {
				document += ',';
			}
			document += std::to_string( n );
		}
		document += ']';

		auto values = iterator( document );
		for( std::size_t n = 0; n < 100U; ++n ) {
			daw_ensure( static_cast<bool>( values ) );
			daw_ensure( *values == static_cast<double>( n ) );
			daw_ensure( *values == static_cast<double>( n ) );
			++values;
		}
		daw_ensure( values == values.end( ) );
	}

	void test_integer_span_buffer_refill( ) {
		auto signed_document = std::string{ "[" };
		auto unsigned_document = std::string{ "[" };
		for( std::size_t n = 0; n < 100U; ++n ) {
			if( n != 0 ) {
				signed_document += ',';
				unsigned_document += ',';
			}
			auto const signed_value = static_cast<std::int64_t>( n ) - 50;
			signed_document += std::to_string( signed_value );
			unsigned_document += std::to_string( n );
		}
		signed_document += ']';
		unsigned_document += ']';

		auto signed_values = signed_iterator( signed_document );
		auto unchecked_signed_values = unchecked_signed_iterator( signed_document );
		auto unsigned_values = unsigned_iterator( unsigned_document );
		for( std::size_t n = 0; n < 100U; ++n ) {
			auto const signed_expected = static_cast<std::int64_t>( n ) - 50;
			daw_ensure( *signed_values == signed_expected );
			daw_ensure( *unchecked_signed_values == signed_expected );
			daw_ensure( *unsigned_values == n );
			++signed_values;
			++unchecked_signed_values;
			++unsigned_values;
		}
		daw_ensure( signed_values == signed_values.end( ) );
		daw_ensure( unchecked_signed_values == unchecked_signed_values.end( ) );
		daw_ensure( unsigned_values == unsigned_values.end( ) );
	}

	void test_integer_values_across_native_blocks( ) {
		auto signed_document = std::string{ "[" };
		signed_document.append(
		  padding_to_last_lane( signed_document.size( ), block::block_size ), ' ' );
		signed_document += "-123456789012345, 42]";
		auto signed_values = signed_iterator( signed_document );
		daw_ensure( *signed_values == -123456789012345LL );
		++signed_values;
		daw_ensure( *signed_values == 42 );
		++signed_values;
		daw_ensure( signed_values == signed_values.end( ) );

		auto unsigned_document = std::string{ "[" };
		unsigned_document.append(
		  padding_to_last_lane( unsigned_document.size( ), block::block_size ),
		  ' ' );
		unsigned_document += "123456789012345, 42]";
		auto unsigned_values = unsigned_iterator( unsigned_document );
		daw_ensure( *unsigned_values == 123456789012345ULL );
		++unsigned_values;
		daw_ensure( *unsigned_values == 42U );
		++unsigned_values;
		daw_ensure( unsigned_values == unsigned_values.end( ) );
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

		auto raw_string_values =
		  raw_string_iterator( R"json(["plain", "escaped \" quote"])json" );
		auto raw_string_first = raw_string_values.begin( );
		daw_ensure( *raw_string_first == "plain" );
		++raw_string_first;
		daw_ensure( *raw_string_first == R"json(escaped \" quote)json" );
		++raw_string_first;
		daw_ensure( raw_string_first == raw_string_values.end( ) );

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

		auto rejected_signed_fraction = false;
		try {
			(void)signed_iterator( "[1.5]" );
		} catch( daw::json::json_exception const & ) {
			rejected_signed_fraction = true;
		}
		daw_ensure( rejected_signed_fraction );

		auto rejected_signed_exponent = false;
		try {
			(void)signed_iterator( "[1e2]" );
		} catch( daw::json::json_exception const & ) {
			rejected_signed_exponent = true;
		}
		daw_ensure( rejected_signed_exponent );

		auto rejected_negative_unsigned = false;
		try {
			(void)unsigned_iterator( "[-1]" );
		} catch( daw::json::json_exception const & ) {
			rejected_negative_unsigned = true;
		}
		daw_ensure( rejected_negative_unsigned );
#endif
	}

} // namespace

int main( ) {
#if defined( DAW_JSON_HAS_STD_SIMD )
	static_assert( test_constexpr_number_iterator( ) );
	static_assert( test_constexpr_integer_iterators( ) );
	static_assert( test_constexpr_bool_iterator( ) );
	static_assert( test_constexpr_string_iterator( ) );
	static_assert( test_constexpr_custom_constructors( ) );
	static_assert( test_constexpr_classifiers( ) );
#else
	daw_ensure( test_constexpr_number_iterator( ) );
	daw_ensure( test_constexpr_integer_iterators( ) );
	daw_ensure( test_constexpr_bool_iterator( ) );
	daw_ensure( test_constexpr_string_iterator( ) );
	daw_ensure( test_constexpr_custom_constructors( ) );
	daw_ensure( test_constexpr_classifiers( ) );
#endif

	static_assert( std::is_same_v<iterator::reference, iterator::value_type> );
	static_assert( std::is_same_v<iterator::value_type, double> );
	static_assert( std::is_same_v<signed_iterator::value_type, std::int64_t> );
	static_assert( std::is_same_v<unsigned_iterator::value_type, std::uint64_t> );
	static_assert(
	  std::is_same_v<constructed_iterator::value_type, parsed_number> );
	static_assert( sizeof( signed_iterator ) < sizeof( iterator ) );
	static_assert( sizeof( unsigned_iterator ) == sizeof( signed_iterator ) );
	static_assert( block::block_size > 0U );
	static_assert( block::block_size <= 64U );
#if defined( DAW_JSON_HAS_AVX2_SIMD ) and not defined( DAW_JSON_HAS_STD_SIMD )
	static_assert( block::block_size == 32U );
#endif
	static_assert( sizeof( daw::json::json_details::integer_span ) * 2U ==
	               sizeof( daw::json::json_details::number_span ) );
	static_assert( has_number_start<block> );
	static_assert( not has_boolean_start<block> );
	static_assert( not has_string_start<block> );
	static_assert( has_boolean_start<bool_block> );
	static_assert( has_boolean_values<bool_block> );
	static_assert( not has_number_start<bool_block> );
	static_assert( not has_string_start<bool_block> );
	static_assert( has_string_start<string_block> );
	static_assert( has_string_end<string_block> );
	static_assert( has_escape_characters<string_block> );
	static_assert( not has_boolean_values<string_block> );
	static_assert( not has_number_start<string_block> );
	static_assert( not has_boolean_start<string_block> );

	test_iterator_semantics( );
	test_classified_number_parts_across_blocks( );
	test_values_across_native_blocks( );
	test_bool_buffer_refill( );
	test_many_string_values( );
	test_number_span_buffer_refill( );
	test_integer_span_buffer_refill( );
	test_integer_values_across_native_blocks( );
	test_json_member_result_type( );
	test_separate_base_type_paths( );
}

#else

int main( ) {}

#endif
