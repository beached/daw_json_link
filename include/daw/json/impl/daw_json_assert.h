// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../daw_json_exception.h"
#include "version.h"

#include <daw/daw_assume.h>
#include <daw/daw_attributes.h>
#include <daw/daw_check_exceptions.h>
#include <daw/daw_likely.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <ciso646>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>

#ifdef DAW_USE_EXCEPTIONS
inline constexpr bool use_daw_json_exceptions_v = true;
#else
inline constexpr bool use_daw_json_exceptions_v = false;
#endif

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		template<bool ShouldThrow = use_daw_json_exceptions_v>
		[[maybe_unused, noreturn]] DAW_ATTRIB_NOINLINE void
		daw_json_error( ErrorReason reason ) {
#ifdef DAW_USE_EXCEPTIONS
			if constexpr( ShouldThrow ) {
				throw json_exception( reason );
			} else {
#endif
				(void)ShouldThrow;
				(void)reason;
				std::terminate( );
#ifdef DAW_USE_EXCEPTIONS
			}
#endif
		}

		template<bool ShouldThrow = use_daw_json_exceptions_v, typename ParseState>
		[[maybe_unused, noreturn]] DAW_ATTRIB_NOINLINE static void
		daw_json_error( ErrorReason reason, ParseState const &location ) {
#ifdef DAW_USE_EXCEPTIONS
			if constexpr( ShouldThrow ) {
				if( location.first ) {
					throw json_exception( reason, location.first );
				}
				if( location.class_first ) {
					throw json_exception( reason, location.class_first );
				}
				throw json_exception( reason );
			} else {
#endif
				(void)ShouldThrow;
				(void)reason;
				(void)location;
				std::terminate( );
#ifdef DAW_USE_EXCEPTIONS
			}
#endif
		}

		template<bool ShouldThrow = use_daw_json_exceptions_v>
		[[maybe_unused, noreturn]] DAW_ATTRIB_NOINLINE static void
		daw_json_error( json_details::missing_member reason ) {
#ifdef DAW_USE_EXCEPTIONS
			if constexpr( ShouldThrow ) {
				throw json_exception( reason );
			} else {
#endif
				(void)ShouldThrow;
				(void)reason;
				std::terminate( );
#ifdef DAW_USE_EXCEPTIONS
			}
#endif
		}

		template<bool ShouldThrow = use_daw_json_exceptions_v>
		[[maybe_unused, noreturn]] DAW_ATTRIB_NOINLINE static void
		daw_json_error( json_details::missing_token reason ) {
#ifdef DAW_USE_EXCEPTIONS
			if constexpr( ShouldThrow ) {
				throw json_exception( reason );
			} else {
#endif
				(void)ShouldThrow;
				(void)reason;
				std::terminate( );
#ifdef DAW_USE_EXCEPTIONS
			}
#endif
		}

		template<bool ShouldThrow = use_daw_json_exceptions_v, typename ParseState>
		[[maybe_unused, noreturn]] DAW_ATTRIB_NOINLINE static void
		daw_json_error( json_details::missing_member reason,
		                ParseState const &location ) {
#ifdef DAW_USE_EXCEPTIONS
			if constexpr( ShouldThrow ) {
				using namespace std::string_literals;
				if( location.class_first and location.first ) {
					static constexpr std::size_t max_len = 150;
					std::size_t const len = [&]( ) -> std::size_t {
						if( location.first == nullptr or location.class_first == nullptr ) {
							if( location.class_first == nullptr or
							    location.class_last == nullptr ) {
								return 0;
							}
							return ( std::min )(
							  static_cast<std::size_t>(
							    std::distance( location.class_first, location.class_last ) ),
							  max_len );
						}
						return ( std::min )( static_cast<std::size_t>( std::distance(
						                       location.class_first, location.first + 1 ) ),
						                     max_len );
					}( );
					throw json_exception( reason,
					                      std::string_view( location.class_first, len ) );
				} else {
					throw json_exception( reason );
				}
			} else {
#endif
				(void)ShouldThrow;
				(void)reason;
				(void)location;
				std::terminate( );
#ifdef DAW_USE_EXCEPTIONS
			}
#endif
		}

		template<bool ShouldThrow = use_daw_json_exceptions_v, typename ParseState>
		[[maybe_unused, noreturn]] DAW_ATTRIB_NOINLINE static void
		daw_json_error( json_details::missing_token reason,
		                ParseState const &location ) {
#ifdef DAW_USE_EXCEPTIONS
			if constexpr( ShouldThrow ) {
				using namespace std::string_literals;
				if( location.first ) {
					throw json_exception( reason, location.first );
				}
				if( location.class_first ) {
					throw json_exception( reason, location.class_first );
				}
				throw json_exception( reason );
			} else {
#endif
				(void)ShouldThrow;
				(void)reason;
				(void)location;
				std::terminate( );
#ifdef DAW_USE_EXCEPTIONS
			}
#endif
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json

/***
 * Ensure that Bool is true
 * If false pass rest of args to daw_json_error
 */
#define daw_json_assert( Bool, ... )    \
	do {                                  \
		if( DAW_UNLIKELY( not( Bool ) ) ) { \
			daw_json_error( __VA_ARGS__ );    \
		}                                   \
	} while( false )

/***
 * Ensure that Bool is true when in Checked Input mode
 * If false pass rest of args to daw_json_error
 */
#define daw_json_assert_weak( Bool, ... )                \
	do {                                                   \
		if constexpr( not ParseState::is_unchecked_input ) { \
			if( DAW_UNLIKELY( not( Bool ) ) ) {                \
				daw_json_error( __VA_ARGS__ );                   \
			}                                                  \
		}                                                    \
	} while( false )
