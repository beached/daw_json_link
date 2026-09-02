// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_simd_iterator.h>

#include <daw/daw_ensure.h>

#if defined( DAW_JSON_HAS_SIMD )

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

struct simd_iterator_child {
	int value;
};

struct simd_iterator_class_value {
	int id;
	std::string text;
	bool enabled;
	simd_iterator_child child;
};

struct simd_iterator_empty_class {};

struct simd_iterator_container_value {
	std::vector<int> values;
	std::optional<std::string> note;
};

struct simd_iterator_real_value {
	double value;
};

namespace daw::json {
	template<>
	struct json_data_contract<simd_iterator_child> {
		static constexpr char const value[] = "value";
		using type = json_member_list<json_number<value, int>>;
	};

	template<>
	struct json_data_contract<simd_iterator_class_value> {
		static constexpr char const id[] = "id";
		static constexpr char const text[] = "text";
		static constexpr char const enabled[] = "enabled";
		static constexpr char const child[] = "child";
		using type =
		  json_member_list<json_number<id, int>, json_string<text, std::string>,
		                   json_bool<enabled, bool>,
		                   json_class<child, simd_iterator_child>>;
	};

	template<>
	struct json_data_contract<simd_iterator_empty_class> {
		using type = json_member_list<>;
	};

	template<>
	struct json_data_contract<simd_iterator_container_value> {
		static constexpr char const values[] = "values";
		static constexpr char const note[] = "note";
		using type =
		  json_member_list<json_array<values, int, std::vector<int>>,
		                   json_string_null<note, std::optional<std::string>>>;
	};

