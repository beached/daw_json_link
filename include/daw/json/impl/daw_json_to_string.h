// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_arith_traits.h"
#include "daw_iso8601_utils.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_bounded_vector.h>
#include <daw/daw_traits.h>
#include <utf8/unchecked.h>

#include <optional>
#include <sstream>
#include <string>
#include <third_party/dtoa_milo.h>
#include <tuple>
#include <type_traits>
#include <variant>

namespace daw::json::json_details::to_strings {

	using std::to_string;
	namespace to_string_test {
		template<typename T>
		[[maybe_unused]] auto to_string_test( T &&v )
		  -> decltype( to_string( std::forward<T>( v ) ) );

		template<typename T>
		using to_string_result = decltype( to_string_test( std::declval<T>( ) ) );
	} // namespace to_string_test

	template<typename T>
	using has_to_string = daw::is_detected<to_string_test::to_string_result, T>;

	template<typename T>
	inline constexpr bool has_to_string_v = has_to_string<T>::value;

	template<typename T>
	[[nodiscard, maybe_unused]] auto to_string( std::optional<T> const &v )
	  -> decltype( to_string( *v ) ) {
		if( not v ) {
			return { "null" };
		}
		return to_string( *v );
	}

} // namespace daw::json::json_details::to_strings

namespace daw::json {
	template<typename T>
	struct custom_to_converter_t {
		template<typename U,
		         std::enable_if_t<json_details::to_strings::has_to_string_v<U>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr decltype( auto )
		operator( )( U &&value ) const {
			using std::to_string;
			return to_string( std::forward<U>( value ) );
		}

		template<typename U,
		         std::enable_if_t<not json_details::to_strings::has_to_string_v<U>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] inline std::string operator( )( U &&value ) const {
			std::stringstream ss;
			ss << std::forward<U>( value );
			return ss.str( );
		}
	};
} // namespace daw::json

namespace daw::json::json_details {
	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline OutputIterator constexpr to_string(
	  ParseTag<JsonParseTypes::Null>, OutputIterator it,
	  parse_to_t const &container );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValueArray>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Variant>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] OutputIterator to_string( ParseTag<JsonParseTypes::Real>,
	                                        OutputIterator it,
	                                        parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::StringRaw>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::StringEscaped>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
	           parse_to_t const &value );

	//************************************************
	template<typename Char>
	constexpr char to_nibble_char( Char c ) {
		daw_json_assert( c < 16, "Unexpected hex nibble" );
		if( c < 10 ) {
			return static_cast<char>( c + '0' );
		} else {
			return static_cast<char>( ( c - 10U ) + 'A' );
		}
	}

	template<typename OutputIterator>
	constexpr OutputIterator output_hex( std::uint16_t c, OutputIterator it ) {
		char const n0 = to_nibble_char( ( c >> 12U ) & 0xFU );
		char const n1 = to_nibble_char( ( c >> 8U ) & 0xFU );
		char const n2 = to_nibble_char( ( c >> 4U ) & 0xFU );
		char const n3 = to_nibble_char( c & 0xFU );

		*it++ = '\\';
		*it++ = 'u';
		*it++ = n0;
		*it++ = n1;
		*it++ = n2;
		*it++ = n3;
		return it;
	}

	template<typename OutputIterator>
	constexpr void utf32_to_utf8( std::uint32_t cp, OutputIterator &it ) {
		if( cp <= 0x7FU ) {
			*it++ = static_cast<char>( cp );
			return;
		}
		if( cp <= 0x7FFU ) {
			*it++ = static_cast<char>( ( cp >> 6U ) | 0b11000000U );
			*it++ = static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U );
			return;
		}
		if( cp <= 0xFFFFU ) {
			*it++ = static_cast<char>( ( cp >> 12U ) | 0b11100000U );
			*it++ = static_cast<char>( ( ( cp >> 6U ) & 0b00111111U ) | 0b10000000U );
			*it++ = static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U );
			return;
		}
		if( cp <= 0x10FFFFU ) {
			*it++ = static_cast<char>( ( cp >> 18U ) | 0b11110000U );
			*it++ =
			  static_cast<char>( ( ( cp >> 12U ) & 0b00111111U ) | 0b10000000U );
			*it++ = static_cast<char>( ( ( cp >> 6U ) & 0b00111111U ) | 0b10000000U );
			*it++ = static_cast<char>( ( cp & 0b00111111U ) | 0b10000000U );
			return;
		}
		daw_json_error( "Invalid code point" );
	}
} // namespace daw::json::json_details

