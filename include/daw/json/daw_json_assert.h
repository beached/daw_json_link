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
#include <daw/daw_visit.h>

#include <algorithm>
#include <ciso646>
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
#elif defined( _MSC_VER )
#define DAW_JSON_LIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNLIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNREACHABLE( ) __assume( false );
#else
#define DAW_JSON_LIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNLIKELY( Bool ) !!( Bool )
#define DAW_JSON_UNREACHABLE( ) std::terminate( )
#endif

#if not( defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or             \
         defined( _CPPUNWIND ) ) or                                            \
  defined( DAW_DONT_USE_JSON_EXCEPTIONS )
#ifdef DAW_USE_JSON_EXCEPTIONS
#undef DAW_USE_JSON_EXCEPTIONS
#endif
#else
#ifndef DAW_USE_JSON_EXCEPTIONS
#define DAW_USE_JSON_EXCEPTIONS
#endif
#endif

namespace daw::json::json_details {
	struct missing_member {
		std::string_view member_name;

		template<typename StringView,
		         std::enable_if_t<(not std::is_same_v<StringView, missing_member>),
		                          std::nullptr_t> = nullptr>
		explicit constexpr missing_member( StringView name )
		  : member_name( name.data( ), name.size( ) ) {}

		template<std::size_t N>
		explicit constexpr missing_member( char const ( &s )[N] )
		  : member_name( s, N - 1 ) {}
	};

	struct missing_token {
		char token;
		explicit constexpr missing_token( char c )
		  : token( c ) {}
	};
} // namespace daw::json::json_details

namespace daw::json {
	// enum class ErrorType { Unknown, MissingMember, UnexpectedCharacter };
	enum class ErrorReason {
		Unknown,
		UnexpectedEndOfData,
		InvalidNumber,
		InvalidNumberStart,
		InvalidNumberUnexpectedQuoting,
		InvalidTimestamp,
		InvalidUTFEscape,
		InvalidUTFCodepoint,
		InvalidEndOfValue,
		InvalidLiteral,
		InvalidString,
		InvalidStringHighASCII,
		ExpectedKeyValueToStartWithBrace,
		ExpectedKeyValueArrayToStartWithBracket,
		InvalidArrayStart,
		InvalidClassStart,
		InvalidStartOfValue,
		InvalidTrue,
		InvalidFalse,
		InvalidNull,
		UnexpectedNull,
		NumberIsNaN,
		NumberIsInf,
		NumberOutOfRange,
		EmptyJSONDocument,
		EmptyJSONPath,
		JSONPathNotFound,
		InvalidJSONPath,
		NullOutputIterator,
		MissingMemberName,
		InvalidMemberName,
		ExpectedArrayOrClassStart,
		UnknownMember,
		InvalidBracketing,
		AttemptToAccessPastEndOfValue,
		OutOfOrderOrderedMembers,
		MissingMemberNameOrEndOfClass,
		MemberNotFound,
		TagMemberNotFound,
		ExpectedMemberNotFound,
		ExpectedTokenNotFound,
		UnexpectedJSONVariantType
	};

