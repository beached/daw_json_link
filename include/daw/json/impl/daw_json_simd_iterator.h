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

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_details {
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
			  simd_type splat( char value ) noexcept {
				return simd_type( static_cast<simd_type::value_type>( value ) );
			}

			template<auto... values, typename simd_type>
			[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR auto one_of( simd_type value ) {
				return ( ( value == splat<simd_type>( values ) ) | ... );
			}

			template<typename JsonMember,
			         std::size_t NumberSpanCacheBlocks =
			           ( JsonMember::expected_type == JsonParseTypes::Real ? 8U
			                                                               : 4U ),
			         typename CharT = char, auto... PolicyFlags>
			class json_simd_number_block_iterator;

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

			/**
			 * State carried from one SIMD block to the next. The three booleans
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
				// 64-lane ABI can require multiple native registers (or scalar
				// chunks).
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
				static constexpr std::size_t number_span_capacity =
				  ( simd_json_block_base<CharT>::block_size + 1U ) / 2U;

				std::uint64_t number_start = 0;
				std::uint64_t number_characters = 0;
				std::uint64_t decimal_points = 0;
				std::uint64_t exponent_markers = 0;
				std::uint64_t invalid_number_characters = 0;
				std::size_t number_span_count = 0;
			};

			template<JsonParseTypes NumberType>
			struct simd_number_span_types;

			template<>
			struct simd_number_span_types<JsonParseTypes::Real> {
				using span = number_span;
				using pending_span = pending_number_span;
			};

			template<>
			struct simd_number_span_types<JsonParseTypes::Signed> {
				using span = integer_span;
				using pending_span = pending_integer_span;
			};

			template<>
			struct simd_number_span_types<JsonParseTypes::Unsigned> {
				using span = integer_span;
				using pending_span = pending_integer_span;
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
				static constexpr std::size_t number_span_capacity =
				  ( block_size + 1U ) / 2U;
				static_assert( block_size <= 64,
				               "The classifier bit set stores at most 64 SIMD lanes" );

			public:
				using state_type = simd_json_classifier_state;

				static DAW_JSON_SIMD_CONSTEXPR auto is_whitespace( simd_type input ) {
					return simd_details::one_of<' ', '\t', '\n', '\r'>( input );
				}

				template<JsonParseTypes NumberType, bool ValidateStart = true,
				         std::size_t NumberSpanCapacity>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type classify_number(
				  char const *first, std::size_t count, state_type &state,
				  std::array<typename simd_number_span_types<NumberType>::span,
				             NumberSpanCapacity> &number_spans,
				  typename simd_number_span_types<NumberType>::pending_span
				    &pending_number,
				  std::size_t number_span_offset = 0 )
				  requires( ExpectedType == JsonBaseParseTypes::Number ) {
					static_assert( NumberType == JsonParseTypes::Real or
					               NumberType == JsonParseTypes::Signed or
					               NumberType == JsonParseTypes::Unsigned );
					static_assert( NumberSpanCapacity >= number_span_capacity );
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );

					auto const scalar = [&] {
						if constexpr( ValidateStart ) {
							auto const whitespace = is_whitespace( input );
							auto const operators = simd_details::one_of<']', ','>( input );
							return not( whitespace | operators );
						} else {
							// Unchecked parsing assumes valid JSON. All JSON whitespace is
							// at or below space, so the four whitespace comparisons
							// collapse to one while retaining array separators.
							auto const separators =
							  simd_details::one_of<' ', ']', ','>( input );
							return not separators;
						}
					}( );
					auto const valid_bits = simd_details::low_bits( count );
					auto const scalar_bits = scalar.to_ullong( ) & valid_bits;
					auto const digit_bits = [&] {
						if constexpr( ValidateStart ) {
							return ( ( input >= simd_details::splat<simd_type>( '0' ) ) &
							         ( input <= simd_details::splat<simd_type>( '9' ) ) )
							         .to_ullong( ) &
							       scalar_bits;
						} else {
							return std::uint64_t{ 0 };
						}
					}( );
					auto const decimal_point_bits = [&] {
						if constexpr( NumberType == JsonParseTypes::Real ) {
							return ( input == simd_details::splat<simd_type>( '.' ) )
							         .to_ullong( ) &
							       scalar_bits;
						} else {
							return std::uint64_t{ 0 };
						}
					}( );
					auto const exponent_marker_bits = [&] {
						if constexpr( NumberType == JsonParseTypes::Real ) {
							return simd_details::one_of<'e', 'E'>( input ).to_ullong( ) &
							       scalar_bits;
						} else {
							return std::uint64_t{ 0 };
						}
					}( );
					auto const follows_scalar_bits =
					  ( scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar = simd_details::last_bit( scalar_bits, count );
					auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
					auto const number_start_bits = [&] {
						if constexpr( ValidateStart ) {
							auto start_bits = digit_bits;
							if constexpr( NumberType != JsonParseTypes::Unsigned ) {
								start_bits |= ( input == simd_details::splat<simd_type>( '-' ) )
								                .to_ullong( );
							}
							return start_bits & scalar_start_bits & valid_bits;
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
					if constexpr( ValidateStart and NumberType != JsonParseTypes::Real ) {
						auto valid_integer_characters = digit_bits;
						if constexpr( NumberType == JsonParseTypes::Signed ) {
							auto const minus_bits =
							  ( input == simd_details::splat<simd_type>( '-' ) ).to_ullong( );
							valid_integer_characters |= minus_bits & number_start_bits;
						}
						result.invalid_number_characters =
						  scalar_bits & ~valid_integer_characters;
					}

					auto append_number_span = [&]( auto pending,
					                               std::size_t first_lane ) {
						auto const remaining = count - first_lane;
						auto const remaining_mask = simd_details::low_bits( remaining );
						auto const characters =
						  ( scalar_bits >> first_lane ) & remaining_mask;
						auto const non_number_characters = ( ~characters ) & remaining_mask;
						auto const length = non_number_characters == 0
						                      ? remaining
						                      : static_cast<std::size_t>( std::countr_zero(
						                          non_number_characters ) );
						auto const number_mask = simd_details::low_bits( length )
						                         << first_lane;

						if constexpr( NumberType == JsonParseTypes::Real ) {
							if( pending.decimal_point == nullptr ) {
								auto const points = decimal_point_bits & number_mask;
								if( points != 0 ) {
									pending.decimal_point = first + std::countr_zero( points );
								}
							}
							if( pending.exponent_marker == nullptr ) {
								auto const markers = exponent_marker_bits & number_mask;
								if( markers != 0 ) {
									pending.exponent_marker = first + std::countr_zero( markers );
								}
							}
						}

						if( length != remaining or count < block_size ) {
							if constexpr( NumberType == JsonParseTypes::Real ) {
								number_spans[number_span_offset + result.number_span_count++] =
								  number_span{ pending.first,
								               first + first_lane + length,
								               pending.decimal_point,
								               pending.exponent_marker };
							} else {
								number_spans[number_span_offset + result.number_span_count++] =
								  integer_span{ pending.first, first + first_lane + length };
							}
							pending_number = { };
						} else {
							pending_number = pending;
						}
					};

					if( pending_number.first != nullptr ) {
						append_number_span( pending_number, 0 );
					}

					auto starts = number_start_bits;
					while( starts != 0 ) {
						auto const lane =
						  static_cast<std::size_t>( std::countr_zero( starts ) );
						if constexpr( NumberType == JsonParseTypes::Real ) {
							append_number_span(
							  pending_number_span{ first + lane, nullptr, nullptr }, lane );
						} else {
							append_number_span( pending_integer_span{ first + lane }, lane );
						}
						starts &= starts - 1U;
					}
					result.state_after = state;
					return result;
				}

				template<bool ValidateStart = true>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type classify_bool(
				  char const *first, std::size_t count, state_type &state ) {
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

				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type classify_string(
				  char const *first, std::size_t count, state_type &state ) {
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

			template<typename JsonMember, std::size_t NumberSpanCacheBlocks,
			         typename CharT, auto... PolicyFlags>
			requires( JsonMember::underlying_json_type ==
			          JsonBaseParseTypes::Number ) //
			  class json_simd_number_block_iterator<JsonMember, NumberSpanCacheBlocks,
			                                        CharT, PolicyFlags...> {
				static_assert( NumberSpanCacheBlocks > 0 );
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  simd_json_classifier<JsonBaseParseTypes::Number, CharT>;
				using block_type = simd_json_block<JsonBaseParseTypes::Number, CharT>;
				static constexpr auto number_type = JsonMember::expected_type;
				static_assert( number_type == JsonParseTypes::Real or
				               number_type == JsonParseTypes::Signed or
				               number_type == JsonParseTypes::Unsigned );
				using span_types = simd_number_span_types<number_type>;
				using span_type = span_types::span;
				using pending_span_type = span_types::pending_span;
				static constexpr std::size_t number_span_capacity =
				  block_type::number_span_capacity * NumberSpanCacheBlocks;

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
				std::array<span_type, number_span_capacity> m_number_spans{ };
				std::size_t m_value_index = 0;
				std::size_t m_value_count = 0;
				simd_json_classifier_state m_state{ };
				pending_span_type m_pending_number{ };

				constexpr void fill_buffer( ) {
					m_value_index = 0;
					m_value_count = 0;
					while( m_first != nullptr and m_first != m_last and
					       m_value_count + block_type::number_span_capacity <=
					         m_number_spans.size( ) ) {
						auto const remaining = static_cast<std::size_t>( m_last - m_first );
						auto const block = classifier_type::template classify_number<
						  number_type,
						  not ParseState::is_unchecked_input>( m_first,
						                                       remaining,
						                                       m_state,
						                                       m_number_spans,
						                                       m_pending_number,
						                                       m_value_count );
						if constexpr( not ParseState::is_unchecked_input ) {
							auto const unexpected_starts =
							  block.scalar_start & ~block.number_start;
							if( unexpected_starts != 0 ) {
								auto const lane = static_cast<std::size_t>(
								  std::countr_zero( unexpected_starts ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error(
								  true, ErrorReason::InvalidNumberStart, error_state );
							}
							if( block.invalid_number_characters != 0 ) {
								auto const lane = static_cast<std::size_t>(
								  std::countr_zero( block.invalid_number_characters ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error( true, ErrorReason::InvalidNumber, error_state );
							}
						}
						m_value_count += block.number_span_count;
						m_first += static_cast<std::ptrdiff_t>( block.size );
					}
				}

			public:
				constexpr json_simd_number_block_iterator( ) = default;

				explicit constexpr json_simd_number_block_iterator(
				  std::string_view document )
				  : m_first( document.data( ) )
				  , m_last( std::next( document.data( ), static_cast<std::ptrdiff_t>(
				                                           document.size( ) ) ) ) {
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
					auto const &span = m_number_spans[m_value_index];
					auto parse_state = [&] {
						if constexpr( number_type == JsonParseTypes::Real ) {
							return ParseState( span.first,
							                   span.last,
							                   span.decimal_point,
							                   span.exponent_marker );
						} else {
							return ParseState( span.first, span.last );
						}
					}( );
					auto parsed_value =
					  json_details::parse_value<raw_number_json_member,
					                            true,
					                            raw_number_json_member::expected_type>(
					    parse_state );
					using constructor_t = json_details::json_constructor_t<json_member>;
					return json_details::construct_value<value_type, constructor_t>(
					  parse_state, std::move( parsed_value ) );
				}

				constexpr json_simd_number_block_iterator &operator++( ) {
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

				[[nodiscard]] constexpr json_simd_number_block_iterator begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_number_block_iterator
				end( ) const noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_number_block_iterator const &lhs,
				            json_simd_number_block_iterator const &rhs ) noexcept {
					auto const lhs_at_end = not lhs;
					auto const rhs_at_end = not rhs;

					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_number_spans[lhs.m_value_index].first ==
					       rhs.m_number_spans[rhs.m_value_index].first;
				}

				friend constexpr bool
				operator!=( json_simd_number_block_iterator const &lhs,
				            json_simd_number_block_iterator const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};
		} // namespace json_details::simd_details

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
			          JsonBaseParseTypes::Number ) //
			  class json_simd_block_iterator<JsonMember, CharT, PolicyFlags...>
			  : public json_details::simd_details::json_simd_number_block_iterator<
			      JsonMember,
			      ( JsonMember::expected_type == JsonParseTypes::Real ? 8U : 4U ),
			      CharT, PolicyFlags...> {
				using base =
				  json_details::simd_details::json_simd_number_block_iterator<
				    JsonMember,
				    ( JsonMember::expected_type == JsonParseTypes::Real ? 8U : 4U ),
				    CharT, PolicyFlags...>;

			public:
				using base::base;
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			requires( JsonMember::underlying_json_type ==
			          JsonBaseParseTypes::Bool ) //
			  class json_simd_block_iterator<JsonMember, CharT, PolicyFlags...> {
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  json_details::simd_details::simd_json_classifier<
				    JsonBaseParseTypes::Bool, CharT>;

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
				json_details::simd_details::simd_json_classifier_state m_state{ };

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
									auto const next_lane =
									  static_cast<std::size_t>( std::countr_zero( starts ) );
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
				  , m_last( std::next( document.data( ), static_cast<std::ptrdiff_t>(
				                                           document.size( ) ) ) ) {
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
			          JsonBaseParseTypes::String ) //
			  class json_simd_block_iterator<JsonMember, CharT, PolicyFlags...> {
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  json_details::simd_details::simd_json_classifier<JsonBaseParseTypes::String, CharT>;

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
				json_details::simd_details::simd_json_classifier_state m_state{ };

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
				  , m_last( std::next( document.data( ), static_cast<std::ptrdiff_t>(
				                                           document.size( ) ) ) ) {
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
