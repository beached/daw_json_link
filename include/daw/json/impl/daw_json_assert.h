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
#define DAW_JSON_LIKELY( Bool ) Bool
#define DAW_JSON_UNLIKELY( Bool ) Bool
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
		  , m_location(
		      location.data( ),
		      std::min( location.size( ), static_cast<std::size_t>( 79U ) ) )
		  , m_parse_loc( location.data( ) + location.size( ) ) {}

		explicit json_exception( std::string_view reason,
		                         ErrorType error_type ) noexcept
		  : m_reason( reason )
		  , m_error_type( error_type ) {}

		explicit json_exception( std::string_view reason, ErrorType error_type,
		                         std::string_view location ) noexcept
		  : m_reason( reason )
		  , m_error_type( error_type )
		  , m_location(
		      location.data( ),
		      std::min( location.size( ), static_cast<std::size_t>( 10U ) ) )
		  , m_parse_loc( location.data( ) + location.size( ) ) {}

		[[nodiscard]] std::string const &reason( ) const {
			return m_reason;
		}

		[[nodiscard]] std::string const &location( ) const {
			return m_location;
		}

		[[nodiscard]] ErrorType type( ) const {
			return m_error_type;
		}
	};

	std::string to_string( json_exception const &je ) {
		using namespace std::string_literals;
		auto loc = std::accumulate(
		  je.location( ).begin( ), je.location( ).end( ), std::string{ },
		  []( std::string r, char c ) -> std::string {
			  if( static_cast<unsigned char>( c ) >= 0x20 ) {
				  r += c;
			  }
			  return r;
		  } );
		std::string result = "reason: "s + je.reason( ) + "\nlocation: "s + loc;
		return result;
	}
} // namespace daw::json

#ifdef DAW_USE_JSON_EXCEPTIONS
inline constexpr bool use_daw_json_exceptions_v = true;
#else
inline constexpr bool use_daw_json_exceptions_v = false;
#endif

template<bool ShouldThrow = use_daw_json_exceptions_v,
         typename Range = std::nullptr_t>
[[maybe_unused, noreturn]] void
daw_json_error( std::string_view reason, Range const &location = nullptr ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
	if constexpr( ShouldThrow ) {
		if constexpr( std::is_same_v<std::nullptr_t, Range> ) {
			(void)location;
			throw daw::json::json_exception( reason );
		} else {
			if( location.class_first and location.first ) {
				auto loc =
				  std::string_view( location.class_first,
				                    static_cast<std::size_t>( ( location.first + 1 ) -
				                                              location.class_first ) );
				throw daw::json::json_exception( reason, loc );
			} else {
				throw daw::json::json_exception( reason );
			}
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

template<bool ShouldThrow = use_daw_json_exceptions_v,
         typename Range = std::nullptr_t>
[[maybe_unused, noreturn]] void
daw_json_error( daw::json::json_details::missing_member reason,
                Range location = nullptr ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
	if constexpr( ShouldThrow ) {
		using namespace std::string_literals;
		std::string msg = "Could not find required class member '"s;
		msg += static_cast<std::string>( reason.member_name ) + "'"s;
		if constexpr( std::is_same_v<Range, std::nullptr_t> ) {
			throw daw::json::json_exception( msg );
		} else {
			if( location.class_first and location.first ) {
				throw daw::json::json_exception(
				  msg,
				  std::string_view( location.class_first,
				                    ( location.first + 1 ) - location.class_first ) );
			} else {
				throw daw::json::json_exception( msg );
			}
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

template<typename Bool>
DAW_ATTRIBUTE_FLATTEN inline constexpr void
daw_json_assert( Bool const &b, std::string_view reason ) {
	if( DAW_JSON_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}
template<typename Bool, typename Range>
DAW_ATTRIBUTE_FLATTEN inline constexpr void
daw_json_assert( Bool const &b, std::string_view reason,
                 Range const &location ) {
	if( DAW_JSON_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason, location );
	}
}
template<typename Bool>
DAW_ATTRIBUTE_FLATTEN inline constexpr void
daw_json_assert_strong( Bool const &b,
                        daw::json::json_details::missing_member reason ) {
	if( DAW_JSON_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}
template<typename Bool, typename Range>
DAW_ATTRIBUTE_FLATTEN inline constexpr void
daw_json_assert_strong( Bool const &b,
                        daw::json::json_details::missing_member reason,
                        Range const &location ) {
	if( DAW_JSON_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason, location );
	}
}

#define daw_json_assert_weak( ... )                                            \
	if constexpr( not Range::is_unchecked_input ) {                              \
		daw_json_assert( __VA_ARGS__ );                                            \
	}                                                                            \
	while( false )
