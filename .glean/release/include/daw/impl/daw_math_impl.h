// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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
#include <type_traits>
#include <utility>

#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // min

namespace daw {
	template<typename Float, std::enable_if_t<std::is_floating_point_v<Float>,
	                                          std::nullptr_t> = nullptr>
	constexpr Float float_abs( Float f ) {
		if( f < 0 ) {
			return -f;
		}
		return f;
	}

	template<typename T>
	constexpr T max( T &&t ) noexcept {
		return std::forward<T>( t );
	}

	template<typename T0, typename T1>
	constexpr auto max( T0 &&val1, T1 &&val2 ) noexcept
	  -> std::common_type_t<T0, T1> {
		if( val1 > val2 ) {
			return std::forward<T0>( val1 );
		}
		return std::forward<T1>( val2 );
	}

	template<typename T0, typename T1, typename... Ts,
	         std::enable_if_t<( sizeof...( Ts ) != 0 ), std::nullptr_t> = nullptr>
	constexpr auto max( T0 &&val1, T1 &&val2, Ts &&... vs ) noexcept
	  -> std::common_type_t<T0, T1, Ts...> {
		auto tmp = max( std::forward<Ts>( vs )... );
		if( val1 > val2 ) {
			if( val1 > tmp ) {
				return std::forward<T0>( val1 );
			}
			return tmp;
		} else if( val2 > tmp ) {
			return std::forward<T1>( val2 );
		}
		return tmp;
	}

	template<typename T>
	constexpr decltype( auto ) min( T &&t ) noexcept {
		return std::forward<T>( t );
	}

	template<typename T0, typename T1>
	constexpr auto min( T0 &&val1, T1 &&val2 ) noexcept
	  -> std::common_type_t<T0, T1> {

		if( std::less_equal<>{}( val1, val2 ) ) {
			return std::forward<T0>( val1 );
		}
		return std::forward<T1>( val2 );
	}

	template<typename T, typename Compare = std::less<>>
	constexpr decltype( auto ) min_comp( T &&lhs, T &&rhs,
	                                     Compare &&comp = Compare{} ) {
		if( comp( lhs, rhs ) ) {
			return std::forward<T>( lhs );
		}
		return std::forward<T>( rhs );
	}

	template<typename T, typename Compare = std::less<>>
	constexpr decltype( auto ) max_comp( T &&lhs, T &&rhs,
	                                     Compare &&comp = Compare{} ) {
		if( comp( rhs, lhs ) ) {
			return std::forward<T>( lhs );
		}
		return std::forward<T>( rhs );
	}

	template<typename T, typename... Ts,
	         std::enable_if_t<( sizeof...( Ts ) != 0 ), std::nullptr_t> = nullptr>
	constexpr auto min( T &&val1, Ts &&... vs ) noexcept
	  -> std::common_type_t<T, Ts...> {

		auto &&tmp = min( std::forward<Ts>( vs )... );

		if( std::less_equal<>{}( val1, tmp ) ) {
			return std::forward<T>( val1 );
		}
		return std::forward<decltype( tmp )>( tmp );
	}

} // namespace daw
