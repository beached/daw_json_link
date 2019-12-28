// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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
#include <string_view>

#pragma once

#if not( defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or             \
         defined( _CPPUNWIND ) )
// account for no exceptions -fno-exceptions
#undef DAW_USE_JSON_EXCEPTIONS
#endif

namespace daw::json {
	class json_exception {
		std::string_view m_reason{};

	public:
		[[maybe_unused]] constexpr json_exception( ) noexcept = default;
		[[maybe_unused]] constexpr json_exception(
		  std::string_view reason ) noexcept
		  : m_reason( reason ) {}

		[[nodiscard, maybe_unused]] constexpr std::string_view reason( ) const
		  noexcept {
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
[[maybe_unused, noreturn]] void
daw_json_error( std::string_view reason ) noexcept( not ShouldThrow ) {
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

#ifdef DAW_JSON_CHECK_ALWAYS
template<typename Bool, size_t N>
static constexpr void daw_json_assert(
  Bool &&b,
  char const ( &reason )[N] ) noexcept( not use_daw_json_exceptions_v ) {
	if( not static_cast<bool>( b ) ) {
		daw_json_error( std::string_view( reason ) );
	}
}

#define daw_json_assert_untrusted( ... )                                       \
	do {                                                                         \
		if constexpr( not IsTrustedInput ) {                                       \
			daw_json_assert( __VA_ARGS__ );                                          \
		}                                                                          \
	} while( false )

#else // undef DAW_JSON_CHECK_ALWAYS
#ifndef NDEBUG
template<typename Bool, size_t N>
static constexpr void daw_json_assert(
  Bool &&b,
  char const ( &reason )[N] ) noexcept( not use_daw_json_exceptions_v ) {
	if( not static_cast<bool>( b ) ) {
		daw_json_error( std::string_view( reason ) );
	}
}

#define daw_json_assert_untrusted( ... )                                       \
	if constexpr( not IsTrustedInput ) {                                         \
		daw_json_assert( __VA_ARGS__ );                                            \
	}                                                                            \
	do {                                                                         \
	} while( false )
#else // NDEBUG set
#define daw_json_assert( ... )                                                 \
	do {                                                                         \
	} while( false )

#define daw_json_assert_untrusted( ... )                                       \
	do {                                                                         \
	} while( false )
#endif
#endif
#undef DAW_UNLIKELY
