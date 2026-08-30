// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/daw_json_simd_iterator_common.h"
#include "daw/json/impl/version.h"

#if defined( DAW_JSON_HAS_SIMD )

#include "daw/json/daw_json_link_types.h"
#include "daw/json/impl/daw_json_location_info.h"
#include "daw/json/impl/daw_json_parse_class.h"
#include "daw/json/impl/daw_json_parse_value.h"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_details {
			/**
			 * Streaming stage-1 state for class iteration.  A classified block may
			 * extend beyond the current class, so unconsumed structural offsets are
			 * retained for the next array element.
			 */
			struct simd_class_structural_state {
				using simd_type = daw::simd::vec<char>;
				static constexpr std::size_t block_size =
				  static_cast<std::size_t>( simd_type::size( ) );
				static_assert( block_size <= 64U );

				char const *next_block = nullptr;
				char const *last = nullptr;
				char const *block_data = nullptr;
				std::size_t block_length = 0U;
				// Structural characters plus the value metadata stage 2 needs without
				// rescanning: string escapes and real-number separators.
				std::uint64_t structural_offsets = 0U;
				bool in_string = false;
				bool escaped = false;

				constexpr void reset( char const *first, char const *last_ptr ) {
					next_block = first;
					last = last_ptr;
					block_data = nullptr;
					block_length = 0U;
					structural_offsets = 0U;
					in_string = false;
					escaped = false;
				}

			private:
				[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR bool classify_next_block( ) {
					if( next_block == nullptr or next_block >= last ) {
						return false;
					}

					block_data = next_block;
					auto const remaining = static_cast<std::size_t>( last - next_block );
					block_length = remaining < block_size ? remaining : block_size;
					auto const input = simd_details::load<simd_type, block_size, char>(
					  block_data, block_length );
					auto const valid_bits = simd_details::low_bits( block_length );
					auto const backslash_bits =
					  ( input == simd_details::splat<simd_type>( '\\' ) ).to_ullong( ) &
					  valid_bits;

					constexpr std::uint64_t odd_bits = 0xAAAAAAAAAAAAAAAAULL;
					auto const previous_escaped = escaped ? std::uint64_t{ 1 } : 0U;
					auto const potential_escape = backslash_bits & ~previous_escaped;
					auto const maybe_escaped = potential_escape << 1U;
					auto const escape_and_terminal =
					  ( ( maybe_escaped | odd_bits ) - potential_escape ) ^ odd_bits;
					auto const escaped_bits =
					  ( escape_and_terminal ^ ( backslash_bits | previous_escaped ) ) &
					  valid_bits;
					auto const escape_bits = escape_and_terminal & backslash_bits;
					escaped = simd_details::last_bit( escape_bits, block_length );

					auto const quote_bits =
					  ( input == simd_details::splat<simd_type>( '"' ) ).to_ullong( ) &
					  ~escaped_bits & valid_bits;
					auto const in_string_bits =
					  ( simd_details::prefix_xor( quote_bits ) ^
					    ( in_string ? valid_bits : std::uint64_t{ 0 } ) ) &
					  valid_bits;
					in_string = simd_details::last_bit( in_string_bits, block_length );

					auto const outside_string_bits =
					  ~( in_string_bits ^ quote_bits ) & valid_bits;
					auto const json_structurals =
					  simd_details::one_of<'{', '}', '[', ']', ':', ','>( input )
					    .to_ullong( ) &
					  outside_string_bits;
					auto const number_metadata =
					  simd_details::one_of<'.', 'e', 'E'>( input ).to_ullong( ) &
					  outside_string_bits;
					structural_offsets =
					  quote_bits | backslash_bits | json_structurals | number_metadata;
					next_block += static_cast<std::ptrdiff_t>( block_length );
					return true;
				}

			public:
				[[nodiscard]] DAW_JSON_SIMD_CONSTEXPR char const *
				next( char const *minimum ) {
					while( true ) {
						while( structural_offsets != 0U ) {
							auto const lane = static_cast<std::size_t>(
							  daw::cxmath::count_trailing_zeros( structural_offsets ) );
							structural_offsets &= structural_offsets - 1U;
							auto const result = block_data + lane;
							if( result >= minimum ) {
								return result;
							}
						}
						if( not classify_next_block( ) ) {
							return nullptr;
						}
					}
				}
			};

			[[nodiscard]] constexpr bool is_json_whitespace( char value ) noexcept {
				return value == ' ' or value == '\t' or value == '\n' or value == '\r';
			}

			[[nodiscard]] constexpr char const *
			trim_value_right( char const *first, char const *last ) noexcept {
				while( last > first and is_json_whitespace( last[-1] ) ) {
					--last;
				}
				return last;
			}

			template<std::size_t N>
			[[nodiscard]] constexpr bool
			matches_literal( char const *first, char const *last,
			                 char const ( &literal )[N] ) noexcept {
				if( static_cast<std::size_t>( last - first ) != N - 1U ) {
					return false;
				}
				for( std::size_t n = 0; n < N - 1U; ++n ) {
					if( first[n] != literal[n] ) {
						return false;
					}
				}
				return true;
			}

			template<typename JsonMemberList>
			struct simd_class_parser;

			template<typename... JsonMembers>
			struct simd_class_parser<json_member_list<JsonMembers...>> {
				template<typename ParseState>
				using locations_type =
				  decltype( make_locations_info<ParseState, JsonMembers...>( ) );

				template<typename ParseState>
				[[nodiscard]] static constexpr locations_type<ParseState>
				make_locations( ) {
					return make_locations_info<ParseState, JsonMembers...>( );
				}

				template<typename JsonClass, typename ParseState>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR char const *
				scan_class( ParseState &parse_state,
				            locations_type<ParseState> &locations,
				            simd_class_structural_state &structural_state ) {
					using result_t = json_result_t<JsonClass>;
					DAW_CPP23_STATIC_LOCAL constexpr bool must_exist =
					  all_json_members_must_exist_v<result_t, ParseState>;

					parse_state.trim_left( );
					daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					auto const old_class_pos = parse_state.get_class_position( );
					auto const class_first = parse_state.data( );
					auto const class_last = parse_state.data_end( );
					parse_state.set_class_position( );

					auto event = structural_state.next( class_first );
					daw_json_assert_weak( event != nullptr and *event == '{',
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					event = structural_state.next( event + 1 );
					auto after_comma = false;

					while( event != nullptr and *event != '}' ) {
						daw_json_assert_weak( *event == '"',
						                      ErrorReason::InvalidMemberName,
						                      parse_state );
						auto const name_first = event + 1;
						auto name_last = structural_state.next( name_first );
						while( name_last != nullptr and *name_last == '\\' ) {
							name_last = structural_state.next( name_last + 1 );
						}
						daw_json_assert_weak( name_last != nullptr and *name_last == '"',
						                      ErrorReason::InvalidMemberName,
						                      parse_state );
						auto const name = daw::string_view(
						  name_first, static_cast<std::size_t>( name_last - name_first ) );
						auto const name_pos =
						  locations.template find_name<ParseState::expect_long_strings, 0>(
						    name );
						if constexpr( must_exist ) {
							daw_json_assert_weak( name_pos < locations.size( ),
							                      ErrorReason::UnknownMember,
							                      parse_state );
						}
						auto const colon = structural_state.next( name_last + 1 );
						daw_json_assert_weak( colon != nullptr and *colon == ':',
						                      ErrorReason::InvalidMemberName,
						                      parse_state );
						auto value_state = ParseState( colon + 1, class_last );
						value_state.trim_left( );
						auto const value_first = value_state.data( );
						daw_json_assert_weak( value_first < class_last,
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );

						auto object_depth = std::size_t{ 1 };
						auto array_depth = std::size_t{ 0 };
						auto container_count = std::size_t{ 0 };
						char const *first_escape = nullptr;
						char const *string_last = nullptr;
						char const *decimal_point = nullptr;
						char const *exponent_marker = nullptr;
						auto const value_is_string = *value_first == '"';
						auto const value_is_array = *value_first == '[';
						auto const value_is_class = *value_first == '{';
						auto delimiter = structural_state.next( colon + 1 );
						auto found_delimiter = false;
						while( delimiter != nullptr and not found_delimiter ) {
							switch( *delimiter ) {
							case '"':
								if( value_is_string and delimiter != value_first and
								    string_last == nullptr ) {
									string_last = delimiter;
								}
								break;
							case '\\':
								if( value_is_string and first_escape == nullptr ) {
									first_escape = delimiter;
								}
								break;
							case '.':
								if( object_depth == 1U and array_depth == 0U and
								    decimal_point == nullptr ) {
									decimal_point = delimiter;
								}
								break;
							case 'e':
							case 'E':
								if( object_depth == 1U and array_depth == 0U and
								    exponent_marker == nullptr ) {
									exponent_marker = delimiter;
								}
								break;
							case '{':
								++object_depth;
								break;
							case '}':
								if( object_depth == 1U and array_depth == 0U ) {
									found_delimiter = true;
									break;
								}
								daw_json_assert_weak( object_depth > 1U,
								                      ErrorReason::InvalidBracketing,
								                      parse_state );
								--object_depth;
								break;
							case '[':
								++array_depth;
								break;
							case ']':
								daw_json_assert_weak( array_depth > 0U,
								                      ErrorReason::InvalidBracketing,
								                      parse_state );
								--array_depth;
								break;
							case ',':
								if( ( value_is_array and object_depth == 1U and
								      array_depth == 1U ) or
								    ( value_is_class and object_depth == 2U and
								      array_depth == 0U ) ) {
									++container_count;
								}
								if( object_depth == 1U and array_depth == 0U ) {
									found_delimiter = true;
								}
								break;
							default:
								break;
							}
							if( not found_delimiter ) {
								delimiter = structural_state.next( delimiter + 1 );
							}
						}

						daw_json_assert_weak( delimiter != nullptr,
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
						auto const value_last =
						  trim_value_right( value_first, delimiter );
						daw_json_assert_weak( value_first < value_last,
						                      ErrorReason::InvalidStartOfValue,
						                      parse_state );
						if constexpr( not ParseState::is_unchecked_input ) {
							switch( *value_first ) {
							case 't':
								daw_json_assert_weak(
								  matches_literal( value_first, value_last, "true" ),
								  ErrorReason::InvalidTrue,
								  parse_state );
								break;
							case 'f':
								daw_json_assert_weak(
								  matches_literal( value_first, value_last, "false" ),
								  ErrorReason::InvalidFalse,
								  parse_state );
								break;
							case 'n':
								daw_json_assert_weak(
								  matches_literal( value_first, value_last, "null" ),
								  ErrorReason::InvalidNull,
								  parse_state );
								break;
							default:
								break;
							}
						}

						if( name_pos < locations.size( ) and
						    locations[name_pos].missing( ) ) {
							auto location_state = ParseState(
							  value_first, value_last, class_first, class_last );
							switch( *value_first ) {
							case '"':
								daw_json_assert_weak( string_last != nullptr and
								                        string_last + 1 == value_last,
								                      ErrorReason::InvalidString,
								                      parse_state );
								location_state.first = value_first + 1;
								location_state.last = string_last;
								location_state.counter =
								  first_escape == nullptr
								    ? static_cast<std::size_t>( -1 )
								    : static_cast<std::size_t>( first_escape - ( value_first + 1 ) );
								break;
							case 't':
								location_state.counter = 1U;
								break;
							case 'f':
								location_state.counter = 0U;
								break;
							case 'n':
								location_state.first = nullptr;
								location_state.last = nullptr;
								break;
							case '[':
							case '{':
								location_state.counter = container_count;
								break;
							default:
								location_state.class_first = decimal_point;
								location_state.class_last = exponent_marker;
								break;
							}
							locations[name_pos].set_range( location_state );
						} else if constexpr( must_exist ) {
							daw_json_error( true, ErrorReason::UnknownMember, parse_state );
						}

						after_comma = *delimiter == ',';
						event = after_comma
						          ? structural_state.next( delimiter + 1 )
						          : delimiter;
					}

					daw_json_assert_weak( event != nullptr,
					                      ErrorReason::UnexpectedEndOfData,
					                      parse_state );
					daw_json_assert_weak( not after_comma,
					                      ErrorReason::TrailingComma,
					                      parse_state );
					daw_json_assert_weak( *event == '}',
					                      ErrorReason::InvalidEndOfValue,
					                      parse_state );
					auto const class_end = event + 1;
					parse_state.first = class_end;
					parse_state.trim_left_checked( );
					parse_state.set_class_position( old_class_pos );
					return class_end;
				}

			private:
				template<typename JsonMember, typename ParseState, typename Location>
				[[nodiscard]] static constexpr json_result_t<JsonMember>
				parse_member( ParseState &parse_state, Location const &location ) {
					auto member_state = [&] {
						if constexpr( ParseState::has_allocator ) {
							return location.template get_range<ParseState>( ).with_allocator(
							  parse_state );
						} else {
							return location.template get_range<ParseState>( );
						}
					}( );
					if( member_state.is_null( ) ) {
						// A stored null and a missing member intentionally use the same null
						// location representation.  Preserve nullable/missing-member handling.
						return parse_class_member_impl<JsonMember, false>(
						  parse_state, find_result<ParseState>{ member_state, true } );
					}
					return parse_value<without_name<JsonMember>,
					                   true,
					                   JsonMember::expected_type>( member_state );
				}

				template<typename JsonClass, typename ParseState, std::size_t... Is>
				[[nodiscard]] static constexpr json_result_t<JsonClass>
				construct_class_impl( ParseState &parse_state,
				                      locations_type<ParseState> const &locations,
				                      std::index_sequence<Is...> ) {
					using result_t = json_result_t<JsonClass>;
					using constructor_t = json_constructor_t<JsonClass>;
					if constexpr( should_construct_explicitly_v<constructor_t,
					                                            result_t,
					                                            ParseState> ) {
						return result_t{
						  parse_member<JsonMembers>( parse_state, locations[Is] )... };
					} else {
						return construct_value_tp<result_t, constructor_t>(
						  parse_state,
						  fwd_pack{
						    parse_member<JsonMembers>( parse_state, locations[Is] )... } );
					}
				}

			public:
				template<typename JsonClass, typename ParseState>
				[[nodiscard]] static constexpr json_result_t<JsonClass>
				construct_class( ParseState &parse_state,
				                 locations_type<ParseState> const &locations ) {
					return construct_class_impl<JsonClass>(
					  parse_state,
					  locations,
					  std::index_sequence_for<JsonMembers...>{ } );
				}
			};

			template<>
			struct simd_class_parser<json_member_list<>> {
				struct empty_locations {};

				template<typename ParseState>
				using locations_type = empty_locations;

				template<typename ParseState>
				[[nodiscard]] static constexpr locations_type<ParseState>
				make_locations( ) {
					return { };
				}

				template<typename JsonClass, typename ParseState>
				[[nodiscard]] static DAW_JSON_SIMD_CONSTEXPR char const *
				scan_class( ParseState &parse_state, empty_locations &,
				            simd_class_structural_state &structural_state ) {
					using result_t = json_result_t<JsonClass>;
					parse_state.trim_left( );
					daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					auto const old_class_pos = parse_state.get_class_position( );
					auto const class_first = parse_state.data( );
					parse_state.set_class_position( );
					auto event = structural_state.next( class_first );
					daw_json_assert_weak( event != nullptr and *event == '{',
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					event = structural_state.next( event + 1 );
					if constexpr( all_json_members_must_exist_v<result_t, ParseState> ) {
						daw_json_assert_weak( event != nullptr and *event == '}',
						                      ErrorReason::UnknownMember,
						                      parse_state );
					} else {
						auto object_depth = std::size_t{ 1 };
						auto array_depth = std::size_t{ 0 };
						auto found_end = false;
						while( event != nullptr and not found_end ) {
							switch( *event ) {
							case '{':
								++object_depth;
								break;
							case '}':
								if( object_depth == 1U and array_depth == 0U ) {
									found_end = true;
									break;
								}
								daw_json_assert_weak( object_depth > 1U,
								                      ErrorReason::InvalidBracketing,
								                      parse_state );
								--object_depth;
								break;
							case '[':
								++array_depth;
								break;
							case ']':
								daw_json_assert_weak( array_depth > 0U,
								                      ErrorReason::InvalidBracketing,
								                      parse_state );
								--array_depth;
								break;
							default:
								break;
							}
							if( not found_end ) {
								event = structural_state.next( event + 1 );
							}
						}
						daw_json_assert_weak( event != nullptr,
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
					}
					auto const class_end = event + 1;
					parse_state.first = class_end;
					parse_state.trim_left_checked( );
					parse_state.set_class_position( old_class_pos );
					return class_end;
				}

				template<typename JsonClass, typename ParseState>
				[[nodiscard]] static constexpr json_result_t<JsonClass>
				construct_class( ParseState &parse_state, empty_locations const & ) {
					using result_t = json_result_t<JsonClass>;
					using constructor_t = json_constructor_t<JsonClass>;
					if constexpr( should_construct_explicitly_v<constructor_t,
					                                            result_t,
					                                            ParseState> ) {
						return result_t{ };
					} else {
						return construct_value_tp<result_t, constructor_t>( parse_state,
						                                                    fwd_pack{ } );
					}
				}
			};

			template<typename Locations>
			struct simd_class_value_state {
				char const *first = nullptr;
				char const *last = nullptr;
				Locations locations;
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			class json_simd_block_iterator_class {
				static_assert( JsonMember::underlying_json_type ==
				               JsonBaseParseTypes::Class );
				static_assert(
				  std::is_same_v<CharT, char>,
				  "SIMD class iteration currently supports char input only" );

				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using parser_type =
				  simd_class_parser<typename JsonMember::json_member_list>;
				using locations_type =
				  typename parser_type::template locations_type<ParseState>;
				using value_state_type = simd_class_value_state<locations_type>;

			public:
				using json_member = JsonMember;
				using value_type = typename json_member::parse_to_t;
				using reference = value_type;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

			private:
				char const *m_first = nullptr;
				char const *m_last = nullptr;
				bool m_previous_value = false;
				simd_class_structural_state m_structural_state{ };
				value_state_type m_current{
				  nullptr,
				  nullptr,
				  parser_type::template make_locations<ParseState>( ) };

				constexpr void move_to_next_value( ) {
					m_current.first = nullptr;
					m_current.last = nullptr;
					if( m_first == nullptr or m_first == m_last ) {
						return;
					}

					auto parse_state = ParseState( m_first, m_last );
					parse_state.trim_left( );
					if( parse_state.empty( ) ) {
						m_first = m_last;
						return;
					}

					if( m_previous_value ) {
						if( parse_state.front( ) == ']' ) {
							parse_state.remove_prefix( );
							m_first = m_last;
							return;
						}
						daw_json_assert_weak( parse_state.front( ) == ',',
						                      ErrorReason::InvalidEndOfValue,
						                      parse_state );
						parse_state.remove_prefix( );
						parse_state.trim_left( );
						daw_json_assert_weak( not parse_state.empty( ) and
						                        parse_state.front( ) != ']',
						                      ErrorReason::TrailingComma,
						                      parse_state );
					} else if( parse_state.front( ) == ']' ) {
						parse_state.remove_prefix( );
						m_first = m_last;
						return;
					}

					daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					m_current.first = parse_state.data( );
					m_current.locations =
					  parser_type::template make_locations<ParseState>( );
					m_current.last = parser_type::template scan_class<JsonMember>(
					  parse_state, m_current.locations, m_structural_state );
					m_first = parse_state.data( );
					m_previous_value = true;
				}

			public:
				constexpr json_simd_block_iterator_class( ) = default;

				explicit constexpr json_simd_block_iterator_class(
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
						m_structural_state.reset( m_first, m_last );
					}
					move_to_next_value( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					auto parse_state = ParseState( m_current.first, m_current.last );
					parse_state.first = m_current.last;
					return parser_type::template construct_class<json_member>(
					  parse_state, m_current.locations );
				}

				constexpr json_simd_block_iterator_class &operator++( ) {
					move_to_next_value( );
					return *this;
				}

				constexpr void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
					return m_current.first != nullptr;
				}

				[[nodiscard]] constexpr json_simd_block_iterator_class begin( ) const {
					return *this;
				}

				[[nodiscard]] constexpr json_simd_block_iterator_class static end( ) noexcept {
					return { };
				}

				friend constexpr bool
				operator==( json_simd_block_iterator_class const &lhs,
				            json_simd_block_iterator_class const &rhs ) noexcept {
					auto const lhs_at_end = not lhs;
					auto const rhs_at_end = not rhs;
					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_current.first == rhs.m_current.first;
				}

				friend constexpr bool
				operator!=( json_simd_block_iterator_class const &lhs,
				            json_simd_block_iterator_class const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};
		} // namespace json_details::simd_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
