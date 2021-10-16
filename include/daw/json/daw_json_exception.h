// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_unreachable.h>

#include <algorithm>
#include <ciso646>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>

#if defined( DAW_JSON_INHERIT_STDEXCEPTION )
#include <exception>
#endif

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		namespace json_details {
			struct missing_member {
				char const *member_name;

				template<typename StringView,
				         std::enable_if_t<
				           ( traits::not_same<StringView, missing_member>::value ),
				           std::nullptr_t> = nullptr>
				explicit constexpr missing_member( StringView name )
				  : member_name( std::data( name ) ) {}

				template<std::size_t N>
				explicit constexpr missing_member( char const ( &s )[N] )
				  : member_name( s ) {}
			};

			struct missing_token {
				char token;
				explicit constexpr missing_token( char c )
				  : token( c ) {}
			};
		} // namespace json_details

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
			UnexpectedJSONVariantType,
			TrailingComma,
			UnexpectedToken
		};

		constexpr std::string_view reason_message( ErrorReason er ) {
			using namespace std::string_view_literals;
			switch( er ) {
			case ErrorReason::Unknown:
				return "Unknown reason for error"sv;
			case ErrorReason::UnexpectedEndOfData:
				return "Unexpected end of data"sv;
			case ErrorReason::InvalidNumber:
				return "Invalid Number"sv;
			case ErrorReason::InvalidNumberStart:
				return R"(Invalid Number started, expected a "0123456789-")"sv;
			case ErrorReason::InvalidNumberUnexpectedQuoting:
				return "Unexpected double quote prior to number"sv;
			case ErrorReason::InvalidTimestamp:
				return "Invalid Timestamp"sv;
			case ErrorReason::InvalidUTFEscape:
				return "Invalid UTF Escape"sv;
			case ErrorReason::InvalidUTFCodepoint:
				return "Invalid UTF Codepoint"sv;
			case ErrorReason::InvalidEndOfValue:
				return R"(Did not find \",}]" at end of value)"sv;
			case ErrorReason::InvalidLiteral:
				return "Literal data was corrupt"sv;
			case ErrorReason::InvalidString:
				return "Invalid or corrupt string"sv;
			case ErrorReason::InvalidStringHighASCII:
				return "String support limited to 0x20 < chr <= 0x7F when "
				       "DisallowHighEightBit is true"sv;
			case ErrorReason::ExpectedKeyValueToStartWithBrace:
				return "Expected key/value's JSON type to be of class type and "
				       "beginning "
				       "with '{'"sv;
			case ErrorReason::ExpectedKeyValueArrayToStartWithBracket:
				return "Expected key/value's JSON type to be of array type and "
				       "beginning "
				       "with '['"sv;
			case ErrorReason::InvalidArrayStart:
				return "Expected array type to begin with '['"sv;
			case ErrorReason::InvalidClassStart:
				return "Expected class type to begin with '{'"sv;
			case ErrorReason::InvalidStartOfValue:
				return "Unexpected character data at start of value"sv;
			case ErrorReason::InvalidTrue:
				return "Expected true not found"sv;
			case ErrorReason::InvalidFalse:
				return "Expected false not found"sv;
			case ErrorReason::InvalidNull:
				return "Expected null not found"sv;
			case ErrorReason::UnexpectedNull:
				return "An unexpected null value was encountered while serializing"sv;
			case ErrorReason::NumberIsNaN:
				return "NaN encountered while serializing to JSON Number literal"sv;
			case ErrorReason::NumberIsInf:
				return "Infinity encountered while serializing to JSON Number literal"sv;
			case ErrorReason::NumberOutOfRange:
				return "Number is outside of the representable range"sv;
			case ErrorReason::EmptyJSONDocument:
				return "Attempt to parse an empty JSON document"sv;
			case ErrorReason::EmptyJSONPath:
				return "Empty JSON Path specified"sv;
			case ErrorReason::JSONPathNotFound:
				return "JSON Path specified not found in document"sv;
			case ErrorReason::InvalidJSONPath:
				return "Invalid JSON Path specified"sv;
			case ErrorReason::NullOutputIterator:
				return "Null pointer specified for output"sv;
			case ErrorReason::MissingMemberNameOrEndOfClass:
				return "Missing member name or end of class"sv;
			case ErrorReason::MissingMemberName:
				return "Missing member name"sv;
			case ErrorReason::InvalidMemberName:
				return "Member names must be JSON strings"sv;
			case ErrorReason::ExpectedArrayOrClassStart:
				return "Expected start of a JSON class or array"sv;
			case ErrorReason::UnknownMember:
				return "Could not find member in JSON class"sv;
			case ErrorReason::InvalidBracketing:
				return "Invalid Bracketing"sv;
			case ErrorReason::AttemptToAccessPastEndOfValue:
				return "A value of known size was accessed past the end"sv;
			case ErrorReason::OutOfOrderOrderedMembers:
				return "Order of ordered members must be ascending"sv;
			case ErrorReason::MemberNotFound:
				return "Expected member not found"sv;
			case ErrorReason::TagMemberNotFound:
				return "Expected tag member not found, they are required for tagged "
				       "Variants"sv;
			case ErrorReason::ExpectedMemberNotFound:
				return "Expected member missing"sv;
			case ErrorReason::ExpectedTokenNotFound:
				return "Expected token missing"sv;
			case ErrorReason::UnexpectedJSONVariantType:
				return "Unexpected JSON Variant Type"sv;
			case ErrorReason::TrailingComma:
				return "Trailing comma"sv;
			case ErrorReason::UnexpectedToken:
				return "Unexpected token"sv;
			}