	template<>
	struct json_data_contract<simd_iterator_real_value> {
		static constexpr char const value[] = "value";
		using type = json_member_list<json_number<value, double>>;
	};
} // namespace daw::json

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
	  daw::json::json_string_raw_no_name<std::string_view>>;
	using class_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_class_no_name<simd_iterator_class_value>>;
	using unchecked_class_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_class_no_name<simd_iterator_class_value>, char,
	  daw::json::options::CheckedParseMode::no,
	  daw::json::options::ExecModeTypes::compile_time>;
	using empty_class_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_class_no_name<simd_iterator_empty_class>>;
	using container_class_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_class_no_name<simd_iterator_container_value>>;
	using real_class_iterator = daw::json::json_simd_block_iterator<
	  daw::json::json_class_no_name<simd_iterator_real_value>>;

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

	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool test_constexpr_number_iterator( ) {
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

	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool
	test_constexpr_integer_iterators( ) {
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

	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool test_constexpr_bool_iterator( ) {
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

	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool
	test_constexpr_raw_string_iterator( ) {
		auto values =
		  raw_string_iterator( R"json(["plain", "escaped \" quote"])json" );
		if( *values != "plain" ) {
			return false;
		}
		++values;
		if( *values != R"json(escaped \" quote)json" ) {
			return false;
		}
		++values;
		if( values != values.end( ) ) {
			return false;
		}

		auto nested = raw_string_iterator(
		  R"json({"payload":{"values":["alpha","beta"]}})json", "payload.values" );
		if( *nested != "alpha" ) {
			return false;
		}
		++nested;
		if( *nested != "beta" ) {
			return false;
		}
		++nested;
		return nested == nested.end( );
	}

	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool
	test_constexpr_custom_constructors( ) {
		auto numbers = constructed_iterator( "[4.5]" );
		auto booleans = constructed_bool_iterator( "[true]" );
		return ( *numbers ).value == 4.5 and ( *booleans ).value;
	}

	void test_start_path( ) {
		auto numbers = signed_iterator(
		  R"json({"payload":{"values":[-42,17]}})json", "payload.values" );
		daw_ensure( *numbers == -42 );
		++numbers;
		daw_ensure( *numbers == 17 );
		++numbers;
		daw_ensure( numbers == numbers.end( ) );

		auto booleans = bool_iterator(
		  R"json({"payload":{"values":[true,false]}})json", "payload.values" );
		daw_ensure( *booleans );
		++booleans;
		daw_ensure( not *booleans );
		++booleans;
		daw_ensure( booleans == booleans.end( ) );

		auto strings = string_iterator(
		  R"json({"payload":{"values":["alpha","beta"]}})json", "payload.values" );
		daw_ensure( *strings == "alpha" );
		++strings;
		daw_ensure( *strings == "beta" );
		++strings;
		daw_ensure( strings == strings.end( ) );

		auto classes = class_iterator(
		  R"json({"payload":{"values":[{"id":7,"text":"path","enabled":true,"child":{"value":9}}]}})json",
		  "payload.values" );
		auto const value = *classes;
		daw_ensure( value.id == 7 );
		daw_ensure( value.text == "path" );
		daw_ensure( value.enabled );
		daw_ensure( value.child.value == 9 );
		++classes;
		daw_ensure( classes == classes.end( ) );

#if defined( DAW_USE_EXCEPTIONS )
		auto rejected_missing_path = false;
		try {
			(void)signed_iterator( R"json({"values":[1]})json", "missing" );
		} catch( daw::json::json_exception const & ) {
			rejected_missing_path = true;
		}
		daw_ensure( rejected_missing_path );
#endif
	}

	[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool test_constexpr_classifiers( ) {
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
		constexpr auto value_count = std::size_t{ 530U };
		for( std::size_t n = 0; n < value_count; ++n ) {
			if( n != 0 ) {
				document += ',';
			}
			document += n % 3U == 0 ? "true" : "false";
		}
		document += ']';

		auto values = bool_iterator( document );
		for( std::size_t n = 0; n < value_count; ++n ) {
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

	template<typename Iterator>
	void ensure_empty_array( ) {
		auto values = Iterator( "[]" );
		daw_ensure( values == values.end( ) );
	}

	void test_empty_arrays_and_json_whitespace( ) {
		ensure_empty_array<iterator>( );
		ensure_empty_array<signed_iterator>( );
		ensure_empty_array<unsigned_iterator>( );
		ensure_empty_array<bool_iterator>( );
		ensure_empty_array<string_iterator>( );

		auto numbers = iterator( "[ \t1\r,\n2 ]" );
		daw_ensure( *numbers == 1.0 );
		++numbers;
		daw_ensure( *numbers == 2.0 );
		++numbers;
		daw_ensure( numbers == numbers.end( ) );

		auto booleans = bool_iterator( "[ \ttrue\r,\nfalse ]" );
		daw_ensure( *booleans );
		++booleans;
		daw_ensure( not *booleans );
		++booleans;
		daw_ensure( booleans == booleans.end( ) );

		auto strings = string_iterator( "[ \t\"a\"\r,\n\"b\" ]" );
		daw_ensure( *strings == "a" );
		++strings;
		daw_ensure( *strings == "b" );
		++strings;
		daw_ensure( strings == strings.end( ) );
	}

	void test_stops_at_array_end( ) {
		auto empty = iterator( "[][]" );
		daw_ensure( empty == empty.end( ) );

		auto numbers = iterator( "[1] 2" );
		daw_ensure( *numbers == 1.0 );
		++numbers;
		daw_ensure( numbers == numbers.end( ) );

		auto booleans = bool_iterator( "[true][false]" );
		daw_ensure( *booleans );
		++booleans;
		daw_ensure( booleans == booleans.end( ) );

		auto strings = string_iterator( R"json(["a"] "b")json" );
		daw_ensure( *strings == "a" );
		++strings;
		daw_ensure( strings == strings.end( ) );
	}

#if defined( DAW_USE_EXCEPTIONS )
	template<typename Iterator>
	[[nodiscard]] bool rejects_array( std::string_view document ) {
		try {
			auto values = Iterator( document );
			while( values != values.end( ) ) {
				(void)*values;
				++values;
			}
		} catch( daw::json::json_exception const & ) { return true; }
		return false;
	}

	template<typename Iterator>
	void
	ensure_rejected_arrays( std::initializer_list<std::string_view> documents ) {
		for( auto const document : documents ) {
			daw_ensure( rejects_array<Iterator>( document ) );
		}
	}

	template<typename Iterator>
	void ensure_boundary_rejections( std::size_t block_size,
	                                 std::string_view first_value,
	                                 std::string_view second_value ) {
		auto double_comma = std::string{ "[" };
		double_comma += first_value;
		double_comma.append(
		  padding_to_last_lane( double_comma.size( ), block_size ), ' ' );
		double_comma += ",,";
		double_comma += second_value;
		double_comma += ']';
		daw_ensure( rejects_array<Iterator>( double_comma ) );

		auto missing_comma = std::string{ "[" };
		missing_comma += first_value;
		missing_comma.append(
		  padding_to_last_lane( missing_comma.size( ), block_size ) + 1U, ' ' );
		missing_comma += second_value;
		missing_comma += ']';
		daw_ensure( rejects_array<Iterator>( missing_comma ) );
	}

	void test_invalid_array_grammar( ) {
		ensure_rejected_arrays<iterator>(
		  { "[,]", "[,,]", "[,1]", "[1,,2]", "[1 2]", "[1", "[}" } );
		ensure_rejected_arrays<bool_iterator>( { "[,]",
		                                         "[,,]",
		                                         "[,true]",
		                                         "[true,]",
		                                         "[true,,false]",
		                                         "[true false]",
		                                         "[true",
		                                         "[truth]" } );
		ensure_rejected_arrays<string_iterator>( { "[,]",
		                                           "[,,]",
		                                           R"json([,"a"])json",
		                                           R"json(["a",])json",
		                                           R"json(["a",,"b"])json",
		                                           R"json(["a" "b"])json",
		                                           R"json(["unterminated])json" } );

		ensure_boundary_rejections<iterator>( block::block_size, "1", "2" );
		ensure_boundary_rejections<bool_iterator>(
		  bool_block::block_size, "true", "false" );
		ensure_boundary_rejections<string_iterator>(
		  string_block::block_size, R"json("a")json", R"json("b")json" );
	}

#endif

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

	void test_class_iterator( ) {
		auto values = class_iterator(
		  R"json([{"id":1,"ignored":{"nested":[1,{"x":"}"}]},"text":"first } value","enabled":true,"child":{"value":11}},{"child":{"value":22},"enabled":false,"text":"second { value","id":2}])json" );
		auto first = values.begin( );
		auto copy = first;
		daw_ensure( first == copy );

		auto const value0 = *first;
		daw_ensure( value0.id == 1 );
		daw_ensure( value0.text == "first } value" );
		daw_ensure( value0.enabled );
		daw_ensure( value0.child.value == 11 );

		++first;
		auto const value1 = *first;
		daw_ensure( value1.id == 2 );
		daw_ensure( value1.text == "second { value" );
		daw_ensure( not value1.enabled );
		daw_ensure( value1.child.value == 22 );
		++first;
		daw_ensure( first == values.end( ) );

		// Force names, escaped strings, nested unknown values, and the transition
		// to the next class across several native SIMD blocks.
		auto const long_prefix = std::string( block::block_size * 3U + 5U, 'a' );
		auto boundary_document = std::string{ "[" };
		boundary_document.append( block::block_size - 2U, ' ' );
		boundary_document += R"json({"ignored":{"padding":")json";
		boundary_document += long_prefix;
		boundary_document += R"json(-unknown-}-]-,-:-tail"},"text":")json";
		boundary_document += long_prefix;
		boundary_document +=
		  R"json(-escaped-\"-quote-\\-slash-}-,-:-tail","child":{"value":33},"enabled":true,"id":3},{"id":4,"text":"after boundary","enabled":false,"child":{"value":44}}])json";
		auto boundary_values = class_iterator( boundary_document );
		auto const boundary0 = *boundary_values;
		daw_ensure( boundary0.id == 3 );
		daw_ensure( boundary0.text ==
		            long_prefix +
		              R"json(-escaped-"-quote-\-slash-}-,-:-tail)json" );
		daw_ensure( boundary0.enabled );
		daw_ensure( boundary0.child.value == 33 );
		++boundary_values;
		auto const boundary1 = *boundary_values;
		daw_ensure( boundary1.id == 4 );
		daw_ensure( boundary1.text == "after boundary" );
		daw_ensure( not boundary1.enabled );
		daw_ensure( boundary1.child.value == 44 );
		++boundary_values;
		daw_ensure( boundary_values == boundary_values.end( ) );

		auto unchecked = unchecked_class_iterator(
		  R"json([{"id":5,"text":"unchecked","enabled":true,"child":{"value":55}}])json" );
		auto const unchecked_value = *unchecked;
		daw_ensure( unchecked_value.id == 5 );
		daw_ensure( unchecked_value.text == "unchecked" );
		daw_ensure( unchecked_value.enabled );
		daw_ensure( unchecked_value.child.value == 55 );
		++unchecked;
		daw_ensure( unchecked == unchecked.end( ) );

		auto containers = container_class_iterator(
		  R"json([{"note":null,"values":[1,2,3]},{"values":[4,5],"note":"present"}])json" );
		auto const container0 = *containers;
		daw_ensure( container0.values == std::vector<int>( { 1, 2, 3 } ) );
		daw_ensure( not container0.note );
		++containers;
		auto const container1 = *containers;
		daw_ensure( container1.values == std::vector<int>( { 4, 5 } ) );
		daw_ensure( container1.note and *container1.note == "present" );
		++containers;
		daw_ensure( containers == containers.end( ) );

		auto real_values = real_class_iterator(
		  R"json([{"value":1.25e30},{"value":-4.5E-20},{"value":6.0}])json" );
		daw_ensure( ( *real_values ).value == 1.25e30 );
		++real_values;
		daw_ensure( ( *real_values ).value == -4.5E-20 );
		++real_values;
		daw_ensure( ( *real_values ).value == 6.0 );
		++real_values;
		daw_ensure( real_values == real_values.end( ) );

		auto empty = class_iterator( "[]" );
		daw_ensure( empty == empty.end( ) );

		auto empty_classes = empty_class_iterator( "[{}, {}]" );
		(void)*empty_classes;
		++empty_classes;
		(void)*empty_classes;
		++empty_classes;
		daw_ensure( empty_classes == empty_classes.end( ) );

