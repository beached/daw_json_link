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
			class json_simd_block_iterator_bool {
				static_assert( JsonMember::underlying_json_type ==
				               JsonBaseParseTypes::Bool );
				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using classifier_type =
				  simd_json_classifier<JsonBaseParseTypes::Bool, CharT>;
				using block_type = simd_json_block<JsonBaseParseTypes::Bool, CharT>;

			public:
				using json_member = JsonMember;
				using value_type = typename json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				static constexpr std::size_t boolean_cache_words = 4U;
				static constexpr std::size_t boolean_cache_capacity =
				  boolean_cache_words * 64U;

				char const *m_first = nullptr;
				char const *m_last = nullptr;
				std::array<std::uint64_t, boolean_cache_words> m_boolean_values{ };
				std::size_t m_value_index = 0;
				std::size_t m_value_count = 0;
				simd_json_classifier_state m_state{ };
				simd_array_grammar_state m_grammar_state{ };

				constexpr void append_boolean_values( std::uint64_t values,
				                                      std::size_t count ) {
					auto const word = m_value_count / 64U;
					auto const shift = m_value_count % 64U;
					m_boolean_values[word] |= values << shift;
					if( shift != 0U and shift + count > 64U ) {
						m_boolean_values[word + 1U] = values >> ( 64U - shift );
					}
					m_value_count += count;
				}

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
					m_boolean_values = { };
					m_value_index = 0;
					m_value_count = 0;

					// Reserving a full classifier block means a classified block is never
					// partially consumed.  In particular, the classifier and grammar
					// state always describe exactly the position held in m_first.
					while( m_first != nullptr and m_first != m_last and
					       m_value_count + block_type::block_size <=
					         boolean_cache_capacity ) {
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
							validate_array_events<ParseState>( block.data,
							                                   m_last,
							                                   block.boolean_start,
							                                   block.comma,
							                                   block.array_end,
							                                   m_grammar_state );
						}

						auto const starts = block.boolean_start;
						auto const block_value_count =
						  static_cast<std::size_t>( daw::cxmath::popcount( starts ) );

						if constexpr( not ParseState::is_unchecked_input ) {
							auto validation_starts = starts;
							auto values = block.boolean_values;
							for( std::size_t n = 0; n < block_value_count; ++n ) {
								auto const lane = static_cast<std::size_t>(
								  daw::cxmath::count_trailing_zeros( validation_starts ) );
								validate_boolean( block.data + lane,
								                  ( values & std::uint64_t{ 1 } ) != 0 );
								validation_starts &= validation_starts - 1U;
								values >>= 1U;
							}
						}

						append_boolean_values( block.boolean_values, block_value_count );
						if( block.array_end != 0 ) {
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
						m_grammar_state.started = true;
					}
					fill_buffer( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					static_assert(
					  json_member::literal_as_string ==
					    options::LiteralAsStringOpt::Never,
					  "SIMD boolean iteration does not support literals encoded as "
					  "strings" );
					auto const word = m_value_index / 64U;
					auto const shift = m_value_index % 64U;
					auto const value =
					  ( ( m_boolean_values[word] >> shift ) & std::uint64_t{ 1 } ) != 0;
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
		} // namespace json_details::simd_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif

