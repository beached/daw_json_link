// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../daw_json_exception.h"

#include <daw/daw_assume.h>
#include <daw/daw_attributes.h>
#include <daw/daw_check_exceptions.h>
#include <daw/daw_likely.h>
#include <daw/daw_move.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>

#if defined( DAW_JSON_SHOW_ERROR_BEFORE_TERMINATE )
#include <iostream>
#endif

#if defined( DAW_USE_EXCEPTIONS )
inline constexpr bool use_daw_json_exceptions_v = true;
#else
inline constexpr bool use_daw_json_exceptions_v = false;
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<bool ShouldThrow = use_daw_json_exceptions_v>
			[[noreturn]] DAW_ATTRIB_NOINLINE void
			handle_error( json_exception &&jex ) {
#if defined( DAW_USE_EXCEPTIONS )
				if constexpr( ShouldThrow ) {
					throw DAW_MOVE( jex );
				} else {
#endif
					(void)ShouldThrow;
					(void)jex;
#if defined( DAW_JSON_SHOW_ERROR_BEFORE_TERMINATE )
					std::cerr << "Error: " << jex.reason( ) << '\n';
#endif
					std::terminate( );
#if defined( DAW_USE_EXCEPTIONS )
				}
#endif
			}
		} // namespace json_details

		[[noreturn]] inline void daw_json_error( ErrorReason reason ) {
			json_details::handle_error( json_exception( reason ) );
			DAW_UNREACHABLE( );
		}

		template<typename ParseState>
		[[noreturn]] inline void daw_json_error( ErrorReason reason,
		                                         ParseState const &location ) {
			if( location.first ) {
				json_details::handle_error( json_exception( reason, location.first ) );
			}
			if( location.class_first ) {
				json_details::handle_error(
				  json_exception( reason, location.class_first ) );
			}
			json_details::handle_error( json_exception( reason ) );
			DAW_UNREACHABLE( );
		}

		[[noreturn]] inline void
		daw_json_error( json_details::missing_member reason ) {
			json_details::handle_error( json_exception( reason ) );
			DAW_UNREACHABLE( );
		}

		[[noreturn]] inline void
		daw_json_error( json_details::missing_token reason ) {
			json_details::handle_error( json_exception( reason ) );
			DAW_UNREACHABLE( );
		}

		template<typename ParseState>
		[[noreturn]] inline void
		daw_json_error( json_details::missing_member reason,
		                ParseState const &location ) {
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
				json_details::handle_error( json_exception(
				  reason, std::string_view( location.class_first, len ) ) );
			} else {
				json_details::handle_error( json_exception( reason ) );
			}
			DAW_UNREACHABLE( );
		}

		template<typename ParseState>
		[[noreturn]] inline void daw_json_error( json_details::missing_token reason,
		                                         ParseState const &location ) {
			using namespace std::string_literals;
			if( location.first ) {
				json_details::handle_error( json_exception( reason, location.first ) );
			}
			if( location.class_first ) {
				json_details::handle_error(
				  json_exception( reason, location.class_first ) );
			}
			json_details::handle_error( json_exception( reason ) );
			DAW_UNREACHABLE( );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json

/// @brief Ensure that Bool is true. If false pass rest of args to
/// daw_json_error
#define daw_json_ensure( Bool, ... )    \
	do {                                  \
		if( DAW_UNLIKELY( not( Bool ) ) ) { \
			daw_json_error( __VA_ARGS__ );    \
		}                                   \
	} while( false )

/// @brief Assert that Bool is true when in Checked Input mode If false pass
/// rest of args to daw_json_error
#define daw_json_assert_weak( Bool, ... )                \
	do {                                                   \
		if constexpr( not ParseState::is_unchecked_input ) { \
			if( DAW_UNLIKELY( not( Bool ) ) ) {                \
				daw_json_error( __VA_ARGS__ );                   \
			}                                                  \
		}                                                    \
	} while( false )
