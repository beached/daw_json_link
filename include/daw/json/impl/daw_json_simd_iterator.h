// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/daw_json_simd.h"
#include "daw/json/impl/version.h"

#if defined( DAW_JSON_HAS_SIMD )

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
			namespace simd_details {
				[[nodiscard]] constexpr std::uint64_t
				prefix_xor( std::uint64_t bits ) noexcept {
					bits ^= bits << 1U;
					bits ^= bits << 2U;
					bits ^= bits << 4U;
					bits ^= bits << 8U;
					bits ^= bits << 16U;
					bits ^= bits << 32U;
					return bits;
				}

				[[nodiscard]] constexpr std::uint64_t
				low_bits( std::size_t count ) noexcept {
					return count == 64 ? ~std::uint64_t{ 0 }
					                   : ( std::uint64_t{ 1 } << count ) - 1;
				}

				[[nodiscard]] constexpr bool last_bit( std::uint64_t bits,
				                                       std::size_t count ) noexcept {
					return count != 0 and
					       ( ( bits >> ( count - 1U ) ) & std::uint64_t{ 1 } ) != 0;
				}

				template<typename simd_type>
				[[nodiscard]]
#if defined( DAW_JSON_HAS_STD_SIMD )
				consteval
#else
				DAW_ATTRIB_INLINE
#endif
				  simd_type
				  splat( char value ) noexcept {
					return simd_type( static_cast<simd_type::value_type>( value ) );
				}

				template<auto... values, typename simd_type>
				[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR auto
				one_of( simd_type value ) {
					return ( ( value == splat<simd_type>( values ) ) | ... );
				}

				template<typename simd_type, std::size_t block_size, typename CharT,
				         typename Chr>
				[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR simd_type
				load( Chr const *first, std::size_t count ) {
					static constexpr auto flags = [] {
						if constexpr( std::same_as<CharT, Chr> ) {
							return daw::simd::flag_default;
						} else {
							return daw::simd::flag_convert;
						}
					}( );
					if( count == block_size ) {
						return daw::simd::unchecked_load<simd_type>(
						  std::span( first, block_size ), flags );
					}
					return daw::simd::partial_load<simd_type>( std::span( first, count ),
					                                           flags );
				}

			} // namespace simd_details

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

			template<typename CharT>
			struct simd_json_block_base {
				// Use the implementation's native SIMD width for CharT. Forcing a
				// 64-lane ABI can require multiple native registers (or scalar chunks).
				using simd_type = daw::simd::vec<CharT, 64>;
				using mask_type = simd_type::mask_type;

				static constexpr std::size_t block_size =
				  static_cast<std::size_t>( simd_type::size( ) );

				char const *data = nullptr;
				std::size_t offset = 0;
				std::size_t size = 0;

				std::uint64_t scalar_start = 0;
				simd_json_classifier_state state_after{ };

				[[nodiscard]] constexpr bool is_full( ) const noexcept {
					return size == block_size;
				}
			};

			template<JsonBaseParseTypes ExpectedType, typename CharT>
			struct simd_json_block;

			template<typename CharT>
			struct simd_json_block<JsonBaseParseTypes::Number, CharT>
			  : simd_json_block_base<CharT> {
				std::uint64_t number_start = 0;
				std::uint64_t number_characters = 0;
				std::uint64_t decimal_points = 0;
				std::uint64_t exponent_markers = 0;
			};

			template<typename CharT>
			struct simd_json_block<JsonBaseParseTypes::Bool, CharT>
			  : simd_json_block_base<CharT> {
				std::uint64_t boolean_start = 0;
				std::uint64_t true_start = 0;
			};

			template<typename CharT>
			struct simd_json_block<JsonBaseParseTypes::String, CharT>
			  : simd_json_block_base<CharT> {
				std::uint64_t string_start = 0;
			};

			template<JsonBaseParseTypes ExpectedType, typename CharT>
			class simd_json_classifier {
				static_assert(
				  ExpectedType == JsonBaseParseTypes::Number or
				    ExpectedType == JsonBaseParseTypes::Bool or
				    ExpectedType == JsonBaseParseTypes::String,
				  "simd_json_classifier supports only Number, Bool, and String" );
				using block_type = simd_json_block<ExpectedType, CharT>;
				using simd_type = block_type::simd_type;
				using mask_type = block_type::mask_type;
				using simd_value_type = simd_type::value_type;

				static constexpr std::size_t block_size = block_type::block_size;
				static_assert( block_size <= 64,
				               "The classifier bit set stores at most 64 SIMD lanes" );

			public:
				using state_type = simd_json_classifier_state;

				static DAW_JSON_SIMD_CONSTEXPR auto
				is_whitespace( simd_type input ) {
					return simd_details::one_of<' ', '\t', '\n', '\r'>( input );
				}

				template<bool ValidateStart = true>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type
				classify_number( char const *first, std::size_t count,
				                 state_type &state )
				  requires( ExpectedType == JsonBaseParseTypes::Number ) {
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );

					auto const scalar = [&] {
						if constexpr( ValidateStart ) {
							auto const whitespace = is_whitespace( input );
							auto const operators = simd_details::one_of<']', ','>( input );
							return not( whitespace | operators );
						} else {
							// Unchecked parsing assumes valid JSON. All JSON whitespace is at
							// or below space, so the four whitespace comparisons collapse to
							// one while retaining array separators.
							auto const separators =
							  simd_details::one_of<' ', ']', ','>( input );
							return not separators;
						}
					}( );
					auto const valid_bits = simd_details::low_bits( count );
					auto const scalar_bits = scalar.to_ullong( ) & valid_bits;
					auto const decimal_point_bits =
					  ( input == simd_details::splat<simd_type>( '.' ) ).to_ullong( ) &
					  scalar_bits;
					auto const exponent_marker_bits =
					  simd_details::one_of<'e', 'E'>( input ).to_ullong( ) & scalar_bits;
					auto const follows_scalar_bits =
					  ( scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar = simd_details::last_bit( scalar_bits, count );
					auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
					auto const number_start_bits = [&] {
						if constexpr( ValidateStart ) {
							auto const number_start =
							  ( input == simd_details::splat<simd_type>( '-' ) ) |
							  ( ( input >= simd_details::splat<simd_type>( '0' ) ) &
							    ( input <= simd_details::splat<simd_type>( '9' ) ) );
							return number_start.to_ullong( ) & scalar_start_bits & valid_bits;
						} else {
							return scalar_start_bits;
						}
					}( );

					auto const offset = state.offset;
					state.offset += count;
					block_type result{ };
					result.data = first;
					result.offset = offset;
					result.size = count;
					result.scalar_start = scalar_start_bits;
					result.number_start = number_start_bits;
					result.number_characters = scalar_bits;
					result.decimal_points = decimal_point_bits;
					result.exponent_markers = exponent_marker_bits;
					result.state_after = state;
					return result;
				}

				template<bool ValidateStart = true>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type
				classify_bool( char const *first, std::size_t count,
				               state_type &state ) {
					static_assert( ExpectedType == JsonBaseParseTypes::Bool );
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );

					auto const true_start =
					  input == simd_details::splat<simd_type>( 't' );
					auto const boolean_start =
					  true_start | ( input == simd_details::splat<simd_type>( 'f' ) );
					auto const valid_bits = simd_details::low_bits( count );
					auto const boolean_bits = boolean_start.to_ullong( ) & valid_bits;
					auto const scalar_start_bits = [&] {
						if constexpr( ValidateStart ) {
							auto const whitespace = is_whitespace( input );
							auto const operators = simd_details::one_of<']', ','>( input );
							auto const scalar_bits =
							  ( not( whitespace | operators ) ).to_ullong( ) & valid_bits;
							auto const follows_scalar_bits =
							  ( scalar_bits << 1U ) |
							  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
							state.previous_scalar =
							  simd_details::last_bit( scalar_bits, count );
							return scalar_bits & ~follows_scalar_bits;
						} else {
							return boolean_bits;
						}
					}( );
					auto const boolean_start_bits = boolean_bits & scalar_start_bits;

					auto const offset = state.offset;
					state.offset += count;
					block_type result{ };
					result.data = first;
					result.offset = offset;
					result.size = count;
					result.scalar_start = scalar_start_bits;
					result.boolean_start = boolean_start_bits;
					result.true_start = true_start.to_ullong( ) & boolean_start_bits;
					result.state_after = state;
					return result;
				}

				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type
				classify_string( char const *first, std::size_t count,
				                 state_type &state ) {
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );

					auto const whitespace = is_whitespace( input );
					auto const operators = simd_details::one_of<']', ','>( input );
					auto const scalar = not( whitespace | operators );
					auto const quote = input == simd_details::splat<simd_type>( '"' );
					auto const backslash =
					  input == simd_details::splat<simd_type>( '\\' );

					auto const valid_bits = simd_details::low_bits( count );
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
					state.escaped = simd_details::last_bit( escape_bits, count );

					auto const quote_bits =
					  quote.to_ullong( ) & ~escaped_bits & valid_bits;
					auto const in_string_bits =
					  ( simd_details::prefix_xor( quote_bits ) ^
					    ( state.in_string ? valid_bits : std::uint64_t{ 0 } ) ) &
					  valid_bits;
					state.in_string = simd_details::last_bit( in_string_bits, count );
					auto const string_tail_bits = in_string_bits ^ quote_bits;
					auto const nonquote_scalar_bits = scalar_bits & ~quote_bits;
					auto const follows_scalar_bits =
					  ( nonquote_scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar =
					  simd_details::last_bit( nonquote_scalar_bits, count );
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
					result.scalar_start = value_start_bits;
					result.string_start = string_start_bits;
					result.state_after = state;
					return result;
				}
			};
		} // namespace json_details

		inline namespace experimental {
			/**
			 * Input iterator over JSON scalar values located using native
			 * SIMD-sized classified blocks.
			 * @tparam JsonMember The JSON Link mapping used to parse each value.
			 */
			template<typename JsonMember, typename CharT = char, auto... PolicyFlags>
			class json_simd_block_iterator {
				static_assert(
				  JsonMember::underlying_json_type == JsonBaseParseTypes::Number or
				    JsonMember::underlying_json_type == JsonBaseParseTypes::Bool or
				    JsonMember::underlying_json_type == JsonBaseParseTypes::String,
				  "json_simd_block_iterator currently supports number, boolean, and "
				  "string JsonMember types" );
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			requires( JsonMember::underlying_json_type ==
			          JsonBaseParseTypes::
			            Number ) class json_simd_block_iterator<JsonMember, CharT,
			                                                    PolicyFlags...> {
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  json_details::simd_json_classifier<JsonBaseParseTypes::Number, CharT>;
				using block_type =
				  json_details::simd_json_block<JsonBaseParseTypes::Number, CharT>;

				struct raw_number_json_member : JsonMember {
					using parse_to_t = JsonMember::wrapped_type;
					using constructor_t = default_constructor<parse_to_t>;
				};

			public:
				using json_member = JsonMember;
				using value_type = json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				char const *m_first = nullptr;
				char const *m_last = nullptr;
				char const *m_current = nullptr;
				char const *m_block_data = nullptr;
				std::uint64_t m_value_starts = 0;
				std::uint64_t m_number_characters = 0;
				std::uint64_t m_decimal_points = 0;
				std::uint64_t m_exponent_markers = 0;
				std::size_t m_block_size = 0;
				json_details::simd_json_classifier_state m_state{ };

				[[nodiscard]] constexpr ParseState number_parse_state( ) const {
					char const *number_last = nullptr;
					char const *decimal_point = nullptr;
					char const *exponent_marker = nullptr;

					auto collect_number_parts = [&]( char const *block_data,
					                                 std::size_t block_size,
					                                 std::uint64_t number_characters,
					                                 std::uint64_t decimal_points,
					                                 std::uint64_t exponent_markers,
					                                 std::size_t first_lane ) {
						auto const remaining = block_size - first_lane;
						auto const remaining_mask =
						  json_details::simd_details::low_bits( remaining );
						auto const characters =
						  ( number_characters >> first_lane ) & remaining_mask;
						auto const non_number_characters = ( ~characters ) & remaining_mask;
						auto const length = non_number_characters == 0
						                      ? remaining
						                      : static_cast<std::size_t>( std::countr_zero(
						                          non_number_characters ) );
						auto const number_mask =
						  json_details::simd_details::low_bits( length ) << first_lane;

						if( decimal_point == nullptr ) {
							auto const points = decimal_points & number_mask;
							if( points != 0 ) {
								decimal_point = block_data + std::countr_zero( points );
							}
						}
						if( exponent_marker == nullptr ) {
							auto const markers = exponent_markers & number_mask;
							if( markers != 0 ) {
								exponent_marker = block_data + std::countr_zero( markers );
							}
						}

						if( length != remaining or block_size < block_type::block_size ) {
							number_last = block_data + first_lane + length;
							return true;
						}
						return false;
					};

					auto const first_lane =
					  static_cast<std::size_t>( m_current - m_block_data );
					if( collect_number_parts( m_block_data,
					                          m_block_size,
					                          m_number_characters,
					                          m_decimal_points,
					                          m_exponent_markers,
					                          first_lane ) ) {
						return ParseState(
						  m_current, number_last, decimal_point, exponent_marker );
					}

					auto lookahead_first = m_first;
					auto lookahead_state = m_state;
					while( lookahead_first != m_last ) {
						auto const remaining =
						  static_cast<std::size_t>( m_last - lookahead_first );
						auto const block = classifier_type::template classify_number<
						  not ParseState::is_unchecked_input>(
						  lookahead_first, remaining, lookahead_state );
						if( collect_number_parts( block.data,
						                          block.size,
						                          block.number_characters,
						                          block.decimal_points,
						                          block.exponent_markers,
						                          0 ) ) {
							return ParseState(
							  m_current, number_last, decimal_point, exponent_marker );
						}
						lookahead_first += static_cast<std::ptrdiff_t>( block.size );
					}

					return ParseState(
					  m_current, m_last, decimal_point, exponent_marker );
				}

				constexpr void move_to_next_value( ) {
					while( m_value_starts == 0 and m_first != nullptr and
					       m_first != m_last ) {
						auto const remaining = static_cast<std::size_t>( m_last - m_first );
						auto const block = classifier_type::template classify_number<
						  not ParseState::is_unchecked_input>(
						  m_first, remaining, m_state );
						m_block_data = block.data;
						m_block_size = block.size;
						m_value_starts = block.number_start;
						m_number_characters = block.number_characters;
						m_decimal_points = block.decimal_points;
						m_exponent_markers = block.exponent_markers;
						if constexpr( not ParseState::is_unchecked_input ) {
							auto const unexpected_starts =
							  block.scalar_start & ~m_value_starts;
							if( unexpected_starts != 0 ) {
								auto const lane = static_cast<std::size_t>(
								  std::countr_zero( unexpected_starts ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error(
								  true, ErrorReason::InvalidNumberStart, error_state );
							}
						}
						m_first += static_cast<std::ptrdiff_t>( block.size );
					}

					if( m_value_starts == 0 ) {
						m_current = nullptr;
						return;
					}

					auto const lane =
					  static_cast<std::size_t>( std::countr_zero( m_value_starts ) );
					m_value_starts &= m_value_starts - 1U;
					m_current = m_block_data + lane;
				}

			public:
				constexpr json_simd_block_iterator( ) = default;

				explicit constexpr json_simd_block_iterator( std::string_view document )
				  : m_first( document.data( ) )
				  , m_last( std::next( document.data( ),
				                       static_cast<std::ptrdiff_t>( document.size( ) ) ) ) {
					if( m_first != m_last ) {
						auto parse_state = ParseState( m_first, m_last );
						parse_state.trim_left( );
						daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
						                      ErrorReason::InvalidArrayStart,
						                      parse_state );
						parse_state.remove_prefix( );
						m_first = parse_state.data( );
					}
					move_to_next_value( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					auto parse_state = number_parse_state( );
					auto parsed_value =
					  json_details::parse_value<raw_number_json_member,
					                            true,
					                            raw_number_json_member::expected_type>(
					    parse_state );
					using constructor_t = json_details::json_constructor_t<json_member>;
					return json_details::construct_value<value_type, constructor_t>(
					  parse_state, std::move( parsed_value ) );
				}

				constexpr json_simd_block_iterator &operator++( ) {
					move_to_next_value( );
					return *this;
				}

				constexpr void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
					return m_current != nullptr;
				}

				[[nodiscard]] constexpr json_simd_block_iterator begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_block_iterator end( ) const noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_block_iterator const &lhs,
				            json_simd_block_iterator const &rhs ) noexcept {
					auto const lhs_at_end = lhs.m_current == nullptr;
					auto const rhs_at_end = rhs.m_current == nullptr;

					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_current == rhs.m_current;
				}

				friend constexpr bool
				operator!=( json_simd_block_iterator const &lhs,
				            json_simd_block_iterator const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			requires( JsonMember::underlying_json_type ==
			          JsonBaseParseTypes::
			            Bool ) class json_simd_block_iterator<JsonMember, CharT,
			                                                  PolicyFlags...> {
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  json_details::simd_json_classifier<JsonBaseParseTypes::Bool, CharT>;

			public:
				using json_member = JsonMember;
				using value_type = json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				char const *m_first = nullptr;
				char const *m_last = nullptr;
				std::uint64_t m_boolean_values = 0;
				std::uint8_t m_value_index = 0;
				std::uint8_t m_value_count = 0;
				json_details::simd_json_classifier_state m_state{ };

				constexpr void validate_boolean( char const *first, bool value ) const {
					if constexpr( not ParseState::is_unchecked_input ) {
						auto parse_state = ParseState( first, m_last );
						if( value ) {
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
						    parse_policy_details::at_end_of_item( parse_state.front( ) ),
						  ErrorReason::InvalidEndOfValue,
						  parse_state );
					}
				}

				constexpr void fill_buffer( ) {
					m_boolean_values = 0;
					m_value_index = 0;
					m_value_count = 0;

					while( m_first != nullptr and m_first != m_last ) {
						auto const remaining = static_cast<std::size_t>( m_last - m_first );
						auto const block = classifier_type::template classify_bool<
						  not ParseState::is_unchecked_input>(
						  m_first, remaining, m_state );
						if constexpr( not ParseState::is_unchecked_input ) {
							auto const unexpected_starts =
							  block.scalar_start & ~block.boolean_start;
							if( unexpected_starts != 0 ) {
								auto const lane = static_cast<std::size_t>(
								  std::countr_zero( unexpected_starts ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error(
								  true, ErrorReason::InvalidLiteral, error_state );
							}
						}

						auto starts = block.boolean_start;
						while( starts != 0 ) {
							auto const lane =
							  static_cast<std::size_t>( std::countr_zero( starts ) );
							auto const value =
							  ( block.true_start & ( std::uint64_t{ 1 } << lane ) ) != 0;
							validate_boolean( block.data + lane, value );
							m_boolean_values |= static_cast<std::uint64_t>( value )
							                    << m_value_count;
							++m_value_count;
							starts &= starts - 1U;
							if( m_value_count == 64 ) {
								if( starts == 0 ) {
									m_first += static_cast<std::ptrdiff_t>( block.size );
								} else {
									auto const next_lane = static_cast<std::size_t>(
									  std::countr_zero( starts ) );
									m_first = block.data + next_lane;
									m_state = { };
								}
								return;
							}
						}
						m_first += static_cast<std::ptrdiff_t>( block.size );
					}
				}

			public:
				constexpr json_simd_block_iterator( ) = default;

				explicit constexpr json_simd_block_iterator( std::string_view document )
				  : m_first( document.data( ) )
				  , m_last( std::next( document.data( ),
				                       static_cast<std::ptrdiff_t>( document.size( ) ) ) ) {
					if( m_first != m_last ) {
						auto parse_state = ParseState( m_first, m_last );
						parse_state.trim_left( );
						daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
						                      ErrorReason::InvalidArrayStart,
						                      parse_state );
						parse_state.remove_prefix( );
						m_first = parse_state.data( );
					}
					fill_buffer( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					auto parse_state = ParseState( m_first, m_last );
					static_assert(
					  json_member::literal_as_string ==
					    options::LiteralAsStringOpt::Never,
					  "SIMD boolean iteration does not support literals encoded as "
					  "strings" );
					auto const value =
					  ( ( m_boolean_values >> m_value_index ) & std::uint64_t{ 1 } ) != 0;
					using constructor_t = json_details::json_constructor_t<json_member>;
					return json_details::construct_value<value_type, constructor_t>(
					  parse_state, value );
				}

				constexpr json_simd_block_iterator &operator++( ) {
					if( m_value_index < m_value_count ) {
						++m_value_index;
					}
					if( m_value_index == m_value_count ) {
						fill_buffer( );
					}
					return *this;
				}

				constexpr void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
					return m_value_index < m_value_count;
				}

				[[nodiscard]] constexpr json_simd_block_iterator begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_block_iterator end( ) const noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_block_iterator const &lhs,
				            json_simd_block_iterator const &rhs ) noexcept {
					auto const lhs_at_end = not lhs;
					auto const rhs_at_end = not rhs;
					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_first == rhs.m_first and
					       lhs.m_value_index == rhs.m_value_index and
					       lhs.m_value_count == rhs.m_value_count;
				}

				friend constexpr bool
				operator!=( json_simd_block_iterator const &lhs,
				            json_simd_block_iterator const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			requires( JsonMember::underlying_json_type ==
			          JsonBaseParseTypes::
			            String ) class json_simd_block_iterator<JsonMember, CharT,
			                                                    PolicyFlags...> {
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  json_details::simd_json_classifier<JsonBaseParseTypes::String, CharT>;

			public:
				using json_member = JsonMember;
				using value_type = json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				char const *m_first = nullptr;
				char const *m_last = nullptr;
				char const *m_current = nullptr;
				char const *m_block_data = nullptr;
				std::uint64_t m_value_starts = 0;
				json_details::simd_json_classifier_state m_state{ };

				constexpr void move_to_next_value( ) {
					while( m_value_starts == 0 and m_first != nullptr and
					       m_first != m_last ) {
						auto const remaining = static_cast<std::size_t>( m_last - m_first );
						auto const block =
						  classifier_type::classify_string( m_first, remaining, m_state );
						m_block_data = block.data;
						m_value_starts = block.string_start;
						if constexpr( not ParseState::is_unchecked_input ) {
							auto const unexpected_starts =
							  block.scalar_start & ~m_value_starts;
							if( unexpected_starts != 0 ) {
								auto const lane = static_cast<std::size_t>(
								  std::countr_zero( unexpected_starts ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error( true, ErrorReason::InvalidString, error_state );
							}
						}
						m_first += static_cast<std::ptrdiff_t>( block.size );
					}

					if( m_value_starts == 0 ) {
						m_current = nullptr;
						return;
					}

					auto const lane =
					  static_cast<std::size_t>( std::countr_zero( m_value_starts ) );
					m_value_starts &= m_value_starts - 1U;
					m_current = m_block_data + lane;
				}

			public:
				constexpr json_simd_block_iterator( ) = default;

				explicit constexpr json_simd_block_iterator( std::string_view document )
				  : m_first( document.data( ) )
				  , m_last( std::next( document.data( ),
				                       static_cast<std::ptrdiff_t>( document.size( ) ) ) ) {
					if( m_first != m_last ) {
						auto parse_state = ParseState( m_first, m_last );
						parse_state.trim_left( );
						daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
						                      ErrorReason::InvalidArrayStart,
						                      parse_state );
						parse_state.remove_prefix( );
						m_first = parse_state.data( );
					}
					move_to_next_value( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					auto parse_state = ParseState( m_current, m_last );
					return json_details::
					  parse_value<json_member, false, json_member::expected_type>(
					    parse_state );
				}

				constexpr json_simd_block_iterator &operator++( ) {
					move_to_next_value( );
					return *this;
				}

				constexpr void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
					return m_current != nullptr;
				}

				[[nodiscard]] constexpr json_simd_block_iterator begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_block_iterator end( ) const noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_block_iterator const &lhs,
				            json_simd_block_iterator const &rhs ) noexcept {
					auto const lhs_at_end = lhs.m_current == nullptr;
					auto const rhs_at_end = rhs.m_current == nullptr;
					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_current == rhs.m_current;
				}

				friend constexpr bool
				operator!=( json_simd_block_iterator const &lhs,
				            json_simd_block_iterator const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};
		} // namespace experimental
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
