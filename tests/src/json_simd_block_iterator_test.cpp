// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/impl/daw_json_simd_iterator.h>

#include <daw/daw_ensure.h>

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace {
	using iterator = daw::json::experimental::json_simd_block_iterator<>;
	using block = iterator::value_type;

	struct classification {
		std::vector<std::size_t> whitespace;
		std::vector<std::size_t> operators;
		std::vector<std::size_t> scalar;
		std::vector<std::size_t> quote;
		std::vector<std::size_t> backslash;
		std::vector<std::size_t> escaped;
		std::vector<std::size_t> string_content;
		std::vector<std::size_t> structural_start;
		std::vector<std::size_t> number_start;
		std::vector<std::size_t> boolean_start;
		std::vector<std::size_t> string_start;
	};

	[[nodiscard]] constexpr bool is_whitespace( char c ) noexcept {
		return c == ' ' or c == '\t' or c == '\n' or c == '\r';
	}

	[[nodiscard]] constexpr bool is_operator( char c ) noexcept {
		return c == '{' or c == '}' or c == '[' or c == ']' or c == ':' or c == ',';
	}

	[[nodiscard]] classification classify_scalar( std::string_view document ) {
		auto result = classification{ };
		auto in_string = false;
		auto previous_scalar = false;
		auto backslash_run = std::size_t{ 0 };

		for( std::size_t pos = 0; pos < document.size( ); ++pos ) {
			auto const c = document[pos];
			auto const whitespace = is_whitespace( c );
			auto const op = is_operator( c );
			auto const scalar = not whitespace and not op;
			auto const escaped = backslash_run % 2U != 0U;
			auto const quote = c == '"' and not escaped;
			auto const was_in_string = in_string;

			if( c == '\\' ) {
				++backslash_run;
			} else {
				backslash_run = 0;
			}
			if( quote ) {
				in_string = not in_string;
			}

			if( whitespace and not in_string ) {
				result.whitespace.push_back( pos );
			}
			if( op ) {
				result.operators.push_back( pos );
			}
			if( scalar ) {
				result.scalar.push_back( pos );
			}
			if( quote ) {
				result.quote.push_back( pos );
			}
			if( c == '\\' ) {
				result.backslash.push_back( pos );
			}
			if( escaped ) {
				result.escaped.push_back( pos );
			}
			if( was_in_string and not quote ) {
				result.string_content.push_back( pos );
			}

			auto const string_tail = was_in_string;
			auto const scalar_start = scalar and not previous_scalar;
			auto const structural_start = ( op or scalar_start ) and not string_tail;
			if( structural_start ) {
				result.structural_start.push_back( pos );
				if( c == '-' or ( c >= '0' and c <= '9' ) ) {
					result.number_start.push_back( pos );
				}
				if( c == 't' or c == 'f' ) {
					result.boolean_start.push_back( pos );
				}
				if( quote and in_string ) {
					result.string_start.push_back( pos );
				}
			}
			previous_scalar = scalar and not quote;
		}
		return result;
	}

	template<typename Mask>
	void append_set_lanes( std::vector<std::size_t> &result, Mask const &mask,
	                       std::size_t offset, std::size_t size ) {
		auto bits = static_cast<std::uint64_t>( mask.to_ullong( ) );
		for( std::size_t lane = 0; lane < size; ++lane ) {
			if( ( bits & ( std::uint64_t{ 1 } << lane ) ) != 0 ) {
				result.push_back( offset + lane );
			}
		}
	}

	[[nodiscard]] constexpr std::uint64_t low_bits( std::size_t count ) {
		return count == 64 ? ~std::uint64_t{ 0 }
		                   : ( std::uint64_t{ 1 } << count ) - 1U;
	}

	void verify_document( std::string_view document ) {
		auto actual = classification{ };
		auto expected_offset = std::size_t{ 0 };
		auto block_count = std::size_t{ 0 };

		for( auto const current : iterator( document ) ) {
			daw_ensure( current.data == document.data( ) + expected_offset );
			daw_ensure( current.offset == expected_offset );
			daw_ensure(
			  current.size ==
			  std::min( block::block_size, document.size( ) - expected_offset ) );
			daw_ensure( current.is_full( ) == ( current.size == block::block_size ) );
			daw_ensure( current.state_after.offset == current.offset + current.size );
			daw_ensure( static_cast<std::uint64_t>( current.valid.to_ullong( ) ) ==
			            low_bits( current.size ) );

			append_set_lanes(
			  actual.whitespace, current.whitespace, current.offset, current.size );
			append_set_lanes(
			  actual.operators, current.operators, current.offset, current.size );
			append_set_lanes(
			  actual.scalar, current.scalar, current.offset, current.size );
			append_set_lanes(
			  actual.quote, current.quote, current.offset, current.size );
			append_set_lanes(
			  actual.backslash, current.backslash, current.offset, current.size );
			append_set_lanes(
			  actual.escaped, current.escaped, current.offset, current.size );
			append_set_lanes( actual.string_content,
			                  current.string_content,
			                  current.offset,
			                  current.size );
			append_set_lanes( actual.structural_start,
			                  current.structural_start,
			                  current.offset,
			                  current.size );
			append_set_lanes( actual.number_start,
			                  current.number_start,
			                  current.offset,
			                  current.size );
			append_set_lanes( actual.boolean_start,
			                  current.boolean_start,
			                  current.offset,
			                  current.size );
			append_set_lanes( actual.string_start,
			                  current.string_start,
			                  current.offset,
			                  current.size );

			expected_offset += current.size;
			++block_count;
		}

		daw_ensure( expected_offset == document.size( ) );
		daw_ensure( block_count == ( document.size( ) + block::block_size - 1U ) /
		                             block::block_size );
		auto const expected = classify_scalar( document );
		daw_ensure( actual.whitespace == expected.whitespace );
		daw_ensure( actual.operators == expected.operators );
		daw_ensure( actual.scalar == expected.scalar );
		daw_ensure( actual.quote == expected.quote );
		daw_ensure( actual.backslash == expected.backslash );
		daw_ensure( actual.escaped == expected.escaped );
		daw_ensure( actual.string_content == expected.string_content );
		daw_ensure( actual.structural_start == expected.structural_start );
		daw_ensure( actual.number_start == expected.number_start );
		daw_ensure( actual.boolean_start == expected.boolean_start );
		daw_ensure( actual.string_start == expected.string_start );
	}

	void test_iterator_semantics( ) {
		auto empty = iterator( std::string_view{ } );
		daw_ensure( not empty );
		daw_ensure( empty.begin( ) == empty.end( ) );
		daw_ensure( empty == iterator{ } );

		auto const document = std::string( block::block_size + 1U, ' ' );
		auto range = iterator( document );
		auto first = range.begin( );
		auto copy = first;
		daw_ensure( first );
		daw_ensure( first == copy );
		daw_ensure( first != range.end( ) );

		auto const first_read = *first;
		auto const second_read = *first;
		daw_ensure( first_read.offset == second_read.offset );
		daw_ensure( first_read.size == second_read.size );
		daw_ensure( first_read.structural_start.to_ullong( ) ==
		            second_read.structural_start.to_ullong( ) );
		daw_ensure( first == copy );

		first++;
		daw_ensure( first != copy );
		daw_ensure( ( *first ).offset == block::block_size );
		++first;
		daw_ensure( not first );
		daw_ensure( first == range.end( ) );
	}

	void test_cross_block_state( ) {
		auto string_across_block = std::string{ "\"" };
		string_across_block.append( block::block_size, 'x' );
		string_across_block += ", y\"";
		verify_document( string_across_block );

		auto escaped_quote_across_block = std::string{ "\"" };
		escaped_quote_across_block.append( block::block_size - 2U, 'x' );
		escaped_quote_across_block += "\\\"x\"";
		verify_document( escaped_quote_across_block );

		auto even_backslashes_across_block = std::string{ "\"" };
		even_backslashes_across_block.append( block::block_size - 3U, 'x' );
		even_backslashes_across_block += "\\\\\"";
		verify_document( even_backslashes_across_block );

		auto scalar_across_block = std::string( block::block_size - 2U, ' ' );
		scalar_across_block += "12345 ";
		verify_document( scalar_across_block );
	}

	void test_json_arrays( ) {
		verify_document(
		  R"json([true, false, true, false, true, false, true, false])json" );
		verify_document(
		  R"json([0, -1, 2.5, 6.022e23, -4.2E-7, 18446744073709551615])json" );
		verify_document(
		  R"json(["", "alpha", "comma,inside", "escaped\\slash", "escaped\"quote"])json" );
	}
} // namespace

int main( ) {
	static_assert( std::is_same_v<iterator::reference, iterator::value_type> );
	static_assert( std::is_same_v<iterator::pointer, void> );
	static_assert( block::block_size > 1U );
	static_assert( block::block_size <= 64U );

	test_iterator_semantics( );
	verify_document( std::string_view{ } );
	verify_document( R"json({"n":-12, "yes":true, "no":false,
	                          "s":"a, b\\\"c"})json" );
	test_json_arrays( );
	test_cross_block_state( );
}

#else

int main( ) {}

#endif