#if defined( DAW_USE_EXCEPTIONS )
		auto rejected_non_class = false;
		try {
			(void)class_iterator( "[1]" );
		} catch( daw::json::json_exception const & ) { rejected_non_class = true; }
		daw_ensure( rejected_non_class );

		auto rejected_trailing_comma = false;
		try {
			auto trailing = class_iterator(
			  R"json([{"id":1,"text":"value","enabled":true},])json" );
			++trailing;
		} catch( daw::json::json_exception const & ) {
			rejected_trailing_comma = true;
		}
		daw_ensure( rejected_trailing_comma );

		auto rejected_missing_member = false;
		try {
			auto missing = class_iterator(
			  R"json([{"id":1,"text":"value","child":{"value":1}}])json" );
			(void)*missing;
		} catch( daw::json::json_exception const & ) {
			rejected_missing_member = true;
		}
		daw_ensure( rejected_missing_member );

		auto rejected_missing_colon = false;
		try {
			(void)class_iterator(
			  R"json([{"id" 1,"text":"value","enabled":true,"child":{"value":1}}])json" );
		} catch( daw::json::json_exception const & ) {
			rejected_missing_colon = true;
		}
		daw_ensure( rejected_missing_colon );

		auto rejected_unterminated_class = false;
		try {
			(void)class_iterator(
			  R"json([{"id":1,"text":"value","enabled":true,"child":{"value":1})json" );
		} catch( daw::json::json_exception const & ) {
			rejected_unterminated_class = true;
		}
		daw_ensure( rejected_unterminated_class );

		auto rejected_class_boolean = false;
		try {
			(void)class_iterator(
			  R"json([{"id":1,"text":"value","enabled":truX,"child":{"value":1}}])json" );
		} catch( daw::json::json_exception const & ) {
			rejected_class_boolean = true;
		}
		daw_ensure( rejected_class_boolean );

		auto rejected_class_null = false;
		try {
			(void)container_class_iterator(
			  R"json([{"values":[1,2],"note":nulX}])json" );
		} catch( daw::json::json_exception const & ) { rejected_class_null = true; }
		daw_ensure( rejected_class_null );
#endif
	}

} // namespace

int main( ) {
#if defined( DAW_JSON_HAS_STD_SIMD )
	static_assert( test_constexpr_number_iterator( ) );
	static_assert( test_constexpr_integer_iterators( ) );
	static_assert( test_constexpr_bool_iterator( ) );
	static_assert( test_constexpr_string_iterator( ) );
	static_assert( test_constexpr_raw_string_iterator( ) );
	static_assert( test_constexpr_custom_constructors( ) );
	static_assert( test_constexpr_classifiers( ) );
#else
	daw_ensure( test_constexpr_number_iterator( ) );
	daw_ensure( test_constexpr_integer_iterators( ) );
	daw_ensure( test_constexpr_bool_iterator( ) );
	daw_ensure( test_constexpr_string_iterator( ) );
	daw_ensure( test_constexpr_raw_string_iterator( ) );
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
	test_start_path( );
	test_empty_arrays_and_json_whitespace( );
	test_stops_at_array_end( );
#if defined( DAW_USE_EXCEPTIONS )
	test_invalid_array_grammar( );
#endif
	test_separate_base_type_paths( );
	test_class_iterator( );
}

#else

int main( ) {}

#endif