namespace daw::json::utils {
	template<bool do_escape = false,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         typename OutputIterator, typename Container,
	         daw::enable_when_t<daw::traits::is_container_like_v<
	           daw::remove_cvref_t<Container>>> = nullptr>
	[[nodiscard]] constexpr OutputIterator
	copy_to_iterator( OutputIterator it, Container const &container ) {
		if constexpr( do_escape ) {
			using iter = daw::remove_cvref_t<decltype( std::begin( container ) )>;
			using it_t = utf8::unchecked::iterator<iter>;
			auto first = it_t( std::begin( container ) );
			auto const last = it_t( std::end( container ) );
			while( first != last ) {
				auto const cp = *first++;
				switch( cp ) {
				case '"':
					*it++ = '\\';
					*it++ = '"';
					break;
				case '\\':
					*it++ = '\\';
					*it++ = '\\';
					break;
				case '/':
					*it++ = '\\';
					*it++ = '/';
					break;
				case '\b':
					*it++ = '\\';
					*it++ = 'b';
					break;
				case '\f':
					*it++ = '\\';
					*it++ = 'f';
					break;
				case '\n':
					*it++ = '\\';
					*it++ = 'n';
					break;
				case '\r':
					*it++ = '\\';
					*it++ = 'r';
					break;
				case '\t':
					*it++ = '\\';
					*it++ = 't';
					break;
				default:
					if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
						if( cp < 0x20U ) {
							it = output_hex( static_cast<std::uint16_t>( cp ), it );
							break;
						}
						if( cp >= 0x7FU and cp <= 0xFFFFU ) {
							it = output_hex( static_cast<std::uint16_t>( cp ), it );
							break;
						}
						if( cp > 0xFFFFU ) {
							it = output_hex(
							  static_cast<std::uint16_t>( 0xD7C0U + ( cp >> 10U ) ), it );
							it = output_hex(
							  static_cast<std::uint16_t>( 0xDC00U + ( cp & 0x3FFU ) ), it );
							break;
						}
					}
					json_details::utf32_to_utf8( cp, it );
					break;
				}
			}
		} else {
			for( auto c : container ) {
				if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
					daw_json_assert( static_cast<unsigned char>( c ) >= 0x20U and
					                   static_cast<unsigned char>( c ) <= 0x7FU,
					                 "string support limited to 0x20 < chr <= 0x7F when "
					                 "DisallowHighEightBit is true" );
				}
				*it++ = c;
			}
		}
		return it;
	}

	template<bool do_escape = false,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         typename OutputIterator>
	[[nodiscard]] constexpr OutputIterator copy_to_iterator( OutputIterator it,
	                                                         char const *ptr ) {
		if( ptr == nullptr ) {
			return it;
		}
		if constexpr( do_escape ) {

			auto chr_it = utf8::unchecked::iterator<char const *>( ptr );
			while( *chr_it.base( ) != '\0' ) {
				auto const cp = *chr_it++;
				switch( cp ) {
				case '"':
					*it++ = '\\';
					*it++ = '"';
					break;
				case '\\':
					*it++ = '\\';
					*it++ = '\\';
					break;
				case '/':
					*it++ = '\\';
					*it++ = '/';
					break;
				case '\b':
					*it++ = '\\';
					*it++ = 'b';
					break;
				case '\f':
					*it++ = '\\';
					*it++ = 'f';
					break;
				case '\n':
					*it++ = '\\';
					*it++ = 'n';
					break;
				case '\r':
					*it++ = '\\';
					*it++ = 'r';
					break;
				case '\t':
					*it++ = '\\';
					*it++ = 't';
					break;
				default:
					if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
						if( cp < 0x20U ) {
							it = json_details::output_hex( static_cast<std::uint16_t>( cp ),
							                               it );
							break;
						}
						if( cp >= 0x7FU and cp <= 0xFFFFU ) {
							it = json_details::output_hex( static_cast<std::uint16_t>( cp ),
							                               it );
							break;
						}
						if( cp > 0xFFFFU ) {
							it = json_details::output_hex(
							  static_cast<std::uint16_t>( 0xD7C0U + ( cp >> 10U ) ), it );
							it = output_hex(
							  static_cast<std::uint16_t>( 0xDC00U + ( cp & 0x3FFU ) ), it );
							break;
						}
					}
					json_details::utf32_to_utf8( cp, it );
					break;
				}
			}
		} else {
			while( *ptr != '\0' ) {
				if constexpr( EightBitMode == EightBitModes::DisallowHigh ) {
					daw_json_assert( static_cast<unsigned>( *ptr ) >= 0x20U and
					                   static_cast<unsigned>( *ptr ) <= 0x7FU,
					                 "string support limited to 0x20 < chr <= 0x7F when "
					                 "DisallowHighEightBit is true" );
				}
				*it++ = *ptr++;
			}
		}
		return it;
	}
} // namespace daw::json::utils

