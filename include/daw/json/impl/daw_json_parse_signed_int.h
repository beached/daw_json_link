// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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
	namespace {
		template<typename Signed>
		struct signed_parser {
			static constexpr auto minus =
			  static_cast<unsigned>( '-' ) - static_cast<unsigned>( '0' );

			template<JsonRangeCheck RangeChecked>
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
				uintmax_t n = 0;
				// This is used to catch when uintmax_t has overflow and wrapped
				int count = std::numeric_limits<Signed>::digits10 + 1U;
				while( dig < 10 ) {
					if constexpr( RangeChecked != JsonRangeCheck::Never ) {
						--count;
					}
					n = n * 10 + static_cast<unsigned>( dig );
					++ptr;
					dig = static_cast<unsigned>( *ptr ) - static_cast<unsigned>( '0' );
				}
				if constexpr( RangeChecked != JsonRangeCheck::Never ) {
					daw_json_assert( n <= std::numeric_limits<Signed>::max( ) and
					                   count >= 0,
					                 "Signed number outside of range of signed numbers" );
				}
				return {daw::construct_a<Signed>( sign ? static_cast<intmax_t>( n )
				                                       : -static_cast<intmax_t>( n ) ),
				        ptr};
			}
		};

		static_assert(
		  signed_parser<int>::template parse<JsonRangeCheck::CheckForNarrowing>(
		    "-12345" )
		    .first == -12345 );
	} // namespace
} // namespace daw::json::impl::signedint

namespace daw::json::impl {
	namespace {
		template<typename E, bool = std::is_enum_v<E>>
		struct enum_base {
			using type =
			  std::conditional_t<std::is_signed_v<std::underlying_type_t<E>>,
			                     intmax_t, uintmax_t>;
		};

		template<typename T>
		struct enum_base<T, false> {
			using type = void;
		};

		template<typename Result, JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
		         typename First, typename Last, bool IsUnCheckedInput>
		[[nodiscard]] constexpr Result
		parse_integer( IteratorRange<First, Last, IsUnCheckedInput> &rng ) {
			daw_json_assert_weak( rng.front( "+-0123456789" ),
			                      "Expected +,-, or a digit" );

			using result_t =
			  std::conditional_t<std::is_enum_v<Result>,
			                     typename enum_base<Result>::type, Result>;

			using namespace daw::json::impl::signedint;

			auto [result, ptr] =
			  signed_parser<result_t>::template parse<RangeCheck>( rng.first );
			rng.first = ptr;
			if constexpr( RangeCheck == JsonRangeCheck::CheckForNarrowing ) {
				return daw::narrow_cast<Result>( result );
			} else {
				return static_cast<Result>( result );
			}
		}
	} // namespace
} // namespace daw::json::impl
