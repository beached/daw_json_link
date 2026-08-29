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
				simd_array_grammar_state m_grammar_state{ };
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
							validate_array_events<ParseState>( block.data,
							                                   m_last,
							                                   block.number_start,
							                                   block.comma,
							                                   block.array_end,
							                                   m_grammar_state );
						}
						m_value_count += block.number_span_count;
						if( block.array_end != 0 ) {
							m_pending_number = { };
							m_first = m_last;
						} else {
							m_first += static_cast<std::ptrdiff_t>( block.size );
						}
					}
					if constexpr( not ParseState::is_unchecked_input ) {
						if( m_first != nullptr and m_first == m_last ) {
							validate_array_ended<ParseState>( m_last, m_grammar_state );
						}
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
						m_grammar_state.started = true;
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
					auto value =
					  json_details::parse_value<raw_number_json_member,
					                            true,
					                            raw_number_json_member::expected_type>(
					    parse_state );
					using constructor_t = json_constructor_t<json_member>;
					static_assert(
					  daw::is_callable_v<constructor_t, bool>,
					  "Unable to construct value with the supplied arguments" );
					return constructor_t{ }( value );
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
		} // namespace json_details::simd_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif

