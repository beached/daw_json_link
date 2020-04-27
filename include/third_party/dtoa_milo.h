// From
// https://github.com/miloyip/dtoa-benchmark/blob/master/src/milo/dtoa_milo.h
#pragma once

#include <daw/cpp_17.h>

#include <cassert>
#include <cmath>

#include <cstdint>
#if defined( _MSC_VER )
#include <intrin.h>
#endif

namespace milo {
	struct DiyFp {
		uint64_t f = 0;
		int e = 0;

		constexpr DiyFp( ) = default;

		constexpr DiyFp( uint64_t F, int E )
		  : f( F )
		  , e( E ) {}

		DiyFp( double d ) {
			uint64_t const u = daw::bit_cast<uint64_t>( d );

			int const biased_e = ( u & kDpExponentMask ) >> kDpSignificandSize;
			uint64_t const significand = ( u & kDpSignificandMask );
			if( biased_e != 0 ) {
				f = significand + kDpHiddenBit;
				e = biased_e - kDpExponentBias;
			} else {
				f = significand;
				e = kDpMinExponent + 1;
			}
		}

		constexpr DiyFp operator-( DiyFp const &rhs ) const {
			// TODO: use exceptions
			assert( e == rhs.e );
			assert( f >= rhs.f );
			return DiyFp( f - rhs.f, e );
		}

		DiyFp operator*( DiyFp const &rhs ) const {
#if defined( _MSC_VER ) && defined( _M_AMD64 )
			uint64_t h = 0;
			uint64_t l = _umul128( f, rhs.f, &h );
			if( l & ( uint64_t( 1 ) << 63 ) ) { // rounding
				h++;
			}
			return DiyFp( h, e + rhs.e + 64U );
#elif defined( __SIZEOF__INT128__ ) or                                         \
  ( ( defined( __GNUC__ ) or defined( __clang__ ) ) and                        \
    defined( __x86_64__ ) )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
			__uint128_t const p =
			  static_cast<__uint128_t>( f ) * static_cast<__uint128_t>( rhs.f );
			uint64_t h = p >> 64U;
			uint64_t l = static_cast<uint64_t>( p );
			if( l & ( uint64_t( 1 ) << 63U ) ) { // rounding
				h++;
			}
			return DiyFp( h, e + rhs.e + 64 );
#pragma GCC diagnostic pop
#else
			static constexpr uint64_t M32 = 0xFFFF'FFFFU;
			uint64_t const a = f >> 32;
			uint64_t const b = f & M32;
			uint64_t const c = rhs.f >> 32U;
			uint64_t const d = rhs.f & M32;
			uint64_t const ac = a * c;
			uint64_t const bc = b * c;
			uint64_t const ad = a * d;
			uint64_t const bd = b * d;
			uint64_t tmp = ( bd >> 32U ) + ( ad & M32 ) + ( bc & M32 );
			tmp += 1U << 31U; /// mult_round
			return DiyFp( ac + ( ad >> 32U ) + ( bc >> 32U ) + ( tmp >> 32U ),
			              e + rhs.e + 64 );
#endif
		}

		inline DiyFp Normalize( ) const {
#if defined( _MSC_VER ) && defined( _M_AMD64 )
			unsigned long index;
			_BitScanReverse64( &index, f );
			return DiyFp( f << ( 63U - index ), e - ( 63U - index ) );
#elif defined( __GNUC__ ) or defined( __clang__ )
			int s = __builtin_clzll( f );
			return DiyFp( f << s, e - s );
#else
			DiyFp res = *this;
			while( not( res.f & kDpHiddenBit ) ) {
				res.f <<= 1;
				res.e--;
			}
			res.f <<= ( kDiySignificandSize - kDpSignificandSize - 1 );
			res.e = res.e - ( kDiySignificandSize - kDpSignificandSize - 1 );
			return res;
#endif
		}

		inline DiyFp NormalizeBoundary( ) const {
#if defined( _MSC_VER ) && defined( _M_AMD64 )
			unsigned long index;
			_BitScanReverse64( &index, f );
			return DiyFp( f << ( 63U - index ), e - ( 63U - index ) );
#else
			DiyFp res = *this;
			while( not( res.f & ( kDpHiddenBit << 1U ) ) ) {
				res.f <<= 1U;
				res.e--;
			}
			res.f <<= ( kDiySignificandSize - kDpSignificandSize - 2 );
			res.e = res.e - ( kDiySignificandSize - kDpSignificandSize - 2 );
			return res;
#endif
		}

