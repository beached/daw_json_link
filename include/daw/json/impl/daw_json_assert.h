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
#undef DAW_JSON_USE_EXCEPTIONS
#endif

namespace daw::json {
	class json_exception {
		std::string_view m_reason{};

	public:
		constexpr json_exception( ) noexcept = default;
		constexpr json_exception( std::string_view reason ) noexcept
		  : m_reason( reason ) {}

		[[nodiscard]] constexpr std::string_view reason( ) const noexcept {
			return m_reason;
		}
	};
#ifdef DAW_JSON_USE_EXCEPTIONS
	inline constexpr bool use_json_exceptions_v = true;
#else
	inline constexpr bool use_json_exceptions_v = false;
#endif

	[[noreturn]] void
	json_error( std::string_view reason ) noexcept( not use_json_exceptions_v ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
		throw json_exception( reason );
#else
		(void)reason;
		std::abort( );
#endif
	}

#ifdef DAW_JSON_CHECK_ALWAYS
	template<typename Bool>
	inline constexpr void
	json_assert( Bool &&b,
	             std::string_view reason ) noexcept( not use_json_exceptions_v ) {
		static_assert( std::is_convertible_v<Bool, bool>,
		               "Argument must be convertable to a bool" );
		if( not static_cast<bool>( b ) ) {
			json_error( reason );
		}
	}
#else // undef DAW_JSON_CHECK_ALWAYS
#ifndef NDEBUG
	template<typename Bool>
	inline constexpr void
	json_assert( Bool &&b,
	             std::string_view reason ) noexcept( not use_json_exceptions_v ) {
		static_assert( std::is_convertible_v<Bool, bool>,
		               "Argument must be convertable to a bool" );
		if( not static_cast<bool>( b ) ) {
			json_error( reason );
		}
	}
#else // NDEBUG set
#define json_assert( ... )                                                     \
	do {                                                                         \
	} while( false )
#endif
#endif

} // namespace daw::json
