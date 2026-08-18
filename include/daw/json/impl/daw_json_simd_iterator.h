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

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
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

			template<typename Byte = std::uint8_t>
			struct simd_json_block {
				using simd_type = std::simd::vec<Byte>;
				using mask_type = typename simd_type::mask_type;

				static constexpr std::size_t block_size =
				  static_cast<std::size_t>( simd_type::size( ) );

				char const *data = nullptr;
				std::size_t offset = 0;
				std::size_t size = 0;

				mask_type valid = false;
				mask_type whitespace = false;
				mask_type operators = false;
				mask_type scalar = false;
				mask_type quote = false;
				mask_type backslash = false;
				mask_type escaped = false;
				mask_type string_content = false;
				mask_type structural_start = false;
				mask_type number_start = false;
				mask_type boolean_start = false;
				mask_type string_start = false;
				simd_json_classifier_state state_after{ };

				[[nodiscard]] constexpr bool is_full( ) const noexcept {
					return size == block_size;
				}
			};

			template<typename Byte = std::uint8_t>
			class simd_json_classifier {
				using block_type = simd_json_block<Byte>;
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

				/**
				 * Classify one native-register-sized block.  Character classes are
				 * computed in parallel.  Prefix operations on their bit sets then
				 * resolve quote, escape, and token-start state for the entire block
				 * at once.
				 */
				[[nodiscard]] static block_type
				classify( char const *first, std::size_t count, state_type &state ) {
					count = count < block_size ? count : block_size;
					auto const input = load( first, count );

					auto const space = input == splat( ' ' );
					auto const whitespace = space | ( input == splat( '\t' ) ) |
					                        ( input == splat( '\n' ) ) |
					                        ( input == splat( '\r' ) );
					auto const operators =
					  ( input == splat( '{' ) ) | ( input == splat( '}' ) ) |
					  ( input == splat( '[' ) ) | ( input == splat( ']' ) ) |
					  ( input == splat( ':' ) ) | ( input == splat( ',' ) );
					auto const potential_scalar = not( whitespace | operators );
					auto const potential_quote = input == splat( '"' );
					auto const backslash = input == splat( '\\' );
					auto const potential_number =
					  ( input == splat( '-' ) ) |
					  ( ( input >= splat( '0' ) ) & ( input <= splat( '9' ) ) );
					auto const potential_boolean =
					  ( input == splat( 't' ) ) | ( input == splat( 'f' ) );

					auto const valid_bits = low_bits( count );
					auto const whitespace_bits = whitespace.to_ullong( ) & valid_bits;
					auto const operator_bits = operators.to_ullong( ) & valid_bits;
					auto const scalar_bits = potential_scalar.to_ullong( ) & valid_bits;
					auto const backslash_bits = backslash.to_ullong( ) & valid_bits;

					// Resolve odd/even runs of backslashes. This is the same
					// subtraction technique used by simdjson's stage-1 escape scanner.
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
					  potential_quote.to_ullong( ) & ~escaped_bits & valid_bits;
					auto const in_string_bits =
					  ( prefix_xor( quote_bits ) ^
					    ( state.in_string ? valid_bits : std::uint64_t{ 0 } ) ) &
					  valid_bits;
					state.in_string = last_bit( in_string_bits, count );
					// Opening quotes are excluded and closing quotes included,
					// preserving an opening quote as the structural start of a string
					// token.
					auto const string_tail_bits = in_string_bits ^ quote_bits;

					auto const nonquote_scalar_bits = scalar_bits & ~quote_bits;
					auto const follows_scalar_bits =
					  ( nonquote_scalar_bits << 1U ) |
					  ( state.previous_scalar ? std::uint64_t{ 1 } : 0 );
					state.previous_scalar = last_bit( nonquote_scalar_bits, count );
					auto const scalar_start_bits = scalar_bits & ~follows_scalar_bits;
					auto const structural_bits = ( operator_bits | scalar_start_bits ) &
					                             ~string_tail_bits & valid_bits;

					auto const offset = state.offset;
					auto const valid = mask_type( valid_bits );
					auto const structural_start = mask_type( structural_bits );
					state.offset += count;

					return block_type{
					  .data = first,
					  .offset = offset,
					  .size = count,
					  .valid = valid,
					  .whitespace =
					    mask_type( whitespace_bits & ~in_string_bits & valid_bits ),
					  .operators = operators & valid,
					  .scalar = potential_scalar & valid,
					  .quote = mask_type( quote_bits ),
					  .backslash = backslash & valid,
					  .escaped = mask_type( escaped_bits ),
					  .string_content =
					    mask_type( in_string_bits & ~quote_bits & valid_bits ),
					  .structural_start = structural_start,
					  .number_start = potential_number & structural_start,
					  .boolean_start = potential_boolean & structural_start,
					  .string_start =
					    mask_type( quote_bits & in_string_bits & structural_bits ),
					  .state_after = state };
				}
			};
		} // namespace json_details

		inline namespace experimental {
			/**
			 * Input iterator over native std::simd-sized classified JSON blocks.
			 * Dereferencing is idempotent: classification state is committed only by
			 * incrementing the iterator.
			 */
			template<typename Byte = std::uint8_t>
			class json_simd_block_iterator {
				using classifier_type = json_details::simd_json_classifier<Byte>;

			public:
				using value_type = json_details::simd_json_block<Byte>;
				using reference = value_type;
				using pointer = void;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				char const *m_first = nullptr;
				char const *m_last = nullptr;
				json_details::simd_json_classifier_state m_state{ };
				mutable json_details::simd_json_classifier_state m_next_state{ };
				mutable std::optional<value_type> m_block{ };

				void classify_current( ) const {
					if( m_block or m_first == nullptr or m_first == m_last ) {
						return;
					}
					m_next_state = m_state;
					auto const remaining = static_cast<std::size_t>( m_last - m_first );
					m_block =
					  classifier_type::classify( m_first, remaining, m_next_state );
				}

			public:
				json_simd_block_iterator( ) = default;

				explicit json_simd_block_iterator( std::string_view document ) noexcept
				  : m_first( document.data( ) )
				  , m_last( document.data( ) + document.size( ) ) {}

				[[nodiscard]] reference operator*( ) const {
					classify_current( );
					return *m_block;
				}

				json_simd_block_iterator &operator++( ) {
					classify_current( );
					if( m_block ) {
						m_first += static_cast<std::ptrdiff_t>( m_block->size );
						m_state = m_next_state;
						m_block.reset( );
					}
					return *this;
				}

				void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] explicit operator bool( ) const noexcept {
					return m_first != nullptr and m_first != m_last;
				}

				[[nodiscard]] json_simd_block_iterator begin( ) const {
					return *this;
				}

				[[nodiscard]] json_simd_block_iterator end( ) const noexcept {
					return { };
				}

				friend bool operator==( json_simd_block_iterator const &lhs,
				                        json_simd_block_iterator const &rhs ) noexcept {
					auto const lhs_at_end =
					  lhs.m_first == nullptr or lhs.m_first == lhs.m_last;
					auto const rhs_at_end =
					  rhs.m_first == nullptr or rhs.m_first == rhs.m_last;

					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_first == rhs.m_first;
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
