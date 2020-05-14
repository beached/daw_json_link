// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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

#include <algorithm>
#include <functional>
#include <iterator>

#include "daw_swap.h"
#include "daw_traits.h"
#include "iterator/daw_random_iterator.h"
#include "iterator/daw_reverse_iterator.h"

namespace daw {
	namespace sort_n_details {
		// Sorting networks are from http://pages.ripco.net/~jgamble/nw.html

		template<intmax_t Pos0, intmax_t Pos1, typename RandomIterator,
		         typename Compare>
		constexpr void swap_if( RandomIterator first, Compare &&comp ) noexcept {
			auto const f = std::next( first, Pos0 );
			auto const l = std::next( first, Pos1 );
			if( not comp( *f, *l ) ) {
				daw::iter_swap( f, l );
			}
		}
		template<typename ForwardIterator, typename Compare>
		constexpr size_t swap_if2( ForwardIterator lhs, ForwardIterator rhs,
		                           Compare &&comp ) noexcept {
			if( comp( *rhs, *lhs ) ) {
				daw::cswap( *rhs, *lhs );
				return 1U;
			}
			return 0U;
		}
	} // namespace sort_n_details

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_32( RandomIterator first,
	                        Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<1, 2>( first, comp );
		sort_n_details::swap_if<4, 5>( first, comp );
		sort_n_details::swap_if<6, 7>( first, comp );
		sort_n_details::swap_if<4, 6>( first, comp );
		sort_n_details::swap_if<5, 7>( first, comp );
		sort_n_details::swap_if<5, 6>( first, comp );
		sort_n_details::swap_if<0, 4>( first, comp );
		sort_n_details::swap_if<1, 5>( first, comp );
		sort_n_details::swap_if<1, 4>( first, comp );
		sort_n_details::swap_if<2, 6>( first, comp );
		sort_n_details::swap_if<3, 7>( first, comp );
		sort_n_details::swap_if<3, 6>( first, comp );
		sort_n_details::swap_if<2, 4>( first, comp );
		sort_n_details::swap_if<3, 5>( first, comp );
		sort_n_details::swap_if<3, 4>( first, comp );
		sort_n_details::swap_if<8, 9>( first, comp );
		sort_n_details::swap_if<10, 11>( first, comp );
		sort_n_details::swap_if<8, 10>( first, comp );
		sort_n_details::swap_if<9, 11>( first, comp );
		sort_n_details::swap_if<9, 10>( first, comp );
		sort_n_details::swap_if<12, 13>( first, comp );
		sort_n_details::swap_if<14, 15>( first, comp );
		sort_n_details::swap_if<12, 14>( first, comp );
		sort_n_details::swap_if<13, 15>( first, comp );
		sort_n_details::swap_if<13, 14>( first, comp );
		sort_n_details::swap_if<8, 12>( first, comp );
		sort_n_details::swap_if<9, 13>( first, comp );
		sort_n_details::swap_if<9, 12>( first, comp );
		sort_n_details::swap_if<10, 14>( first, comp );
		sort_n_details::swap_if<11, 15>( first, comp );
		sort_n_details::swap_if<11, 14>( first, comp );
		sort_n_details::swap_if<10, 12>( first, comp );
		sort_n_details::swap_if<11, 13>( first, comp );
		sort_n_details::swap_if<11, 12>( first, comp );
		sort_n_details::swap_if<0, 8>( first, comp );
		sort_n_details::swap_if<1, 9>( first, comp );
		sort_n_details::swap_if<1, 8>( first, comp );
		sort_n_details::swap_if<2, 10>( first, comp );
		sort_n_details::swap_if<3, 11>( first, comp );
		sort_n_details::swap_if<3, 10>( first, comp );
		sort_n_details::swap_if<2, 8>( first, comp );
		sort_n_details::swap_if<3, 9>( first, comp );
		sort_n_details::swap_if<3, 8>( first, comp );
		sort_n_details::swap_if<4, 12>( first, comp );
		sort_n_details::swap_if<5, 13>( first, comp );
		sort_n_details::swap_if<5, 12>( first, comp );
		sort_n_details::swap_if<6, 14>( first, comp );
		sort_n_details::swap_if<7, 15>( first, comp );
		sort_n_details::swap_if<7, 14>( first, comp );
		sort_n_details::swap_if<6, 12>( first, comp );
		sort_n_details::swap_if<7, 13>( first, comp );
		sort_n_details::swap_if<7, 12>( first, comp );
		sort_n_details::swap_if<4, 8>( first, comp );
		sort_n_details::swap_if<5, 9>( first, comp );
		sort_n_details::swap_if<5, 8>( first, comp );
		sort_n_details::swap_if<6, 10>( first, comp );
		sort_n_details::swap_if<7, 11>( first, comp );
		sort_n_details::swap_if<7, 10>( first, comp );
		sort_n_details::swap_if<6, 8>( first, comp );
		sort_n_details::swap_if<7, 9>( first, comp );
		sort_n_details::swap_if<7, 8>( first, comp );
		sort_n_details::swap_if<16, 17>( first, comp );
		sort_n_details::swap_if<18, 19>( first, comp );
		sort_n_details::swap_if<16, 18>( first, comp );
		sort_n_details::swap_if<17, 19>( first, comp );
		sort_n_details::swap_if<17, 18>( first, comp );
		sort_n_details::swap_if<20, 21>( first, comp );
		sort_n_details::swap_if<22, 23>( first, comp );
		sort_n_details::swap_if<20, 22>( first, comp );
		sort_n_details::swap_if<21, 23>( first, comp );
		sort_n_details::swap_if<21, 22>( first, comp );
		sort_n_details::swap_if<16, 20>( first, comp );
		sort_n_details::swap_if<17, 21>( first, comp );
		sort_n_details::swap_if<17, 20>( first, comp );
		sort_n_details::swap_if<18, 22>( first, comp );
		sort_n_details::swap_if<19, 23>( first, comp );
		sort_n_details::swap_if<19, 22>( first, comp );
		sort_n_details::swap_if<18, 20>( first, comp );
		sort_n_details::swap_if<19, 21>( first, comp );
		sort_n_details::swap_if<19, 20>( first, comp );
		sort_n_details::swap_if<24, 25>( first, comp );
		sort_n_details::swap_if<26, 27>( first, comp );
		sort_n_details::swap_if<24, 26>( first, comp );
		sort_n_details::swap_if<25, 27>( first, comp );
		sort_n_details::swap_if<25, 26>( first, comp );
		sort_n_details::swap_if<28, 29>( first, comp );
		sort_n_details::swap_if<30, 31>( first, comp );
		sort_n_details::swap_if<28, 30>( first, comp );
		sort_n_details::swap_if<29, 31>( first, comp );
		sort_n_details::swap_if<29, 30>( first, comp );
		sort_n_details::swap_if<24, 28>( first, comp );
		sort_n_details::swap_if<25, 29>( first, comp );
		sort_n_details::swap_if<25, 28>( first, comp );
		sort_n_details::swap_if<26, 30>( first, comp );
		sort_n_details::swap_if<27, 31>( first, comp );
		sort_n_details::swap_if<27, 30>( first, comp );
		sort_n_details::swap_if<26, 28>( first, comp );
		sort_n_details::swap_if<27, 29>( first, comp );
		sort_n_details::swap_if<27, 28>( first, comp );
		sort_n_details::swap_if<16, 24>( first, comp );
		sort_n_details::swap_if<17, 25>( first, comp );
		sort_n_details::swap_if<17, 24>( first, comp );
		sort_n_details::swap_if<18, 26>( first, comp );
		sort_n_details::swap_if<19, 27>( first, comp );
		sort_n_details::swap_if<19, 26>( first, comp );
		sort_n_details::swap_if<18, 24>( first, comp );
		sort_n_details::swap_if<19, 25>( first, comp );
		sort_n_details::swap_if<19, 24>( first, comp );
		sort_n_details::swap_if<20, 28>( first, comp );
		sort_n_details::swap_if<21, 29>( first, comp );
		sort_n_details::swap_if<21, 28>( first, comp );
		sort_n_details::swap_if<22, 30>( first, comp );
		sort_n_details::swap_if<23, 31>( first, comp );
		sort_n_details::swap_if<23, 30>( first, comp );
		sort_n_details::swap_if<22, 28>( first, comp );
		sort_n_details::swap_if<23, 29>( first, comp );
		sort_n_details::swap_if<23, 28>( first, comp );
		sort_n_details::swap_if<20, 24>( first, comp );
		sort_n_details::swap_if<21, 25>( first, comp );
		sort_n_details::swap_if<21, 24>( first, comp );
		sort_n_details::swap_if<22, 26>( first, comp );
		sort_n_details::swap_if<23, 27>( first, comp );
		sort_n_details::swap_if<23, 26>( first, comp );
		sort_n_details::swap_if<22, 24>( first, comp );
		sort_n_details::swap_if<23, 25>( first, comp );
		sort_n_details::swap_if<23, 24>( first, comp );
		sort_n_details::swap_if<0, 16>( first, comp );
		sort_n_details::swap_if<1, 17>( first, comp );
		sort_n_details::swap_if<1, 16>( first, comp );
		sort_n_details::swap_if<2, 18>( first, comp );
		sort_n_details::swap_if<3, 19>( first, comp );
		sort_n_details::swap_if<3, 18>( first, comp );
		sort_n_details::swap_if<2, 16>( first, comp );
		sort_n_details::swap_if<3, 17>( first, comp );
		sort_n_details::swap_if<3, 16>( first, comp );
		sort_n_details::swap_if<4, 20>( first, comp );
		sort_n_details::swap_if<5, 21>( first, comp );
		sort_n_details::swap_if<5, 20>( first, comp );
		sort_n_details::swap_if<6, 22>( first, comp );
		sort_n_details::swap_if<7, 23>( first, comp );
		sort_n_details::swap_if<7, 22>( first, comp );
		sort_n_details::swap_if<6, 20>( first, comp );
		sort_n_details::swap_if<7, 21>( first, comp );
		sort_n_details::swap_if<7, 20>( first, comp );
		sort_n_details::swap_if<4, 16>( first, comp );
		sort_n_details::swap_if<5, 17>( first, comp );
		sort_n_details::swap_if<5, 16>( first, comp );
		sort_n_details::swap_if<6, 18>( first, comp );
		sort_n_details::swap_if<7, 19>( first, comp );
		sort_n_details::swap_if<7, 18>( first, comp );
		sort_n_details::swap_if<6, 16>( first, comp );
		sort_n_details::swap_if<7, 17>( first, comp );
		sort_n_details::swap_if<7, 16>( first, comp );
		sort_n_details::swap_if<8, 24>( first, comp );
		sort_n_details::swap_if<9, 25>( first, comp );
		sort_n_details::swap_if<9, 24>( first, comp );
		sort_n_details::swap_if<10, 26>( first, comp );
		sort_n_details::swap_if<11, 27>( first, comp );
		sort_n_details::swap_if<11, 26>( first, comp );
		sort_n_details::swap_if<10, 24>( first, comp );
		sort_n_details::swap_if<11, 25>( first, comp );
		sort_n_details::swap_if<11, 24>( first, comp );
		sort_n_details::swap_if<12, 28>( first, comp );
		sort_n_details::swap_if<13, 29>( first, comp );
		sort_n_details::swap_if<13, 28>( first, comp );
		sort_n_details::swap_if<14, 30>( first, comp );
		sort_n_details::swap_if<15, 31>( first, comp );
		sort_n_details::swap_if<15, 30>( first, comp );
		sort_n_details::swap_if<14, 28>( first, comp );
		sort_n_details::swap_if<15, 29>( first, comp );
		sort_n_details::swap_if<15, 28>( first, comp );
		sort_n_details::swap_if<12, 24>( first, comp );
		sort_n_details::swap_if<13, 25>( first, comp );
		sort_n_details::swap_if<13, 24>( first, comp );
		sort_n_details::swap_if<14, 26>( first, comp );
		sort_n_details::swap_if<15, 27>( first, comp );
		sort_n_details::swap_if<15, 26>( first, comp );
		sort_n_details::swap_if<14, 24>( first, comp );
		sort_n_details::swap_if<15, 25>( first, comp );
		sort_n_details::swap_if<15, 24>( first, comp );
		sort_n_details::swap_if<8, 16>( first, comp );
		sort_n_details::swap_if<9, 17>( first, comp );
		sort_n_details::swap_if<9, 16>( first, comp );
		sort_n_details::swap_if<10, 18>( first, comp );
		sort_n_details::swap_if<11, 19>( first, comp );
		sort_n_details::swap_if<11, 18>( first, comp );
		sort_n_details::swap_if<10, 16>( first, comp );
		sort_n_details::swap_if<11, 17>( first, comp );
		sort_n_details::swap_if<11, 16>( first, comp );
		sort_n_details::swap_if<12, 20>( first, comp );
		sort_n_details::swap_if<13, 21>( first, comp );
		sort_n_details::swap_if<13, 20>( first, comp );
		sort_n_details::swap_if<14, 22>( first, comp );
		sort_n_details::swap_if<15, 23>( first, comp );
		sort_n_details::swap_if<15, 22>( first, comp );
		sort_n_details::swap_if<14, 20>( first, comp );
		sort_n_details::swap_if<15, 21>( first, comp );
		sort_n_details::swap_if<15, 20>( first, comp );
		sort_n_details::swap_if<12, 16>( first, comp );
		sort_n_details::swap_if<13, 17>( first, comp );
		sort_n_details::swap_if<13, 16>( first, comp );
		sort_n_details::swap_if<14, 18>( first, comp );
		sort_n_details::swap_if<15, 19>( first, comp );
		sort_n_details::swap_if<15, 18>( first, comp );
		sort_n_details::swap_if<14, 16>( first, comp );
		sort_n_details::swap_if<15, 17>( first, comp );
		sort_n_details::swap_if<15, 16>( first, comp );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_16( RandomIterator first,
	                        Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
		sort_n_details::swap_if<4, 5>( first, comp );
		sort_n_details::swap_if<6, 7>( first, comp );
		sort_n_details::swap_if<8, 9>( first, comp );
		sort_n_details::swap_if<10, 11>( first, comp );
		sort_n_details::swap_if<12, 13>( first, comp );
		sort_n_details::swap_if<14, 15>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<4, 6>( first, comp );
		sort_n_details::swap_if<8, 10>( first, comp );
		sort_n_details::swap_if<12, 14>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<5, 7>( first, comp );
		sort_n_details::swap_if<9, 11>( first, comp );
		sort_n_details::swap_if<13, 15>( first, comp );
		sort_n_details::swap_if<0, 4>( first, comp );
		sort_n_details::swap_if<8, 12>( first, comp );
		sort_n_details::swap_if<1, 5>( first, comp );
		sort_n_details::swap_if<9, 13>( first, comp );
		sort_n_details::swap_if<2, 6>( first, comp );
		sort_n_details::swap_if<10, 14>( first, comp );
		sort_n_details::swap_if<3, 7>( first, comp );
		sort_n_details::swap_if<11, 15>( first, comp );
		sort_n_details::swap_if<0, 8>( first, comp );
		sort_n_details::swap_if<1, 9>( first, comp );
		sort_n_details::swap_if<2, 10>( first, comp );
		sort_n_details::swap_if<3, 11>( first, comp );
		sort_n_details::swap_if<4, 12>( first, comp );
		sort_n_details::swap_if<5, 13>( first, comp );
		sort_n_details::swap_if<6, 14>( first, comp );
		sort_n_details::swap_if<7, 15>( first, comp );
		sort_n_details::swap_if<5, 10>( first, comp );
		sort_n_details::swap_if<6, 9>( first, comp );
		sort_n_details::swap_if<3, 12>( first, comp );
		sort_n_details::swap_if<13, 14>( first, comp );
		sort_n_details::swap_if<7, 11>( first, comp );
		sort_n_details::swap_if<1, 2>( first, comp );
		sort_n_details::swap_if<4, 8>( first, comp );
		sort_n_details::swap_if<1, 4>( first, comp );
		sort_n_details::swap_if<7, 13>( first, comp );
		sort_n_details::swap_if<2, 8>( first, comp );
		sort_n_details::swap_if<11, 14>( first, comp );
		sort_n_details::swap_if<2, 4>( first, comp );
		sort_n_details::swap_if<5, 6>( first, comp );
		sort_n_details::swap_if<9, 10>( first, comp );
		sort_n_details::swap_if<11, 13>( first, comp );
		sort_n_details::swap_if<3, 8>( first, comp );
		sort_n_details::swap_if<7, 12>( first, comp );
		sort_n_details::swap_if<6, 8>( first, comp );
		sort_n_details::swap_if<10, 12>( first, comp );
		sort_n_details::swap_if<3, 5>( first, comp );
		sort_n_details::swap_if<7, 9>( first, comp );
		sort_n_details::swap_if<3, 4>( first, comp );
		sort_n_details::swap_if<5, 6>( first, comp );
		sort_n_details::swap_if<7, 8>( first, comp );
		sort_n_details::swap_if<9, 10>( first, comp );
		sort_n_details::swap_if<11, 12>( first, comp );
		sort_n_details::swap_if<6, 7>( first, comp );
		sort_n_details::swap_if<8, 9>( first, comp );
	}
	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_3( RandomIterator first,
	                       Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<1, 2>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<0, 1>( first, comp );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_4( RandomIterator first,
	                       Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<1, 2>( first, comp );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_5( RandomIterator first,
	                       Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<3, 4>( first, comp );
		sort_n_details::swap_if<2, 4>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
		sort_n_details::swap_if<0, 3>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<1, 4>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<1, 2>( first, comp );
	}
	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_6( RandomIterator first,
	                       Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<1, 2>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<4, 5>( first, comp );
		sort_n_details::swap_if<3, 5>( first, comp );
		sort_n_details::swap_if<3, 4>( first, comp );
		sort_n_details::swap_if<0, 3>( first, comp );
		sort_n_details::swap_if<1, 4>( first, comp );
		sort_n_details::swap_if<2, 5>( first, comp );
		sort_n_details::swap_if<2, 4>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_7( RandomIterator first,
	                       Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<1, 2>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<3, 4>( first, comp );
		sort_n_details::swap_if<5, 6>( first, comp );
		sort_n_details::swap_if<3, 5>( first, comp );
		sort_n_details::swap_if<4, 6>( first, comp );
		sort_n_details::swap_if<4, 5>( first, comp );
		sort_n_details::swap_if<0, 4>( first, comp );
		sort_n_details::swap_if<0, 3>( first, comp );
		sort_n_details::swap_if<1, 5>( first, comp );
		sort_n_details::swap_if<2, 6>( first, comp );
		sort_n_details::swap_if<2, 5>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<2, 4>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort_8( RandomIterator first,
	                       Compare &&comp = Compare{} ) noexcept {
		sort_n_details::swap_if<0, 1>( first, comp );
		sort_n_details::swap_if<2, 3>( first, comp );
		sort_n_details::swap_if<0, 2>( first, comp );
		sort_n_details::swap_if<1, 3>( first, comp );
		sort_n_details::swap_if<1, 2>( first, comp );
		sort_n_details::swap_if<4, 5>( first, comp );
		sort_n_details::swap_if<6, 7>( first, comp );
		sort_n_details::swap_if<4, 6>( first, comp );
		sort_n_details::swap_if<5, 7>( first, comp );
		sort_n_details::swap_if<5, 6>( first, comp );
		sort_n_details::swap_if<0, 4>( first, comp );
		sort_n_details::swap_if<1, 5>( first, comp );
		sort_n_details::swap_if<1, 4>( first, comp );
		sort_n_details::swap_if<2, 6>( first, comp );
		sort_n_details::swap_if<3, 7>( first, comp );
		sort_n_details::swap_if<3, 6>( first, comp );
		sort_n_details::swap_if<2, 4>( first, comp );
		sort_n_details::swap_if<3, 5>( first, comp );
		sort_n_details::swap_if<3, 4>( first, comp );
	}
	namespace sort_n_details {
		template<typename ForwardIterator, typename Compare>
		inline constexpr bool is_nothrow_sortable_v = std::is_nothrow_swappable_v<
		  typename std::iterator_traits<ForwardIterator>::value_type>
		  and std::is_nothrow_invocable_v<
		    Compare, typename std::iterator_traits<ForwardIterator>::value_type,
		    typename std::iterator_traits<ForwardIterator>::value_type>;

		template<typename ForwardIterator, typename Compare>
		constexpr size_t sort_3_impl( ForwardIterator it0, ForwardIterator it1,
		                              ForwardIterator it2,
		                              Compare &&comp ) noexcept {

			auto result = sort_n_details::swap_if2( it1, it2, comp );
			result += sort_n_details::swap_if2( it0, it2, comp );
			result += sort_n_details::swap_if2( it0, it1, comp );
			return result;
		}
		template<typename ForwardIterator, typename Compare>
		constexpr size_t sort_5_impl( ForwardIterator it0, ForwardIterator it1,
		                              ForwardIterator it2, ForwardIterator it3,
		                              ForwardIterator it4,
		                              Compare &&comp ) noexcept {
			auto result = sort_n_details::swap_if2( it0, it1, comp );
			result += sort_n_details::swap_if2( it3, it4, comp );
			result += sort_n_details::swap_if2( it2, it4, comp );
			result += sort_n_details::swap_if2( it2, it3, comp );
			result += sort_n_details::swap_if2( it0, it3, comp );
			result += sort_n_details::swap_if2( it0, it2, comp );
			result += sort_n_details::swap_if2( it1, it4, comp );
			result += sort_n_details::swap_if2( it1, it3, comp );
			result += sort_n_details::swap_if2( it1, it2, comp );
			return result;
		}

		template<typename RandomIterator, typename Compare = std::less<>>
		constexpr bool insertion_sort_incomplete(
		  RandomIterator first, RandomIterator last,
		  Compare
		    &&comp ) noexcept( is_nothrow_sortable_v<RandomIterator, Compare> ) {

			switch( last - first ) {
			case 0:
			case 1:
				return true;
			case 2:
				if( comp( *( --last ), *first ) ) {
					daw::cswap( *last, *first );
				}
				return true;
			case 3:
				sort_3( first, comp );
				return true;
			case 4:
				sort_4( first, comp );
				return true;
			case 5:
				sort_5( first, comp );
				return true;
			case 6:
				sort_6( first, comp );
				return true;
			case 7:
				sort_7( first, comp );
				return true;
			case 8:
				sort_8( first, comp );
				return true;
			case 16:
				sort_16( first, comp );
				return true;
			case 32:
				sort_32( first, comp );
				return true;
			}
			auto j = std::next( first, 2 );
			sort_3_impl( first, std::next( first ), j, comp );

			uint_fast8_t const limit = 8;
			uint_fast8_t count = 0;
			for( auto i = std::next( j ); i != last; ++i ) {
				if( comp( *i, *j ) ) {
					auto t = daw::move( *i );
					auto k = j;
					j = i;
					do {
						*j = daw::move( *k );
						j = k;
					} while( j != first and comp( t, *--k ) );
					*j = daw::move( t );
					if( ++count == limit ) {
						return std::next( i ) == last;
					}
				}
				j = i;
			}
			return true;
		}

		template<typename RandomIterator, typename Compare = std::less<>>
		constexpr void insertion_sort_3(
		  RandomIterator first, RandomIterator last,
		  Compare
		    &&comp ) noexcept( is_nothrow_sortable_v<RandomIterator, Compare> ) {

			auto j = std::next( first, 2 );
			daw::sort_3( first, comp );
			for( auto i = std::next( j ); i != last; ++i ) {
				if( comp( *i, *j ) ) {
					auto t = daw::move( *i );
					auto k = j;
					j = i;
					do {
						*j = daw::move( *k );
						j = k;
					} while( j != first and comp( t, *--k ) );
					*j = daw::move( t );
				}
				j = i;
			}
		}
	} // namespace sort_n_details

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void sort(
	  RandomIterator first, RandomIterator last,
	  Compare &&comp =
	    Compare{} ) noexcept( sort_n_details::
	                            is_nothrow_sortable_v<RandomIterator, Compare> );

	template<typename RandomIterator, typename Compare>
	constexpr void
	sort( RandomIterator first, RandomIterator last, Compare &&comp ) noexcept(
	  sort_n_details::is_nothrow_sortable_v<RandomIterator, Compare> ) {
		using difference_type =
		  typename std::iterator_traits<RandomIterator>::difference_type;
		using value_type =
		  typename std::iterator_traits<RandomIterator>::value_type;

		difference_type const limit =
		  std::is_trivially_copy_constructible_v<value_type> and
		      std::is_trivially_copy_assignable_v<value_type>
		    ? 30
		    : 6;

		while( true ) {
			bool should_restart = false;
			auto const len = std::distance( first, last );
			switch( len ) {
			case 0:
			case 1:
				return;
			case 2:
				if( comp( *( --last ), *first ) ) {
					daw::cswap( *last, *first );
				}
				return;
			case 3:
				sort_3( first, comp );
				return;
			case 4:
				sort_4( first, comp );
				return;
			case 5:
				sort_5( first, comp );
				return;
			case 6:
				sort_6( first, comp );
				return;
			case 7:
				sort_7( first, comp );
				return;
			case 8:
				sort_8( first, comp );
				return;
			case 16:
				sort_16( first, comp );
				return;
			case 32:
				sort_32( first, comp );
				return;
			}
			if( len < limit ) {
				sort_n_details::insertion_sort_3( first, last, comp );
				return;
			}
			auto m = first;
			auto lm1 = std::prev( last );
			auto swap_count = [&] {
				difference_type delta = len / 2;
				m += delta;
				if( len >= 1000 ) {
					delta /= 2;
					return sort_n_details::sort_5_impl( first, std::next( first, delta ),
					                                    m, std::next( m, delta ), lm1,
					                                    comp );
				}
				return sort_n_details::sort_3_impl( first, m, lm1, comp );
			}( );

			auto i = first;
			auto j = lm1;
			if( not comp( *i, *m ) ) {
				while( not should_restart ) {
					if( i == --j ) {
						++i;
						j = last;
						if( not comp( *first, *( --j ) ) ) {
							while( true ) {
								if( i == j ) {
									return;
								}
								if( comp( *first, *i ) ) {
									daw::cswap( *i, *j );
									++swap_count;
									++i;
									break;
								}
								++i;
							}
						}
						if( i == j ) {
							return;
						}
						while( true ) {
							while( not comp( *first, *i ) ) {
								++i;
							}
							while( comp( *first, *--j ) ) {}
							if( i >= j ) {
								break;
							}
							daw::cswap( *i, *j );
							++swap_count;
							++i;
						}
						first = i;
						should_restart = true;
						continue;
					}
					if( comp( *j, *m ) ) {
						daw::cswap( *i, *j );
						++swap_count;
						break;
					}
				}
				if( should_restart ) {
					continue;
				}
			}
			++i;
			if( i < j ) {
				while( true ) {
					while( comp( *i, *m ) ) {
						++i;
					}
					while( not comp( *( --j ), *m ) ) {}
					if( i > j ) {
						break;
					}
					daw::cswap( *i, *j );
					++swap_count;

					if( m == i ) {
						m = j;
					}
					++i;
				}
			}
			if( i != m and comp( *m, *i ) ) {
				daw::cswap( *i, *m );
				++swap_count;
			}
			if( swap_count == 0 ) {
				bool const fs =
				  sort_n_details::insertion_sort_incomplete( first, i, comp );
				if( sort_n_details::insertion_sort_incomplete( std::next( i ), last,
				                                               comp ) ) {
					if( fs ) {
						return;
					}
					last = i;
					continue;
				} else {
					if( fs ) {
						first = ++i;
						continue;
					}
				}
			}
			if( i - first < last - i ) {
				daw::sort( first, i, comp );
				first = ++i;
			} else {
				daw::sort( std::next( i ), last, comp );
				last = i;
			}
		}
	}

	template<
	  typename InputIterator, typename RandomOutputIterator,
	  typename Compare = std::less<>,
	  std::enable_if_t<not std::is_integral_v<typename std::iterator_traits<
	                     InputIterator>::value_type>,
	                   std::nullptr_t> = nullptr>
	constexpr RandomOutputIterator
	sort_to( InputIterator first_in, InputIterator last_in,
	         RandomOutputIterator first_out, Compare &&comp = Compare{} ) {

		auto last_out = daw::algorithm::copy( first_in, last_in, first_out );
		daw::sort( first_out, last_out, std::forward<Compare>( comp ) );
		return last_out;
	}

	namespace sort_n_details {
		template<typename ForwardIterator, typename RandomOutputIterator,
		         typename Compare>
		constexpr RandomOutputIterator
		counting_sort( ForwardIterator first_in, ForwardIterator const last_in,
		               RandomOutputIterator first_out, Compare &&comp ) {
			using value_t =
			  typename std::iterator_traits<ForwardIterator>::value_type;
			bool const is_ascend =
			  comp( static_cast<value_t>( 0 ), static_cast<value_t>( 1 ) );

			std::array<size_t, 10> counts{};
			size_t d = 0;
			for( auto it = first_in; it != last_in; ++it ) {
				++counts[static_cast<size_t>( ( *it ) % 10 )];
				++d;
			}
			daw::algorithm::partial_sum( std::cbegin( counts ), std::cend( counts ),
			                             std::begin( counts ) );

			if( is_ascend ) {
				while( first_in != last_in ) {
					auto const pos = static_cast<size_t>( *first_in % 10 );
					first_out[counts[pos] - 1U] = *first_in;
					--counts[pos];
					++first_in;
				}
			} else {
				auto rf = daw::make_reverse_iterator( first_out + d );
				auto const rl = daw::make_reverse_iterator( first_out );
				while( rf != rl ) {
					auto const pos = static_cast<size_t>( *first_in % 10 );
					rf[counts[pos] - 1U] = *first_in;
					--counts[pos];
					++rf;
				}
			}
			return first_out + d;
		}
	} // namespace sort_n_details

	template<typename ForwardIterator, typename RandomOutputIterator,
	         typename Compare = std::less<>,
	         std::enable_if_t<std::is_integral_v<typename std::iterator_traits<
	                            ForwardIterator>::value_type>,
	                          std::nullptr_t> = nullptr>
	constexpr RandomOutputIterator
	sort_to( ForwardIterator first_in, ForwardIterator last_in,
	         RandomOutputIterator first_out, Compare &&comp = Compare{} ) {

		auto last_out =
		  sort_n_details::counting_sort( first_in, last_in, first_out, comp );
		daw::sort( first_out, last_out, std::forward<Compare>( comp ) );
		return last_out;
	}
} // namespace daw
