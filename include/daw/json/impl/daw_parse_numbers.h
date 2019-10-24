// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_bool.h"
#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_signed_int.h"
#include "daw_unsigned_int.h"

namespace daw::json::impl {
	template<typename Result, bool RangeCheck = false, typename First,
	         typename Last>
	[[nodiscard]] static constexpr auto
	parse_unsigned_integer2( IteratorRange<First, Last> &rng ) noexcept {
		json_assert( rng.front( "0123456789" ), "Expecting a digit as first item" );

		using namespace daw::json::impl::unsignedint;
		using iresult_t = std::conditional_t<RangeCheck, uintmax_t, Result>;
		auto [v, new_p] = unsigned_parser<iresult_t>::parse( rng.first );
		uint_fast8_t c = static_cast<uint_fast8_t>( new_p - rng.first );
		rng.first = new_p;

		struct result_t {
			Result value;
			uint_fast8_t count;
		};

		if constexpr( RangeCheck ) {
			return result_t{daw::narrow_cast<Result>( v ), c};
		} else {
			return result_t{v, c};
		}
	}

	template<typename Result, bool RangeCheck = false, typename First,
	         typename Last>
	[[nodiscard]] static constexpr Result
	parse_unsigned_integer( IteratorRange<First, Last> &rng ) noexcept {
		json_assert( rng.front( "0123456789" ), "Expecting a digit as first item" );

		using namespace daw::json::impl::unsignedint;
		using result_t = std::conditional_t<RangeCheck, uintmax_t, Result>;
		auto [result, ptr] = unsigned_parser<result_t>::parse( rng.first );
		rng.first = ptr;

		if constexpr( RangeCheck ) {
			return daw::narrow_cast<Result>( result );
		} else {
			return result;
		}
	}

	// For testing
	template<typename Result>
	[[nodiscard]] static constexpr auto
	parse_unsigned_integer( daw::string_view const &sv ) noexcept {
		auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
		return parse_unsigned_integer<Result, true>( rng );
	}

	// For testing
	template<typename Result>
	[[nodiscard]] static constexpr auto
	parse_unsigned_integer2( daw::string_view const &sv ) noexcept {
		auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
		return parse_unsigned_integer2<Result, true>( rng );
	}

	template<typename Result, bool RangeCheck = false, typename First,
	         typename Last>
	[[nodiscard]] static constexpr Result
	parse_integer( IteratorRange<First, Last> &rng ) noexcept {
		json_assert( rng.front( "+-0123456789" ),
		             "Expected range to begin with a number or +-" );

		using result_t = std::conditional_t<RangeCheck, intmax_t, Result>;
		using namespace daw::json::impl::signedint;
		auto [result, ptr] = signed_parser<result_t>::parse( rng.first );
		rng.first = ptr;
		if constexpr( RangeCheck ) {
			return daw::narrow_cast<Result>( result );
		} else {
			return result;
		}
	}

	template<typename Result>
	[[nodiscard]] static constexpr Result
	parse_integer( daw::string_view const &sv ) noexcept {
		auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
		return parse_integer<Result, true>( rng );
	}

	template<typename Result, typename First, typename Last>
	[[nodiscard]] static constexpr Result
	parse_real( IteratorRange<First, Last> &rng ) noexcept {
		// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]
		json_assert( rng.is_real_number_part( ), "Expected a real number" );
		auto const sign = [&rng] {
			if( rng.in( '-' ) ) {
				rng.remove_prefix( );
				return -1;
			}
			return 1;
		}( );
		auto const whole_part =
		  static_cast<Result>( sign * parse_unsigned_integer<int64_t>( rng ) );

		Result fract_part = 0.0;
		if( rng.in( '.' ) ) {
			rng.remove_prefix( );

			auto fract_tmp = parse_unsigned_integer2<uint64_t>( rng );
			fract_part = static_cast<Result>( fract_tmp.value );
			fract_part *= static_cast<Result>(
			  daw::cxmath::dpow10( -static_cast<int32_t>( fract_tmp.count ) ) );
			fract_part = daw::cxmath::copy_sign( fract_part, whole_part );
		}

		int32_t exp_part = 0;
		if( rng.in( "eE" ) ) {
			rng.remove_prefix( );
			exp_part = parse_integer<int32_t>( rng );
		}
		if constexpr( std::is_same_v<Result, float> ) {
			return ( whole_part + fract_part ) * daw::cxmath::fpow10( exp_part );

		} else {
			return ( whole_part + fract_part ) *
			       static_cast<Result>( daw::cxmath::dpow10( exp_part ) );
		}
	}

	template<typename Result>
	[[nodiscard]] static constexpr Result
	parse_real( daw::string_view const &sv ) noexcept {
		auto rng = IteratorRange( sv.data( ), sv.data( ) + sv.size( ) );
		return parse_real<Result>( rng );
	}
} // namespace daw::json::impl