		inline void NormalizedBoundaries( DiyFp *minus, DiyFp *plus ) const {
			DiyFp pl = DiyFp( ( f << 1 ) + 1, e - 1 ).NormalizeBoundary( );
			DiyFp mi = ( f == kDpHiddenBit ) ? DiyFp( ( f << 2 ) - 1, e - 2 )
			                                 : DiyFp( ( f << 1 ) - 1, e - 1 );
			mi.f <<= mi.e - pl.e;
			mi.e = pl.e;
			*plus = pl;
			*minus = mi;
		}

		static constexpr int kDiySignificandSize = 64;
		static constexpr int kDpSignificandSize = 52;
		static constexpr int kDpExponentBias = 0x3FF + kDpSignificandSize;
		static constexpr int kDpMinExponent = -kDpExponentBias;
		static constexpr uint64_t kDpExponentMask = 0x7FF0000000000000U;
		static constexpr uint64_t kDpSignificandMask = 0x000FFFFFFFFFFFFFU;
		static constexpr uint64_t kDpHiddenBit = 0x0010000000000000U;
	};

	inline DiyFp GetCachedPower( int e, int *K ) {
		// 10^-348, 10^-340, ..., 10^340
		static constexpr uint64_t kCachedPowers_F[] = {
		  0xfa8fd5a0081c0288U, 0xbaaee17fa23ebf76U, 0x8b16fb203055ac76U,
		  0xcf42894a5dce35eaU, 0x9a6bb0aa55653b2dU, 0xe61acf033d1a45dfU,
		  0xab70fe17c79ac6caU, 0xff77b1fcbebcdc4fU, 0xbe5691ef416bd60cU,
		  0x8dd01fad907ffc3cU, 0xd3515c2831559a83U, 0x9d71ac8fada6c9b5U,
		  0xea9c227723ee8bcbU, 0xaecc49914078536dU, 0x823c12795db6ce57U,
		  0xc21094364dfb5637U, 0x9096ea6f3848984fU, 0xd77485cb25823ac7U,
		  0xa086cfcd97bf97f4U, 0xef340a98172aace5U, 0xb23867fb2a35b28eU,
		  0x84c8d4dfd2c63f3bU, 0xc5dd44271ad3cdbaU, 0x936b9fcebb25c996U,
		  0xdbac6c247d62a584U, 0xa3ab66580d5fdaf6U, 0xf3e2f893dec3f126U,
		  0xb5b5ada8aaff80b8U, 0x87625f056c7c4a8bU, 0xc9bcff6034c13053U,
		  0x964e858c91ba2655U, 0xdff9772470297ebdU, 0xa6dfbd9fb8e5b88fU,
		  0xf8a95fcf88747d94U, 0xb94470938fa89bcfU, 0x8a08f0f8bf0f156bU,
		  0xcdb02555653131b6U, 0x993fe2c6d07b7facU, 0xe45c10c42a2b3b06U,
		  0xaa242499697392d3U, 0xfd87b5f28300ca0eU, 0xbce5086492111aebU,
		  0x8cbccc096f5088ccU, 0xd1b71758e219652cU, 0x9c40000000000000U,
		  0xe8d4a51000000000U, 0xad78ebc5ac620000U, 0x813f3978f8940984U,
		  0xc097ce7bc90715b3U, 0x8f7e32ce7bea5c70U, 0xd5d238a4abe98068U,
		  0x9f4f2726179a2245U, 0xed63a231d4c4fb27U, 0xb0de65388cc8ada8U,
		  0x83c7088e1aab65dbU, 0xc45d1df942711d9aU, 0x924d692ca61be758U,
		  0xda01ee641a708deaU, 0xa26da3999aef774aU, 0xf209787bb47d6b85U,
		  0xb454e4a179dd1877U, 0x865b86925b9bc5c2U, 0xc83553c5c8965d3dU,
		  0x952ab45cfa97a0b3U, 0xde469fbd99a05fe3U, 0xa59bc234db398c25U,
		  0xf6c69a72a3989f5cU, 0xb7dcbf5354e9beceU, 0x88fcf317f22241e2U,
		  0xcc20ce9bd35c78a5U, 0x98165af37b2153dfU, 0xe2a0b5dc971f303aU,
		  0xa8d9d1535ce3b396U, 0xfb9b7cd9a4a7443cU, 0xbb764c4ca7a44410U,
		  0x8bab8eefb6409c1aU, 0xd01fef10a657842cU, 0x9b10a4e5e9913129U,
		  0xe7109bfba19c0c9dU, 0xac2820d9623bf429U, 0x80444b5e7aa7cf85U,
		  0xbf21e44003acdd2dU, 0x8e679c2f5e44ff8fU, 0xd433179d9c8cb841U,
		  0x9e19db92b4e31ba9U, 0xeb96bf6ebadf77d9U, 0xaf87023b9bf0ee6bU };

		static constexpr int16_t kCachedPowers_E[] = {
		  -1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980, -954,
		  -927,  -901,  -874,  -847,  -821,  -794,  -768,  -741,  -715,  -688, -661,
		  -635,  -608,  -582,  -555,  -529,  -502,  -475,  -449,  -422,  -396, -369,
		  -343,  -316,  -289,  -263,  -236,  -210,  -183,  -157,  -130,  -103, -77,
		  -50,   -24,   3,     30,    56,    83,    109,   136,   162,   189,  216,
		  242,   269,   295,   322,   348,   375,   402,   428,   455,   481,  508,
		  534,   561,   588,   614,   641,   667,   694,   720,   747,   774,  800,
		  827,   853,   880,   907,   933,   960,   986,   1013,  1039,  1066 };

		// int k = static_cast<int>(ceil((-61 - e) * 0.30102999566398114)) + 374;
		double const dk = ( -61 - e ) * 0.30102999566398114 +
		                  347; // dk must be positive, so can do ceiling in positive
		int k = static_cast<int>( dk );
		if( k != dk ) {
			k++;
		}

		unsigned const index = static_cast<unsigned>( ( k >> 3U ) + 1 );
		*K = -( -348 + static_cast<int>(
		                 index << 3 ) ); // decimal exponent no need lookup table

		assert( index < sizeof( kCachedPowers_F ) / sizeof( kCachedPowers_F[0] ) );
		return DiyFp( kCachedPowers_F[index], kCachedPowers_E[index] );
	}

