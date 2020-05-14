// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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

#include <random>

#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw::impl {
	static inline auto &global_rng( ) {
		static thread_local auto e = std::mt19937_64( std::random_device{}( ) );
		return e;
	}
} // namespace daw::impl

namespace daw {
	template<typename IntType>
	inline IntType randint( IntType a, IntType b ) {
		static_assert( std::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );
		using distribution_type = std::uniform_int_distribution<IntType>;
		using param_type = typename distribution_type::param_type;

		thread_local auto d = distribution_type( );
		return d( impl::global_rng( ), param_type( a, b ) );
	}

	template<typename IntType>
	inline IntType randint( ) {
		return randint<IntType>( std::numeric_limits<IntType>::min( ),
		                         std::numeric_limits<IntType>::max( ) );
	}

	inline void reseed( ) {
		impl::global_rng( ) = std::mt19937_64( );
	}

	inline void reseed( std::default_random_engine::result_type value ) {
		impl::global_rng( ).seed( value );
	}

	template<typename RandomIterator>
	inline void shuffle( RandomIterator first, RandomIterator last ) {
		using diff_t =
		  typename std::iterator_traits<RandomIterator>::difference_type;

		diff_t n = last - first;
		for( diff_t i = n - 1; i > 0; --i ) {
			daw::cswap( first[i], first[randint<diff_t>( 0, i )] );
		}
	}

	template<typename IntType, typename ForwardIterator>
	inline void random_fill( ForwardIterator first, ForwardIterator const last,
	                         IntType a, IntType b ) {
		static_assert( std::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );
		while( first != last ) {
			*first++ = randint<IntType>( a, b );
		}
	}

	/***
	 * Generate Random Numbers
	 * @tparam IntType type of integral to make
	 * @tparam Result Container type to hold values
	 * @param count How many to make
	 * @param a lower bound of random numbers
	 * @param b upper bound of random numbers
	 * @return container with values
	 */
	template<typename IntType, typename Result = std::vector<IntType>>
	inline Result
	make_random_data( size_t count,
	                  IntType a = std::numeric_limits<IntType>::min( ),
	                  IntType b = std::numeric_limits<IntType>::max( ) ) {

		static_assert( std::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );

		daw::exception::precondition_check( a <= b, "a <= b must be true" );

		Result result{};
		result.resize( count );
		random_fill( result.begin( ), result.end( ), a, b );
		return result;
	}

	namespace cxrand_impl {
#ifdef USE_CXSEED
		constexpr size_t generate_seed( char const *first,
		                                size_t seed = 0 ) noexcept {
			// Use djb2 to hash the string to generate seed
			size_t result = seed == 0 ? 5381U : seed;
			while( first and *first != 0 ) {
				result =
				  ( ( result << 5U ) + result ) + static_cast<size_t>( *first++ );
			}
			return result;
		}

		constexpr size_t generate_seed( ) noexcept {
			size_t result = generate_seed( __FILE__ );
			result = generate_seed( __DATE__, result );
			result = generate_seed( __TIME__, result );
			result = ( ( result << 5U ) + result ) + __LINE__;
			return result;
		}
#endif
		template<size_t N, std::enable_if_t<( N == 4 ), std::nullptr_t> = nullptr>
		constexpr size_t rand_lcg( size_t x_prev ) noexcept {
			return x_prev * 1664525UL + 1013904223UL;
		}

		template<size_t N, std::enable_if_t<( N == 8 ), std::nullptr_t> = nullptr>
		constexpr size_t rand_lcg( size_t x_prev ) noexcept {
			return x_prev * 2862933555777941757ULL + 3037000493ULL;
		}
	} // namespace cxrand_impl

	struct static_random {
#ifdef USE_CXSEED
		static constexpr auto m_seed = cxrand_impl::generate_seed( );
#endif

	private:
#ifdef USE_CXSEED
		size_t m_state = cxrand_impl::rand_lcg<sizeof( size_t )>( m_seed );
#else
		size_t m_state;
#endif

	public:
#ifdef USE_CXSEED
		constexpr static_random( ) noexcept = default;
#endif
		constexpr static_random( size_t seed ) noexcept
		  : m_state( cxrand_impl::rand_lcg<sizeof( size_t )>( seed ) ) {}

		constexpr size_t operator( )( ) noexcept {
			m_state = cxrand_impl::rand_lcg<sizeof( size_t )>( m_state );
			return daw::fnv1a_hash( m_state );
		}
	};
} // namespace daw
