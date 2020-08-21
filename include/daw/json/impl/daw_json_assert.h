// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_hide.h>

#include <cstdio>
#include <cstdlib>
#include <memory>
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

	public:
		json_exception( ) = default;
		explicit json_exception( std::string_view reason ) noexcept
		  : m_reason( reason ) {}

		explicit json_exception( std::string_view reason,
		                         ErrorType error_type ) noexcept
		  : m_reason( reason )
		  , m_error_type( error_type ) {}

		[[nodiscard]] std::string const &reason( ) const {
			return m_reason;
		}

		[[nodiscard]] ErrorType type( ) const {
			return m_error_type;
		}
	};
} // namespace daw::json

#ifdef DAW_USE_JSON_EXCEPTIONS
inline constexpr bool use_daw_json_exceptions_v = true;
#else
inline constexpr bool use_daw_json_exceptions_v = false;
#endif

template<bool ShouldThrow = use_daw_json_exceptions_v>
[[maybe_unused, noreturn]] void daw_json_error( std::string_view reason ) {
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

template<bool ShouldThrow = use_daw_json_exceptions_v>
[[maybe_unused, noreturn]] void
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

template<typename Bool>
DAW_ATTRIBUTE_FLATTEN inline constexpr void
daw_json_assert( Bool const &b, std::string_view reason ) {
	if( DAW_JSON_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}
template<typename Bool>
DAW_ATTRIBUTE_FLATTEN inline constexpr void
daw_json_assert( Bool const &b,
                 daw::json::json_details::missing_member reason ) {
	if( DAW_JSON_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}

#define daw_json_assert_weak( ... )                                            \
	if constexpr( not Range::is_unchecked_input ) {                              \
		daw_json_assert( __VA_ARGS__ );                                            \
	}                                                                            \
	while( false )