	constexpr void GrisuRound( char *buffer, int len, uint64_t delta,
	                           uint64_t rest, uint64_t ten_kappa,
	                           uint64_t wp_w ) {
		while( rest < wp_w && delta - rest >= ten_kappa &&
		       ( rest + ten_kappa < wp_w || /// closer
		         wp_w - rest > rest + ten_kappa - wp_w ) ) {
			--buffer[len - 1];
			rest += ten_kappa;
		}
	}

	inline unsigned CountDecimalDigit32( uint32_t n ) {
		// Simple pure C++ implementation was faster than __builtin_clz version in
		// this situation.
		if( n < 10 )
			return 1;
		if( n < 100 )
			return 2;
		if( n < 1000 )
			return 3;
		if( n < 10000 )
			return 4;
		if( n < 100000 )
			return 5;
		if( n < 1000000 )
			return 6;
		if( n < 10000000 )
			return 7;
		if( n < 100000000 )
			return 8;
		if( n < 1000000000 )
			return 9;
		return 10;
	}

	inline void DigitGen( DiyFp const &W, DiyFp const &Mp, uint64_t delta,
	                      char *buffer, int *len, int *K ) {
		static uint32_t const kPow10[] = { 1,         10,        100,     1000,
		                                   10000,     100000,    1000000, 10000000,
		                                   100000000, 1000000000 };
		DiyFp const one( uint64_t( 1 ) << -Mp.e, Mp.e );
		DiyFp const wp_w = Mp - W;
		uint32_t p1 = static_cast<uint32_t>( Mp.f >> -one.e );
		uint64_t p2 = Mp.f & ( one.f - 1 );
		int kappa = static_cast<int>( CountDecimalDigit32( p1 ) );
		*len = 0;

		while( kappa > 0 ) {
			uint32_t d;
			switch( kappa ) {
			case 10:
				d = p1 / 1000000000;
				p1 %= 1000000000;
				break;
			case 9:
				d = p1 / 100000000;
				p1 %= 100000000;
				break;
			case 8:
				d = p1 / 10000000;
				p1 %= 10000000;
				break;
			case 7:
				d = p1 / 1000000;
				p1 %= 1000000;
				break;
			case 6:
				d = p1 / 100000;
				p1 %= 100000;
				break;
			case 5:
				d = p1 / 10000;
				p1 %= 10000;
				break;
			case 4:
				d = p1 / 1000;
				p1 %= 1000;
				break;
			case 3:
				d = p1 / 100;
				p1 %= 100;
				break;
			case 2:
				d = p1 / 10;
				p1 %= 10;
				break;
			case 1:
				d = p1;
				p1 = 0;
				break;
			default:
#if defined( _MSC_VER )
				__assume( 0 );
#elif defined( __GNUC ) or defined( __clang__ )
				__builtin_unreachable( );
#else
				d = 0;
#endif
			}
			if( d || *len )
				buffer[( *len )++] = '0' + static_cast<char>( d );
			kappa--;
			uint64_t tmp = ( static_cast<uint64_t>( p1 ) << -one.e ) + p2;
			if( tmp <= delta ) {
				*K += kappa;
				GrisuRound( buffer, *len, delta, tmp,
				            static_cast<uint64_t>( kPow10[kappa] ) << -one.e, wp_w.f );
				return;
			}
		}

		// kappa = 0
		for( ;; ) {
			p2 *= 10;
			delta *= 10;
			char d = static_cast<char>( p2 >> -one.e );
			if( d || *len )
				buffer[( *len )++] = '0' + d;
			p2 &= one.f - 1;
			kappa--;
			if( p2 < delta ) {
				*K += kappa;
				GrisuRound( buffer, *len, delta, p2, one.f, wp_w.f * kPow10[-kappa] );
				return;
			}
		}
	}