namespace daw::json::json_details {
	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
	           parse_to_t const &value ) {

		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		if( value ) {
			it = utils::copy_to_iterator( it, "true" );
		} else {
			it = utils::copy_to_iterator( it, "false" );
		}
		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		return it;
	}

	template<std::size_t idx, typename JsonMembers, typename OutputIterator,
	         typename parse_to_t>
	constexpr void to_variant_string( OutputIterator &it,
	                                  parse_to_t const &value ) {
		if constexpr( idx < std::variant_size_v<parse_to_t> ) {
			if( value.index( ) != idx ) {
				to_variant_string<idx + 1, JsonMembers>( it, value );
				return;
			}
			using element_t = typename JsonMembers::json_elements;
			using JsonMember =
			  typename std::tuple_element<idx,
			                              typename element_t::element_map_t>::type;
			it = to_string<JsonMember>( ParseTag<JsonMember::base_expected_type>{ },
			                            it, std::get<idx>( value ) );
		}
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Variant>, OutputIterator it,
	           parse_to_t const &value ) {

		to_variant_string<0, JsonMember>( it, value );
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::VariantTagged>, OutputIterator it,
	           parse_to_t const &value ) {

		to_variant_string<0, JsonMember>( it, value );
		return it;
	}

	template<typename T>
	[[maybe_unused]] constexpr auto deref_detect( T &&value )
	  -> decltype( *value );

	[[maybe_unused]] inline constexpr void deref_detect( ... ) {}

	template<typename T>
	using deref_t =
	  daw::remove_cvref_t<decltype( deref_detect( std::declval<T>( ) ) )>;

	template<typename Optional>
	inline constexpr bool is_valid_optional_v =
	  daw::is_detected_v<deref_t, Optional>;

	template<typename JsonMember, typename OutputIterator, typename Optional>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Null>, OutputIterator it,
	           Optional const &value ) {
		static_assert( is_valid_optional_v<Optional> );
		daw_json_assert( value, "Should Never get here without a value" );
		using tag_type = ParseTag<JsonMember::base_expected_type>;
		return to_string<JsonMember>( tag_type{ }, it, *value );
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline OutputIterator to_string( ParseTag<JsonParseTypes::Real>,
	                                               OutputIterator it,
	                                               parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		if constexpr( daw::is_floating_point_v<parse_to_t> ) {
			char buff[50]; // dtoa_milo will zero term the output
			buff[49] = 0;
			static_assert( sizeof( parse_to_t ) <= sizeof( double ) );
			milo::dtoa_milo( static_cast<double>( value ), buff );
			auto ptr = buff;
			while( *ptr != '\0' ) {
				*it++ = *ptr;
				++ptr;
			}
		} else {
			using std::to_string;
			using to_strings::to_string;
			it = utils::copy_to_iterator( it, to_string( value ) );
		}
		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		return it;
	}

	template<typename T, bool>
	struct base_int_type_impl {
		using type = T;
	};

	template<typename T>
	struct base_int_type_impl<T, true> {
		using type = std::underlying_type_t<T>;
	};

	template<typename T>
	using base_int_type_t =
	  typename base_int_type_impl<T, std::is_enum_v<T>>::type;

	inline constexpr auto digits100 = [] {
		std::array<char[2], 100> result{ };
		for( size_t n = 10; n < 100; ++n ) {
			result[n][0] = static_cast<char>( n % 10 ) + '0';
			result[n][1] = static_cast<char>( n / 10 ) + '0';
		}
		return result;
	}( );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using std::to_string;
		using to_strings::to_string;
		using under_type = base_int_type_t<parse_to_t>;

		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		if constexpr( std::is_enum_v<parse_to_t> or
		              daw::is_integral_v<parse_to_t> ) {
			auto v = static_cast<under_type>( value );

			char buff[std::numeric_limits<under_type>::digits10 + 1]{ };
			char *ptr = buff;
			if( v < 0 ) {
				*it++ = '-';
				// Do 1 round here just incase we are
				// std::numeric_limits<intmax_t>::min( ) and cannot negate
				*ptr++ = '0' - static_cast<char>( v % 10 );
				v /= -10;
				if( v == 0 ) {
					*it++ = buff[0];
					if constexpr( JsonMember::literal_as_string ==
					              LiteralAsStringOpt::Always ) {
						*it++ = '"';
					}
					return it;
				}
			}
			/*
			do {
				*ptr++ = '0' + static_cast<char>( v % 10 );
				v /= 10;
			} while( v > 0 );
			 */
			if( v == 0 ) {
				*ptr++ ='0';
			}
			while( v >= 10 ) {
				auto const tmp = static_cast<std::size_t>( v%100 );
				v /= 100;
				ptr[0] = digits100[tmp][0];
				ptr[1] = digits100[tmp][1];
				ptr += 2;
			}
			if( v > 0 ) {
				*ptr++ = '0' + static_cast<char>( v );
			}
			--ptr;
			*it++ = *ptr;
			while( ptr != buff ) {
				--ptr;
				*it++ = *ptr;
			}
		} else {
			// Fallback to ADL
			it = utils::copy_to_iterator( it, to_string( value ) );
		}
		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using std::to_string;
		using to_strings::to_string;
		using under_type = base_int_type_t<parse_to_t>;

		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		if constexpr( std::is_enum_v<parse_to_t> or
		              daw::is_integral_v<parse_to_t> ) {
			auto v = static_cast<under_type>( value );
			daw_json_assert(
			  v >= 0, "Negative numbers are not supported for unsigned types" );
			char buff[std::numeric_limits<under_type>::digits10 + 1]{ };
			char *ptr = buff;
			do {
				*ptr++ = '0' + static_cast<char>( v % 10 );
				v /= 10;
			} while( v > 0 );
			--ptr;
			*it++ = *ptr;
			while( ptr != buff ) {
				--ptr;
				*it++ = *ptr;
			}
		} else {
			// Fallback to ADL
			it = utils::copy_to_iterator( it, to_string( value ) );
		}
		if constexpr( JsonMember::literal_as_string ==
		              LiteralAsStringOpt::Always ) {
			*it++ = '"';
		}
		return it;
	}
} // namespace daw::json::json_details

