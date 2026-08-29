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

#include <daw/daw_span.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <type_traits>

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
				return simd_type(
				  static_cast<typename simd_type::value_type>( value ) );
			}

			template<auto... values, typename simd_type>
			[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR auto one_of( simd_type value ) {
				return ( ( value == splat<simd_type>( values ) ) | ... );
			}

			template<typename simd_type, std::size_t block_size, typename CharT,
			         typename Chr>
			[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR simd_type
			load( Chr const *first, std::size_t count ) {
				static constexpr auto flags = [] {
					if constexpr( std::is_same_v<CharT, Chr> ) {
						return daw::simd::flag_default;
					} else {
						return daw::simd::flag_convert;
					}
				}( );
				if( count == block_size ) {
					return daw::simd::unchecked_load<simd_type>(
					  daw::span( first, block_size ), flags );
				}
				return daw::simd::partial_load<simd_type>( daw::span( first, count ),
				                                           flags );
			}

			/**
			 * State carried from one SIMD block to the next. The three booleans
			 * correspond to the cross-register dependencies in simdjson's stage 1:
			 * string continuation, escape continuation, and scalar continuation.
			 */
			struct simd_json_classifier_state {
				bool in_string = false;
				bool escaped = false;
				bool previous_scalar = false;
			};

			struct simd_array_grammar_state {
				bool previous_event_was_value = false;
				bool saw_value = false;
				bool started = false;
				bool ended = false;
			};

			template<typename CharT>
			struct simd_json_block_base {
				// Classify one native SIMD register at a time. The iterators aggregate
				// results from as many blocks as are needed to fill their caches.
				using simd_type = daw::simd::vec<CharT>;

				static constexpr std::size_t block_size =
				  static_cast<std::size_t>( simd_type::size( ) );

				char const *data = nullptr;
				std::size_t size = 0;

				std::uint64_t scalar_start = 0;
				std::uint64_t comma = 0;
				std::uint64_t array_end = 0;
			};

			template<typename ParseState>
			constexpr void validate_array_events( char const *data, char const *last,
			                                      std::uint64_t value_starts,
			                                      std::uint64_t commas,
			                                      std::uint64_t array_end,
			                                      simd_array_grammar_state &state ) {
				auto events = value_starts | commas | array_end;
				while( events != 0 ) {
					auto const lane = static_cast<std::size_t>(
					  daw::cxmath::count_trailing_zeros( events ) );
					auto const bit = std::uint64_t{ 1 } << lane;
					if( ( array_end & bit ) != 0 ) {
						if( state.saw_value and not state.previous_event_was_value ) {
							auto error_state = ParseState( data + lane, last );
							daw_json_error( true, ErrorReason::TrailingComma, error_state );
						}
						state.ended = true;
					} else if( ( commas & bit ) != 0 ) {
						if( not state.previous_event_was_value ) {
							auto error_state = ParseState( data + lane, last );
							daw_json_error(
							  true, ErrorReason::InvalidStartOfValue, error_state );
						}
						state.previous_event_was_value = false;
					} else {
						if( state.previous_event_was_value ) {
							auto error_state = ParseState( data + lane, last );
							daw_json_error(
							  true, ErrorReason::InvalidEndOfValue, error_state );
						}
						state.previous_event_was_value = true;
						state.saw_value = true;
					}
					events &= events - 1U;
				}
			}

			template<typename ParseState>
			constexpr void
			validate_array_ended( char const *last,
			                      simd_array_grammar_state const &state ) {
				if( state.started and not state.ended ) {
					auto error_state = ParseState( last, last );
					daw_json_error( true, ErrorReason::UnexpectedEndOfData, error_state );
				}
			}

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
				std::uint64_t boolean_values = 0;
			};

			template<typename CharT>
			struct simd_json_block<JsonBaseParseTypes::String, CharT>
			  : simd_json_block_base<CharT> {
				std::uint64_t string_start = 0;
				std::uint64_t string_end = 0;
				std::uint64_t escape_characters = 0;
			};

			template<JsonBaseParseTypes ExpectedType, typename CharT>
			class simd_json_classifier {
				static_assert(
				  ExpectedType == JsonBaseParseTypes::Number or
				    ExpectedType == JsonBaseParseTypes::Bool or
				    ExpectedType == JsonBaseParseTypes::String,
				  "simd_json_classifier supports only Number, Bool, and String" );
				using block_type = simd_json_block<ExpectedType, CharT>;
				using simd_type = typename block_type::simd_type;
				using simd_value_type = typename simd_type::value_type;

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
				  std::size_t number_span_offset = 0 ) {

					static_assert( ExpectedType == JsonBaseParseTypes::Number );
					static_assert( NumberType == JsonParseTypes::Real or
					               NumberType == JsonParseTypes::Signed or
					               NumberType == JsonParseTypes::Unsigned );
					static_assert( NumberSpanCapacity >= number_span_capacity );
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );
					auto const comma = input == simd_details::splat<simd_type>( ',' );
					auto const array_end = input == simd_details::splat<simd_type>( ']' );
					auto const valid_bits = simd_details::low_bits( count );
					auto const array_end_bits = array_end.to_ullong( ) & valid_bits;
					auto const active_bits =
					  array_end_bits == 0
					    ? valid_bits
					    : simd_details::low_bits(
					        static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
					          static_cast<std::uint64_t>( array_end_bits ) ) ) );

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
					auto const scalar_bits = scalar.to_ullong( ) & active_bits;
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

					block_type result{ };
					result.data = first;
					result.size = count;
					result.scalar_start = scalar_start_bits;
					result.comma = comma.to_ullong( ) & active_bits;
					result.array_end = array_end_bits;
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
						auto const length =
						  non_number_characters == 0
						    ? remaining
						    : static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
						        static_cast<std::uint64_t>( non_number_characters ) ) );
						auto const number_mask = simd_details::low_bits( length )
						                         << first_lane;

						if constexpr( NumberType == JsonParseTypes::Real ) {
							if( pending.decimal_point == nullptr ) {
								auto const points = decimal_point_bits & number_mask;
								if( points != 0 ) {
									pending.decimal_point =
									  first + daw::cxmath::count_trailing_zeros(
									            static_cast<std::uint64_t>( points ) );
								}
							}
							if( pending.exponent_marker == nullptr ) {
								auto const markers = exponent_marker_bits & number_mask;
								if( markers != 0 ) {
									pending.exponent_marker =
									  first + daw::cxmath::count_trailing_zeros(
									            static_cast<std::uint64_t>( markers ) );
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
						  static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
						    static_cast<std::uint64_t>( starts ) ) );
						if constexpr( NumberType == JsonParseTypes::Real ) {
							append_number_span(
							  pending_number_span{ first + lane, nullptr, nullptr }, lane );
						} else {
							append_number_span( pending_integer_span{ first + lane }, lane );
						}
						starts &= starts - 1U;
					}
					return result;
				}

				template<bool ValidateStart = true>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type classify_bool(
				  char const *first, std::size_t count, state_type &state ) {
					static_assert( ExpectedType == JsonBaseParseTypes::Bool );
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );
					auto const comma = input == simd_details::splat<simd_type>( ',' );
					auto const array_end = input == simd_details::splat<simd_type>( ']' );

					auto const true_start =
					  input == simd_details::splat<simd_type>( 't' );
					auto const boolean_start =
					  true_start | ( input == simd_details::splat<simd_type>( 'f' ) );
					auto const valid_bits = simd_details::low_bits( count );
					auto const array_end_bits = array_end.to_ullong( ) & valid_bits;
					auto const active_bits =
					  array_end_bits == 0
					    ? valid_bits
					    : simd_details::low_bits(
					        static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
					          static_cast<std::uint64_t>( array_end_bits ) ) ) );
					auto const boolean_bits = boolean_start.to_ullong( ) & active_bits;
					auto const scalar_start_bits = [&] {
						if constexpr( ValidateStart ) {
							auto const whitespace = is_whitespace( input );
							auto const operators = simd_details::one_of<']', ','>( input );
							auto const scalar_bits =
							  ( not( whitespace | operators ) ).to_ullong( ) & active_bits;
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

					block_type result{ };
					result.data = first;
					result.size = count;
					result.scalar_start = scalar_start_bits;
					result.comma = comma.to_ullong( ) & active_bits;
					result.array_end = array_end_bits;
					result.boolean_start = boolean_start_bits;
					result.boolean_values =
					  daw::simd_impl::compress_bits( true_start, boolean_start_bits );
					return result;
				}

				template<bool ValidateStart = true>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR block_type classify_string(
				  char const *first, std::size_t count, state_type &state ) {
					count = count < block_size ? count : block_size;
					auto const input =
					  simd_details::load<simd_type, block_size, CharT>( first, count );

					auto const quote = input == simd_details::splat<simd_type>( '"' );
					auto const backslash =
					  input == simd_details::splat<simd_type>( '\\' );
					auto const comma = input == simd_details::splat<simd_type>( ',' );
					auto const array_end = input == simd_details::splat<simd_type>( ']' );

					auto const valid_bits = low_bits( count );
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
					auto const outside_string_bits = ~string_tail_bits & valid_bits;
					auto const array_end_bits =
					  array_end.to_ullong( ) & outside_string_bits;
					auto const active_bits =
					  array_end_bits == 0
					    ? valid_bits
					    : simd_details::low_bits(
					        static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
					          static_cast<std::uint64_t>( array_end_bits ) ) ) );
					auto value_start_bits = std::uint64_t{ 0 };
					auto string_start_bits = quote_bits & in_string_bits & active_bits;
					if constexpr( ValidateStart ) {
						auto const whitespace = is_whitespace( input );
						auto const operators = simd_details::one_of<']', ','>( input );
						auto const scalar = not( whitespace | operators );
						auto const operator_bits = operators.to_ullong( ) & active_bits;
						auto const scalar_bits = scalar.to_ullong( ) & active_bits;
						auto const nonquote_scalar_bits = scalar_bits & ~quote_bits;
						auto const follows_scalar_bits =
						  ( nonquote_scalar_bits << 1U ) |
						  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
						state.previous_scalar =
						  simd_details::last_bit( nonquote_scalar_bits, count );
						auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
						value_start_bits =
						  scalar_start_bits & ~string_tail_bits & active_bits;
						auto const structural_bits = ( operator_bits | scalar_start_bits ) &
						                             ~string_tail_bits & active_bits;
						string_start_bits &= structural_bits;
					}
					auto const string_end_bits = quote_bits & ~in_string_bits;

					block_type result{ };
					result.data = first;
					result.size = count;
					result.scalar_start = value_start_bits;
					result.comma = comma.to_ullong( ) & outside_string_bits & active_bits;
					result.array_end = array_end_bits;
					result.string_start = string_start_bits;
					result.string_end = string_end_bits & active_bits;
					result.escape_characters = backslash_bits & active_bits;
					return result;
				}
			};
		} // namespace json_details::simd_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
