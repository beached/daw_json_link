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

#pragma once

#if not( defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or             \
         defined( _CPPUNWIND ) )
// account for no exceptions -fno-exceptions
#undef DAW_USE_JSON_EXCEPTIONS
#endif

namespace daw::json {
	namespace {
		class json_exception {
			std::string_view m_reason{};

		public:
			constexpr json_exception( ) noexcept = default;
			constexpr json_exception( std::string_view reason ) noexcept
			  : m_reason( reason ) {}

			[[nodiscard, maybe_unused]] constexpr std::string_view reason( ) const noexcept {
				return m_reason;
			}
		};
#ifdef DAW_USE_JSON_EXCEPTIONS
		inline constexpr bool use_json_exceptions_v = true;
#else
		inline constexpr bool use_json_exceptions_v = false;
#endif

		template<bool ShouldThrow = use_json_exceptions_v>
		[[noreturn]] void json_error( std::string_view reason ) noexcept(
		  not use_json_exceptions_v ) {
			if constexpr( ShouldThrow ) {
				throw daw::json::json_exception( reason );
			} else {
				(void)reason;
				std::abort( );
			}
		}

#ifdef DAW_JSON_CHECK_ALWAYS
		template<typename Bool, size_t N>
		constexpr void json_assert( Bool &&b, char const ( &reason )[N] ) noexcept(
		  not use_json_exceptions_v ) {
			if( not static_cast<bool>( b ) ) {
				json_error( std::string_view( reason ) );
			}
		}

#define json_assert_untrusted( ... )                                           \
	do {                                                                         \
		if constexpr( not TrustedInput ) {                                         \
			json_assert( __VA_ARGS__ );                                              \
		}                                                                          \
	} while( false )

#else // undef DAW_JSON_CHECK_ALWAYS
#ifndef NDEBUG
		template<typename Bool, size_t N>
		constexpr void json_assert( Bool &&b, char const ( &reason )[N] ) noexcept(
		  not use_json_exceptions_v ) {
			if( not static_cast<bool>( b ) ) {
				json_error( std::string_view( reason ) );
			}
		}

#define json_assert_untrusted( ... )                                           \
	if constexpr( not TrustedInput ) {                                           \
		json_assert( __VA_ARGS__ );                                                \
	}
#else // NDEBUG set
#define json_assert( ... )                                                     \
	do {                                                                         \
	} while( false )

#define json_assert_untrusted( ... )                                           \
	do {                                                                         \
	} while( false )
#endif
#endif
#undef DAW_UNLIKELY
	} // namespace
} // namespace daw::json
