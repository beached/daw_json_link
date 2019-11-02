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

#include <cstddef>
#ifdef __cpp_nontype_template_parameter_class
#include <daw/daw_bounded_string.h>
#endif

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_literal_end.h"
#include "daw_json_parse_string_quote.h"

namespace daw::json::impl {
	template<typename T>
	using json_parser_description_t = daw::remove_cvref_t<decltype(
	  describe_json_class( std::declval<T &>( ) ) )>;

	template<typename JsonMember>
	using json_result = typename JsonMember::parse_to_t;

	template<size_t I, typename... JsonMembers>
	using json_result_n =
	  json_result<daw::traits::nth_element<I, JsonMembers...>>;

	template<typename Container>
	struct basic_appender {
		daw::back_inserter<Container> appender;

		constexpr basic_appender( Container &container ) noexcept
		  : appender( container ) {}

		template<typename Value>
		constexpr void operator( )( Value &&value ) {
			*appender = std::forward<Value>( value );
		}
	};

	template<typename T>
	static inline constexpr bool has_json_parser_description_v =
	  daw::is_detected_v<json_parser_description_t, T>;

	template<typename T>
	using json_parser_to_json_data_t =
	  decltype( to_json_data( std::declval<T &>( ) ) );

	template<typename T>
	static inline constexpr bool has_json_to_json_data_v =
	  daw::is_detected_v<json_parser_to_json_data_t, T>;
} // namespace daw::json::impl

namespace daw::json {
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

	template<typename T, typename First, typename Last, bool TrustedInput>
	[[maybe_unused, nodiscard]] static constexpr T
	from_json( daw::json::impl::IteratorRange<First, Last, TrustedInput> &rng ) {
		static_assert( impl::has_json_parser_description_v<T>,
		               "A function call describe_json_class must exist for type." );
		json_assert( not rng.empty( ), "Attempt to parse empty string" );

		/*
		return impl::parse_value( ParseTag<JsonParseTypes::Class>{}, rng );
		 */
		T result = impl::json_parser_description_t<T>::template parse<T>( rng );
		rng.trim_left( );
		return result;
	}

	enum class JsonParseTypes : uint_fast8_t {
		Real,
		Signed,
		Unsigned,
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

	enum class LiteralAsStringOpt : uint8_t { never, maybe, always };

	template<typename T>
	inline constexpr bool is_range_constructable_v =
	  not std::is_trivially_constructible_v<T>;
} // namespace daw::json

namespace daw::json::impl {
	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_string_nq( IteratorRange<First, Last, TrustedInput> &rng ) {
		// Not always starting with string json_assert( rng.front( '"' ),
		// "Expected \" at the start of a string" );
		auto result = rng;
		rng.first = string_quote::string_quote_parser::parse_nq( rng.first );

		if constexpr( not TrustedInput ) {
			json_assert( rng.front( ) == '"',
			             "Expected trailing \" at the end of string" );
		}
		result.last = rng.first;
		rng.remove_prefix( );
		return result;
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_string( IteratorRange<First, Last, TrustedInput> &rng ) {
		// Not always starting with string json_assert( rng.front( '"' ),
		// "Expected \" at the start of a string" );
		if( rng.front( '"' ) ) { // TODO
			rng.remove_prefix( );
		}
		return skip_string_nq( rng );
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_name( IteratorRange<First, Last, TrustedInput> &rng ) {
		auto result = skip_string( rng );
		return result;
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_literal( IteratorRange<First, Last, TrustedInput> &rng ) {
		auto result = rng;
		result.last = literal_end::literal_end_parser::parse( rng.first );
		rng.first = result.last;
		if constexpr( not TrustedInput ) {
			json_assert( rng.front( ",}]" ),
			             "Expected a ',', '}', ']' to trail literal" );
		}
		return result;
	}

	template<char Left, char Right, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_bracketed_item( IteratorRange<First, Last, TrustedInput> &rng ) {
		if constexpr( not TrustedInput ) {
			json_assert( rng.front( Left ), "Expected start bracket/brace" );
		}
		size_t bracket_count = 1;
		bool in_quotes = false;
		auto result = rng;
		while( not rng.empty( ) and bracket_count > 0 ) {
			rng.remove_prefix( );
			rng.trim_left_no_check( );
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
		if constexpr( not TrustedInput ) {
			json_assert( rng.front( Right ), "Expected closing bracket/brace" );
		}

		rng.remove_prefix( );
		if constexpr( not TrustedInput ) {
			json_assert( not rng.empty( ), "Unexpected empty range" );
		}

		result.last = rng.begin( );
		return result;
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_class( IteratorRange<First, Last, TrustedInput> &rng ) {
		return skip_bracketed_item<'{', '}'>( rng );
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_array( IteratorRange<First, Last, TrustedInput> &rng ) {
		return skip_bracketed_item<'[', ']'>( rng );
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_value( IteratorRange<First, Last, TrustedInput> &rng ) {
		if constexpr( not TrustedInput ) {
			json_assert( not rng.empty( ), "Expected value, not empty range" );
		}

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

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_known_value( IteratorRange<First, Last, TrustedInput> &rng ) {
		if constexpr( JsonMember::expected_type == JsonParseTypes::Date or
		              JsonMember::expected_type == JsonParseTypes::String or
		              JsonMember::expected_type == JsonParseTypes::Custom ) {
			if constexpr( not TrustedInput ) {
				json_assert( rng.front( '"' ),
				             "Expected start of value to begin with '\"'" );
			}
			rng.remove_prefix( );
			return impl::skip_string_nq( rng );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Real or
		                     JsonMember::expected_type == JsonParseTypes::Signed or
		                     JsonMember::expected_type ==
		                       JsonParseTypes::Unsigned or
		                     JsonMember::expected_type == JsonParseTypes::Null or
		                     JsonMember::expected_type == JsonParseTypes::Bool ) {
			return impl::skip_literal( rng );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Array ) {
			if constexpr( not TrustedInput ) {
				json_assert( rng.front( '[' ), "Expected start of array with '['" );
			}
			return impl::skip_array( rng );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Class ) {
			if constexpr( not TrustedInput ) {
				json_assert( rng.front( '{' ), "Expected start of class with '{'" );
			}
			return impl::skip_class( rng );
		} else {
			// Woah there
			std::terminate( );
		}
	}

	template<typename JsonMember, typename First, typename Last,
	         bool TrustedInput>
	static constexpr void
	skip_quotes( IteratorRange<First, Last, TrustedInput> &rng ) noexcept {
		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::always ) {
			if constexpr( not TrustedInput ) {
				json_assert( rng.front( '"' ),
				             "Expected start of quoted item with \"" );
			}
			rng.remove_prefix( );
		} else if constexpr( JsonMember::literal_as_string ==
		                     LiteralAsStringOpt::maybe ) {
			if( rng.in( '"' ) ) {
				rng.remove_prefix( );
			}
		}
	}

} // namespace daw::json::impl