	constexpr char const *reason_message( ErrorReason er ) {
		switch( er ) {
		case ErrorReason::Unknown:
			return "Unknown reason for error";
		case ErrorReason::UnexpectedEndOfData:
			return "Unexpected end of data";
		case ErrorReason::InvalidNumber:
			return "Invalid Number";
		case ErrorReason::InvalidNumberStart:
			return "Invalid Number started, expected a \"0123456789-\"";
		case ErrorReason::InvalidNumberUnexpectedQuoting:
			return "Unexpected double quote prior to number";
		case ErrorReason::InvalidTimestamp:
			return "Invalid Timestamp";
		case ErrorReason::InvalidUTFEscape:
			return "Invalid UTF Escape";
		case ErrorReason::InvalidUTFCodepoint:
			return "Invalid UTF Codepoint";
		case ErrorReason::InvalidEndOfValue:
			return "Did not find \",}]\" at end of value";
		case ErrorReason::InvalidLiteral:
			return "Literal data was corrupt";
		case ErrorReason::InvalidString:
			return "Invalid or corrupt string";
		case ErrorReason::InvalidStringHighASCII:
			return "String support limited to 0x20 < chr <= 0x7F when "
			       "DisallowHighEightBit is true";
		case ErrorReason::ExpectedKeyValueToStartWithBrace:
			return "Expected key/value's JSON type to be of class type and beginning "
			       "with '{'";
		case ErrorReason::ExpectedKeyValueArrayToStartWithBracket:
			return "Expected key/value's JSON type to be of array type and beginning "
			       "with '['";
		case ErrorReason::InvalidArrayStart:
			return "Expected array type to begin with '['";
		case ErrorReason::InvalidClassStart:
			return "Expected class type to begin with '{'";
		case ErrorReason::InvalidStartOfValue:
			return "Unexpected character data at start of value";
		case ErrorReason::InvalidTrue:
			return "Expected true not found";
		case ErrorReason::InvalidFalse:
			return "Expected false not found";
		case ErrorReason::InvalidNull:
			return "Expected null not found";
		case ErrorReason::UnexpectedNull:
			return "An unexpected null value was encountered while serializing";
		case ErrorReason::NumberIsNaN:
			return "NaN encountered while serializing to JSON Number literal";
		case ErrorReason::NumberIsInf:
			return "Infinity encountered while serializing to JSON Number literal";
		case ErrorReason::NumberOutOfRange:
			return "Number is outside of the representable range";
		case ErrorReason::EmptyJSONDocument:
			return "Attempt to parse an empty JSON document";
		case ErrorReason::EmptyJSONPath:
			return "Empty JSON Path specified";
		case ErrorReason::JSONPathNotFound:
			return "JSON Path specified not found in document";
		case ErrorReason::InvalidJSONPath:
			return "Invalid JSON Path specified";
		case ErrorReason::NullOutputIterator:
			return "Null pointer specified for output";
		case ErrorReason::MissingMemberNameOrEndOfClass:
			return "Missing member name or end of class";
		case ErrorReason::MissingMemberName:
			return "Missing member name";
		case ErrorReason::InvalidMemberName:
			return "Member names must be JSON strings";
		case ErrorReason::ExpectedArrayOrClassStart:
			return "Expected start of a JSON class or array";
		case ErrorReason::UnknownMember:
			return "Could not find member in JSON class";
		case ErrorReason::InvalidBracketing:
			return "Invalid Bracketing";
		case ErrorReason::AttemptToAccessPastEndOfValue:
			return "A value of known size was accessed past the end";
		case ErrorReason::OutOfOrderOrderedMembers:
			return "Order of ordered members must be ascending";
		case ErrorReason::MemberNotFound:
			return "Expected member not found";
		case ErrorReason::TagMemberNotFound:
			return "Expected tag member not found, they are required for tagged "
			       "Variants";
		case ErrorReason::ExpectedMemberNotFound:
			return "Expected member missing";
		case ErrorReason::ExpectedTokenNotFound:
			return "Expected token missing";
		case ErrorReason::UnexpectedJSONVariantType:
			return "Unexpected JSON Variant Type";
		}
		DAW_JSON_UNREACHABLE( );
	}

	/***
	 * When a parser error occurs this is thrown.  It will provide the local
	 * reason for the error and some information about the location in the parser
	 * if available.
	 */
	class json_exception {
		std::variant<std::monostate, ErrorReason, json_details::missing_member,
		             json_details::missing_token>
		  m_reason{ };
		char const *m_parse_loc = nullptr;

	public:
		constexpr json_exception( ) = default;

		explicit constexpr json_exception( ErrorReason reason ) noexcept
		  : m_reason( reason ) {}

		explicit constexpr json_exception(
		  json_details::missing_member mm ) noexcept
		  : m_reason( mm ) {}

		explicit constexpr json_exception( json_details::missing_token mt ) noexcept
		  : m_reason( mt ) {}

		explicit constexpr json_exception( json_details::missing_member mm,
		                                   std::string_view location ) noexcept
		  : m_reason( mm )
		  , m_parse_loc( location.data( ) ) {}

		explicit constexpr json_exception( json_details::missing_token mt,
		                                   std::string_view location ) noexcept
		  : m_reason( mt )
		  , m_parse_loc( location.data( ) ) {}