	inline void Grisu2( double value, char *buffer, int *length, int *K ) {
		DiyFp const v( value );
		DiyFp w_m, w_p;
		v.NormalizedBoundaries( &w_m, &w_p );

		DiyFp const c_mk = GetCachedPower( w_p.e, K );
		DiyFp const W = v.Normalize( ) * c_mk;
		DiyFp Wp = w_p * c_mk;
		DiyFp Wm = w_m * c_mk;
		Wm.f++;
		Wp.f--;
		DigitGen( W, Wp, Wp.f - Wm.f, buffer, length, K );
	}

	inline constexpr char const cDigitsLut[200] = {
	  '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0',
	  '7', '0', '8', '0', '9', '1', '0', '1', '1', '1', '2', '1', '3', '1', '4',
	  '1', '5', '1', '6', '1', '7', '1', '8', '1', '9', '2', '0', '2', '1', '2',
	  '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
	  '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3',
	  '7', '3', '8', '3', '9', '4', '0', '4', '1', '4', '2', '4', '3', '4', '4',
	  '4', '5', '4', '6', '4', '7', '4', '8', '4', '9', '5', '0', '5', '1', '5',
	  '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
	  '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6',
	  '7', '6', '8', '6', '9', '7', '0', '7', '1', '7', '2', '7', '3', '7', '4',
	  '7', '5', '7', '6', '7', '7', '7', '8', '7', '9', '8', '0', '8', '1', '8',
	  '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
	  '9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9',
	  '7', '9', '8', '9', '9' };

	constexpr void WriteExponent( int K, char *buffer ) {
		if( K < 0 ) {
			*buffer++ = '-';
			K = -K;
		}

		if( K >= 100 ) {
			*buffer++ = '0' + static_cast<char>( K / 100 );
			K %= 100;
			char const *d = cDigitsLut + K * 2;
			*buffer++ = d[0];
			*buffer++ = d[1];
		} else if( K >= 10 ) {
			char const *d = cDigitsLut + K * 2;
			*buffer++ = d[0];
			*buffer++ = d[1];
		} else
			*buffer++ = '0' + static_cast<char>( K );

		*buffer = '\0';
	}

	inline void Prettify( char *buffer, int length, int k ) {
		int const kk = length + k; // 10^(kk-1) <= v < 10^kk

		if( length <= kk && kk <= 21 ) {
			// 1234e7 -> 12340000000
			for( int i = length; i < kk; i++ )
				buffer[i] = '0';
			buffer[kk] = '.';
			buffer[kk + 1] = '0';
			buffer[kk + 2] = '\0';
		} else if( 0 < kk && kk <= 21 ) {
			// 1234e-2 -> 12.34
			memmove( &buffer[kk + 1], &buffer[kk],
			         static_cast<size_t>( length - kk ) );
			buffer[kk] = '.';
			buffer[length + 1] = '\0';
		} else if( -6 < kk && kk <= 0 ) {
			// 1234e-6 -> 0.001234
			int const offset = 2 - kk;
			memmove( &buffer[offset], &buffer[0], static_cast<size_t>( length ) );
			buffer[0] = '0';
			buffer[1] = '.';
			for( int i = 2; i < offset; i++ )
				buffer[i] = '0';
			buffer[length + offset] = '\0';
		} else if( length == 1 ) {
			// 1e30
			buffer[1] = 'e';
			WriteExponent( kk - 1, &buffer[2] );
		} else {
			// 1234e30 -> 1.234e33
			memmove( &buffer[2], &buffer[1], static_cast<size_t>( length - 1 ) );
			buffer[1] = '.';
			buffer[length + 1] = 'e';
			WriteExponent( kk - 1, &buffer[0 + length + 2] );
		}
	}

	inline void dtoa_milo( double value, char *buffer ) {
		// Not handling NaN and inf
		assert( not std::isnan( value ) );
		assert( not std::isinf( value ) );

		if( value == 0 ) {
			buffer[0] = '0';
			buffer[1] = '.';
			buffer[2] = '0';
			buffer[3] = '\0';
		} else {
			if( value < 0 ) {
				*buffer++ = '-';
				value = -value;
			}
			int length, K;
			Grisu2( value, buffer, &length, &K );
			Prettify( buffer, length, K );
		}
	}
} // namespace milo
