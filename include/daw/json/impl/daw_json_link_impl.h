// The MIT License( MIT )
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

#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/iterator/daw_inserter.h>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"

namespace daw::json {
	namespace to_strings {
		namespace {
			using std::to_string;
		}

		template<typename T>
		auto to_string( std::optional<T> const &v ) -> decltype( to_string( *v ) ) {
			if( not v ) {
				return {"null"};
			}
			return to_string( *v );
		}
	} // namespace to_strings

	template<typename T>
	constexpr T from_json( std::string_view sv );

	template<typename T, typename First, typename Last>
	constexpr T from_json( impl::IteratorRange<First, Last> &rng );

	enum class LiteralAsStringOpt : uint8_t { never, maybe, always };

	namespace impl {
		template<typename T>
		using is_a_json_type_detect = typename T::i_am_a_json_type;

		template<typename T>
		inline constexpr bool is_a_json_type_v =
		  daw::is_detected_v<is_a_json_type_detect, T>;

		template<typename T>
		bool is_null( std::optional<T> const &v ) {
			return not static_cast<bool>( v );
		}

		template<typename T>
		bool is_null( T const & ) {
			return false;
		}
	} // namespace impl
#ifdef __cpp_nontype_template_parameter_class
	// C++ 20 Non-Type Class Template Arguments
#define JSONNAMETYPE daw::bounded_string

	template<typename String>
	constexpr size_t json_name_len( String const &str ) noexcept {
		return str.size( );
	}

	template<typename Lhs, typename Rhs>
	constexpr bool json_name_eq( Lhs const &lhs, Rhs const &rhs ) noexcept {
		return lhs == rhs;
	}
	// Convienience for array members
	static inline constexpr JSONNAMETYPE const no_name = "";
#else
#define JSONNAMETYPE char const *

	constexpr size_t json_name_len( char const *const str ) noexcept {
		return daw::string_view( str ).size( );
	}

	constexpr bool json_name_eq( char const *const lhs,
	                             char const *const rhs ) noexcept {
		return std::string_view( lhs ) == std::string_view( rhs );
	}

	// Convienience for array members
	static constexpr char const no_name[] = "";
#endif

	struct parse_js_date {
		constexpr std::chrono::time_point<std::chrono::system_clock,
		                                  std::chrono::milliseconds>
		operator( )( char const *ptr, size_t sz ) const {
			return daw::date_parsing::parse_javascript_timestamp(
			  daw::string_view( ptr, sz ) );
		}
	};

	template<typename T>
	struct custom_to_converter_t {
		constexpr decltype( auto ) operator( )( T &&value ) const {
			using std::to_string;
			return to_string( std::move( value ) );
		}

		constexpr decltype( auto ) operator( )( T const &value ) const {
			using std::to_string;
			return to_string( value );
		}
	};

	template<typename T>
	struct custom_from_converter_t {
		constexpr decltype( auto ) operator( )( std::string_view sv ) {
			return from_string( daw::tag<T>, sv );
		}
	};

	enum class JsonParseTypes : uint_fast8_t {
		Number,
		Bool,
		String,
		Date,
		Class,
		Array,
		Null,
		KeyValue,
		Custom
	};

	template<JsonParseTypes v>
	using ParseTag = std::integral_constant<JsonParseTypes, v>;

	namespace impl {
		// Paths are specified with dot separators, if the name has a dot in it,
		// it must be escaped
		// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
		constexpr daw::string_view pop_json_path( daw::string_view &path ) {
			return path.pop_front( [in_escape = false]( char c ) mutable {
				if( in_escape ) {
					in_escape = false;
					return false;
				}
				if( c == '\\' ) {
					in_escape = true;
					return false;
				}
				return ( c == '.' );
			} );
		}

		namespace data_size {
			constexpr char const *data( char const *ptr ) noexcept {
				return ptr;
			}

			constexpr size_t size( char const *ptr ) noexcept {
				return std::string_view( ptr ).size( );
			}

			using std::data;
			template<typename T>
			using data_detect = decltype( data( std::declval<T &>( ) ) );

			using std::size;
			template<typename T>
			using size_detect = decltype( size( std::declval<T &>( ) ) );

			template<typename T>
			inline constexpr bool has_data_size_v = daw::is_detected_v<data_detect, T>
			  and daw::is_detected_v<size_detect, T>;
		} // namespace data_size

		template<typename Container, typename OutputIterator,
		         daw::enable_when_t<daw::traits::is_container_like_v<
		           daw::remove_cvref_t<Container>>> = nullptr>
		constexpr OutputIterator copy_to_iterator( Container const &c,
		                                           OutputIterator it ) {
			for( auto const &value : c ) {
				*it++ = value;
			}
			return it;
		}

