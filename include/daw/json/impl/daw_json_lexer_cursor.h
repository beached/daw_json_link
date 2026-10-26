// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/simd/daw_simd.h"

#include <daw/daw_attributes.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_span.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

#if defined( DAW_HAS_SIMD )

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace lexer_cursor_details {
				using simd_type = daw::simd::vec<char>;
				static constexpr std::size_t simd_size =
				  static_cast<std::size_t>( simd_type::size( ) );
				static_assert( simd_size <= 64 );

				[[nodiscard]] constexpr std::uint64_t
				low_bits( std::size_t count ) noexcept {
					return count == 64 ? ~std::uint64_t{ 0 }
					                   : ( std::uint64_t{ 1 } << count ) - 1;
				}

				using daw::simd_impl::prefix_xor;

				[[nodiscard]] constexpr std::uint64_t
				last_bit( std::uint64_t bits, std::size_t count ) noexcept {
					return count == 0 ? 0
					                  : ( bits >> ( count - 1U ) ) & std::uint64_t{ 1 };
				}

				[[nodiscard]]
				DAW_ATTRIB_INLINE simd_type splat( char value ) noexcept {
					return simd_type( value );
				}

				template<char... Values>
				[[nodiscard]] inline auto one_of( simd_type value_vec ) {
					return ( ( value_vec == splat( Values ) ) | ... );
				}

				/// The structural characters {}[],: of the block, and the control
				/// characters 0x0C and 0x1A which callers remove with whitespace.
				/// '[' and ']' are '{' and '}' less 0x20, so one lookup by the low four
				/// bits finds all six
				[[nodiscard]] inline auto structural_candidates( simd_type input_vec ) {
					DAW_CPP23_STATIC_LOCAL constexpr std::array<char, 16> table = {
					  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
					return daw::simd_impl::lookup16( table, input_vec ) ==
					       ( input_vec | splat( '\x20' ) );
				}

				using mask_type = typename simd_type::mask_type;
				static constexpr std::size_t vector_count = 64U / simd_size;

				/// The candidate bytes of each class in one vector
				struct vector_classes {
					mask_type backslash;
					mask_type quote;
					mask_type structural;
					mask_type whitespace;
				};

				[[nodiscard]] DAW_ATTRIB_INLINE vector_classes
				classify_vector( simd_type input_vec ) {
					return vector_classes{
					  input_vec == splat( '\\' ),
					  input_vec == splat( '"' ),
					  structural_candidates( input_vec ),
					  ( input_vec >= splat( '\x01' ) ) & ( input_vec <= splat( '\x20' ) ) };
				}

				/// A bit per byte of a 64 byte block for each class
				struct block_classes {
					std::uint64_t backslash;
					std::uint64_t quote;
					std::uint64_t structural;
					std::uint64_t whitespace;
				};

				/// Classify the vectors load_vector( offset ) of a block.  The masks of
				/// a class are packed together, which on NEON is a quarter of the
				/// instructions of a to_ullong for each vector
				template<typename LoadVector, std::size_t... Is>
				[[nodiscard]] DAW_ATTRIB_INLINE block_classes
				classify_vectors( LoadVector load_vector, std::index_sequence<Is...> ) {
					auto const classes = std::array<vector_classes, vector_count>{
					  classify_vector( load_vector( Is * simd_size ) )... };
					auto const pack = [&]( mask_type vector_classes::*cls ) {
						return daw::simd::pack_masks(
						  std::array<mask_type, vector_count>{ ( classes[Is].*cls )... } );
					};
					return block_classes{ pack( &vector_classes::backslash ),
					                      pack( &vector_classes::quote ),
					                      pack( &vector_classes::structural ),
					                      pack( &vector_classes::whitespace ) };
				}

				// Copy the tail to a zeroed block so loads stay within the input.
				[[nodiscard]] inline simd_type load( char const *first,
				                                     std::size_t count ) {
					if( count == simd_size ) {
						return daw::simd::unchecked_load<simd_type>(
						  daw::span( first, simd_size ) );
					}
					char buffer[simd_size] = { };
					for( std::size_t n = 0; n < count; ++n ) {
						buffer[n] = first[n];
					}
					return daw::simd::unchecked_load<simd_type>(
					  daw::span<char const>( buffer, simd_size ) );
				}
			} // namespace lexer_cursor_details

			/// Stage 1 of parsing.  Each 64 byte block is classified once and the
			/// positions the parser needs are handed out in order: the structural
			/// characters {}[],: outside of strings, the opening quotes of strings
			/// and the first byte of each literal or number.  pop_string gives the
			/// closing quote of a string.  Whitespace is never surfaced.  Numbers,
			/// escapes and grammar are left to the parser, which keeps working
			/// without the cursor.
			///
			/// Blocks are classified when the cursor reaches them, and the last
			/// block_count of them are kept.  The parse state's first stays
			/// authoritative, peek( first ) brings the cursor to it.  Inside of the
			/// kept blocks that only masks off the events below first in its saved
			/// block, so it can move back.  Outside of them the cursor reloads at
			/// first assuming first is not in a string, which holds at a value
			/// boundary.  A first at the end of the classified bytes continues with
			/// their state.  So a first in a string or a literal, like one past an
			/// opening quote, is only valid right after the peek that returned the
			/// event before it.  Loads start exactly at first, bytes before it are
			/// never classified.
			class lexer_cursor {
				using simd_type = lexer_cursor_details::simd_type;
				static constexpr std::size_t simd_size =
				  lexer_cursor_details::simd_size;
				static_assert( 64U % simd_size == 0 );

			public:
				static constexpr std::size_t block_size = 64U;
				static constexpr std::size_t block_count = 4U;
				/// build_structural_index writes up to this many entries past the last
				static constexpr std::size_t structural_index_padding =
				  daw::simd_impl::flatten_bits_capacity;

				struct structural_index_result {
					std::uint32_t size;
					bool in_string;
				};

			private:
				char const *m_first = nullptr;
				char const *m_loaded_last = nullptr;
				char const *m_last = nullptr;
				std::uint64_t m_block_events[block_count] = { };
				std::uint64_t m_close_quotes[block_count] = { };
				// Closing quotes of strings holding a backslash
				std::uint64_t m_escaped_closes[block_count] = { };
				// The events left in the current block
				std::uint64_t m_events = 0;
				std::size_t m_block = 0;
				std::size_t m_loaded_blocks = 0;
				// State carried to the next block
				bool m_in_string = false;
				bool m_escaped = false;
				bool m_in_token = false;
				// The string carried in has had no backslash
				bool m_string_clean = false;

				// Each carry is 0 or 1.  Bools let clang pack the carries into one
				// register, which chains every carry of the next block on all of them
				struct carry_state {
					std::uint64_t in_string = 0;
					std::uint64_t escaped = 0;
					std::uint64_t in_token = 0;
					std::uint64_t string_clean = 0;
				};

				struct classified_block {
					std::uint64_t events;
					std::uint64_t closes;
					std::uint64_t escaped_closes;
					std::uint64_t token_bytes;
					std::uint64_t token_starts;
					carry_state carry;
				};

				template<bool TrackStringMetadata = true>
				[[nodiscard]] DAW_ATTRIB_INLINE static classified_block
				classify_block( char const *first, std::size_t count,
				                carry_state carry ) {
					using namespace lexer_cursor_details;

					constexpr auto vectors = std::make_index_sequence<vector_count>{ };
					auto classes = block_classes{ };
					if( DAW_LIKELY( count == block_size ) ) {
						// A full block, the trip count and lanes are known
						classes = classify_vectors(
						  [&]( std::size_t offset ) {
							  return daw::simd::unchecked_load<simd_type>(
							    daw::span( first + offset, simd_size ) );
						  },
						  vectors );
					} else {
						// The zeroed lanes past count are in no class, valid_bits
						// removes them anyway
						classes = classify_vectors(
						  [&]( std::size_t offset ) {
							  if( offset >= count ) {
								  return splat( '\0' );
							  }
							  return load( first + offset, count - offset < simd_size
							                                 ? count - offset
							                                 : simd_size );
						  },
						  vectors );
					}

					auto const valid_bits = low_bits( count );
					auto const backslash_bits = classes.backslash & valid_bits;
					auto const raw_quote_bits = classes.quote & valid_bits;
					auto const raw_structural_bits = classes.structural & valid_bits;
					auto const whitespace_bits = classes.whitespace & valid_bits;

					DAW_CPP23_STATIC_LOCAL constexpr std::uint64_t odd_bits =
					  0xAAAAAAAAAAAAAAAAULL;

					auto const previous_escaped = carry.escaped;

					auto const potential_escape = backslash_bits & ~previous_escaped;

					auto const maybe_escaped = potential_escape << 1U;

					auto const escape_and_terminal =
					  ( ( maybe_escaped | odd_bits ) - potential_escape ) ^ odd_bits;

					auto const escaped_bits =
					  ( escape_and_terminal ^ ( backslash_bits | previous_escaped ) ) &
					  valid_bits;

					auto const escape_bits = escape_and_terminal & backslash_bits;

					carry.escaped = last_bit( escape_bits, count );

					auto const quote_bits = raw_quote_bits & ~escaped_bits;

					// The opening quote and the bytes of a string, not its closing quote
					auto const in_string_bits =
					  ( prefix_xor( quote_bits ) ^
					    ( valid_bits & ( std::uint64_t{ 0 } - carry.in_string ) ) ) &
					  valid_bits;

					auto const outside = ~( in_string_bits | quote_bits ) & valid_bits;

					auto const structural =
					  raw_structural_bits & outside & ~escaped_bits & ~whitespace_bits;

					auto const token = outside & ~structural & ~whitespace_bits;

					auto const token_start =
					  token &
					  ~( ( token << 1U ) | carry.in_token );

					auto const open_quotes = quote_bits & in_string_bits;

					auto close_quotes = std::uint64_t{ 0 };
					auto escaped_closes = std::uint64_t{ 0 };
					if constexpr( TrackStringMetadata ) {
						close_quotes = quote_bits & ~in_string_bits;

						// Adding a string's first bit carries through its bytes up to the
						// first backslash, or to its closing quote when there is none
						auto const clean_bytes = in_string_bits & ~backslash_bits;
						auto const string_starts =
						  open_quotes |
						  ( carry.in_string & carry.string_clean );
						auto const carried = clean_bytes + string_starts;
						escaped_closes = close_quotes & ~carried;
						carry.string_clean = carried < clean_bytes ? 1U : 0U;
					}
					carry.in_string = last_bit( in_string_bits, count );
					carry.in_token = last_bit( token, count );

					return classified_block{ .events =
					                           structural | open_quotes | token_start,
					                         .closes = close_quotes,
					                         .escaped_closes = escaped_closes,
					                         .token_bytes = token,
					                         .token_starts = token_start,
					                         .carry = carry };
				}

				/// Classify the block at the end of the classified bytes, continuing
				/// with the carried state.  A full window starts again at that block
				inline void classify_next( ) {
					if( m_loaded_blocks == block_count ) {
						m_first = m_loaded_last;
						m_loaded_blocks = 0;
					}
					auto const remaining =
					  static_cast<std::size_t>( m_last - m_loaded_last );
					auto const count = remaining < block_size ? remaining : block_size;
					auto const classified = classify_block(
					  m_loaded_last,
					  count,
					  { m_in_string, m_escaped, m_in_token, m_string_clean } );
					m_block_events[m_loaded_blocks] = classified.events;
					m_close_quotes[m_loaded_blocks] = classified.closes;
					m_escaped_closes[m_loaded_blocks] = classified.escaped_closes;
					m_in_string = classified.carry.in_string != 0;
					m_escaped = classified.carry.escaped != 0;
					m_in_token = classified.carry.in_token != 0;
					m_string_clean = classified.carry.string_clean != 0;
					m_loaded_last += count;
					m_block = m_loaded_blocks++;
					m_events = m_block_events[m_block];
				}

				inline void reload( char const *first ) {
					m_in_string = false;
					m_escaped = false;
					m_in_token = false;
					m_string_clean = false;
					m_first = first;
					m_loaded_last = first;
					m_loaded_blocks = 0;
					classify_next( );
				}

				/// Write the events of the block at block_offset as offsets from first,
				/// returning how many were written
				DAW_ATTRIB_INLINE static std::uint32_t
				index_block( char const *first, std::size_t block_offset,
				             std::size_t count, carry_state &carry,
				             std::uint32_t *offsets ) {
					auto const classified =
					  classify_block<false>( first + block_offset, count, carry );
					carry = classified.carry;
					return static_cast<std::uint32_t>( daw::simd_impl::flatten_bits(
					  classified.events,
					  static_cast<std::uint32_t>( block_offset ),
					  offsets ) );
				}

			public:
				explicit constexpr lexer_cursor( char const *last ) noexcept
				  : m_last( last ) {}

				/// Build the ordered structural offsets for a complete range. Strings
				/// occupy one entry at their opening quote. A scalar or string ends before
				/// the next entry, with only whitespace between. offsets[token_count] is
				/// a document-end sentinel and offsets needs room for doc.size( ) plus
				/// structural_index_padding entries.
				template<typename Offsets>
				[[nodiscard]] static inline structural_index_result
				build_structural_index( std::string_view const doc, Offsets &&offsets ) {
					auto carry = carry_state{ };
					auto token_count = std::uint32_t{ 0 };
					auto const doc_sz = doc.size( );
					// Full blocks have a constant count, so the carries are the top bits
					auto const full_end = doc_sz - doc_sz % block_size;
					auto block_offset = std::size_t{ 0 };
					for( ; block_offset < full_end; block_offset += block_size ) {
						token_count += index_block( doc.data( ),
						                            block_offset,
						                            block_size,
						                            carry,
						                            &offsets[token_count] );
					}
					if( block_offset < doc_sz ) {
						token_count += index_block( doc.data( ),
						                            block_offset,
						                            doc_sz - block_offset,
						                            carry,
						                            &offsets[token_count] );
					}
					offsets[token_count] = static_cast<std::uint32_t>( doc_sz );
					if constexpr( not std::is_pointer_v<
					                std::remove_reference_t<Offsets>> ) {
						offsets.set_size( token_count );
					}
					return structural_index_result{ token_count, carry.in_string != 0 };
				}

				/// The first event at or after first, whatever was popped before.
				/// nullptr when there is none before the end of the data
				[[nodiscard]] inline char const *peek( char const *first ) {
					using lexer_cursor_details::low_bits;
					if( DAW_LIKELY( m_loaded_blocks != 0 and first >= m_first and
					                first < m_loaded_last ) ) {
						auto const position = static_cast<std::size_t>( first - m_first );
						m_block = position / block_size;
						m_events =
						  m_block_events[m_block] & ~low_bits( position % block_size );
					} else if( m_loaded_blocks != 0 and first == m_loaded_last ) {
						m_block = m_loaded_blocks - 1U;
						m_events = 0;
					} else {
						if( first >= m_last ) {
							return nullptr;
						}
						reload( first );
					}
					return peek( );
				}

				/// The next event from where the cursor is, after those popped since
				/// the last peek( first ).  nullptr when there is none before the end
				/// of the data
				[[nodiscard]] inline char const *peek( ) {
					while( m_events == 0 ) {
						if( m_block + 1U < m_loaded_blocks ) {
							++m_block;
							m_events = m_block_events[m_block];
						} else if( m_loaded_last >= m_last ) {
							return nullptr;
						} else {
							classify_next( );
						}
					}
					return m_first + m_block * block_size +
					       daw::cxmath::count_trailing_zeros( m_events );
				}

				/// Remove the event returned by the last peek
				DAW_ATTRIB_INLINE constexpr void pop( ) noexcept {
					m_events = daw::cxmath::blsr( m_events );
				}

				/// Remove the opening quote returned by the last peek and give its
				/// closing quote, nullptr when the data ends in the string.  The bytes
				/// of a string have no events, so the next peek is after it
				[[nodiscard]] inline char const *pop_string( ) {
					using lexer_cursor_details::low_bits;
					auto block = m_block;
					auto closes = m_close_quotes[block] &
					              ~low_bits( static_cast<std::size_t>(
					                daw::cxmath::count_trailing_zeros( m_events ) ) );
					m_events = daw::cxmath::blsr( m_events );
					while( closes == 0 ) {
						if( block + 1U < m_loaded_blocks ) {
							++block;
							closes = m_close_quotes[block];
						} else if( m_loaded_last >= m_last ) {
							return nullptr;
						} else {
							// The rest of the current block is in the string, so it has no
							// events left
							classify_next( );
							block = m_block;
							closes = m_close_quotes[block];
						}
					}
					return m_first + block * block_size +
					       daw::cxmath::count_trailing_zeros( closes );
				}

				/// The classified bytes end in a string.  Once peek has returned
				/// nullptr this is whether the data ends in one
				[[nodiscard]] constexpr bool in_string( ) const noexcept {
					return m_in_string;
				}

				/// The closing quote returned by the last pop_string is the end of a
				/// string that has a backslash in it
				[[nodiscard]] constexpr bool
				string_has_escape( char const *close_quote ) const noexcept {
					auto const position =
					  static_cast<std::size_t>( close_quote - m_first );
					return ( ( m_escaped_closes[position / block_size] >>
					           ( position % block_size ) ) &
					         1U ) != 0;
				}
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
