// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_hide.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>

#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_JSON_LIKELY( Bool ) ( __builtin_expect( !!( Bool ), 1 ) )
#define DAW_JSON_UNLIKELY( Bool ) ( __builtin_expect( !!( Bool ), 0 ) )
#define DAW_JSON_UNREACHABLE( ) ( __builtin_unreachable( ) )
#else
#define DAW_JSON_LIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNLIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNREACHABLE( ) std::terminate( )
#endif

#if not( defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or             \
         defined( _CPPUNWIND ) )
// account for no exceptions -fno-exceptions
#ifdef DAW_USE_JSON_EXCEPTIONS
#undef DAW_USE_JSON_EXCEPTIONS
#endif
#endif

namespace daw::json::json_details {
	struct missing_member {
		std::string_view member_name;

		explicit inline constexpr missing_member( std::string_view name )
		  : member_name( name ) {}
	};
} // namespace daw::json::json_details

namespace daw::json {
	enum class ErrorType { Unknown, MissingMember, UnexpectedCharacter };

	class json_exception {
		std::string m_reason{ };
		ErrorType m_error_type = ErrorType::Unknown;
		std::string m_location{ };
		char const *m_parse_loc = nullptr;

	public:
		json_exception( ) = default;
		explicit json_exception( std::string_view reason ) noexcept
		  : m_reason( reason ) {}

		explicit json_exception( std::string_view reason,
		                         std::string_view location ) noexcept
		  : m_reason( reason )
		  , m_location( location )
		  , m_parse_loc( location.data( ) ) {}

		explicit json_exception( std::string_view reason,
		                         ErrorType error_type ) noexcept
		  : m_reason( reason )
		  , m_error_type( error_type ) {}

		explicit json_exception( std::string_view reason, ErrorType error_type,
		                         std::string_view location ) noexcept
		  : m_reason( reason )
		  , m_error_type( error_type )
		  , m_location( location )
		  , m_parse_loc( location.data( ) ) {}

		[[nodiscard]] std::string const &reason( ) const {
			return m_reason;
		}

		[[nodiscard]] std::string const &location( ) const {
			return m_location;
		}

		[[nodiscard]] ErrorType type( ) const {
			return m_error_type;
		}

		[[nodiscard]] char const *parse_location( ) const {
			return m_parse_loc;
		}
	};

	DAW_ATTRIBUTE_FLATTEN inline std::string
	to_formatted_string( json_exception const &je ) {
		using namespace std::string_literals;
		std::string result = "reason: "s + je.reason( );
		if( not je.location( ).empty( ) ) {
			result += "\nlocation: "s;
#ifndef _WIN32
			result += "\x1b[1m";
#endif
			result += std::accumulate(
			  je.location( ).begin( ), je.location( ).end( ), std::string{ },
			  []( std::string r, char c ) -> std::string {
				  if( static_cast<unsigned char>( c ) >= 0x20 ) {
					  r += c;
				  }
				  return r;
			  } );
#ifndef _WIN32
			result += "\x1b[0m\n";
#endif
		}
		return result;
	}
} // namespace daw::json

#ifdef DAW_USE_JSON_EXCEPTIONS
inline constexpr bool use_daw_json_exceptions_v = true;
#else
inline constexpr bool use_daw_json_exceptions_v = false;
#endif

template<bool ShouldThrow = use_daw_json_exceptions_v>
[[maybe_unused, noreturn]] static void
daw_json_error( std::string_view reason ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
	if constexpr( ShouldThrow ) {
		throw daw::json::json_exception( reason );
	} else {
#endif
		(void)ShouldThrow;
		(void)reason;
		std::terminate( );
#ifdef DAW_USE_JSON_EXCEPTIONS
	}
#endif
}

template<bool ShouldThrow = use_daw_json_exceptions_v, typename Range>
[[maybe_unused, noreturn]] static void daw_json_error( std::string_view reason,
                                                       Range const &location ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
	if constexpr( ShouldThrow ) {
		if( location.first ) {
			if( location.class_first ) {
				auto const len =
				  static_cast<std::size_t>( location.first - location.class_first );
				auto loc = std::string_view( location.class_first, len );
				throw daw::json::json_exception( reason, loc );
			}
			if( location.last ) {
				auto const len =
				  static_cast<std::size_t>( location.last - location.first );
				auto loc = std::string_view( location.first, len );
				throw daw::json::json_exception( reason, loc );
			}
			throw daw::json::json_exception( reason,
			                                 std::string_view( location.first, 0 ) );
		} else {
			throw daw::json::json_exception( reason );
		}
	} else {
#endif
		(void)ShouldThrow;
		(void)reason;
		(void)location;
		std::terminate( );
#ifdef DAW_USE_JSON_EXCEPTIONS
	}
#endif
}

template<bool ShouldThrow = use_daw_json_exceptions_v>
[[maybe_unused, noreturn]] static void
daw_json_error( daw::json::json_details::missing_member reason ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
	if constexpr( ShouldThrow ) {
		using namespace std::string_literals;
		std::string msg = "Could not find required class member '"s;
		msg += static_cast<std::string>( reason.member_name ) + "'"s;
		throw daw::json::json_exception( msg );
	} else {
#endif
		(void)ShouldThrow;
		(void)reason;
		std::terminate( );
#ifdef DAW_USE_JSON_EXCEPTIONS
	}
#endif
}

template<bool ShouldThrow = use_daw_json_exceptions_v, typename Range>
[[maybe_unused, noreturn]] static void
daw_json_error( daw::json::json_details::missing_member reason,
                Range const &location ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
	if constexpr( ShouldThrow ) {
		using namespace std::string_literals;
		std::string msg = "Could not find required class member '"s;
		msg += static_cast<std::string>( reason.member_name ) + "'"s;
		if( location.class_first and location.first ) {
			throw daw::json::json_exception(
			  msg, std::string_view( location.class_first, ( location.first + 1 ) -
			                                                 location.class_first ) );
		} else {
			throw daw::json::json_exception( msg );
		}
	} else {
#endif
		(void)ShouldThrow;
		(void)reason;
		(void)location;
		std::terminate( );
#ifdef DAW_USE_JSON_EXCEPTIONS
	}
#endif
}

/***
 * Ensure that Bool is true
 * If false pass rest of args to daw_json_error
 */
#define daw_json_assert( Bool, ... )                                           \
	if( DAW_JSON_UNLIKELY( not( Bool ) ) ) {                                     \
		daw_json_error( __VA_ARGS__ );                                             \
	}                                                                            \
	while( false )

/***
 * Ensure that Bool is true when in Checked Input mode
 * If false pass rest of args to daw_json_error
 */
#define daw_json_assert_weak( Bool, ... )                                      \
	if constexpr( not Range::is_unchecked_input ) {                              \
		if( DAW_JSON_UNLIKELY( not( Bool ) ) ) {                                   \
			daw_json_error( __VA_ARGS__ );                                           \
		}                                                                          \
	}                                                                            \
	while( false )
