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

#include <daw/daw_traits.h>
#include <iterator>

#include "daw_iterator_range.h"
#include "daw_json_assert.h"
#include "daw_json_parse_literal_end.h"
#include "daw_json_parse_string_quote.h"

namespace daw::json::impl {
	template<typename Container, typename Value>
	using detect_push_back = decltype(
	  std::declval<Container &>( ).push_back( std::declval<Value>( ) ) );

	template<typename Container, typename Value>
	using detect_insert_end = decltype( std::declval<Container &>( ).insert(
	  std::end( std::declval<Container &>( ) ), std::declval<Value>( ) ) );

	template<typename Container, typename Value>
	inline constexpr bool has_push_back_v =
	  daw::is_detected_v<detect_push_back, Container, Value>;

	template<typename Container, typename Value>
	inline constexpr bool has_insert_end_v =
	  daw::is_detected_v<detect_insert_end, Container, Value>;

	template<typename T>
	using json_parser_description_t = daw::remove_cvref_t<decltype(
	  describe_json_class( std::declval<std::add_lvalue_reference_t<T>>( ) ) )>;

	template<typename T>
	static inline constexpr bool has_json_parser_description_v =
	  daw::is_detected_v<json_parser_description_t, T>;

	template<typename JsonMember>
	using json_result = typename JsonMember::parse_to_t;

	template<size_t I, typename... JsonMembers>
	using json_result_n =
	  json_result<daw::traits::nth_element<I, JsonMembers...>>;

	template<typename Container>
	struct basic_appender {
		Container *m_container;

		explicit constexpr basic_appender( Container &container ) noexcept
		  : m_container( &container ) {}

		template<typename Value>
		constexpr void operator( )( Value &&value ) {
			if constexpr( has_push_back_v<Container, daw::remove_cvref_t<Value>> ) {
				m_container->push_back( std::forward<Value>( value ) );
			} else if constexpr( has_insert_end_v<Container,
			                                      daw::remove_cvref_t<Value>> ) {
				m_container->insert( std::end( *m_container ),
				                     std::forward<Value>( value ) );
			} else {
				static_assert(
				  has_push_back_v<Container, daw::remove_cvref_t<Value>> or
				    has_insert_end_v<Container, daw::remove_cvref_t<Value>>,
				  "basic_appender requires a Container that either has push_back or "
				  "insert with the end iterator as first argument" );
			}
		}
	};

	template<typename T>
	using json_parser_to_json_data_t =
	  decltype( to_json_data( std::declval<T &>( ) ) );

	template<typename T>
	static inline constexpr bool has_json_to_json_data_v =
	  daw::is_detected_v<json_parser_to_json_data_t, T>;
} // namespace daw::json::impl

namespace daw::json {
#if defined( __cpp_nontype_template_parameter_class )
	// C++ 20 Non-Type Class Template Arguments

	/**
	 * A fixed string used for member names in json descriptions
	 * @tparam N size of string plus 1.  Do not set explicitly.  Use CTAD
	 */
	template<size_t N>
	struct json_name {
		static_assert( N > 0 );
		char const m_data[N]{};

	private:
		template<size_t... Is>
		constexpr json_name( char const ( &ptr )[N],
		                     std::index_sequence<Is...> ) noexcept
		  : m_data{ptr[Is]...} {}

	public:
		constexpr json_name( char const ( &ptr )[N] ) noexcept
		  : json_name( ptr, std::make_index_sequence<N>{} ) {}

		constexpr operator daw::string_view( ) const noexcept {
			return {m_data, N - 1};
		}

		// Needed for copy_to_iterator
		[[nodiscard]] constexpr char const *begin( ) const {
			return m_data;
		}

		// Needed for copy_to_iterator
		[[nodiscard]] constexpr char const *end( ) const {
			return m_data + static_cast<ptrdiff_t>( size( ) );
		}

		[[nodiscard]] constexpr size_t size( ) const noexcept {
			return N - 1;
		}

		template<size_t M>
		constexpr bool operator==( json_name<M> const &rhs ) const noexcept {
			if( N != M ) {
				return false;
			}
			for( size_t n = 0; n < N; ++n ) {
				if( m_data[n] != rhs.m_data[n] ) {
					return false;
				}
			}
			return true;
		}

		constexpr bool operator==( daw::string_view sv ) const noexcept {
			return daw::string_view( m_data, N - 1 ) == sv;
		}
	};

	template<typename... Chars>
	json_name( Chars... )->json_name<sizeof...( Chars )>;

#define JSONNAMETYPE daw::json::json_name

	// Convienience for array members that are required to be unnamed
	inline constexpr JSONNAMETYPE no_name{""};
	namespace impl {
		inline constexpr JSONNAMETYPE default_key_name{"key"};
		inline constexpr JSONNAMETYPE default_value_name{"value"};
	} // namespace impl

