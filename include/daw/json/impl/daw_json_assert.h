// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>

#pragma once

#if defined( __GNUC__ ) or defined( __clang__ )
#define DAW_LIKELY( Bool ) ( __builtin_expect( !!( Bool ), 1 ) )
#define DAW_UNLIKELY( Bool ) ( __builtin_expect( !!( Bool ), 0 ) )
#else
#define DAW_LIKELY( Bool ) Bool
#define DAW_UNLIKELY( Bool ) Bool
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

		explicit constexpr missing_member( std::string_view name )
		  : member_name( name ) {}
	};
} // namespace daw::json::json_details

namespace daw::json {
	class json_exception {
		std::string m_reason{ };

	public:
		[[maybe_unused]] json_exception( ) = default;
		[[maybe_unused]] inline json_exception( std::string_view reason ) noexcept
		  : m_reason( reason ) {}

		[[nodiscard, maybe_unused]] std::string const &reason( ) const noexcept {
			return m_reason;
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
		std::abort( );
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
		std::abort( );
#ifdef DAW_USE_JSON_EXCEPTIONS
	}
#endif
}

#ifndef DAW_JSON_CHECK_DEBUG_ONLY
template<typename Bool>
static constexpr void daw_json_assert( Bool const &b,
                                       std::string_view reason ) {
	if( DAW_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}
template<typename Bool>
static constexpr void
daw_json_assert( Bool const &b,
                 daw::json::json_details::missing_member reason ) {
	if( DAW_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}

#define daw_json_assert_weak( ... )                                            \
	do {                                                                         \
		if constexpr( not IsUnCheckedInput ) {                                     \
			daw_json_assert( __VA_ARGS__ );                                          \
		}                                                                          \
	} while( false )

#else // undef DAW_JSON_CHECK_DEBUG_ONLY
#ifndef NDEBUG
template<typename Bool>
static constexpr void daw_json_assert( Bool const &b,
                                       std::string_view reason ) {
	if( DAW_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}
template<typename Bool>
static constexpr void
daw_json_assert( Bool const &b,
                 daw::json::json_details::missing_member reason ) {
	if( DAW_UNLIKELY( not static_cast<bool>( b ) ) ) {
		daw_json_error( reason );
	}
}

#define daw_json_assert_weak( ... )                                            \
	if constexpr( not IsUnCheckedInput ) {                                       \
		daw_json_assert( __VA_ARGS__ );                                            \
	}                                                                            \
	do {                                                                         \
	} while( false )
#else // NDEBUG set
#define daw_json_assert( ... )                                                 \
	do {                                                                         \
	} while( false )

#define daw_json_assert_weak( ... )                                            \
	do {                                                                         \
	} while( false )
#endif
#endif
#undef DAW_UNLIKELY
#undef DAW_LIKELY