#ifdef NDEBUG
			DAW_UNREACHABLE( );
#else
			std::terminate( );
#endif
		}

		/***
		 * This allows a codebase to catch( std::exception const & ) and still catch
		 * a json_exception. Not all actionable information is available through the
		 * std::exception interface
		 */
#if defined( DAW_JSON_USE_STDEXCEPT )
#define DAW_JSON_STDEXCEPTION_FLAG true
#define DAW_JSON_EXCEPTION_PARENT <true> : std::exception
#define DAW_JSON_EXCEPTION_CONSTEXPR inline
#else
#define DAW_JSON_STDEXCEPTION_FLAG false
#define DAW_JSON_EXCEPTION_PARENT <false>
#define DAW_JSON_EXCEPTION_CONSTEXPR constexpr
#endif
		/***
		 * When a parser error occurs this is thrown.  It will provide the local
		 * reason for the error and some information about the location in the
		 * parser if available.  Using the bool flag to ensure that the exception
		 * type matches the compiler define and has a different name
		 */
		template<bool /*uses std::exception*/>
		class basic_json_exception;

		template<>
		class basic_json_exception DAW_JSON_EXCEPTION_PARENT {
			ErrorReason m_reason = ErrorReason::Unknown;
			union data_t {
				char const *pointer;
				char token;

				explicit constexpr data_t( char const *p )
				  : pointer( p ) {}
				explicit constexpr data_t( char t )
				  : token( t ) {}
			} m_data{ nullptr };
			char const *m_parse_loc = nullptr;

		public:
			DAW_JSON_EXCEPTION_CONSTEXPR basic_json_exception( ) = default;

			explicit DAW_JSON_EXCEPTION_CONSTEXPR
			basic_json_exception( ErrorReason reason )
			  : m_reason( reason ) {}

			explicit DAW_JSON_EXCEPTION_CONSTEXPR
			basic_json_exception( json_details::missing_member mm )
			  : m_reason( ErrorReason::MemberNotFound )
			  , m_data( mm.member_name ) {}

			explicit DAW_JSON_EXCEPTION_CONSTEXPR
			basic_json_exception( json_details::missing_token mt )
			  : m_reason( ErrorReason::ExpectedTokenNotFound )
			  , m_data( mt.token ) {}

			explicit DAW_JSON_EXCEPTION_CONSTEXPR
			basic_json_exception( json_details::missing_member mm,
			                      std::string_view location )
			  : m_reason( ErrorReason::MemberNotFound )
			  , m_data( mm.member_name )
			  , m_parse_loc( std::data( location ) ) {}

			explicit DAW_JSON_EXCEPTION_CONSTEXPR
			basic_json_exception( json_details::missing_token mt,
			                      char const *location )
			  : m_reason( ErrorReason::ExpectedTokenNotFound )
			  , m_data( mt.token )
			  , m_parse_loc( location ) {}

			explicit DAW_JSON_EXCEPTION_CONSTEXPR
			basic_json_exception( ErrorReason reason, char const *location )
			  : m_reason( reason )
			  , m_parse_loc( location ) {}

			[[nodiscard]] DAW_JSON_EXCEPTION_CONSTEXPR ErrorReason
			reason_type( ) const {
				return m_reason;
			}

			[[nodiscard]] inline std::string reason( ) const {
#if defined( __clang__ )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif
				switch( m_reason ) {
				case ErrorReason::MemberNotFound: {
					using namespace std::string_literals;
					return "Could not find required class member '"s +
					       static_cast<std::string>( m_data.pointer ) + "'"s;
				}
				case ErrorReason::ExpectedTokenNotFound: {
					using namespace std::string_literals;
					return "Could not find expected parse token '"s + m_data.token + "'"s;
				}
				default:
					return std::string( ( reason_message( m_reason ) ) );
				}
#if defined( __clang__ )
#pragma clang diagnostic pop
#endif
			}

			[[nodiscard]] constexpr char const *parse_location( ) const {
				return m_parse_loc;
			}
#if defined( DAW_JSON_USE_STDEXCEPT )
			inline char const *what( ) const noexcept override {
				// reason_message returns a string_view to a literal
				return reason_message( m_reason ).data( );
			}
			basic_json_exception( basic_json_exception const & ) = default;
			basic_json_exception( basic_json_exception && ) noexcept = default;
			basic_json_exception &operator=( basic_json_exception const & ) = default;
			basic_json_exception &
			operator=( basic_json_exception && ) noexcept = default;
			inline ~basic_json_exception( ) override = default;
#endif
		};

		using json_exception = basic_json_exception<DAW_JSON_STDEXCEPTION_FLAG>;

		/***
		 * Helper to provide output formatted information about json_exception
		 * @param je json_exception to be formatted
		 * @return string representation of json_exception
		 */
		template<bool B>
		std::string to_formatted_string( basic_json_exception<B> const &je,
		                                 char const *json_document = nullptr ) {
			using namespace std::string_literals;
			std::string result = "reason: "s + je.reason( );
			if( json_document == nullptr or je.parse_location( ) == nullptr ) {
				return result;
			}
			auto const previous_char_count =
			  ( std::min )( static_cast<std::size_t>( 50 ),
			                static_cast<std::size_t>( std::distance(
			                  json_document, je.parse_location( ) + 1 ) ) );
			auto const loc_data = std::string_view(
			  std::prev( je.parse_location( ),
			             static_cast<std::ptrdiff_t>( previous_char_count ) ),
			  previous_char_count + 1 );
#ifndef _WIN32
			result += "\nlocation:\x1b[1m";
#endif
			result +=
			  std::accumulate( std::data( loc_data ), daw::data_end( loc_data ),
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
	} // namespace DAW_JSON_VER
} // namespace daw::json
#if defined( DAW_JSON_EXCEPTION_PARENT )
#undef DAW_JSON_EXCEPTION_PARENT
#endif
#if defined( DAW_JSON_EXCEPTION_CONSTEXPR )
#undef DAW_JSON_EXCEPTION_CONSTEXPR
#endif
#if defined( DAW_JSON_STDEXCEPTION_FLAG )
#undef DAW_JSON_STDEXCEPTION_FLAG
#endif
