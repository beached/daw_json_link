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

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include <daw/cpp_17.h>
#ifdef __cpp_nontype_template_parameter_class
#include <daw/daw_bounded_string.h>
#endif
#include <daw/daw_string_view.h>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"

namespace daw::json {
	namespace to_strings {
		namespace {
			using std::to_string;
		}

		template<typename T>
		[[nodiscard]] auto to_string( std::optional<T> const &v )
		  -> decltype( to_string( *v ) ) {
			if( not v ) {
				return {"null"};
			}
			return to_string( *v );
		}
	} // namespace to_strings

	template<typename T, typename First, typename Last>
	[[nodiscard, maybe_unused]] static constexpr T
	from_json( daw::json::impl::IteratorRange<First, Last> &rng );

	enum class LiteralAsStringOpt : uint8_t { never, maybe, always };

#ifdef __cpp_nontype_template_parameter_class
	// C++ 20 Non-Type Class Template Arguments
#define JSONNAMETYPE daw::bounded_string

	template<typename String>
	[[nodiscard]] static constexpr size_t
	json_name_len( String const &str ) noexcept {
		return str.size( );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] static constexpr bool json_name_eq( Lhs const &lhs,
	                                                  Rhs const &rhs ) noexcept {
		return lhs == rhs;
	}
	// Convienience for array members
	static inline constexpr JSONNAMETYPE const no_name = "";
#else
#define JSONNAMETYPE char const *

	[[nodiscard]] static constexpr size_t
	json_name_len( char const *const str ) noexcept {
		return daw::string_view( str ).size( );
	}

	[[nodiscard]] static constexpr bool
	json_name_eq( char const *const lhs, char const *const rhs ) noexcept {
		return std::string_view( lhs ) == std::string_view( rhs );
	}

	// Convienience for array members
	static constexpr char const no_name[] = "";
#endif

	struct parse_js_date {
		[[maybe_unused,
		  nodiscard]] constexpr std::chrono::time_point<std::chrono::system_clock,
		                                                std::chrono::milliseconds>
		operator( )( char const *ptr, size_t sz ) const {
			return daw::date_parsing::parse_javascript_timestamp(
			  daw::string_view( ptr, sz ) );
		}
	};

	template<typename T>
	struct custom_to_converter_t {
		[[nodiscard]] constexpr decltype( auto ) operator( )( T &&value ) const {
			using std::to_string;
			return to_string( daw::move( value ) );
		}

		[[nodiscard]] constexpr decltype( auto )
		operator( )( T const &value ) const {
			using std::to_string;
			return to_string( value );
		}
	};

	template<typename T>
	struct custom_from_converter_t {
		[[nodiscard]] constexpr decltype( auto )
		operator( )( std::string_view sv ) {
			return from_string( daw::tag<T>, sv );
		}
	};

	enum class JsonParseTypes : uint_fast8_t {
		Real,
		Unsigned,
		Signed,
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
		struct location_info_t {

			using range_t = IteratorRange<char const *, char const *>;
			using variant_t = std::variant<std::monostate, uintmax_t, intmax_t,
			                               double, bool, range_t>;
			enum class parse_statuses : uint8_t { empty, found, preparsed };

		public:
			// Data Members
			JSONNAMETYPE name;
			JsonParseTypes expected_type;
			variant_t data{};
			parse_statuses parse_status = parse_statuses::empty;
			// **************

			template<JsonParseTypes JPT>
			constexpr decltype( auto ) as( ) {
				json_assert( expected_type == JPT and not empty( ),
				             "expected_type does not match requested" );
				if constexpr( JPT == JsonParseTypes::Bool ) {
					return std::get<bool>( data );
				} else if constexpr( JPT == JsonParseTypes::Real ) {
					return std::get<double>( data );
				} else if constexpr( JPT == JsonParseTypes::Signed ) {
					return std::get<intmax_t>( data );
				} else if constexpr( JPT == JsonParseTypes::Unsigned ) {
					return std::get<uintmax_t>( data );
				} else {
					return std::get<range_t>( data );
				}
			}

			template<JsonParseTypes JPT>
			constexpr decltype( auto ) as( ) const {
				json_assert( expected_type == JPT and not empty( ),
				             "expected_type does not match requested" );
				if constexpr( JPT == JsonParseTypes::Bool ) {
					return std::get<bool>( data );
				} else if constexpr( JPT == JsonParseTypes::Real ) {
					return std::get<double>( data );
				} else if constexpr( JPT == JsonParseTypes::Signed ) {
					return std::get<intmax_t>( data );
				} else if constexpr( JPT == JsonParseTypes::Unsigned ) {
					return std::get<uintmax_t>( data );
				} else {
					return std::get<range_t>( data );
				}
			}

			[[nodiscard]] constexpr range_t const &rng( ) const {
				json_assert( std::holds_alternative<range_t>( data ),
				             "Expected a range value" );
				return std::get<range_t>( data );
			}

			[[nodiscard]] constexpr range_t &rng( ) {
				json_assert( std::holds_alternative<range_t>( data ),
				             "Expected a range value" );
				return std::get<range_t>( data );
			}

			[[nodiscard]] constexpr bool can_be_null( ) const {
				return expected_type == JsonParseTypes::Null;
			}

			template<JsonParseTypes JPT>
			[[nodiscard]] constexpr bool contains( ) const {
				if( empty( ) or parse_status == parse_statuses::found ) {
					return false;
				}
				return expected_type == JPT;
			}

			[[nodiscard]] constexpr bool empty( ) const {
				return not( parse_status != parse_statuses::empty or
				            data.index( ) != 0 );
			}
		};

		template<typename JsonMember, typename... Args>
		using json_result_type_t = daw::remove_cvref_t<decltype(
		  std::declval<typename JsonMember::constructor_t>( )(
		    std::declval<Args>( )... ) )>;

		template<typename T>
		using json_parser_description_t = daw::remove_cvref_t<decltype(
		  describe_json_class( std::declval<T &>( ) ) )>;

		template<typename First, typename Last>
		constexpr void process_literal_as_string_impl(
		  std::integral_constant<LiteralAsStringOpt, LiteralAsStringOpt::never>,
		  IteratorRange<First, Last> &rng ) {}

		template<typename First, typename Last>
		constexpr void process_literal_as_string_impl(
		  std::integral_constant<LiteralAsStringOpt, LiteralAsStringOpt::always>,
		  IteratorRange<First, Last> &rng ) {
			rng.remove_prefix( );
		}

		template<typename First, typename Last>
		constexpr void process_literal_as_string_impl(
		  std::integral_constant<LiteralAsStringOpt, LiteralAsStringOpt::maybe>,
		  IteratorRange<First, Last> &rng ) {
			if( rng.front( ) == '"' ) {
				rng.remove_prefix( );
			}
		}
		template<LiteralAsStringOpt litasstr, typename First, typename Last>
		constexpr void
		process_literal_as_string( IteratorRange<First, Last> &rng ) {
			process_literal_as_string_impl(
			  std::integral_constant<LiteralAsStringOpt, litasstr>{}, rng );
		}
	} // namespace impl
} // namespace daw::json
