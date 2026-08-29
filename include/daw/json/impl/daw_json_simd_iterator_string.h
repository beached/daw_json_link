// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/daw_json_simd_iterator_common.h"

#if defined( DAW_JSON_HAS_SIMD )

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_details {
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
				simd_array_grammar_state m_grammar_state{ };

				[[nodiscard]] constexpr bool classify_next_block( ) {
					if( m_first == nullptr or m_first == m_last ) {
						if constexpr( not ParseState::is_unchecked_input ) {
							if( m_first != nullptr ) {
								validate_array_ended<ParseState>( m_last, m_grammar_state );
							}
						}
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
						validate_array_events<ParseState>( block.data,
						                                   m_last,
						                                   block.string_start,
						                                   block.comma,
						                                   block.array_end,
						                                   m_grammar_state );
					}
					if( block.array_end != 0 ) {
						m_first = m_last;
					} else {
						m_first += static_cast<std::ptrdiff_t>( block.size );
					}
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
						m_grammar_state.started = true;
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
		} // namespace json_details::simd_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif

