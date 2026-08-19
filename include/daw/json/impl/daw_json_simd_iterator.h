// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include <daw/daw_cpp_feature_check.h>

#if __has_include( <simd> )

#include <simd>

#if defined( __cpp_lib_simd ) or defined( __glibcxx_simd )

#include "daw/json/impl/daw_json_parse_value.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/**
			 * State carried from one SIMD block to the next.  The three booleans
			 * correspond to the cross-register dependencies in simdjson's stage 1:
			 * string continuation, escape continuation, and scalar continuation.
			 */
			struct simd_json_classifier_state {
				std::size_t offset = 0;
				bool in_string = false;
				bool escaped = false;
				bool previous_scalar = false;
			};

			template<typename Byte>
			struct simd_json_block_base {
				using simd_type = std::simd::vec<Byte>;
				using mask_type = typename simd_type::mask_type;

				static constexpr std::size_t block_size =
				  static_cast<std::size_t>( simd_type::size( ) );

				char const *data = nullptr;
				std::size_t offset = 0;
				std::size_t size = 0;

				mask_type scalar_start{ false };
				simd_json_classifier_state state_after{ };

				[[nodiscard]] constexpr bool is_full( ) const noexcept {
					return size == block_size;
				}
			};

			template<JsonBaseParseTypes ExpectedType, typename Byte = std::uint8_t>
			struct simd_json_block;

			template<typename Byte>
			struct simd_json_block<JsonBaseParseTypes::Number, Byte>
			  : simd_json_block_base<Byte> {
				using mask_type = typename simd_json_block_base<Byte>::mask_type;
				mask_type number_start{ false };
			};

			template<typename Byte>
			struct simd_json_block<JsonBaseParseTypes::Bool, Byte>
			  : simd_json_block_base<Byte> {
				using mask_type = typename simd_json_block_base<Byte>::mask_type;
				mask_type boolean_start{ false };
				mask_type true_start{ false };
			};

			template<typename Byte>
			struct simd_json_block<JsonBaseParseTypes::String, Byte>
			  : simd_json_block_base<Byte> {
				using mask_type = typename simd_json_block_base<Byte>::mask_type;
				mask_type string_start{ false };
			};

			template<JsonBaseParseTypes ExpectedType, typename Byte = std::uint8_t>
			class simd_json_classifier {
				static_assert(
				  ExpectedType == JsonBaseParseTypes::Number or
				    ExpectedType == JsonBaseParseTypes::Bool or
				    ExpectedType == JsonBaseParseTypes::String,
				  "simd_json_classifier supports only Number, Bool, and String" );
				using block_type = simd_json_block<ExpectedType, Byte>;
				using simd_type = typename block_type::simd_type;
				using mask_type = typename block_type::mask_type;
				using simd_value_type = typename simd_type::value_type;

				static constexpr std::size_t block_size = block_type::block_size;
				static_assert( block_size <= 64,
				               "The classifier bit set stores at most 64 SIMD lanes" );

				[[nodiscard]] static constexpr simd_type splat( char value ) noexcept {
					return simd_type( static_cast<simd_value_type>( value ) );
				}

				[[nodiscard]] static simd_type load( char const *first,
				                                     std::size_t count ) {
					auto const bytes = reinterpret_cast<std::uint8_t const *>( first );
					if( count == block_size ) {
						return std::simd::unchecked_load<simd_type>(
						  std::span( bytes, block_size ) );
					}
					return std::simd::partial_load<simd_type>(
					  std::span( bytes, count ) );
				}

				[[nodiscard]] static constexpr std::uint64_t
				low_bits( std::size_t count ) noexcept {
					return count == 64 ? ~std::uint64_t{ 0 }
					                   : ( std::uint64_t{ 1 } << count ) - 1;
				}

				[[nodiscard]] static constexpr std::uint64_t
				prefix_xor( std::uint64_t bits ) noexcept {
					bits ^= bits << 1U;
					bits ^= bits << 2U;
					bits ^= bits << 4U;
					bits ^= bits << 8U;
					bits ^= bits << 16U;
					bits ^= bits << 32U;
					return bits;
				}

				[[nodiscard]] static constexpr bool
				last_bit( std::uint64_t bits, std::size_t count ) noexcept {
					return count != 0 and
					       ( ( bits >> ( count - 1U ) ) & std::uint64_t{ 1 } ) != 0;
				}

			public:
				using state_type = simd_json_classifier_state;

				[[nodiscard]] static block_type classify_number( char const *first,
				                                                 std::size_t count,
				                                                 state_type &state )
				  requires( ExpectedType == JsonBaseParseTypes::Number ) {
					count = count < block_size ? count : block_size;
					auto const input = load( first, count );

					auto const whitespace =
					  ( input == splat( ' ' ) ) | ( input == splat( '\t' ) ) |
					  ( input == splat( '\n' ) ) | ( input == splat( '\r' ) );
					auto const operators = ( input == splat( '[' ) ) |
					                       ( input == splat( ']' ) ) |
					                       ( input == splat( ',' ) );
					auto const scalar = not( whitespace | operators );
					auto const number_start =
					  ( input == splat( '-' ) ) |
					  ( ( input >= splat( '0' ) ) & ( input <= splat( '9' ) ) );
					auto const valid_bits = low_bits( count );
					auto const scalar_bits = scalar.to_ullong( ) & valid_bits;
					auto const follows_scalar_bits =
					  ( scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar = last_bit( scalar_bits, count );
					auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
					auto const number_start_bits =
					  number_start.to_ullong( ) & scalar_start_bits & valid_bits;

					auto const offset = state.offset;
					state.offset += count;
					block_type result{ };
					result.data = first;
					result.offset = offset;
					result.size = count;
					result.scalar_start = mask_type( scalar_start_bits );
					result.number_start = mask_type( number_start_bits );
					result.state_after = state;
					return result;
				}

				[[nodiscard]] static block_type
				classify_bool( char const *first, std::size_t count, state_type &state )
				  requires( ExpectedType == JsonBaseParseTypes::Bool ) {
					count = count < block_size ? count : block_size;
					auto const input = load( first, count );

					auto const whitespace =
					  ( input == splat( ' ' ) ) | ( input == splat( '\t' ) ) |
					  ( input == splat( '\n' ) ) | ( input == splat( '\r' ) );
					auto const operators = ( input == splat( '[' ) ) |
					                       ( input == splat( ']' ) ) |
					                       ( input == splat( ',' ) );
					auto const scalar = not( whitespace | operators );
					auto const true_start = input == splat( 't' );
					auto const boolean_start = true_start | ( input == splat( 'f' ) );
					auto const valid_bits = low_bits( count );
					auto const scalar_bits = scalar.to_ullong( ) & valid_bits;
					auto const follows_scalar_bits =
					  ( scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar = last_bit( scalar_bits, count );
					auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
					auto const boolean_start_bits =
					  boolean_start.to_ullong( ) & scalar_start_bits & valid_bits;

					auto const offset = state.offset;
					state.offset += count;
					block_type result{ };
					result.data = first;
					result.offset = offset;
					result.size = count;
					result.scalar_start = mask_type( scalar_start_bits );
					result.boolean_start = mask_type( boolean_start_bits );
					result.true_start =
					  mask_type( true_start.to_ullong( ) & boolean_start_bits );
					result.state_after = state;
					return result;
				}

			private:
				[[nodiscard]] static block_type classify_string( char const *first,
				                                                 std::size_t count,
				                                                 state_type &state ) {
					count = count < block_size ? count : block_size;
					auto const input = load( first, count );

					auto const whitespace =
					  ( input == splat( ' ' ) ) | ( input == splat( '\t' ) ) |
					  ( input == splat( '\n' ) ) | ( input == splat( '\r' ) );
					auto const operators = ( input == splat( '[' ) ) |
					                       ( input == splat( ']' ) ) |
					                       ( input == splat( ',' ) );
					auto const scalar = not( whitespace | operators );
					auto const quote = input == splat( '"' );
					auto const backslash = input == splat( '\\' );

					auto const valid_bits = low_bits( count );
					auto const operator_bits = operators.to_ullong( ) & valid_bits;
					auto const scalar_bits = scalar.to_ullong( ) & valid_bits;
					auto const backslash_bits = backslash.to_ullong( ) & valid_bits;

					constexpr std::uint64_t odd_bits = 0xAAAAAAAAAAAAAAAAULL;
					auto const previous_escaped = state.escaped ? std::uint64_t{ 1 } : 0;
					auto const potential_escape = backslash_bits & ~previous_escaped;
					auto const maybe_escaped = potential_escape << 1U;
					auto const escape_and_terminal =
					  ( ( maybe_escaped | odd_bits ) - potential_escape ) ^ odd_bits;
					auto const escaped_bits =
					  ( escape_and_terminal ^ ( backslash_bits | previous_escaped ) ) &
					  valid_bits;
					auto const escape_bits = escape_and_terminal & backslash_bits;
					state.escaped = last_bit( escape_bits, count );

					auto const quote_bits =
					  quote.to_ullong( ) & ~escaped_bits & valid_bits;
					auto const in_string_bits =
					  ( prefix_xor( quote_bits ) ^
					    ( state.in_string ? valid_bits : std::uint64_t{ 0 } ) ) &
					  valid_bits;
					state.in_string = last_bit( in_string_bits, count );
					auto const string_tail_bits = in_string_bits ^ quote_bits;
					auto const nonquote_scalar_bits = scalar_bits & ~quote_bits;
					auto const follows_scalar_bits =
					  ( nonquote_scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar = last_bit( nonquote_scalar_bits, count );
					auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
					auto const value_start_bits =
					  scalar_start_bits & ~string_tail_bits & valid_bits;
					auto const structural_bits = ( operator_bits | scalar_start_bits ) &
					                             ~string_tail_bits & valid_bits;
					auto const string_start_bits =
					  quote_bits & in_string_bits & structural_bits;

					auto const offset = state.offset;
					state.offset += count;
					block_type result{ };
					result.data = first;
					result.offset = offset;
					result.size = count;
					result.scalar_start = mask_type( value_start_bits );
					result.string_start = mask_type( string_start_bits );
					result.state_after = state;
					return result;
				}

			public:
				[[nodiscard]] static block_type
				classify( char const *first, std::size_t count, state_type &state ) {
					if constexpr( ExpectedType == JsonBaseParseTypes::Number ) {
						return classify_number( first, count, state );
					} else if constexpr( ExpectedType == JsonBaseParseTypes::Bool ) {
						return classify_bool( first, count, state );
					} else {
						static_assert( ExpectedType == JsonBaseParseTypes::String );
						return classify_string( first, count, state );
					}
				}
			};
		} // namespace json_details

		inline namespace experimental {
			/**
			 * Input iterator over JSON scalar values located using native
			 * std::simd-sized classified blocks.
			 * @tparam JsonMember The JSON Link mapping used to parse each value.
			 */
			template<typename JsonMember, typename Byte = std::uint8_t,
			         auto... PolicyFlags>
			class json_simd_block_iterator {
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  json_details::simd_json_classifier<JsonMember::underlying_json_type,
				                                     Byte>;
				using block_type =
				  json_details::simd_json_block<JsonMember::underlying_json_type, Byte>;

				static constexpr auto json_base_type = JsonMember::underlying_json_type;
				static constexpr bool is_number =
				  json_base_type == JsonBaseParseTypes::Number;
				static constexpr bool is_boolean =
				  json_base_type == JsonBaseParseTypes::Bool;
				static constexpr bool is_string =
				  json_base_type == JsonBaseParseTypes::String;
				static constexpr ErrorReason invalid_start_reason = [] {
					if constexpr( is_number ) {
						return ErrorReason::InvalidNumberStart;
					} else if constexpr( is_boolean ) {
						return ErrorReason::InvalidLiteral;
					} else {
						return ErrorReason::InvalidString;
					}
				}( );

				static_assert(
				  is_number or is_boolean or is_string,
				  "json_simd_block_iterator currently supports number, boolean, and "
				  "string JsonMember types" );

				[[nodiscard]] static std::uint64_t
				value_starts( block_type const &block ) noexcept {
					if constexpr( is_number ) {
						return static_cast<std::uint64_t>(
						  block.number_start.to_ullong( ) );
					} else if constexpr( is_boolean ) {
						return static_cast<std::uint64_t>(
						  block.boolean_start.to_ullong( ) );
					} else {
						return static_cast<std::uint64_t>(
						  block.string_start.to_ullong( ) );
					}
				}

			public:
				using json_member = JsonMember;
				using value_type = typename json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				char const *m_first = nullptr;
				char const *m_last = nullptr;
				char const *m_current = nullptr;
				char const *m_block_data = nullptr;
				std::uint64_t m_value_starts = 0;
				std::uint64_t m_true_starts = 0;
				bool m_current_boolean = false;
				json_details::simd_json_classifier_state m_state{ };

				void move_to_next_value( ) {
					while( m_value_starts == 0 and m_first != nullptr and
					       m_first != m_last ) {
						auto const remaining = static_cast<std::size_t>( m_last - m_first );
						auto const block =
						  classifier_type::classify( m_first, remaining, m_state );
						m_block_data = block.data;
						m_value_starts = value_starts( block );
						if constexpr( is_boolean ) {
							m_true_starts =
							  static_cast<std::uint64_t>( block.true_start.to_ullong( ) );
						}
						auto const scalar_starts =
						  static_cast<std::uint64_t>( block.scalar_start.to_ullong( ) );
						auto const unexpected_starts = scalar_starts & ~m_value_starts;
						if( unexpected_starts != 0 ) {
							auto const lane = static_cast<std::size_t>(
							  std::countr_zero( unexpected_starts ) );
							auto error_state = ParseState( block.data + lane, m_last );
							daw_json_error( true, invalid_start_reason, error_state );
						}
						m_first += static_cast<std::ptrdiff_t>( block.size );
					}

					if( m_value_starts == 0 ) {
						m_current = nullptr;
						return;
					}

					auto const lane =
					  static_cast<std::size_t>( std::countr_zero( m_value_starts ) );
					if constexpr( is_boolean ) {
						m_current_boolean =
						  ( m_true_starts & ( std::uint64_t{ 1 } << lane ) ) != 0;
					}
					m_value_starts &= m_value_starts - 1U;
					m_current = m_block_data + lane;
				}

			public:
				json_simd_block_iterator( ) = default;

				explicit json_simd_block_iterator( std::string_view document )
				  : m_first( document.data( ) )
				  , m_last( document.data( ) + document.size( ) ) {
					move_to_next_value( );
				}

				[[nodiscard]] reference operator*( ) const {
					auto parse_state = ParseState( m_current, m_last );
					if constexpr( is_boolean ) {
						static_assert(
						  json_member::literal_as_string ==
						    options::LiteralAsStringOpt::Never,
						  "SIMD boolean iteration does not support literals encoded as "
						  "strings" );
						if constexpr( not ParseState::is_unchecked_input ) {
							if( m_current_boolean ) {
								daw_json_assert_weak( parse_state.starts_with( "true" ),
								                      ErrorReason::InvalidLiteral,
								                      parse_state );
								parse_state.remove_prefix( 4 );
							} else {
								daw_json_assert_weak( parse_state.starts_with( "false" ),
								                      ErrorReason::InvalidLiteral,
								                      parse_state );
								parse_state.remove_prefix( 5 );
							}
							parse_state.trim_left( );
							daw_json_assert_weak(
							  not parse_state.has_more( ) or
							    ParseState::at_end_of_item( parse_state.front( ) ),
							  ErrorReason::InvalidEndOfValue,
							  parse_state );
						}
						using constructor_t = json_details::json_constructor_t<json_member>;
						return json_details::construct_value<value_type, constructor_t>(
						  parse_state, m_current_boolean );
					} else {
						return json_details::
						  parse_value<json_member, false, json_member::expected_type>(
						    parse_state );
					}
				}

				json_simd_block_iterator &operator++( ) {
					move_to_next_value( );
					return *this;
				}

				void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] explicit operator bool( ) const noexcept {
					return m_current != nullptr;
				}

				[[nodiscard]] json_simd_block_iterator begin( ) const {
					return *this;
				}

				[[nodiscard]] json_simd_block_iterator end( ) const noexcept {
					return { };
				}

				friend bool operator==( json_simd_block_iterator const &lhs,
				                        json_simd_block_iterator const &rhs ) noexcept {
					auto const lhs_at_end = lhs.m_current == nullptr;
					auto const rhs_at_end = rhs.m_current == nullptr;

					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_current == rhs.m_current;
				}

				friend bool operator!=( json_simd_block_iterator const &lhs,
				                        json_simd_block_iterator const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};
		} // namespace experimental
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
#endif