namespace daw::json::utils {
	namespace utils_details {
		template<typename Integer>
		struct number {
			using parse_to_t = Integer;
			static constexpr LiteralAsStringOpt literal_as_string =
			  LiteralAsStringOpt::Never;
		};
	} // namespace utils_details

	template<typename Integer, typename OutputIterator>
	inline constexpr OutputIterator integer_to_string( OutputIterator it,
	                                                   Integer const &value ) {
		static_assert( daw::is_integral_v<Integer> );

		if constexpr( daw::is_unsigned_v<Integer> ) {
			return json_details::to_string<utils_details::number<Integer>>(
			  ParseTag<JsonParseTypes::Unsigned>{ }, it, value );
		} else {
			return json_details::to_string<utils_details::number<Integer>>(
			  ParseTag<JsonParseTypes::Signed>{ }, it, value );
		}
	}
} // namespace daw::json::utils

namespace daw::json::json_details {
	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::StringRaw>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;
		*it++ = '"';
		if( value.size( ) > 0U ) {
			it = utils::copy_to_iterator<false, eight_bit_mode>( it, value );
		}
		*it++ = '"';
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::StringEscaped>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;
		*it++ = '"';
		it = utils::copy_to_iterator<true, eight_bit_mode>( it, value );
		*it++ = '"';
		return it;
	}

	template<typename T>
	[[nodiscard]] inline constexpr bool is_null( std::optional<T> const &v ) {
		return not static_cast<bool>( v );
	}

	template<typename T>
	[[nodiscard]] inline constexpr bool is_null( T const & ) {
		return false;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using daw::json::json_details::is_null;
		if( is_null( value ) ) {
			it = utils::copy_to_iterator( it, "null" );
		} else {
			*it++ = '"';
			datetime::ymdhms const civil = datetime::time_point_to_civil( value );
			it = utils::integer_to_string( it, civil.year );
			*it++ = '-';
			if( civil.month < 10 ) {
				*it++ = '0';
			}
			it = utils::integer_to_string( it, civil.month );
			*it++ = '-';
			if( civil.day < 10 ) {
				*it++ = '0';
			}
			it = utils::integer_to_string( it, civil.day );
			*it++ = 'T';
			if( civil.hour < 10 ) {
				*it++ = '0';
			}
			it = utils::integer_to_string( it, civil.hour );
			*it++ = ':';
			if( civil.minute < 10 ) {
				*it++ = '0';
			}
			it = utils::integer_to_string( it, civil.minute );
			*it++ = ':';
			if( civil.second < 10 ) {
				*it++ = '0';
			}
			it = utils::integer_to_string( it, civil.second );
			if( civil.millisecond > 0 ) {
				*it++ = '.';
				it = utils::integer_to_string( it, civil.millisecond );
			}
			*it++ = 'Z';
			*it++ = '"';
		}
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Unknown>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		if( value.is_null( ) ) {
			it = utils::copy_to_iterator( it, "null" );
		} else {
			it = utils::copy_to_iterator( it, value.get_string_view( ) );
		}
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		return json_data_contract_trait_t<parse_to_t>::serialize(
		  it, daw::json::json_data_contract<parse_to_t>::to_json_data( value ),
		  value );
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] inline constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		if constexpr( JsonMember::custom_json_type != CustomJsonTypes::Literal ) {
			*it++ = '"';
			if constexpr( std::is_invocable_r_v<OutputIterator,
			                                    typename JsonMember::to_converter_t,
			                                    OutputIterator, parse_to_t> ) {

				it = typename JsonMember::to_converter_t{ }( it, value );
			} else {
				it = utils::copy_to_iterator(
				  it, typename JsonMember::to_converter_t{ }( value ) );
			}
			*it++ = '"';
			return it;
		} else {
			return utils::copy_to_iterator(
			  it, typename JsonMember::to_converter_t{ }( value ) );
		}
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
	           parse_to_t const &container ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		*it++ = '[';
		if( not std::empty( container ) ) {
			auto count = std::size( container ) - 1U;
			for( auto const &v : container ) {
				it = to_string<typename JsonMember::json_element_t>(
				  ParseTag<JsonMember::json_element_t::expected_type>{ }, it, v );
				if( count-- > 0 ) {
					*it++ = ',';
				}
			}
		}
		*it++ = ']';
		return it;
	}

	template<typename Key, typename Value>
	[[maybe_unused]] inline constexpr Key const &
	json_get_key( std::pair<Key, Value> const &kv ) {
		return kv.first;
	}

	template<typename Key, typename Value>
	[[maybe_unused]] inline constexpr Value const &
	json_get_value( std::pair<Key, Value> const &kv ) {
		return kv.second;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValueArray>, OutputIterator it,
	           parse_to_t const &container ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );
		using key_t = typename JsonMember::json_key_t;
		using value_t = typename JsonMember::json_value_t;
		*it++ = '[';
		if( not std::empty( container ) ) {
			auto count = std::size( container ) - 1U;
			for( auto const &v : container ) {
				*it++ = '{';
				// Append Key Name
				*it++ = '"';
				it = utils::copy_to_iterator( it, key_t::name );
				*it++ = '"';
				*it++ = ':';
				// Append Key Value
				it = to_string<key_t>( ParseTag<key_t::expected_type>{ }, it,
				                       json_get_key( v ) );

				*it++ = ',';
				// Append Value Name
				*it++ = '"';
				it = utils::copy_to_iterator( it, value_t::name );
				*it++ = '"';
				*it++ = ':';
				// Append Value Value
				it = to_string<value_t>( ParseTag<value_t::expected_type>{ }, it,
				                         json_get_value( v ) );

				*it++ = '}';
				if( count-- > 0 ) {
					*it++ = ',';
				}
			}
		}
		*it++ = ']';
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
	           parse_to_t const &container ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		*it++ = '{';
		if( not std::empty( container ) ) {
			auto count = std::size( container ) - 1U;
			for( auto const &v : container ) {
				it = to_string<typename JsonMember::json_key_t>(
				  ParseTag<JsonMember::json_key_t::expected_type>{ }, it,
				  json_get_key( v ) );
				*it++ = ':';
				it = to_string<typename JsonMember::json_element_t>(
				  ParseTag<JsonMember::json_element_t::expected_type>{ }, it,
				  json_get_value( v ) );
				if( count-- > 0 ) {
					*it++ = ',';
				}
			}
		}
		*it++ = '}';
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename T>
	[[nodiscard]] inline constexpr OutputIterator
	member_to_string( OutputIterator it, T const &value ) {
		return to_string<JsonMember>( ParseTag<JsonMember::expected_type>{ },
		                              daw::move( it ), value );
	}

	template<typename JsonMember>
	using tag_member_t = typename JsonMember::tag_member;

	template<typename JsonMember>
	inline constexpr bool has_tag_member_v =
	  daw::is_detected_v<tag_member_t, JsonMember>;
	template<std::size_t, typename JsonMember, typename OutputIterator,
	         typename Value, typename VisitedMembers,
	         std::enable_if_t<not has_tag_member_v<JsonMember>, std::nullptr_t> =
	           nullptr>
	inline constexpr void tags_to_json_str( bool &, OutputIterator const &,
	                                        Value const &,
	                                        VisitedMembers const & ) {}
	template<
	  std::size_t pos, typename JsonMember, typename OutputIterator,
	  typename Value, typename VisitedMembers,
	  std::enable_if_t<has_tag_member_v<JsonMember>, std::nullptr_t> = nullptr>
	constexpr void tags_to_json_str( bool &is_first, OutputIterator it,
	                                 Value const &v,
	                                 VisitedMembers &visited_members ) {
		using tag_member = tag_member_t<JsonMember>;
		constexpr auto tag_member_name = daw::string_view( tag_member::name );
		if( daw::algorithm::contains( visited_members.begin( ),
		                              visited_members.end( ), tag_member_name ) ) {
			return;
		}
		visited_members.push_back( tag_member_name );
		if( not is_first ) {
			*it++ = ',';
		}
		is_first = false;
		*it++ = '"';
		it = utils::copy_to_iterator<false, EightBitModes::AllowFull>(
		  it, tag_member_name );
		it = utils::copy_to_iterator<false, EightBitModes::AllowFull>( it, "\":" );
		it = member_to_string<tag_member>( daw::move( it ),
		                                   typename JsonMember::switcher{ }( v ) );
	}

	template<std::size_t pos, typename JsonMember, typename OutputIterator,
	         typename... Args, typename Value, typename Visited>
	inline constexpr void to_json_str( bool &is_first, OutputIterator &it,
	                                   std::tuple<Args...> const &tp,
	                                   Value const &, Visited &visited_members ) {
		constexpr auto json_member_name = daw::string_view( JsonMember::name );
		if( daw::algorithm::contains( visited_members.begin( ),
		                              visited_members.end( ), json_member_name ) ) {
			return;
		}
		visited_members.push_back( json_member_name );
		static_assert( is_a_json_type_v<JsonMember>, "Unsupported data type" );
		if constexpr( is_json_nullable_v<JsonMember> ) {
			if( not std::get<pos>( tp ) ) {
				return;
			}
		}
		if( not is_first ) {
			*it++ = ',';
		}
		is_first = false;
		*it++ = '"';
		it = utils::copy_to_iterator<false, EightBitModes::AllowFull>(
		  it, JsonMember::name );
		it = utils::copy_to_iterator<false, EightBitModes::AllowFull>( it, "\":" );
		it = member_to_string<JsonMember>( daw::move( it ), std::get<pos>( tp ) );
	}

	template<size_t TupleIdx, typename JsonMember, typename OutputIterator,
	         typename... Args>
	constexpr void to_json_ordered_str( std::size_t &array_idx,
	                                    OutputIterator &it,
	                                    std::tuple<Args...> const &tp ) {

		using json_member_type = ordered_member_subtype_t<JsonMember>;
		static_assert( is_a_json_type_v<json_member_type>,
		               "Unsupported data type" );
		// json_tagged_variant like members cannot work as we have no member names
		// to work with
		static_assert(
		  not is_a_json_tagged_variant_v<json_member_type>,
		  "JSON tagged variant types are not supported when inside an array "
		  "as an ordered structure" );

		if constexpr( is_an_ordered_member_v<JsonMember> ) {
			for( ; array_idx < JsonMember::member_index; ++array_idx ) {
				if( array_idx > 0 ) {
					*it++ = ',';
				}
				*it++ = 'n';
				*it++ = 'u';
				*it++ = 'l';
				*it++ = 'l';
			}
		}
		if( array_idx > 0 ) {
			*it++ = ',';
		}
		it = member_to_string<json_member_type>( it, std::get<TupleIdx>( tp ) );
		++array_idx;
	}
} // namespace daw::json::json_details