	template<JSONNAMETYPE n>
	inline constexpr bool is_no_name = ( n == no_name );
#else
#define JSONNAMETYPE char const *
	// Convienience for array members that are required to be unnamed
	inline constexpr char const no_name[] = "";
	namespace impl {
		inline constexpr char const default_key_name[] = "key";
		inline constexpr char const default_value_name[] = "value";
	} // namespace impl
	template<JSONNAMETYPE n>
	inline constexpr bool
	  is_no_name = daw::string_view( n ) == daw::string_view( "" );
#endif

	template<typename T, typename First, typename Last, bool TrustedInput>
	[[maybe_unused, nodiscard]] static constexpr T
	from_json( daw::json::impl::IteratorRange<First, Last, TrustedInput> &rng ) {
		static_assert( impl::has_json_parser_description_v<T>,
		               "A function call describe_json_class must exist for type." );
		daw_json_assert_untrusted( rng.has_more( ),
		                           "Attempt to parse empty string" );

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
		StringEscaped,
		Date,
		Class,
		Array,
		Null,
		KeyValue,
		KeyValueArray,
		Custom
	};

	namespace impl {
		template<typename T>
		inline constexpr JsonParseTypes number_parse_type_impl_v =
		  std::is_signed_v<T> ? JsonParseTypes::Signed : JsonParseTypes::Unsigned;

		template<typename T>
		constexpr auto number_parse_type_test( )
		  -> std::enable_if_t<std::is_enum_v<T>, JsonParseTypes> {

			return number_parse_type_impl_v<std::underlying_type_t<T>>;
		}
		template<typename T>
		constexpr auto number_parse_type_test( )
		  -> std::enable_if_t<not std::is_enum_v<T>, JsonParseTypes> {

			return number_parse_type_impl_v<T>;
		}
		template<typename T>
		inline constexpr JsonParseTypes
		  number_parse_type_v = number_parse_type_test<T>( );
	} // namespace impl

	/**
	 * Tag lookup for parsing overload selection
	 */
	template<JsonParseTypes v>
	using ParseTag = std::integral_constant<JsonParseTypes, v>;

	/**
	 * Allows having literals parse that are encoded as strings. It allows
	 * one to have it be never true, maybe true or always true.  This controls
	 * whether the parser will never remove quotes, check if quotes exist, or
	 * always remove quotes around the literal
	 */
	enum class LiteralAsStringOpt : uint8_t { never, maybe, always };

	template<typename T>
	inline constexpr bool is_range_constructable_v =
	  not std::is_trivially_constructible_v<T>;
} // namespace daw::json

namespace daw::json::impl {
	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_string_nq( IteratorRange<First, Last, TrustedInput> &rng ) {
		auto result = rng;
		rng.first = string_quote::string_quote_parser::parse_nq( rng.first );

		daw_json_assert_untrusted( rng.front( ) == '"',
		                           "Expected trailing \" at the end of string" );
		result.last = rng.first;
		rng.remove_prefix( );
		return result;
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_string( IteratorRange<First, Last, TrustedInput> &rng ) {
		if( rng.front( '"' ) ) {
			rng.remove_prefix( );
		}
		return skip_string_nq( rng );
	}

	template<typename First, typename Last, bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_literal( IteratorRange<First, Last, TrustedInput> &rng ) {
		auto result = rng;
		result.last = literal_end::literal_end_parser::parse( rng.first );
		rng.first = result.last;
		daw_json_assert_untrusted( rng.front( ",}]" ),
		                           "Expected a ',', '}', ']' to trail literal" );
		return result;
	}

	template<char Left, char Right, typename First, typename Last,
	         bool TrustedInput>
	[[nodiscard]] static constexpr IteratorRange<First, Last, TrustedInput>
	skip_bracketed_item( IteratorRange<First, Last, TrustedInput> &rng ) {
		daw_json_assert_untrusted( rng.front( Left ),
		                           "Expected start bracket/brace" );
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
		daw_json_assert_untrusted( rng.front( Right ),
		                           "Expected closing bracket/brace" );

		rng.remove_prefix( );
		daw_json_assert_untrusted( rng.has_more( ), "Unexpected empty range" );

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
		daw_json_assert_untrusted( rng.has_more( ),
		                           "Expected value, not empty range" );

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
			daw_json_assert_untrusted( rng.front( '"' ),
			                           "Expected start of value to begin with '\"'" );
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
			daw_json_assert_untrusted( rng.front( '[' ),
			                           "Expected start of array with '['" );
			return impl::skip_array( rng );
		} else if constexpr( JsonMember::expected_type == JsonParseTypes::Class ) {
			daw_json_assert_untrusted( rng.front( '{' ),
			                           "Expected start of class with '{'" );
			return impl::skip_class( rng );
		} else {
			// Woah there
			std::terminate( );
		}
	}
} // namespace daw::json::impl
