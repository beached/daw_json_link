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

#include "daw/json/impl/daw_json_location_info.h"
#include "daw/json/impl/daw_json_parse_class.h"
#include "daw/json/impl/daw_json_parse_name.h"
#include "daw/json/impl/daw_json_parse_value.h"

#include <cstddef>
#include <iterator>
#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_details {
			template<typename JsonMemberList>
			struct simd_class_parser;

			template<typename... JsonMembers>
			struct simd_class_parser<json_member_list<JsonMembers...>> {
				template<typename ParseState>
				using locations_type = decltype(
				  make_locations_info<ParseState, JsonMembers...>( ) );

				template<typename ParseState>
				[[nodiscard]] static constexpr locations_type<ParseState>
				make_locations( ) {
					return make_locations_info<ParseState, JsonMembers...>( );
				}

				template<typename JsonClass, typename ParseState>
				[[nodiscard]] static constexpr char const *
				scan_class( ParseState &parse_state,
				            locations_type<ParseState> &locations ) {
					using result_t = json_result_t<JsonClass>;
					constexpr auto must_exist =
					  all_json_members_must_exist_v<result_t, ParseState>;

					parse_state.trim_left( );
					daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					auto const old_class_pos = parse_state.get_class_position( );
					parse_state.set_class_position( );
					parse_state.remove_prefix( );
					parse_state.trim_left( );

					while( not parse_state.empty( ) and parse_state.front( ) != '}' ) {
						auto const name = parse_name( parse_state );
						auto const name_pos =
						  locations.template find_name<ParseState::expect_long_strings, 0>(
						    name );
						if constexpr( must_exist ) {
							daw_json_assert_weak( name_pos < locations.size( ),
							                      ErrorReason::UnknownMember,
							                      parse_state );
						}

						if( name_pos < locations.size( ) and
						    locations[name_pos].missing( ) ) {
							locations[name_pos].set_range( skip_value( parse_state ) );
						} else {
							if constexpr( must_exist ) {
								daw_json_assert_weak( name_pos >= locations.size( ),
								                      ErrorReason::UnknownMember,
								                      parse_state );
							}
							(void)skip_value( parse_state );
						}
						parse_state.move_next_member_or_end( );
					}

					daw_json_assert_weak( not parse_state.empty( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      parse_state );
					daw_json_assert_weak( parse_state.front( ) == '}',
					                      ErrorReason::InvalidEndOfValue,
					                      parse_state );
					parse_state.remove_prefix( );
					auto const class_end = parse_state.data( );
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
					return parse_class_member_impl<JsonMember, false>(
					  parse_state,
					  find_result<ParseState>{ member_state, true } );
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
						return result_t{ parse_member<JsonMembers>( parse_state,
						                                                   locations[Is] )... };
					} else {
						return construct_value_tp<result_t, constructor_t>(
						  parse_state,
						  fwd_pack{ parse_member<JsonMembers>( parse_state,
						                                               locations[Is] )... } );
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
				[[nodiscard]] static constexpr char const *
				scan_class( ParseState &parse_state, empty_locations & ) {
					using result_t = json_result_t<JsonClass>;
					parse_state.trim_left( );
					daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
					                      ErrorReason::InvalidClassStart,
					                      parse_state );
					auto const old_class_pos = parse_state.get_class_position( );
					parse_state.set_class_position( );

					char const *class_end = nullptr;
					if constexpr( all_json_members_must_exist_v<result_t, ParseState> ) {
						parse_state.remove_prefix( );
						parse_state.trim_left( );
						daw_json_assert_weak( not parse_state.empty( ) and
						                        parse_state.front( ) == '}',
						                      ErrorReason::UnknownMember,
						                      parse_state );
						parse_state.remove_prefix( );
						class_end = parse_state.data( );
					} else {
						auto const class_range = parse_state.skip_class( );
						class_end = class_range.data_end( );
					}
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
			struct simd_class_structural_state {
				char const *first = nullptr;
				char const *last = nullptr;
				Locations locations;
			};

			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			class json_simd_block_iterator_class {
				static_assert( JsonMember::underlying_json_type ==
				               JsonBaseParseTypes::Class );
				static_assert( std::is_same_v<CharT, char>,
				               "SIMD class iteration currently supports char input only" );

				using ParseState = TryDefaultParsePolicy<BasicParsePolicy<
				  options::details::make_parse_flags<PolicyFlags...>( ).value>>;
				using parser_type = simd_class_parser<typename JsonMember::json_member_list>;
				using locations_type = typename parser_type::template locations_type<ParseState>;
				using structural_state_type =
				  simd_class_structural_state<locations_type>;

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
				structural_state_type m_current{
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
					  parse_state, m_current.locations );
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
					}
					move_to_next_value( );
				}

				[[nodiscard]] constexpr reference operator*( ) const {
					auto parse_state = ParseState( m_current.first, m_current.last );
					parse_state.first = m_current.last;
					return parser_type::template construct_class<json_member>( parse_state,
					                                                               m_current.locations );
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

				[[nodiscard]] constexpr json_simd_block_iterator_class static
				end( ) noexcept {
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
