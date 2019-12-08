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

namespace daw::json::impl::signedint {
	template<typename Signed>
	struct signed_parser {
		static constexpr auto minus =
		  static_cast<unsigned>( '-' ) - static_cast<unsigned>( '0' );

		[[nodiscard]] static constexpr std::pair<Signed, char const *>
		parse( char const *ptr ) {
			daw_json_assert( ptr != nullptr, "Unexpected nullptr" );
			bool sign = true;

			auto dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			if( dig >= 10 ) {
				if( dig == minus ) {
					sign = false;
				}
				++ptr;
				dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			}
			intmax_t n = 0;
			while( dig < 10 ) {
				n = n * 10 + static_cast<Signed>( dig );
				++ptr;
				dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
			}
			return {daw::construct_a<Signed>( sign ? n : -n ), ptr};
		}
	};

	static_assert( signed_parser<int>::parse( "-12345" ).first == -12345 );
} // namespace daw::json::impl::signedint

namespace daw::json::impl {
	template<typename Result, JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
	         typename First, typename Last, bool IsTrustedInput>
	[[nodiscard]] static constexpr Result
	parse_integer( IteratorRange<First, Last, IsTrustedInput> &rng ) noexcept {
		daw_json_assert_untrusted( rng.front( "+-0123456789" ),
		                           "Expected +,-, or a digit" );

		using result_t =
		  std::conditional_t<RangeCheck == JsonRangeCheck::CheckForNarrowing or
		                       std::is_enum_v<Result>,
		                     intmax_t, Result>;
		using namespace daw::json::impl::signedint;
		auto [result, ptr] = signed_parser<result_t>::parse( rng.first );
		rng.first = ptr;
		if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
			return daw::narrow_cast<Result>( result );
		} else {
			return static_cast<Result>( result );
		}
	}
} // namespace daw::json::impl
