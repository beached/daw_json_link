// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <daw/json/impl/daw_json_parse_real_lemire.h>

#include <daw/daw_bit_cast.h>

#include <array>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <type_traits>

namespace {
	using daw::json::json_details::parse_real_lemire;
	using daw::json::json_details::lemire_details::try_append_digit;

	constexpr bool append_digit_check( ) {
		std::uint64_t value = 1844674407370955161ULL;
		if( not try_append_digit( value, 5U ) or
		    value != std::numeric_limits<std::uint64_t>::max( ) ) {
			return false;
		}
		return not try_append_digit( value, 0U ) and
		       value == std::numeric_limits<std::uint64_t>::max( );
	}

	static_assert( append_digit_check( ) );

	constexpr auto multiplication_check =
	  daw::json::json_details::lemire_details::full_multiplication_generic(
	    std::numeric_limits<std::uint64_t>::max( ),
	    std::numeric_limits<std::uint64_t>::max( ) );
	static_assert( multiplication_check.low == 1 );
	static_assert( multiplication_check.high ==
	               std::numeric_limits<std::uint64_t>::max( ) - 1 );
	static_assert( parse_real_lemire( false, 0, 1 ) == 1.0 );
	static_assert( parse_real_lemire( true, 0, 1 ) == -1.0 );
	static_assert( parse_real_lemire( false, -2, 12345 ) == 123.45 );
	static_assert( parse_real_lemire( false, -324, 5 ) ==
	               std::numeric_limits<double>::denorm_min( ) );
	static_assert( DAW_BIT_CAST( std::uint64_t,
	                             parse_real_lemire( true, 0, 0 ) ) ==
	               ( std::uint64_t{ 1 } << 63U ) );
	static_assert( parse_real_lemire<float>( false, 0, 1 ) == 1.0F );
	static_assert( parse_real_lemire<float>( false, -2, 12345 ) == 123.45F );
	static_assert( parse_real_lemire<float>( false, -45, 1 ) ==
	               std::numeric_limits<float>::denorm_min( ) );
	static_assert( DAW_BIT_CAST( std::uint32_t,
	                             parse_real_lemire<float>( true, 0, 0 ) ) ==
	               ( std::uint32_t{ 1 } << 31U ) );

	template<typename Real>
	[[nodiscard]] Real reference_value( bool negative, std::int64_t exponent,
	                                    std::uint64_t significant_digits ) {
		std::array<char, 64> buffer{ };
		auto *first = buffer.data( );
		if( negative ) {
			*first++ = '-';
		}
		auto result = std::to_chars( first, buffer.data( ) + buffer.size( ) - 1,
		                             significant_digits );
		*result.ptr++ = 'e';
		result = std::to_chars( result.ptr, buffer.data( ) + buffer.size( ) - 1,
		                        exponent );
		*result.ptr = '\0';
		if constexpr( std::is_same_v<Real, float> ) {
			return std::strtof( buffer.data( ), nullptr );
		} else {
			return std::strtod( buffer.data( ), nullptr );
		}
	}

	template<typename Real>
	[[nodiscard]] bool same_bits( Real lhs, Real rhs ) {
		using uint_type =
		  std::conditional_t<std::is_same_v<Real, float>, std::uint32_t,
		                     std::uint64_t>;
		return DAW_BIT_CAST( uint_type, lhs ) == DAW_BIT_CAST( uint_type, rhs );
	}
} // namespace

int main( ) {
	struct test_case {
		std::uint64_t significant_digits;
		std::int64_t exponent;
		bool negative;
	};
	constexpr test_case cases[] = {
	  { 0, 0, false },
	  { 1, 0, false },
	  { 1, 0, true },
	  { 5, -324, false },
	  { 4, -324, false },
	  { 2, -324, false },
	  { 22250738585072013ULL, -324, false },
	  { 22250738585072014ULL, -324, false },
	  { 17976931348623157ULL, 292, false },
	  { 17976931348623158ULL, 292, false },
	  { std::numeric_limits<std::uint64_t>::max( ), -342, false },
	};

	for( auto const &test : cases ) {
		auto const actual = parse_real_lemire(
		  test.negative, test.exponent, test.significant_digits );
		auto const expected = reference_value<double>(
		  test.negative, test.exponent, test.significant_digits );
		if( not same_bits( actual, expected ) ) {
			return 1;
		}
	}

	std::mt19937_64 rng( 0xDA'77'50'4EULL );
	std::uniform_int_distribution<std::int64_t> exponent_dist( -350, 315 );
	for( std::size_t n = 0; n < 100'000; ++n ) {
		auto const significant_digits = rng( );
		auto const exponent = exponent_dist( rng );
		auto const negative = ( rng( ) & 1U ) != 0;
		auto const actual =
		  parse_real_lemire( negative, exponent, significant_digits );
		auto const expected =
		  reference_value<double>( negative, exponent, significant_digits );
		if( not same_bits( actual, expected ) ) {
			return 1;
		}
	}

	constexpr test_case float_cases[] = {
	  { 0, 0, false },
	  { 1, 0, true },
	  { 1, -45, false },
	  { 7, -46, false },
	  { 11754944, -45, false },
	  { 34028235, 31, false },
	  { 34028236, 31, false },
	  { std::numeric_limits<std::uint64_t>::max( ), -64, false },
	};
	for( auto const &test : float_cases ) {
		auto const actual = parse_real_lemire<float>(
		  test.negative, test.exponent, test.significant_digits );
		auto const expected = reference_value<float>(
		  test.negative, test.exponent, test.significant_digits );
		if( not same_bits( actual, expected ) ) {
			return 1;
		}
	}

	std::uniform_int_distribution<std::int64_t> float_exponent_dist( -70, 45 );
	for( std::size_t n = 0; n < 100'000; ++n ) {
		auto const significant_digits = rng( );
		auto const exponent = float_exponent_dist( rng );
		auto const negative = ( rng( ) & 1U ) != 0;
		auto const actual =
		  parse_real_lemire<float>( negative, exponent, significant_digits );
		auto const expected =
		  reference_value<float>( negative, exponent, significant_digits );
		if( not same_bits( actual, expected ) ) {
			return 1;
		}
	}
}