		template<typename OutputIterator>
		constexpr OutputIterator copy_to_iterator( char const *ptr,
		                                           OutputIterator it ) {
			if( ptr == nullptr ) {
				return it;
			}
			while( *ptr != '\0' ) {
				*it = *ptr;
				++it;
				++ptr;
			}
			return it;
		}

		template<typename T>
		using json_parser_description_t = daw::remove_cvref_t<decltype(
		  describe_json_class( std::declval<T &>( ) ) )>;

		template<typename T>
		constexpr auto deref_detect( T &&value ) noexcept -> decltype( *value ) {
			return *value;
		}

		template<typename Optional>
		using deref_t = decltype(
		  deref_detect( std::declval<daw::remove_cvref_t<Optional> &>( ) ) );

		template<typename Optional>
		inline constexpr bool is_valid_optional_v =
		  daw::is_detected_v<deref_t, Optional>;

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Null>,
		                                    OutputIterator it,
		                                    parse_to_t const &container );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Array>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Custom>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Class>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Number>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::String>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Date>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Bool>,
		                                    OutputIterator it,
		                                    parse_to_t const &value );

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Bool>,
		                                    OutputIterator it,
		                                    parse_to_t const &value ) {
			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );
			if( value ) {
				return copy_to_iterator( "true", it );
			}
			return copy_to_iterator( "false", it );
		}

		template<typename JsonMember, typename OutputIterator, typename Optional>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Null>,
		                                    OutputIterator it,
		                                    Optional const &value ) {
			static_assert( is_valid_optional_v<Optional> );
			if( not value ) {
				it = copy_to_iterator( "null", it );
				return it;
			}
			using sub_type = typename JsonMember::sub_type;
			using tag_type = ParseTag<sub_type::expected_type>;
			return to_string<sub_type>( tag_type{}, it, *value );
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Number>,
		                                    OutputIterator it,
		                                    parse_to_t const &value ) {

			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );

			using ::daw::json::to_strings::to_string;
			using std::to_string;
			return copy_to_iterator( to_string( value ), it );
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::String>,
		                                    OutputIterator it,
		                                    parse_to_t const &value ) {

			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );

			*it++ = '"';
			it = copy_to_iterator( value, it );
			*it++ = '"';
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Date>,
		                                    OutputIterator it,
		                                    parse_to_t const &value ) {
			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );

			using ::daw::json::impl::is_null;
			if( is_null( value ) ) {
				it = copy_to_iterator( "null", it );
			} else {
				*it++ = '"';
				using namespace ::daw::date_formatting::formats;
				it = copy_to_iterator( ::daw::date_formatting::fmt_string(
				                         "{0}T{1}:{2}:{3}Z", value, YearMonthDay{},
				                         Hour{}, Minute{}, Second{} ),
				                       it );
				*it++ = '"';
			}
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Class>,
		                                    OutputIterator it,
		                                    parse_to_t const &value ) {

			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );

			return json_parser_description_t<parse_to_t>::template serialize(
			  it, to_json_data( value ) );
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Custom>,
		                                    OutputIterator it,
		                                    parse_to_t const &value ) {

			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );

			if constexpr( JsonMember::is_string ) {
				*it++ = '"';
				it = copy_to_iterator( typename JsonMember::to_converter_t{}( value ),
				                       it );
				*it++ = '"';
				return it;
			} else {
				return copy_to_iterator( typename JsonMember::to_converter_t{}( value ),
				                         it );
			}
		}

		template<typename JsonMember, typename OutputIterator, typename parse_to_t>
		constexpr OutputIterator to_string( ParseTag<JsonParseTypes::Array>,
		                                    OutputIterator it,
		                                    parse_to_t const &container ) {

			static_assert(
			  std::is_same_v<typename JsonMember::parse_to_t, parse_to_t> );

			*it++ = '[';
			if( not std::empty( container ) ) {
				auto count = std::size( container ) - 1;
				for( auto const &v : container ) {
					it = to_string<typename JsonMember::json_element_t>(
					  ParseTag<JsonMember::json_element_t::expected_type>{}, it, v );
					if( count-- > 0 ) {
						*it++ = ',';
					}
				}
			}
			*it++ = ']';
			return it;
		}

		template<typename JsonMember, typename OutputIterator, typename T>
		constexpr OutputIterator member_to_string( OutputIterator &&it,
		                                           T &&value ) {
			it = to_string<JsonMember>( ParseTag<JsonMember::expected_type>{},
			                            std::forward<OutputIterator>( it ),
			                            std::forward<T>( value ) );
			return std::move( it );
		}

		template<typename JsonMember, size_t pos, typename OutputIterator,
		         typename... Args>
		void to_json_str( OutputIterator it, std::tuple<Args...> &&args ) {

			static_assert( is_a_json_type_v<JsonMember>, "Unsupported data type" );
			*it++ = '"';
			it = copy_to_iterator( JsonMember::name, it );
			it = copy_to_iterator( "\":", it );
			it = member_to_string<JsonMember>( std::move( it ),
			                                   std::get<pos>( std::move( args ) ) );
			if constexpr( pos < ( sizeof...( Args ) - 1U ) ) {
				*it++ = ',';
			}
		}

		constexpr char to_lower( char c ) noexcept {
			return static_cast<char>( static_cast<unsigned>( c ) |
			                          static_cast<unsigned>( ' ' ) );
		}

		template<typename Result, bool RangeCheck = false, typename First,
		         typename Last>
		constexpr auto
		parse_unsigned_integer2( IteratorRange<First, Last> &rng ) noexcept {
			json_assert( rng.front( "0123456789" ),
			             "Expecting a digit as first item" );

			uintmax_t v = 0;
			uint_fast8_t c = 0;

			uint32_t dig =
			  static_cast<uint32_t>( rng.front( ) ) - static_cast<uint32_t>( '0' );
			while( dig < 10U ) {
				rng.remove_prefix( );
				++c;
				v *= 10U;
				v += dig;
				dig =
				  static_cast<uint32_t>( rng.front( ) ) - static_cast<uint32_t>( '0' );
			}
			struct result_t {
				Result value;
				uint_fast8_t count;
			};

			if constexpr( RangeCheck ) {
				return result_t{daw::narrow_cast<Result>( v ), c};
			} else {
				return result_t{static_cast<Result>( v ), c};
			}
		}

		template<typename Result, bool RangeCheck = false, typename First,
		         typename Last>
		constexpr Result
		parse_unsigned_integer( IteratorRange<First, Last> &rng ) noexcept {
			json_assert( rng.front( "0123456789" ) );

			uintmax_t result = 0;
			uintmax_t dig =
			  static_cast<uintmax_t>( rng.front( ) ) - static_cast<uintmax_t>( '0' );
			while( dig < 10U ) {
				rng.remove_prefix( );
				result *= 10U;
				result += dig;
				dig = static_cast<uintmax_t>( rng.front( ) ) -
				      static_cast<uintmax_t>( '0' );
			}
			if constexpr( RangeCheck ) {
				return daw::narrow_cast<Result>( result );
			} else {
				return static_cast<Result>( result );
			}
		}

		// For testing
		template<typename Result>
		constexpr auto
		parse_unsigned_integer( daw::string_view const &sv ) noexcept {
			auto rng =
			  IteratorRange( std::data( sv ), std::data( sv ) + std::size( sv ) );
			return parse_unsigned_integer<Result, true>( rng );
		}

		// For testing
		template<typename Result>
		constexpr auto
		parse_unsigned_integer2( daw::string_view const &sv ) noexcept {
			auto rng =
			  IteratorRange( std::data( sv ), std::data( sv ) + std::size( sv ) );
			return parse_unsigned_integer2<Result, true>( rng );
		}

		template<typename Result, bool RangeCheck = false, typename First,
		         typename Last>
		constexpr Result parse_integer( IteratorRange<First, Last> &rng ) noexcept {
			json_assert( rng.front( "+-0123456789" ) );
			int sign = 1;
			// This gets rid of warnings when parse_integer is called on unsigned
			// types
			if constexpr( std::is_signed_v<Result> ) {
				if( rng.in( '-' ) ) {
					sign = -1;
					rng.remove_prefix( );
				} else if( rng.in( '+' ) ) {
					rng.remove_prefix( );
				}
			}
			// Assumes there are digits
			if constexpr( RangeCheck ) {
				return daw::narrow_cast<Result>(
				  sign * parse_unsigned_integer<intmax_t, false>( rng ) );
			} else {
				return sign * parse_unsigned_integer<Result, RangeCheck>( rng );
			}
		}

		// For testing
		template<typename Result>
		constexpr Result parse_integer( daw::string_view const &sv ) noexcept {
			auto rng =
			  IteratorRange( std::data( sv ), std::data( sv ) + std::size( sv ) );
			return parse_integer<Result, true>( rng );
		}

		template<typename Result, typename First, typename Last>
		constexpr Result parse_real( IteratorRange<First, Last> &rng ) noexcept {
			// [-]WHOLE[.FRACTION][(e|E)[+|-]EXPONENT]

			json_assert( rng.is_real_number_part( ) );
			int_fast8_t sign = 1;
			if( rng.in( '-' ) ) {
				sign = -1;
				rng.remove_prefix( );
			}
			auto const whole_part =
			  static_cast<Result>( sign * parse_unsigned_integer<int64_t>( rng ) );

			Result fract_part = 0.0;
			if( rng.in( '.' ) ) {
				rng.remove_prefix( );
				auto fract_tmp = parse_unsigned_integer2<uint64_t>( rng );
				fract_part = static_cast<Result>( fract_tmp.value );
				fract_part *= static_cast<Result>(
				  daw::cxmath::dpow10( -static_cast<int32_t>( fract_tmp.count ) ) );
				fract_part = daw::cxmath::copy_sign( fract_part, whole_part );
			}

			int32_t exp_part = 0;
			if( rng.in( "eE" ) ) {
				rng.remove_prefix( );
				exp_part = parse_integer<int32_t>( rng );
			}
			if constexpr( std::is_same_v<Result, float> ) {
				return ( whole_part + fract_part ) * daw::cxmath::fpow10( exp_part );

			} else {
				return ( whole_part + fract_part ) *
				       static_cast<Result>( daw::cxmath::dpow10( exp_part ) );
			}
		}

		template<typename Result>
		constexpr Result parse_real( daw::string_view const &sv ) noexcept {
			auto rng =
			  IteratorRange( std::data( sv ), std::data( sv ) + std::size( sv ) );
			return parse_real<Result>( rng );
		}

		template<typename T>
		inline constexpr bool has_json_parser_description_v =
		  daw::is_detected_v<json_parser_description_t, T>;

		template<typename T>
		using json_parser_to_json_data_t =
		  decltype( to_json_data( std::declval<T &>( ) ) );

		template<typename T>
		inline constexpr bool has_json_to_json_data_v =
		  daw::is_detected_v<json_parser_to_json_data_t, T>;

		template<typename string_t>
		struct kv_t {
			string_t name;
			JsonParseTypes expected_type;
			// bool nullable;
			size_t pos;

			constexpr kv_t( string_t Name, JsonParseTypes Expected, /*bool Nullable,*/
			                size_t Pos )
			  : name( std::move( Name ) )
			  , expected_type( Expected )
			  //				  , nullable( Nullable )
			  , pos( Pos ) {}
		};

		template<typename JsonType>
		using json_parse_to = typename JsonType::parse_to_t;

		template<typename JsonType>
		inline constexpr bool is_json_nullable_v =
		  JsonType::expected_type == JsonParseTypes::Null;

		template<typename JsonType>
		inline constexpr bool is_json_empty_null_v = JsonType::empty_is_null;

		struct member_name_parse_error {};

		// Get the next member name
		// Assumes that the current item in stream is a double quote
		// Ensures that the stream is left at the position of the associated
		// value(e.g after the colon(:) and trimmed)
		template<typename First, typename Last>
		constexpr daw::string_view parse_name( IteratorRange<First, Last> &rng ) {
			auto tmp = skip_string( rng );
			auto name = daw::string_view( tmp.begin( ), tmp.size( ) );

			// All names are followed by a semi-colon
			rng.munch( ':' );
			rng.trim_left( );

			json_assert( not name.empty( ) and not rng.empty( ),
			             "Expected a non empty name and data after name" );
			return name;
		}

		template<typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_string( IteratorRange<First, Last> &rng ) {
			// Not always starting with string json_assert( rng.front( '"' ),
			// "Expected \" at the start of a string" );
			if( rng.front( '"' ) ) {
				rng.remove_prefix( );
			}
			auto result = rng;
			if( rng.in( '"' ) ) {
				// DAW
				// result.last = result.first;
				result.last = rng.first;
				return result;
			}
			auto p = rng.begin( );
			while( *p != '\0' ) {
				if( *p == '\\' ) {
					++p;
					json_assert( *p != '\0', "Unexpected end of stream" );
					++p;
					continue;
				}
				if( *p == '"' ) {
					break;
				}
				++p;
			}
			rng.first = p;
			json_assert( rng.front( ) == '"',
			             "Expected trailing \" at the end of string" );
			result.last = p;
			rng.remove_prefix( );
			return result;
		}

		template<typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_name( IteratorRange<First, Last> &rng ) {
			auto result = skip_string( rng );
			return result;
		}

		template<typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_literal( IteratorRange<First, Last> &rng ) {
			auto result = rng.move_to_first_of( ",}]" );
			json_assert( rng.front( ",}]" ),
			             "Expected a ',', '}', ']' to trail literal" );
			return result;
		}

		template<char Left, char Right, typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_bracketed_item( IteratorRange<First, Last> &rng ) {
			json_assert( rng.front( Left ), "Expected start bracket/brace" );
			size_t bracket_count = 1;
			bool in_quotes = false;
			auto result = rng;
			while( not rng.empty( ) and bracket_count > 0 ) {
				rng.remove_prefix( );
				rng.trim_left( );
				switch( rng.front( ) ) {
				case '\\':
					rng.remove_prefix( 1 );
					continue;
				case '"':
					in_quotes = not in_quotes;
					continue;
				case Left:
					if( not in_quotes ) {
						++bracket_count;
					}
					break;
				case Right:
					if( not in_quotes ) {
						--bracket_count;
					}
					break;
				}
			}
			json_assert( rng.front( Right ), "Expected closing bracket/brace" );

			rng.remove_prefix( );
			json_assert( not rng.empty( ), "Unexpected empty range" );

			result.last = rng.begin( );
			return result;
		}

		template<typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_class( IteratorRange<First, Last> &rng ) {
			return skip_bracketed_item<'{', '}'>( rng );
		}

		template<typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_array( IteratorRange<First, Last> &rng ) {
			return skip_bracketed_item<'[', ']'>( rng );
		}

		template<typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_value( IteratorRange<First, Last> &rng ) {
			json_assert( not rng.empty( ), "Expected value, not empty range" );

			switch( rng.front( ) ) {
			case '"':
				return skip_string( rng );
			case '[':
				return skip_array( rng );
			case '{':
				return skip_class( rng );
			default:
				return skip_literal( rng );
			}
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr IteratorRange<First, Last>
		skip_known_value( IteratorRange<First, Last> &rng ) {
			if constexpr( JsonMember::expected_type == JsonParseTypes::Date or
			              JsonMember::expected_type == JsonParseTypes::String or
			              JsonMember::expected_type == JsonParseTypes::Custom ) {
				json_assert( rng.front( '"' ),
				             "Expected start of value to begin with '\"'" );
				return impl::skip_string( rng );
			} else if constexpr( JsonMember::expected_type ==
			                       JsonParseTypes::Number or
			                     JsonMember::expected_type == JsonParseTypes::Null or
			                     JsonMember::expected_type == JsonParseTypes::Bool ) {
				return impl::skip_literal( rng );
			} else if constexpr( JsonMember::expected_type ==
			                     JsonParseTypes::Array ) {
				json_assert( rng.front( '[' ), "Expected start of array with '['" );
				return impl::skip_array( rng );
			} else if constexpr( JsonMember::expected_type ==
			                     JsonParseTypes::Class ) {
				json_assert( rng.front( '{' ), "Expected start of class with '{'" );
				return impl::skip_class( rng );
			} else {
				// Woah there
				std::terminate( );
			}
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr void skip_quotes( IteratorRange<First, Last> &rng ) noexcept {
			if constexpr( JsonMember::literal_as_string ==
			              LiteralAsStringOpt::always ) {
				json_assert( rng.front( '"' ),
				             "Expected start of quoted item with \"" );
				rng.remove_prefix( );
			} else if constexpr( JsonMember::literal_as_string ==
			                     LiteralAsStringOpt::maybe ) {
				if( rng.in( '"' ) ) {
					rng.remove_prefix( );
				}
			}
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Number>,
		                            IteratorRange<First, Last> &rng ) {
			using constructor_t = typename JsonMember::constructor_t;
			using element_t = typename JsonMember::parse_to_t;

			skip_quotes<JsonMember>( rng );
			json_assert( rng.is_real_number_part( ),
			             "Expected number to start with on of \"0123456789eE+-\"" );

			if constexpr( std::is_floating_point_v<element_t> ) {
				auto result = constructor_t{}( parse_real<element_t>( rng ) );
				skip_quotes<JsonMember>( rng );
				json_assert( rng.at_end_of_item( ),
				             "Expected whitespace or one of \",}]\" at end of number" );
				return result;
			} else if constexpr( std::is_signed_v<element_t> ) {
				auto result = constructor_t{}(
				  parse_integer<element_t, JsonMember::range_check>( rng ) );
				skip_quotes<JsonMember>( rng );
				json_assert( rng.at_end_of_item( ),
				             "Expected whitespace or one of \",}]\" at end of number" );
				return result;
			} else {
				auto result = constructor_t{}(
				  parse_unsigned_integer<element_t, JsonMember::range_check>( rng ) );
				skip_quotes<JsonMember>( rng );
				json_assert( rng.at_end_of_item( ),
				             "Expected whitespace or one of \",}]\" at end of number" );
				return result;
			}
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Null>,
		                            IteratorRange<First, Last> &rng ) {
			using constructor_t = typename JsonMember::constructor_t;
			using element_t = typename JsonMember::sub_type;
			size_t const null_size = 4;
			if( rng.empty( ) or rng.is_null( ) ) {
				return constructor_t{}( );
			}
			if( rng.size( ) >= null_size and rng.in( 'n' ) ) {
				rng.remove_prefix( null_size );
				rng.trim_left( );
				return constructor_t{}( );
			}

			return parse_value<element_t>( ParseTag<element_t::expected_type>{},
			                               rng );
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Bool>,
		                            IteratorRange<First, Last> &rng ) {
			json_assert( not rng.empty( ) and rng.size( ) >= 4,
			             "Range to small to be a bool" );

			using constructor_t = typename JsonMember::constructor_t;

			if( rng.in( 't' ) and rng.size( ) > 4 ) {
				rng.remove_prefix( 4 );
				rng.trim_left( );
				return constructor_t{}( true );
			}
			rng.remove_prefix( 5 );
			rng.trim_left( );
			return constructor_t{}( false );
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::String>,
		                            IteratorRange<First, Last> &rng ) {

			auto str = skip_string( rng );
			using constructor_t = typename JsonMember::constructor_t;
			return constructor_t{}( str.begin( ), str.size( ) );
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Date>,
		                            IteratorRange<First, Last> &rng ) {

			auto str = skip_string( rng );
			using constructor_t = typename JsonMember::constructor_t;
			return constructor_t{}( str.begin( ), str.size( ) );
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Custom>,
		                            IteratorRange<First, Last> &rng ) {

			json_assert( rng.front( '"' ),
			             "Custom types requite a string at the beginning" );
			auto str = skip_string( rng );
			json_assert( str.front( '"' ),
			             "Custom types requite a string at the end" );
			// TODO make custom require a ptr/sz pair
			using constructor_t = typename JsonMember::from_converter_t;
			return constructor_t{}( std::string_view( str.begin( ), str.size( ) ) );
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Class>,
		                            IteratorRange<First, Last> &rng ) {

			using element_t = typename JsonMember::parse_to_t;
			return from_json<element_t>( rng );
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Array>,
		                            IteratorRange<First, Last> & );

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::KeyValue>,
		                            IteratorRange<First, Last> &rng ) {

			json_assert(
			  rng.front( '{' ),
			  "Expected keyvalue type to be of class type and beging with '{'" );

			rng.remove_prefix( );
			rng.trim_left( );

			auto array_container = typename JsonMember::constructor_t{}( );
			auto container_appender =
			  typename JsonMember::appender_t( array_container );

			using key_t = typename JsonMember::json_key_t;
			using value_t = typename JsonMember::json_element_t;
			while( not rng.in( "}" ) ) {
				auto key = parse_name( rng );
				rng.trim_left( );
				container_appender(
				  typename key_t::constructor_t{}( key.data( ), key.size( ) ),
				  parse_value<value_t>( ParseTag<value_t::expected_type>{}, rng ) );

				rng.clean_tail( );
			}
			json_assert( rng.front( '}' ),
			             "Expected keyvalue type to end with a '}'" );
			rng.remove_prefix( );
			rng.trim_left( );
			return array_container;
		}

		template<typename JsonMember, typename First, typename Last>
		constexpr auto parse_value( ParseTag<JsonParseTypes::Array>,
		                            IteratorRange<First, Last> &rng ) {

			using element_t = typename JsonMember::json_element_t;
			json_assert( rng.front( '[' ), "Expected array to start with a '['" );

			rng.remove_prefix( );
			rng.trim_left( );

			auto array_container = typename JsonMember::constructor_t{}( );
			auto container_appender =
			  typename JsonMember::appender_t( array_container );

			while( not rng.empty( ) and not rng.in( "]" ) ) {
				container_appender(
				  parse_value<element_t>( ParseTag<element_t::expected_type>{}, rng ) );
				rng.clean_tail( );
			}
			json_assert( rng.front( ']' ), "Expected array to end with a ']'" );
			rng.remove_prefix( );
			rng.trim_left( );
			return array_container;
		}

		template<typename Container>
		struct basic_appender {
			daw::back_inserter_iterator<Container> appender;

			constexpr basic_appender( Container &container ) noexcept
			  : appender( container ) {}

			template<typename Value>
			constexpr void operator( )( Value &&value ) {
				*appender = std::forward<Value>( value );
			}
		};

		template<typename Container>
		struct basic_kv_appender {
			daw::inserter_iterator<Container> appender;

			constexpr basic_kv_appender( Container &container ) noexcept
			  : appender( container ) {}

			template<typename Key, typename Value>
			constexpr void operator( )( Key &&key, Value &&value ) {
				*appender = std::make_pair( std::forward<Key>( key ),
				                            std::forward<Value>( value ) );
			}
		};

		template<size_t N, typename string_t, typename... JsonMembers>
		constexpr kv_t<string_t> get_item( ) noexcept {
			using type_t = traits::nth_type<N, JsonMembers...>;
			return {type_t::name, type_t::expected_type, /*type_t::nullable,*/ N};
		}

		template<typename... JsonMembers>
		constexpr size_t find_string_capacity( ) noexcept {
			return ( json_name_len( JsonMembers::name ) + ... );
		}

		template<typename... JsonMembers, size_t... Is>
		constexpr auto make_map( std::index_sequence<Is...> ) noexcept {
			using string_t =
			  basic_bounded_string<char, find_string_capacity<JsonMembers...>( )>;

			return daw::make_array( get_item<Is, string_t, JsonMembers...>( )... );
		}

		template<typename... JsonMembers>
		struct name_map_t {
			static constexpr auto name_map =
			  make_map<JsonMembers...>( std::index_sequence_for<JsonMembers...>{} );

			static constexpr bool has_name( daw::string_view key ) noexcept {
				using std::begin;
				using std::end;
				auto result = algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				return result != std::end( name_map );
			}

			static constexpr size_t find_name( daw::string_view key ) noexcept {
				using std::begin;
				using std::end;
				auto result = algorithm::find_if(
				  begin( name_map ), end( name_map ),
				  [key]( auto const &kv ) { return kv.name == key; } );
				if( result == std::end( name_map ) ) {
					std::terminate( );
				}
				return static_cast<size_t>(
				  std::distance( begin( name_map ), result ) );
			}
		};

		struct location_info_t {
			JSONNAMETYPE name;
			IteratorRange<char const *, char const *> location{};

			constexpr bool missing( ) const {
				return location.empty( ) or location.is_null( );
			}
		};

		template<size_t JsonMemberPosition, typename... JsonMembers, typename First,
		         typename Last>
		constexpr decltype( auto )
		parse_item( std::array<IteratorRange<First, Last>,
		                       sizeof...( JsonMembers )> const &locations ) {

			using JsonMember = traits::nth_type<JsonMemberPosition, JsonMembers...>;
			return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
			                                locations[JsonMemberPosition] );
		}

		template<size_t pos, typename... JsonMembers, typename First, typename Last>
		constexpr IteratorRange<First, Last> find_location(
		  std::array<location_info_t, sizeof...( JsonMembers )> &locations,
		  IteratorRange<First, Last> &rng ) {

			json_assert(
			  is_json_nullable_v<daw::traits::nth_element<pos, JsonMembers...>> or
			    not locations[pos].missing( ) or not rng.front( '}' ),
			  "Unexpected end of class.  Non-nullable members still not found" );

			rng.trim_left( );
			while( locations[pos].missing( ) and not rng.in( '}' ) ) {
				auto name = parse_name( rng );

				if( not name_map_t<JsonMembers...>::has_name( name ) ) {
					// This is not a member we are concerned with
					skip_value( rng );
					rng.clean_tail( );
					continue;
				}
				auto const name_pos = name_map_t<JsonMembers...>::find_name( name );
				if( name_pos != pos ) {
					// We are out of order, store position for later
					// TODO:	use type knowledge to speed up skip
					// TODO:	on skipped classes see if way to store
					// 				member positions so that we don't have to
					//				reparse them after
					locations[name_pos].location = skip_value( rng );
					rng.clean_tail( );
					continue;
				}
				locations[pos].location = rng;
			}
			return locations[pos].location;
		}

		template<size_t JsonMemberPosition, typename... JsonMembers, typename First,
		         typename Last>
		constexpr decltype( auto ) parse_item(
		  std::array<location_info_t, sizeof...( JsonMembers )> &locations,
		  IteratorRange<First, Last> &rng ) {

			using JsonMember = traits::nth_type<JsonMemberPosition, JsonMembers...>;

			// If we are an array element
			if constexpr( json_name_eq( JsonMember::name, no_name ) ) {
				auto result =
				  parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{}, rng );
				rng.clean_tail( );
				return result;
			} else {
				auto loc =
				  find_location<JsonMemberPosition, JsonMembers...>( locations, rng );

				constexpr auto const name = JsonMember::name;
				::Unused( name );
				// Only allow missing members for Null-able type
				json_assert( JsonMember::expected_type == JsonParseTypes::Null or
				               not loc.empty( ),
				             "Could not find required class member" );

				if( loc.is_null( ) or
				    ( not rng.is_null( ) and rng.begin( ) != loc.begin( ) ) ) {

					return parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{},
					                                loc );
				} else {
					auto result = parse_value<JsonMember>(
					  ParseTag<JsonMember::expected_type>{}, rng );
					rng.clean_tail( );
					return result;
				}
			}
		}

		template<size_t N, typename... JsonMembers>
		using nth = daw::traits::nth_element<N, JsonMembers...>;

		template<typename... JsonMembers, typename OutputIterator, size_t... Is,
		         typename... Args>
		constexpr OutputIterator
		serialize_json_class( OutputIterator it, std::index_sequence<Is...>,
		                      std::tuple<Args...> &&args ) {

			*it++ = '{';

			(void)( ( to_json_str<nth<Is, JsonMembers...>, Is>( it,
			                                                    std::move( args ) ),
			          ... ),
			        0 );

			*it++ = '}';
			return it;
		}

		template<typename Result, typename... JsonMembers, size_t... Is,
		         typename First, typename Last>
		constexpr Result parse_json_class( IteratorRange<First, Last> &rng,
		                                   std::index_sequence<Is...> ) {
			static_assert(
			  can_construct_a_v<Result, typename JsonMembers::parse_to_t...>,
			  "Supplied types cannot be used for construction of this type" );

			rng.trim_left( );
			json_assert( rng.front( '{' ), "Expected class to begin with '{'" );
			rng.remove_prefix( );
			rng.trim_left( );
			if constexpr( sizeof...( JsonMembers ) == 0 ) {
				return construct_a<Result>( );
				json_assert( rng.front( '}' ), "Expected class to end with '}'" );
				rng.remove_prefix( );
				rng.trim_left( );
			} else {
				constexpr auto cknown_locations =
				  daw::make_array( location_info_t{JsonMembers::name}... );
				auto known_locations = cknown_locations;

				auto result = daw::construct_a<Result>(
				  parse_item<Is, JsonMembers...>( known_locations, rng )... );
				rng.trim_left( );
				// If we fullfill the contract before all values are found
				while( not rng.in( '}' ) ) {
					parse_name( rng );
					skip_value( rng );
					rng.clean_tail( );
				}

				json_assert( rng.front( '}' ), "Expected class to end with '}'" );
				rng.remove_prefix( );
				rng.trim_left( );
				return result;
			}
		}

		template<typename Result, typename... JsonMembers, size_t... Is>
		constexpr Result parse_json_class( std::string_view sv,
		                                   std::index_sequence<Is...> is ) {

			auto rng =
			  IteratorRange( std::data( sv ), std::data( sv ) + std::size( sv ) );
			return parse_json_class<Result, JsonMembers...>( rng, is );
		}

		constexpr bool json_path_compare( daw::string_view json_path_item,
		                                  daw::string_view member_name ) noexcept {
			if( json_path_item.front( ) == '\\' ) {
				json_path_item.remove_prefix( );
			}
			while( not json_path_item.empty( ) and not member_name.empty( ) ) {
				if( json_path_item.front( ) != member_name.front( ) ) {
					return false;
				}
				json_path_item.remove_prefix( );
				if( not json_path_item.empty( ) and json_path_item.front( ) == '\\' ) {
					json_path_item.remove_prefix( );
				}
				member_name.remove_prefix( );
			}
			return json_path_item.size( ) == member_name.size( );
		}

		template<typename First, typename Last>
		constexpr void find_range2( IteratorRange<First, Last> &rng,
		                            daw::string_view path ) {
			auto current = impl::pop_json_path( path );
			while( not current.empty( ) ) {
				json_assert( rng.front( '{' ), "Invalid Path Entry" );
				rng.remove_prefix( );
				rng.trim_left( );
				auto name = parse_name( rng );
				while( not json_path_compare( current, name ) ) {
					skip_value( rng );
					rng.clean_tail( );
					name = parse_name( rng );
				}
				current = impl::pop_json_path( path );
			}
		}

		template<typename String>
		constexpr auto find_range( String &&str, daw::string_view start_path ) {

			using std::data;
			using std::size;
			auto rng = IteratorRange( data( str ), data( str ) + size( str ) );
			if( not start_path.empty( ) ) {
				find_range2( rng, start_path );
			}
			return rng;
		}
	} // namespace impl
} // namespace daw::json
