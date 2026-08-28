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

					block_type result{ };
					result.data = first;
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

					block_type result{ };
					result.data = first;
					result.size = count;
					result.scalar_start = scalar_start_bits;
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
					auto value_start_bits = std::uint64_t{ 0 };
					auto string_start_bits = quote_bits & in_string_bits;
					if constexpr( ValidateStart ) {
						auto const whitespace = is_whitespace( input );
						auto const operators = simd_details::one_of<']', ','>( input );
						auto const scalar = not( whitespace | operators );
						auto const operator_bits = operators.to_ullong( ) & valid_bits;
						auto const scalar_bits = scalar.to_ullong( ) & valid_bits;
						auto const string_tail_bits = in_string_bits ^ quote_bits;
						auto const nonquote_scalar_bits = scalar_bits & ~quote_bits;
						auto const follows_scalar_bits =
						  ( nonquote_scalar_bits << 1U ) |
						  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
						state.previous_scalar =
						  simd_details::last_bit( nonquote_scalar_bits, count );
						auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
						value_start_bits =
						  scalar_start_bits & ~string_tail_bits & valid_bits;
						auto const structural_bits = ( operator_bits | scalar_start_bits ) &
						                             ~string_tail_bits & valid_bits;
						string_start_bits &= structural_bits;
					}
					auto const string_end_bits = quote_bits & ~in_string_bits;

					block_type result{ };
					result.data = first;
					result.size = count;
					result.scalar_start = value_start_bits;
					result.string_start = string_start_bits;
					result.string_end = string_end_bits;
					result.escape_characters = backslash_bits;
					return result;
				}
			};

			template<typename JsonMember,
			         std::size_t NumberSpanCacheBlocks =
			           ( JsonMember::expected_type == JsonParseTypes::Real ? 8U
			                                                               : 4U ),
			         typename CharT = char, auto... PolicyFlags>
			class json_simd_number_block_iterator {
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
				using span_type = typename span_types::span;
				using pending_span_type = typename span_types::pending_span;
				static constexpr std::size_t number_span_capacity =
				  block_type::number_span_capacity * NumberSpanCacheBlocks;

				struct raw_number_json_member : JsonMember {
					using parse_to_t = typename JsonMember::wrapped_type;
					using constructor_t = default_constructor<parse_to_t>;
				};

			public:
				using json_member = JsonMember;
				using value_type = typename json_member::parse_to_t;
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
								  daw::cxmath::count_trailing_zeros( unexpected_starts ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error(
								  true, ErrorReason::InvalidNumberStart, error_state );
							}
							if( block.invalid_number_characters != 0 ) {
								auto const lane =
								  static_cast<std::size_t>( daw::cxmath::count_trailing_zeros(
								    block.invalid_number_characters ) );
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
					return json_details::parse_value<
					  raw_number_json_member,
					  true,
					  raw_number_json_member::expected_type>( parse_state );
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

				[[nodiscard]] constexpr json_simd_number_block_iterator static end( ) noexcept {
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

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			class json_simd_block_iterator_number
			  : public json_details::simd_details::json_simd_number_block_iterator<
			      JsonMember,
			      ( JsonMember::expected_type == JsonParseTypes::Real ? 8U : 4U ),
			      CharT, PolicyFlags...> {
				static_assert( JsonMember::underlying_json_type ==
				               JsonBaseParseTypes::Number );
				using base =
				  json_details::simd_details::json_simd_number_block_iterator<
				    JsonMember,
				    ( JsonMember::expected_type == JsonParseTypes::Real ? 8U : 4U ),
				    CharT, PolicyFlags...>;

			public:
				using base::base;
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			class json_simd_block_iterator_bool {
				static_assert( JsonMember::underlying_json_type ==
				               JsonBaseParseTypes::Bool );
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  simd_json_classifier<JsonBaseParseTypes::Bool, CharT>;

			public:
				using json_member = JsonMember;
				using value_type = typename json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				static constexpr std::uint8_t boolean_cache_capacity = 64U;

				char const *m_first = nullptr;
				char const *m_last = nullptr;
				std::uint64_t m_boolean_values = 0;
				std::uint8_t m_value_index = 0;
				std::uint8_t m_value_count = 0;
				simd_json_classifier_state m_state{ };

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
								  daw::cxmath::count_trailing_zeros( unexpected_starts ) );
								auto error_state = ParseState( block.data + lane, m_last );
								daw_json_error(
								  true, ErrorReason::InvalidLiteral, error_state );
							}
						}

						auto starts = block.boolean_start;
						auto const block_value_count =
						  static_cast<std::uint8_t>( daw::cxmath::popcount( starts ) );
						auto const available = static_cast<std::uint8_t>(
						  boolean_cache_capacity - m_value_count );
						auto const consumed =
						  block_value_count < available ? block_value_count : available;

						if constexpr( not ParseState::is_unchecked_input ) {
							auto validation_starts = starts;
							auto values = block.boolean_values;
							for( std::uint8_t n = 0; n < consumed; ++n ) {
								auto const lane = static_cast<std::size_t>(
								  daw::cxmath::count_trailing_zeros( validation_starts ) );
								validate_boolean( block.data + lane,
								                  ( values & std::uint64_t{ 1 } ) != 0 );
								validation_starts &= validation_starts - 1U;
								values >>= 1U;
							}
						}

						m_boolean_values |= block.boolean_values
						                    << static_cast<unsigned>( m_value_count );
						m_value_count =
						  static_cast<std::uint8_t>( m_value_count + consumed );
						if( consumed == block_value_count ) {
							m_first += static_cast<std::ptrdiff_t>( block.size );
							if( m_value_count == boolean_cache_capacity ) {
								return;
							}
							continue;
						}

						for( std::uint8_t n = 0; n < consumed; ++n ) {
							starts &= starts - 1U;
						}
						auto const next_lane = static_cast<std::size_t>(
						  daw::cxmath::count_trailing_zeros( starts ) );
						m_first = block.data + next_lane;
						m_state = { };
						return;
					}
				}

			public:
				constexpr json_simd_block_iterator_bool( ) = default;

				explicit constexpr json_simd_block_iterator_bool(
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
					static_assert(
					  json_member::literal_as_string ==
					    options::LiteralAsStringOpt::Never,
					  "SIMD boolean iteration does not support literals encoded as "
					  "strings" );
					auto const value =
					  ( ( m_boolean_values >> m_value_index ) & std::uint64_t{ 1 } ) != 0;
					using constructor_t = json_constructor_t<json_member>;
					static_assert(
					  daw::is_callable_v<constructor_t, bool>,
					  "Unable to construct value with the supplied arguments" );
					return constructor_t{ }( value );
				}

				constexpr json_simd_block_iterator_bool &operator++( ) {
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

				[[nodiscard]] constexpr json_simd_block_iterator_bool begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_block_iterator_bool static end( ) noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_block_iterator_bool const &lhs,
				            json_simd_block_iterator_bool const &rhs ) noexcept {
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
				operator!=( json_simd_block_iterator_bool const &lhs,
				            json_simd_block_iterator_bool const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			class json_simd_block_iterator_string {
				static_assert( JsonMember::underlying_json_type ==
				               JsonBaseParseTypes::String );
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  simd_json_classifier<JsonBaseParseTypes::String, CharT>;

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
				char const *m_current_end = nullptr;
				char const *m_current_escape = nullptr;
				char const *m_block_data = nullptr;
				std::uint64_t m_value_starts = 0;
				std::uint64_t m_string_ends = 0;
				std::uint64_t m_escape_characters = 0;
				simd_json_classifier_state m_state{ };

				[[nodiscard]] constexpr bool classify_next_block( ) {
					if( m_first == nullptr or m_first == m_last ) {
						return false;
					}

					auto const remaining = static_cast<std::size_t>( m_last - m_first );
					auto const block = classifier_type::template classify_string<
					  not ParseState::is_unchecked_input>( m_first, remaining, m_state );
					m_block_data = block.data;
					m_value_starts = block.string_start;
					m_string_ends = block.string_end;
					m_escape_characters = block.escape_characters;
					if constexpr( not ParseState::is_unchecked_input ) {
						auto const unexpected_starts = block.scalar_start & ~m_value_starts;
						if( unexpected_starts != 0 ) {
							auto const lane = static_cast<std::size_t>(
							  daw::cxmath::count_trailing_zeros( unexpected_starts ) );
							auto error_state = ParseState( block.data + lane, m_last );
							daw_json_error( true, ErrorReason::InvalidString, error_state );
						}
					}
					m_first += static_cast<std::ptrdiff_t>( block.size );
					return true;
				}

				constexpr void move_to_next_value( ) {
					while( m_value_starts == 0 ) {
						if( not classify_next_block( ) ) {
							m_current = nullptr;
							m_current_end = nullptr;
							m_current_escape = nullptr;
							return;
						}
					}

					auto const lane = static_cast<std::size_t>(
					  daw::cxmath::count_trailing_zeros( m_value_starts ) );
					m_value_starts &= m_value_starts - 1U;
					m_current = m_block_data + lane;
					m_current_escape = nullptr;

					auto first_lane = lane + 1U;
					while( true ) {
						if( m_string_ends != 0 ) {
							auto const end_lane = static_cast<std::size_t>(
							  daw::cxmath::count_trailing_zeros( m_string_ends ) );
							auto const escapes = m_escape_characters &
							                     simd_details::low_bits( end_lane ) &
							                     ~simd_details::low_bits( first_lane );
							if( m_current_escape == nullptr and escapes != 0 ) {
								m_current_escape =
								  m_block_data + daw::cxmath::count_trailing_zeros( escapes );
							}
							m_string_ends &= m_string_ends - 1U;
							m_current_end = m_block_data + end_lane;
							return;
						}

						auto const escapes =
						  m_escape_characters & ~simd_details::low_bits( first_lane );
						if( m_current_escape == nullptr and escapes != 0 ) {
							m_current_escape =
							  m_block_data + daw::cxmath::count_trailing_zeros( escapes );
						}
						if( not classify_next_block( ) ) {
							auto error_state = ParseState( m_current, m_last );
							daw_json_error( true, ErrorReason::InvalidString, error_state );
						}
						first_lane = 0;
					}
				}

			public:
				constexpr json_simd_block_iterator_string( ) = default;

				explicit constexpr json_simd_block_iterator_string(
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
					move_to_next_value( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					auto parse_state = ParseState( m_current + 1, m_current_end );
					parse_state.counter = m_current_escape == nullptr
					                        ? static_cast<std::size_t>( -1 )
					                        : static_cast<std::size_t>(
					                            m_current_escape - ( m_current + 1 ) );
					return json_details::
					  parse_value<json_member, true, json_member::expected_type>(
					    parse_state );
				}

				constexpr json_simd_block_iterator_string &operator++( ) {
					move_to_next_value( );
					return *this;
				}

				constexpr void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
					return m_current != nullptr;
				}

				[[nodiscard]] constexpr json_simd_block_iterator_string begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_block_iterator_string static end( ) noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_block_iterator_string const &lhs,
				            json_simd_block_iterator_string const &rhs ) noexcept {
					auto const lhs_at_end = lhs.m_current == nullptr;
					auto const rhs_at_end = rhs.m_current == nullptr;
					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_current == rhs.m_current;
				}

				friend constexpr bool
				operator!=( json_simd_block_iterator_string const &lhs,
				            json_simd_block_iterator_string const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};

			template<typename JsonMember>
			struct unknown_json_simd_block_iterator_error;
		} // namespace json_details::simd_details

		inline namespace experimental {
			/**
			 * Input iterator over JSON scalar values located using native
			 * SIMD-sized classified blocks.
			 * @tparam JsonMember The JSON Link mapping used to parse each value.
			 */
			template<typename JsonMember, typename CharT = char, auto... PolicyFlags>
			using json_simd_block_iterator = std::conditional_t<
			  ( JsonMember::underlying_json_type == JsonBaseParseTypes::Bool ),
			  json_details::simd_details::json_simd_block_iterator_bool<
			    JsonMember, CharT, PolicyFlags...>,
			  std::conditional_t<
			    ( JsonMember::underlying_json_type == JsonBaseParseTypes::Number ),
			    json_details::simd_details::json_simd_block_iterator_number<
			      JsonMember, CharT, PolicyFlags...>,
			    std::conditional_t<
			      ( JsonMember::underlying_json_type == JsonBaseParseTypes::String ),
			      json_details::simd_details::json_simd_block_iterator_string<
			        JsonMember, CharT, PolicyFlags...>,
			      json_details::simd_details::unknown_json_simd_block_iterator_error<
			        JsonMember>>>>;
		} // namespace experimental
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
