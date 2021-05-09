// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../daw_json_exception.h"
#include "daw_json_assert.h"
#include "daw_json_defs.h"
#include "daw_json_location_info.h"
#include "daw_json_name.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_value.h"
#include "daw_json_skip.h"
#include "version.h"

#include <daw/daw_traits.h>
#include <daw/daw_tuple.h>

#include <ciso646>
#include <cstddef>
#include <exception>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			namespace pocm_details {
				/***
				 * Maybe skip json members
				 * @tparam ParseState see IteratorRange
				 * @param parse_state JSON data
				 * @param current_position current member index
				 * @param desired_position desired member index
				 */
				template<bool Nullable, typename ParseState>
				constexpr void maybe_skip_members( ParseState &parse_state,
				                                   std::size_t &current_position,
				                                   std::size_t desired_position ) {

					parse_state.clean_tail( );
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					daw_json_assert_weak( current_position <= desired_position,
					                      ErrorReason::OutOfOrderOrderedMembers,
					                      parse_state );
					constexpr bool skip_check_end =
					  ParseState::is_unchecked_input and Nullable;
					while( ( current_position < desired_position ) &
					       ( skip_check_end or parse_state.front( ) != ']' ) ) {
						(void)skip_value( parse_state );
						parse_state.clean_tail( );
						++current_position;
						daw_json_assert_weak( parse_state.has_more( ),
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
					}
				}
			} // namespace pocm_details

			/***
			 * Parse a class member in an order json class(class as array)
			 * @tparam JsonMember type description of member to parse
			 * @tparam ParseState see IteratorRange
			 * @param member_index current position in array
			 * @param parse_state JSON data
			 * @return A reified value of type JsonMember::parse_to_t
			 */
			template<typename JsonMember, typename ParseState>
			[[nodiscard]] constexpr json_result<JsonMember>
			parse_ordered_class_member( std::size_t &member_index,
			                            ParseState &parse_state ) {

				using json_member_t = ordered_member_subtype_t<JsonMember>;
				/***
				 * Some members specify their index so there may be gaps between member
				 * data elements in the array.
				 */
				if constexpr( is_an_ordered_member_v<JsonMember> ) {
					pocm_details::maybe_skip_members<is_json_nullable_v<json_member_t>>(
					  parse_state, member_index, JsonMember::member_index );
				} else {
					parse_state.clean_tail( );
				}

				// this is an out value, get position ready
				++member_index;
				if constexpr( ParseState::is_unchecked_input ) {
					if constexpr( is_json_nullable_v<json_member_t> ) {
						if( parse_state.front( ) == ']' ) {
							using constructor_t = typename json_member_t::constructor_t;
							return constructor_t{ }( );
						}
					}
				} else {
					if( DAW_JSON_UNLIKELY( parse_state.front( ) == ']' ) ) {
						if constexpr( is_json_nullable_v<json_member_t> ) {
							using constructor_t = typename json_member_t::constructor_t;
							return constructor_t{ }( );
						} else {
							daw_json_error( missing_member( "ordered_class_member" ),
							                parse_state );
						}
					}
				}
				return parse_value<json_member_t>(
				  ParseTag<json_member_t::expected_type>{ }, parse_state );
			}

			/***
			 * Parse a member from a json_class
			 * @tparam member_position position in json_class member list
			 * @tparam JsonMember type description of member to parse
			 * @tparam N Number of members in json_class
			 * @tparam ParseState see IteratorRange
			 * @param locations location info for members
			 * @param parse_state JSON data
			 * @return parsed value from JSON data
			 */
			template<std::size_t member_position, typename JsonMember, std::size_t N,
			         typename ParseState, bool B>
			[[nodiscard]] constexpr json_result<JsonMember>
			parse_class_member( locations_info_t<N, B> &locations,
			                    ParseState &parse_state ) {
				parse_state.clean_tail( );
				static_assert(
				  not is_no_name<JsonMember>,
				  "Array processing should never call parse_class_member" );

				daw_json_assert_weak( parse_state.is_at_next_class_member( ),
				                      ErrorReason::MissingMemberNameOrEndOfClass,
				                      parse_state );
				ParseState loc = [&] {
					if constexpr( ParseState::has_allocator ) {
						return find_class_member<member_position>(
						         locations, parse_state, is_json_nullable_v<JsonMember>,
						         JsonMember::name )
						  .with_allocator( parse_state.get_allocator( ) );
					} else {
						return find_class_member<member_position>(
						  locations, parse_state, is_json_nullable_v<JsonMember>,
						  JsonMember::name );
					}
				}( );

				// If the member was found loc will have it's position
				if( loc.first == parse_state.first ) {
					return parse_value<JsonMember>(
					  ParseTag<JsonMember::expected_type>{ }, parse_state );
				}
				// We cannot find the member, check if the member is nullable
				if constexpr( is_json_nullable_v<JsonMember> ) {
					if( loc.is_null( ) ) {
						return parse_value<JsonMember, true>(
						  ParseTag<JsonMember::expected_type>{ }, loc );
					}
				} else {
					daw_json_assert_weak(
					  not loc.is_null( ),
					  missing_member( std::string_view( std::data( JsonMember::name ),
					                                    std::size( JsonMember::name ) ) ),
					  parse_state );
				}
				return parse_value<JsonMember, true>(
				  ParseTag<JsonMember::expected_type>{ }, loc );
			}

			template<typename ParseState>
			DAW_ATTRIBUTE_FLATTEN inline constexpr void
			class_cleanup_now( ParseState &parse_state ) {
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				parse_state.clean_tail( );
				// If we fulfill the contract before all values are parses
				parse_state.move_to_next_class_member( );
				(void)parse_state.skip_class( );
				// Yes this must be checked.  We maybe at the end of document.  After
				// the 2nd try, give up
				parse_state.trim_left_checked( );
			}

			/***
			 * Parse to the user supplied class.  The parser will run left->right if
			 * it can when the JSON document's order matches that of the order of the
			 * supplied classes ctor.  If there is an order mismatch, store the
			 * start/finish of JSON members we are interested in and return that to
			 * the members parser when needed.
			 */
			template<typename JsonClass, typename... JsonMembers, std::size_t... Is,
			         typename ParseState>
			[[nodiscard]] static constexpr json_result<JsonClass>
			parse_json_class( ParseState &parse_state, std::index_sequence<Is...> ) {
				static_assert( is_a_json_type_v<JsonClass> );
				using T = typename JsonClass::base_type;
				using Constructor = typename JsonClass::constructor_t;
				static_assert( has_json_data_contract_trait_v<T>, "Unexpected type" );

				parse_state.trim_left( );
				// TODO, use member name
				daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
				                      ErrorReason::InvalidClassStart, parse_state );
				parse_state.set_class_position( );
				parse_state.remove_prefix( );
				parse_state.trim_left( );

				if constexpr( sizeof...( JsonMembers ) == 0 ) {
					// Clang-CL with MSVC has issues if we don't do empties this way
					class_cleanup_now( parse_state );
					return construct_value<T>( Constructor{ }, parse_state );
				} else {

#if not defined( _MSC_VER ) or defined( __clang__ )
					// Prior to C++20, this will guarantee the data structure is
					// initialized at compile time.  In the future, constinit should be
					// fine.
					constexpr auto known_locations_v =
					  make_locations_info<ParseState, JsonMembers...>( );

					auto known_locations = known_locations_v;
#else
					// MSVC is doing the murmur3 hash at compile time incorrectly
					// this puts it at runtime.
					auto known_locations =
					  make_locations_info<ParseState, JsonMembers...>( );

#endif
					if constexpr( is_guaranteed_rvo_v<ParseState> ) {
						struct cleanup_t {
							ParseState *parse_state_ptr;
							CPP20CONSTEXPR
							inline ~cleanup_t( ) noexcept( false ) {
#ifdef HAS_CPP20CONSTEXPR
								if( not std::is_constant_evaluated( ) ) {
#endif
									if( std::uncaught_exceptions( ) == 0 ) {
										class_cleanup_now( *parse_state_ptr );
									}
#ifdef HAS_CPP20CONSTEXPR
								} else {
									class_cleanup_now( *parse_state_ptr );
								}
#endif
							}
						} const run_after_parse{ &parse_state };
						(void)run_after_parse;
						/*
						 * Rather than call directly use apply/tuple to evaluate left->right
						 */
						if constexpr( force_aggregate_construction_v<T> ) {
							return T{
							  parse_class_member<Is, traits::nth_type<Is, JsonMembers...>>(
							    known_locations, parse_state )... };
						} else {
							using tp_t = decltype( daw::forward_as_tuple(
							  parse_class_member<Is, traits::nth_type<Is, JsonMembers...>>(
							    known_locations, parse_state )... ) );

							return construct_value_tp<T>(
							  Constructor{ }, parse_state,
							  tp_t{
							    parse_class_member<Is, traits::nth_type<Is, JsonMembers...>>(
							      known_locations, parse_state )... } );
						}
					} else {
						using tp_t = decltype( daw::forward_as_tuple(
						  parse_class_member<Is, traits::nth_type<Is, JsonMembers...>>(
						    known_locations, parse_state )... ) );
						auto result = construct_value_tp<T>(
						  Constructor{ }, parse_state,
						  tp_t{
						    parse_class_member<Is, traits::nth_type<Is, JsonMembers...>>(
						      known_locations, parse_state )... } );
						class_cleanup_now( parse_state );
						return result;
					}
				}
			}

			/***
			 * Parse to a class where the members are constructed from the values of a
			 * JSON array. Often this is used for geometric types like Point
			 */
			template<typename JsonClass, typename... JsonMembers, typename ParseState>
			[[nodiscard]] static constexpr json_result<JsonClass>
			parse_ordered_json_class( ParseState &parse_state ) {
				static_assert( is_a_json_type_v<JsonClass> );
				using T = typename JsonClass::base_type;
				using Constructor = typename JsonClass::constructor_t;
				static_assert( has_json_data_contract_trait_v<T>, "Unexpected type" );
				static_assert(
				  std::is_invocable<Constructor,
				                    typename JsonMembers::parse_to_t...>::value,
				  "Supplied types cannot be used for construction of this type" );

				parse_state.trim_left( ); // Move to array start '['
				daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, parse_state );
				parse_state.set_class_position( );
				parse_state.remove_prefix( );
				parse_state.trim_left( );

				size_t current_idx = 0;

				using tp_t = decltype( daw::forward_as_tuple(
				  parse_ordered_class_member<JsonMembers>( current_idx,
				                                           parse_state )... ) );

				if constexpr( is_guaranteed_rvo_v<ParseState> ) {
					struct cleanup_t {
						ParseState *ptr;
						CPP20CONSTEXPR inline ~cleanup_t( ) noexcept( false ) {
#ifdef HAS_CPP20CONSTEXPR
							if( not std::is_constant_evaluated( ) ) {
#endif
								if( std::uncaught_exceptions( ) == 0 ) {
									(void)ptr->skip_array( );
								}
#ifdef HAS_CPP20CONSTEXPR
							} else {
								(void)ptr->skip_array( );
							}
#endif
						}
					} const run_after_parse{ &parse_state };
					(void)run_after_parse;
					return construct_value_tp<T>(
					  Constructor{ }, parse_state,
					  tp_t{ parse_ordered_class_member<JsonMembers>( current_idx,
					                                                 parse_state )... } );
				} else {
					auto result = construct_value_tp<T>(
					  Constructor{ }, parse_state,
					  tp_t{ parse_ordered_class_member<JsonMembers>( current_idx,
					                                                 parse_state )... } );

					(void)parse_state.skip_array( );
					return result;
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