		explicit constexpr json_exception( ErrorReason reason,
		                                   std::string_view location ) noexcept
		  : m_reason( reason )
		  , m_parse_loc( location.data( ) ) {}

		[[nodiscard]] constexpr ErrorReason reason_type( ) const {
			return daw::visit_nt(
			  m_reason, []( std::monostate ) { return ErrorReason::Unknown; },
			  []( ErrorReason er ) { return er; },
			  []( json_details::missing_member const & ) {
				  return ErrorReason::ExpectedMemberNotFound;
			  },
			  []( json_details::missing_token const & ) {
				  return ErrorReason::ExpectedTokenNotFound;
			  } );
		}

		[[nodiscard]] inline std::string reason( ) const {
			return daw::visit_nt(
			  m_reason, []( std::monostate ) { return std::string{ }; },
			  []( ErrorReason er ) { return std::string( reason_message( er ) ); },
			  []( json_details::missing_member const &mm ) {
				  using namespace std::string_literals;
				  return "Could not find required class member '"s +
				         static_cast<std::string>( mm.member_name ) + "'"s;
			  },
			  []( json_details::missing_token const &mt ) {
				  using namespace std::string_literals;
				  return "Could not find expected parse token '"s + mt.token + "'"s;
			  } );
		}

		[[nodiscard]] inline char const *parse_location( ) const {
			return m_parse_loc;
		}
	};

	/***
	 * Helper to provide output formatted information about json_exception
	 * @param je json_exception to be formatted
	 * @return string representation of json_exception
	 */
	inline std::string
	to_formatted_string( json_exception const &je,
	                     char const *json_document = nullptr ) {
		using namespace std::string_literals;
		std::string result = "reason: "s + je.reason( );
		if( json_document == nullptr or je.parse_location( ) == nullptr ) {
			return result;
		}
		auto const previous_char_count =
		  std::min( static_cast<std::size_t>( 100 ),
		            static_cast<std::size_t>(
		              std::distance( json_document, je.parse_location( ) ) ) );
		auto const loc_data = std::string_view(
		  std::prev( je.parse_location( ),
		             static_cast<std::ptrdiff_t>( previous_char_count ) ),
		  previous_char_count );
#ifndef _WIN32
		result += "\nlocation:\x1b[1m";
#endif
		result +=
		  std::accumulate( loc_data.data( ), loc_data.data( ) + loc_data.size( ),
		                   std::string{ }, []( std::string s, char c ) {
			                   if( ( c != '\n' ) & ( c != '\r' ) ) {
				                   s += c;
			                   }
			                   return s;
		                   } );
#ifndef _WIN32
		result += "\x1b[0m\n";
#endif
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
daw_json_error( daw::json::ErrorReason reason ) {
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
[[maybe_unused, noreturn]] static void
daw_json_error( daw::json::ErrorReason reason, Range const &location ) {
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
[[maybe_unused, noreturn]] static void
daw_json_error( daw::json::json_details::missing_token reason ) {
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
[[maybe_unused, noreturn]] static void
daw_json_error( daw::json::json_details::missing_member reason,
                Range const &location ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
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
					return std::min( static_cast<std::size_t>( std::distance(
					                   location.class_first, location.class_last ) ),
					                 max_len );
				}
				return std::min( static_cast<std::size_t>( std::distance(
				                   location.class_first, location.first + 1 ) ),
				                 max_len );
			}( );
			throw daw::json::json_exception(
			  reason, std::string_view( location.class_first, len ) );
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

template<bool ShouldThrow = use_daw_json_exceptions_v, typename Range>
[[maybe_unused, noreturn]] static void
daw_json_error( daw::json::json_details::missing_token reason,
                Range const &location ) {
#ifdef DAW_USE_JSON_EXCEPTIONS
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
					return std::min( static_cast<std::size_t>( std::distance(
					                   location.class_first, location.class_last ) ),
					                 max_len );
				}
				return std::min( static_cast<std::size_t>( std::distance(
				                   location.class_first, location.first + 1 ) ),
				                 max_len );
			}( );
			throw daw::json::json_exception(
			  reason, std::string_view( location.class_first, len ) );
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
