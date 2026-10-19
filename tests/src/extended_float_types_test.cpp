// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Parsing of the std floating point types other than float/double/long double
// (std::float16_t, std::bfloat16_t, std::float32_t, std::float64_t and
// std::float128_t), and float values that are not exact in the power10 path

#include "defines.h"

#include <daw/daw_bit_cast.h>
#include <daw/json/daw_json_link.h>

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#if __has_include( <stdfloat>)
#include <stdfloat>
#endif

template<typename T>
struct Holder {
	T v;
};

namespace daw::json {
	template<typename T>
	struct json_data_contract<Holder<T>> {
		static constexpr char const v[] = "v";
		using type = json_member_list<json_number<v, T>>;
	};
} // namespace daw::json

template<typename T>
static bool same_bits( T lhs, T rhs ) {
	using bytes_t = std::array<std::uint8_t, sizeof( T )>;
	return DAW_BIT_CAST( bytes_t, lhs ) == DAW_BIT_CAST( bytes_t, rhs );
}

template<typename T>
static void check_one( std::string const &number, T expected ) {
	auto const as_value = daw::json::from_json<T>( number );
	auto const as_element =
	  daw::json::from_json_array<T>( "[" + number + "]" ).front( );
	auto const as_member =
	  daw::json::from_json<Holder<T>>( R"({"v":)" + number + "}" ).v;
	auto const unchecked = daw::json::from_json<T>(
	  number,
	  daw::json::options::parse_flags<daw::json::options::CheckedParseMode::no> );
	for( T result : { as_value, as_element, as_member, unchecked } ) {
		if( not same_bits( result, expected ) ) {
			daw_ensure_error( true, "Unexpected result parsing " + number );
		}
	}
}

/// Checks number and -number against the correctly rounded value
template<typename T>
static void check( std::string_view number, T expected ) {
	check_one<T>( std::string( number ), expected );
	check_one<T>( "-" + std::string( number ), static_cast<T>( -expected ) );
}

template<typename T>
static void check_bits( std::string_view number, std::uint16_t bits ) {
	check<T>( number, DAW_BIT_CAST( T, bits ) );
}

#define DAW_CHECK( Type, Literal, Suffix ) \
	check<Type>( #Literal, Literal##Suffix )

namespace dj = daw::json::json_details;

int main( ) {
	// Exponent outside of the range float can represent 10^p exactly
	DAW_CHECK( float, 975.382e-18, f );
	DAW_CHECK( float, 84.46022e17, f );
	DAW_CHECK( float, 1.17549435e-38, f );
	DAW_CHECK( float, 3.40282347e+38, f );

	// Expected bits for the 16 bit types are from exact rational arithmetic.
	// Literals are not used as compilers can double round them through float.
#if defined( __STDCPP_FLOAT16_T__ )
	check_bits<std::float16_t>( "1.5", 0x3E00U );
	check_bits<std::float16_t>( "1.23456", 0x3CF0U );
	check_bits<std::float16_t>( "65504", 0x7BFFU );
	check_bits<std::float16_t>( "65519.99999", 0x7BFFU );
	// Halfway above the max rounds to infinity
	check_bits<std::float16_t>( "65520", 0x7C00U );
	// Exact halfway points round to even
	check_bits<std::float16_t>( "1.00048828125", 0x3C00U );
	check_bits<std::float16_t>( "1.00146484375", 0x3C02U );
	check_bits<std::float16_t>( "2049", 0x6800U );
	check_bits<std::float16_t>( "2051", 0x6802U );
	// Just off a halfway point, where the float result is the halfway point
	check_bits<std::float16_t>( "1.000488281250000000001", 0x3C01U );
	check_bits<std::float16_t>( "1.000488281249999999999", 0x3C00U );
	check_bits<std::float16_t>( "2049.0000000001", 0x6801U );
	// Subnormals
	check_bits<std::float16_t>( "0.00001", 0x00A8U );
	check_bits<std::float16_t>( "6.103515625e-5", 0x0400U );
	check_bits<std::float16_t>( "5.9604644775390625e-8", 0x0001U );
	// Subnormal ties round to even, and just above the smallest one rounds up
	check_bits<std::float16_t>( "2.98023223876953125e-8", 0x0000U );
	check_bits<std::float16_t>( "2.98023223876953126e-8", 0x0001U );
	check_bits<std::float16_t>( "8.94069671630859375e-8", 0x0002U );
	check_bits<std::float16_t>( "1.490116119384765625e-7", 0x0002U );
	check_bits<std::float16_t>( "2.682209014892578125e-7", 0x0004U );
	check_bits<std::float16_t>( "3.874301910400390625e-7", 0x0006U );
#endif

#if defined( __STDCPP_BFLOAT16_T__ )
	check_bits<std::bfloat16_t>( "1.5", 0x3FC0U );
	check_bits<std::bfloat16_t>( "0.00001", 0x3728U );
	check_bits<std::bfloat16_t>( "100000", 0x47C3U );
	check_bits<std::bfloat16_t>( "1.00390625", 0x3F80U );
	check_bits<std::bfloat16_t>( "257", 0x4380U );
	check_bits<std::bfloat16_t>( "259", 0x4382U );
	check_bits<std::bfloat16_t>( "1.00390625000000001", 0x3F81U );
	check_bits<std::bfloat16_t>( "1.00390624999999999", 0x3F80U );
#endif

#if defined( __STDCPP_FLOAT32_T__ )
	DAW_CHECK( std::float32_t, 1.5, f32 );
	DAW_CHECK( std::float32_t, 975.382e-18, f32 );
	DAW_CHECK( std::float32_t, 84.46022e17, f32 );
	DAW_CHECK( std::float32_t, 1.00000005960464477539062500001, f32 );
#endif

#if defined( __STDCPP_FLOAT64_T__ )
	DAW_CHECK( std::float64_t, 1.5, f64 );
	DAW_CHECK( std::float64_t, 65.613616999999977, f64 );
	DAW_CHECK( std::float64_t, 2.2250738585072014e-308, f64 );
	DAW_CHECK( std::float64_t,
	           1.00000000000000011102230246251565404236316680908203125,
	           f64 );
#endif

#if defined( __STDCPP_FLOAT128_T__ )
	if constexpr( dj::has_precise_fallback_v<std::float128_t> ) {
		DAW_CHECK( std::float128_t, 1.5, f128 );
		DAW_CHECK( std::float128_t, 0.1, f128 );
		DAW_CHECK( std::float128_t, 1e30, f128 );
		DAW_CHECK( std::float128_t, 1e-30, f128 );
		DAW_CHECK( std::float128_t, 3.14159265358979323846264338327950288, f128 );
	}
#endif
}
