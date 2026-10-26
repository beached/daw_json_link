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

#if defined( DAW_HAS_SIMD )

#include "daw/json/daw_json_link_types.h"
#include "daw/json/impl/daw_json_lexer_cursor.h"
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
			/// Whether a comma may come before the closing bracket of a class or of
			/// the array.  The scalar parser accepts them and there is no parse
			/// option for it yet
			inline constexpr bool simd_class_allows_trailing_comma = true;

			/**
			 * Streaming stage 1 for class iteration.  The input is indexed a chunk at a
			 * time into a fixed buffer of structural offsets, see
			 * lexer_cursor::build_structural_chunk.  The events are the structural
			 * characters, the opening quote of each string and the first byte of each
			 * number or literal, so a string or scalar ends before the next event.
			 * Walking the buffer only branches on running out of a chunk, not on each
			 * block.
			 */
			struct simd_class_event_buffer {
				/// Bytes of input indexed by a refill
				static constexpr std::size_t chunk_size =
				  16U * lexer_cursor::block_size;
				static constexpr std::size_t capacity =
				  chunk_size + lexer_cursor::structural_index_padding;

				char const *chunk_first = nullptr;
				char const *next_chunk = nullptr;
				char const *last = nullptr;
				std::uint32_t position = 0U;
				std::uint32_t size = 0U;
				lexer_cursor::chunk_carry carry{ };
				// Only [0, size) is read, so it is not initialized
				std::uint32_t offsets[capacity];

				/// first must be outside of a string
				void reset( char const *first, char const *last_ptr ) {
					chunk_first = first;
					next_chunk = first;
					last = last_ptr;
					position = 0U;
					size = 0U;
					carry = { };
				}

				/// Index the next chunk that has events.  false at the end of the data
				DAW_ATTRIB_NOINLINE bool refill( ) {
					while( next_chunk < last ) {
						auto const remaining = static_cast<std::size_t>( last - next_chunk );
						auto const count = remaining < chunk_size ? remaining : chunk_size;
						chunk_first = next_chunk;
						size = lexer_cursor::build_structural_chunk(
						  next_chunk, count, carry, offsets );
						position = 0U;
						next_chunk += count;
						if( size != 0U ) {
							return true;
						}
					}
					return false;
				}

				/// The next event, nullptr at the end of the data
				[[nodiscard]] DAW_ATTRIB_INLINE char const *next( ) {
					if( DAW_UNLIKELY( position == size ) ) {
						if( not refill( ) ) {
							return nullptr;
						}
					}
					return chunk_first + offsets[position++];
				}
			};

			/// The whitespace of the lexer_cursor stage 1, 0x01 to 0x20
			[[nodiscard]] constexpr bool is_json_whitespace( char value ) noexcept {
				return static_cast<unsigned char>( value ) - 1U < 0x20U;
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

			/// The name of the member whose opening quote is before name_first, with
			/// colon the event after it.  Stage 1 found the real closing quote, so
			/// escaped quotes need no handling here.  Like the scalar parser, names
			/// are compared as written, AllowEscapedNames is about the mapped names
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE daw::string_view
			scan_member_name( char const *name_first, char const *colon,
			                  ParseState const &parse_state ) {
				// Only whitespace is between the closing quote and the colon
				auto const name_last = trim_value_right( name_first, colon ) - 1;
				daw_json_assert_weak( name_last >= name_first and *name_last == '"',
				                      ErrorReason::InvalidMemberName,
				                      parse_state );
				return daw::string_view(
				  name_first, static_cast<std::size_t>( name_last - name_first ) );
			}

			/// The event after the value of a mapped member starting at value_first.
			/// The value is parsed and validated when the class is constructed, only
			/// the spelling of literals is checked here
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE char const *
			scan_mapped_value( char const *value_first,
			                   simd_class_event_buffer &events,
			                   ParseState const &parse_state ) {
				switch( *value_first ) {
				case '{':
				case '[': {
					// Strings and scalars are one event, so only the brackets matter
					auto depth = std::size_t{ 1 };
					auto event = events.next( );
					while( event != nullptr ) {
						auto const c = *event;
						if( c == '{' or c == '[' ) {
							++depth;
						} else if( c == '}' or c == ']' ) {
							if( --depth == 0U ) {
								break;
							}
						}
						event = events.next( );
					}
					daw_json_assert_weak(
					  event != nullptr, ErrorReason::UnexpectedEndOfData, parse_state );
					return events.next( );
				}
				case 't':
				case 'f':
				case 'n': {
					auto const delimiter = events.next( );
					if constexpr( not ParseState::is_unchecked_input ) {
						auto const value_last = trim_value_right(
						  value_first, delimiter == nullptr ? parse_state.last : delimiter );
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
						default:
							daw_json_assert_weak(
							  matches_literal( value_first, value_last, "null" ),
							  ErrorReason::InvalidNull,
							  parse_state );
							break;
						}
					}
					return delimiter;
				}
				default:
					// A string or a number ends before the next event
					return events.next( );
				}
			}

			/// Walk the events of the class or array at value_state.first, whose
			/// event has been taken, to its closing bracket, which is returned.  By
			/// default, like simdjson's On-Demand, only the depth is counted.  With
			/// options::StrictSkipChecks::yes the bracket kinds, member names, colons
			/// and commas are checked too, nesting at most
			/// DAW_JSON_STRICT_SKIP_MAX_DEPTH deep.  A string, number or literal is
			/// one event and its spelling is not checked
			template<typename ParseState>
			[[nodiscard]] char const *
			skip_container_events( ParseState const &value_state,
			                       simd_class_event_buffer &events ) {
				if constexpr( ParseState::is_unchecked_input or
				              not ParseState::strict_skip_checks ) {
					auto depth = std::size_t{ 1 };
					while( true ) {
						auto const event = events.next( );
						if( DAW_UNLIKELY( event == nullptr ) ) {
							daw_json_assert_weak(
							  false, ErrorReason::UnexpectedEndOfData, value_state );
							return value_state.last;
						}
						auto const c = *event;
						if( c == '{' or c == '[' ) {
							++depth;
						} else if( ( c == '}' or c == ']' ) and --depth == 0U ) {
							return event;
						}
					}
				} else {
					enum class expect { first_item, item, separator };
					auto brackets = strict_skip_brackets{ };
					(void)brackets.push( *value_state.first == '{' );
					auto state = expect::first_item;
					while( true ) {
						auto event = events.next( );
						daw_json_assert_weak(
						  event != nullptr, ErrorReason::UnexpectedEndOfData, value_state );
						auto c = *event;
						auto const in_class = brackets.in_class( );
						if( state == expect::separator and c == ',' ) {
							state = expect::item;
							continue;
						}
						if( c == '}' or c == ']' ) {
							daw_json_assert_weak( state != expect::item or
							                        simd_class_allows_trailing_comma,
							                      ErrorReason::TrailingComma,
							                      value_state );
							daw_json_assert_weak( c == ( in_class ? '}' : ']' ),
							                      ErrorReason::InvalidBracketing,
							                      value_state );
							if( brackets.pop( ) ) {
								return event;
							}
							state = expect::separator;
							continue;
						}
						daw_json_assert_weak( state != expect::separator,
						                      ErrorReason::ExpectedTokenNotFound,
						                      value_state );
						if( in_class ) {
							daw_json_assert_weak(
							  c == '"', ErrorReason::InvalidMemberName, value_state );
							auto const colon = events.next( );
							daw_json_assert_weak( colon != nullptr and *colon == ':',
							                      ErrorReason::InvalidMemberName,
							                      value_state );
							event = events.next( );
							daw_json_assert_weak( event != nullptr,
							                      ErrorReason::UnexpectedEndOfData,
							                      value_state );
							c = *event;
						}
						state = expect::separator;
						switch( c ) {
						case '{':
						case '[':
						{
							auto const pushed = brackets.push( c == '{' );
							daw_json_assert_weak(
							  pushed, ErrorReason::MaxDepthExceeded, value_state );
						}
							state = expect::first_item;
							break;
						case ',':
						case ':':
						case '}':
						case ']':
							daw_json_error(
							  true, ErrorReason::InvalidStartOfValue, value_state );
						default:
							// A string, number or literal
							break;
						}
					}
				}
			}

			/// Skip a member value that is not parsed and return the event after
			/// it, see skip_container_events
			template<typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE char const *
			skip_unmapped_value( ParseState const &value_state,
			                     simd_class_event_buffer &events ) {
				auto const c = *value_state.first;
				if( c == '{' or c == '[' ) {
					(void)skip_container_events( value_state, events );
				}
				return events.next( );
			}

			template<typename JsonMemberList>
			struct simd_class_parser;

			template<typename... JsonMembers>
			struct simd_class_parser<json_member_list<JsonMembers...>> {
				// Members are parsed from the positions found by one scan of the
				// class, the members they depend on are not looked up
				static_assert( not( has_class_dependent_v<JsonMembers> or ... ),
				               "json_simd_block_iterator does not support members that "
				               "depend on another member, like tagged variants and "
				               "sized arrays.  Use json_array_iterator for these "
				               "classes" );

				template<typename ParseState>
				using locations_type =
				  decltype( make_locations_info<ParseState, JsonMembers...>( ) );

				template<typename ParseState>
				[[nodiscard]] static constexpr locations_type<ParseState>
				make_locations( ) {
					return make_locations_info<ParseState, JsonMembers...>( );
				}

				/// Record where the members of the class whose opening brace is
				/// parse_state.first start, the brace's event was taken from events.
				/// Returns one past the closing brace.  Members not mapped, and repeats
				/// of mapped ones, are skipped, see skip_container_events
				template<typename JsonClass, typename ParseState>
				[[nodiscard]] static char const *
				scan_class( ParseState &parse_state,
				            locations_type<ParseState> &locations,
				            simd_class_event_buffer &events ) {
					using result_t = json_result_t<JsonClass>;
					DAW_CPP23_STATIC_LOCAL constexpr bool must_exist =
					  all_json_members_must_exist_v<result_t, ParseState>;

					auto const class_first = parse_state.first;
					auto const last = parse_state.last;
					auto event = events.next( );
					while( true ) {
						daw_json_assert_weak(
						  event != nullptr, ErrorReason::UnexpectedEndOfData, parse_state );
						if( *event == '}' ) {
							break;
						}
						daw_json_assert_weak(
						  *event == '"', ErrorReason::InvalidMemberName, parse_state );
						auto const colon = events.next( );
						daw_json_assert_weak( colon != nullptr and *colon == ':',
						                      ErrorReason::InvalidMemberName,
						                      parse_state );
						auto const name =
						  scan_member_name( event + 1, colon, parse_state );
						auto const value_first = events.next( );
						daw_json_assert_weak( value_first != nullptr,
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
						daw_json_assert_weak( *value_first != ',' and
						                        *value_first != '}' and
						                        *value_first != ']' and
						                        *value_first != ':',
						                      ErrorReason::InvalidStartOfValue,
						                      parse_state );

						auto const name_pos = locations.template find_name<0>( name );
						char const *delimiter = nullptr;
						if( name_pos < locations.size( ) and
						    locations[name_pos].missing( ) ) {
							locations[name_pos].set_range(
							  ParseState( value_first, last, class_first, last ) );
							delimiter = scan_mapped_value( value_first, events, parse_state );
						} else {
							if constexpr( must_exist ) {
								daw_json_error( true, ErrorReason::UnknownMember, parse_state );
							}
							delimiter = skip_unmapped_value(
							  ParseState( value_first, last, class_first, last ), events );
						}

						daw_json_assert_weak(
						  delimiter != nullptr, ErrorReason::UnexpectedEndOfData, parse_state );
						if( *delimiter == '}' ) {
							event = delimiter;
							break;
						}
						daw_json_assert_weak( *delimiter == ',',
						                      ErrorReason::ExpectedTokenNotFound,
						                      parse_state );
						event = events.next( );
						if( event != nullptr and *event == '}' ) {
							daw_json_assert_weak( simd_class_allows_trailing_comma,
							                      ErrorReason::TrailingComma,
							                      parse_state );
							break;
						}
					}
					return event + 1;
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
						// A missing member, which may be nullable
						return parse_class_member_impl<JsonMember, false>(
						  parse_state, find_result<ParseState>{ member_state, true } );
					}
					// The value is parsed in place, as the scalar parser does for a
					// member in order
					return parse_value<without_name<JsonMember>,
					                   false,
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
				[[nodiscard]] static char const *
				scan_class( ParseState &parse_state, empty_locations &,
				            simd_class_event_buffer &events ) {
					using result_t = json_result_t<JsonClass>;
					if constexpr( all_json_members_must_exist_v<result_t, ParseState> ) {
						auto const event = events.next( );
						daw_json_assert_weak( event != nullptr and *event == '}',
						                      ErrorReason::UnknownMember,
						                      parse_state );
						return event + 1;
					} else {
						// Every member is skipped, see skip_container_events
						return skip_container_events( parse_state, events ) + 1;
					}
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
				value_state_type m_current{
				  nullptr,
				  nullptr,
				  parser_type::template make_locations<ParseState>( ) };
				simd_class_event_buffer m_events;

				void move_to_next_value( ) {
					m_current.first = nullptr;
					m_current.last = nullptr;
					if( m_first == nullptr or m_first == m_last ) {
						return;
					}
					auto const error_state = ParseState( m_first, m_last );
					auto event = m_events.next( );
					daw_json_assert_weak(
					  event != nullptr, ErrorReason::UnexpectedEndOfData, error_state );
					if( m_previous_value ) {
						if( *event == ']' ) {
							m_first = m_last;
							return;
						}
						daw_json_assert_weak(
						  *event == ',', ErrorReason::InvalidEndOfValue, error_state );
						event = m_events.next( );
						daw_json_assert_weak(
						  event != nullptr, ErrorReason::UnexpectedEndOfData, error_state );
						if( *event == ']' ) {
							daw_json_assert_weak( simd_class_allows_trailing_comma,
							                      ErrorReason::TrailingComma,
							                      error_state );
							m_first = m_last;
							return;
						}
					} else if( *event == ']' ) {
						m_first = m_last;
						return;
					}

					auto parse_state = ParseState( event, m_last );
					daw_json_assert_weak(
					  *event == '{', ErrorReason::InvalidClassStart, parse_state );
					m_current.first = event;
					m_current.locations =
					  parser_type::template make_locations<ParseState>( );
					m_current.last = parser_type::template scan_class<JsonMember>(
					  parse_state, m_current.locations, m_events );
					m_previous_value = true;
				}

			public:
				json_simd_block_iterator_class( ) = default;

				explicit json_simd_block_iterator_class( std::string_view document )
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
						m_events.reset( m_first, m_last );
					}
					move_to_next_value( );
				}

				explicit json_simd_block_iterator_class( std::string_view document,
				                                         daw::string_view start_path )
				  : json_simd_block_iterator_class(
				      find_array_range<ParseState>( document, start_path ) ) {}

				[[nodiscard]] reference operator*( ) const {
					auto parse_state = ParseState( m_current.first, m_current.last );
					parse_state.first = m_current.last;
					return parser_type::template construct_class<json_member>(
					  parse_state, m_current.locations );
				}

				json_simd_block_iterator_class &operator++( ) {
					move_to_next_value( );
					return *this;
				}

				void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] explicit operator bool( ) const noexcept {
					return m_current.first != nullptr;
				}

				[[nodiscard]] json_simd_block_iterator_class begin( ) const {
					return *this;
				}

				[[nodiscard]] static json_simd_block_iterator_class end( ) noexcept {
					return { };
				}

				friend bool
				operator==( json_simd_block_iterator_class const &lhs,
				            json_simd_block_iterator_class const &rhs ) noexcept {
					auto const lhs_at_end = not lhs;
					auto const rhs_at_end = not rhs;
					if( lhs_at_end or rhs_at_end ) {
						return lhs_at_end == rhs_at_end;
					}
					return lhs.m_current.first == rhs.m_current.first;
				}

				friend bool
				operator!=( json_simd_block_iterator_class const &lhs,
				            json_simd_block_iterator_class const &rhs ) noexcept {
					return not( lhs == rhs );
				}
			};
		} // namespace json_details::simd_details
	} // namespace DAW_JSON_VER
} // namespace daw::json

#endif
