// Copyright 2020 Junekey Jeon
//
// The contents of this file may be used under the terms of
// the Apache License v2.0 with LLVM Exceptions.
//
//    (See accompanying file LICENSE-Apache or copy at
//     https://llvm.org/foundation/relicensing/LICENSE.txt)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#pragma once

#include <daw/json/impl/version.h>

#include "dragonbox_to_decimal.h"

#include <daw/daw_algorithm.h>

namespace daw::jkj::dragonbox {
	inline namespace DAW_JSON_VER {
		namespace to_chars_detail {
			inline char *to_chars( unsigned_fp_t<float> v, char *buffer );
			inline char *to_chars( unsigned_fp_t<double> v, char *buffer );
		} // namespace to_chars_detail

		// Returns the next-to-end position
		template<class Float, class... Policies>
		inline char *to_chars_n( Float x, char *buffer, Policies... policies ) {
			using namespace daw::jkj::dragonbox::detail::policy_impl;
			using policy_holder = decltype( make_policy_holder(
			  base_default_pair_list<
			    base_default_pair<trailing_zero::base, trailing_zero::remove>,
			    base_default_pair<rounding_mode::base,
			                      rounding_mode::nearest_to_even>,
			    base_default_pair<correct_rounding::base, correct_rounding::to_even>,
			    base_default_pair<cache::base, cache::normal>>{ },
			  policies... ) );

			static_assert( not policy_holder::report_trailing_zeros,
			               "jkj::dragonbox::policy::trailing_zeros::report is not "
			               "valid for to_chars & to_chars_n" );

			using ieee754_format_info =
			  ieee754_format_info<ieee754_traits<Float>::format>;

			auto br = ieee754_bits( x );
			if( br.is_finite( ) ) {
				if( br.is_negative( ) ) {
					*buffer = '-';
					++buffer;
				}
				if( br.is_nonzero( ) ) {
					return to_chars_detail::to_chars(
					  to_decimal( x, policy::sign::ignore,
					              typename policy_holder::trailing_zero_policy{ },
					              typename policy_holder::rounding_mode_policy{ },
					              typename policy_holder::correct_rounding_policy{ },
					              typename policy_holder::cache_policy{ } ),
					  buffer );
				} else {
					daw::algorithm::copy_n( "0E0", buffer, 3 );
					return buffer + 3;
				}
			} else {
				if( ( br.u << ( ieee754_format_info::exponent_bits + 1 ) ) != 0 ) {
					daw::algorithm::copy_n( "NaN", buffer, 3 );
					return buffer + 3;
				} else {
					if( br.is_negative( ) ) {
						*buffer = '-';
						++buffer;
					}
					daw::algorithm::copy_n( "Infinity", buffer, 8 );
					return buffer + 8;
				}
			}
		}

		// Null-terminate and bypass the return value of fp_to_chars_n
		template<class Float, class... Policies>
		inline char *to_chars( Float x, char *buffer, Policies... policies ) {
			auto ptr = to_chars_n( x, buffer, policies... );
			*ptr = '\0';
			return ptr;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::jkj::dragonbox
