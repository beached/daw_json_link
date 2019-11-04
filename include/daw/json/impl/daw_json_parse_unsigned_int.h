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

#include <cstddef>
#include <utility>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"

namespace daw::json::impl::unsignedint {
	template<typename Unsigned>
	struct unsigned_parser {
		[[nodiscard]] static constexpr std::pair<Unsigned, char const *>
		parse( char const *ptr ) {
			Unsigned n = 0;
			auto dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			while( dig < 10U ) {
				n *= static_cast<Unsigned>( 10 );
				n += dig;
				++ptr;
				dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			}
			return {n, ptr};
		}
	};

	static_assert( unsigned_parser<unsigned>::parse( "12345" ).first == 12345 );
} // namespace daw::json::impl::unsignedint

namespace daw::json::impl {
	template<typename Result, bool RangeCheck = false, typename First,
	         typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr auto parse_unsigned_integer2(
	  IteratorRange<First, Last, TrustedInput> &rng ) noexcept {
		json_assert_untrusted( rng.is_number( ),
		                       "Expecting a digit as first item" );

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
	         typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr Result parse_unsigned_integer(
	  IteratorRange<First, Last, TrustedInput> &rng ) noexcept {
		json_assert_untrusted( rng.is_number( ),
		                       "Expecting a digit as first item" );

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
} // namespace daw::json::impl
