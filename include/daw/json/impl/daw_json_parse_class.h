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
#include "daw_json_location_info.h"
#include "daw_json_name.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_value.h"
#include "daw_json_skip.h"
#include "version.h"

#include <daw/daw_consteval.h>
#include <daw/daw_constinit.h>
#include <daw/daw_fwd_pack_apply.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <cstddef>
#include <exception>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {

			///
			/// Parse a class member in an ordered json class(class as array).  These
			/// are often referred to as JSON tuples
			/// @tparam JsonMember type mapping of member to parse
			/// @tparam ParseState The state/policy type for current parse @link
			/// daw_json_parse_policy.h @endlink
			/// @param member_index current position in array
			/// @param parse_state JSON data
			/// @return A reified value of type JsonMember::parse_to_t
			/// @pre parse_state.has_more( ) == true
			/// @pre parse_state.front( ) == '['
			///
			template<typename JsonMember, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_ordered_class_member( template_param<JsonMember>,
			                            std::size_t &member_index,
			                            ParseState &parse_state ) {

				using json_member_t = ordered_member_subtype_t<JsonMember>;

				parse_state.move_next_member_or_end( );
				/***
				 * Some members specify their index so there may be gaps between
				 * member data elements in the array.
				 */
				if constexpr( is_an_ordered_member_v<JsonMember> ) {
					pocm_details::maybe_skip_members<is_json_nullable_v<json_member_t>>(
					  parse_state, member_index, JsonMember::member_index );
				} else {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
				}

				// this is an out value, get position ready
				++member_index;

				if( DAW_UNLIKELY( parse_state.front( ) == ']' ) ) {
					if constexpr( is_json_nullable_v<json_member_t> ) {
						using constructor_t = typename json_member_t::constructor_t;
						return construct_value(
						  template_args<json_result<json_member_t>, constructor_t>,
						  parse_state );
					} else {
						daw_json_error( missing_member( "ordered_class_member" ),
						                parse_state );
					}
				}
				return parse_value<json_member_t>(
				  parse_state, ParseTag<json_member_t::expected_type>{ } );
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
			template<std::size_t member_position, typename JsonMember,
			         AllMembersMustExist must_exist, bool NeedsClassPositions,
			         typename ParseState, std::size_t N, typename CharT, bool B>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr json_result<
			  without_name<JsonMember>>
			parse_class_member( ParseState &parse_state,
			                    locations_info_t<N, CharT, B> &locations ) {
				parse_state.move_next_member_or_end( );

				daw_json_assert_weak( parse_state.is_at_next_class_member( ),
				                      ErrorReason::MissingMemberNameOrEndOfClass,
				                      parse_state );

				auto [loc, known] = find_class_member<member_position, must_exist>(
				  parse_state, locations, is_json_nullable_v<without_name<JsonMember>>,
				  JsonMember::name );

				// If the member was found loc will have it's position
				if( not known ) {
					if constexpr( NeedsClassPositions ) {
						auto const cf = parse_state.class_first;
						auto const cl = parse_state.class_last;
						if constexpr( use_direct_construction_v<
						                ParseState, without_name<JsonMember>> ) {
							auto const after_parse = daw::on_scope_exit( [&] {
								parse_state.class_first = cf;
								parse_state.class_last = cl;
							} );
							return parse_value<without_name<JsonMember>>(
							  parse_state, ParseTag<JsonMember::expected_type>{ } );
						} else {
							auto result = parse_value<without_name<JsonMember>>(
							  parse_state, ParseTag<JsonMember::expected_type>{ } );
							parse_state.class_first = cf;
							parse_state.class_last = cl;
							return result;
						}
					} else {
						return parse_value<without_name<JsonMember>>(
						  parse_state,
						  ParseTag<without_name<JsonMember>::expected_type>{ } );
					}
				}
				// We cannot find the member, check if the member is nullable
				if( loc.is_null( ) ) {
					if constexpr( is_json_nullable_v<without_name<JsonMember>> ) {
						return parse_value<without_name<JsonMember>, true>(
						  loc, ParseTag<without_name<JsonMember>::expected_type>{ } );
					} else {
						constexpr auto member_name = JsonMember::name;
						daw_json_error( missing_member( member_name ), parse_state );
					}
				}

				// Member was previously skipped
				return parse_value<without_name<JsonMember>, true>(
				  loc, ParseTag<without_name<JsonMember>::expected_type>{ } );
			}

			inline namespace {
				template<bool IsExactClass, typename ParseState, typename OldClassPos>
				DAW_ATTRIB_INLINE constexpr void

				class_cleanup_now( ParseState &parse_state,
				                   OldClassPos const &old_class_pos ) {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					parse_state.move_next_member_or_end( );
					// If we fulfill the contract before all values are parses
					parse_state.move_to_next_class_member( );
					if constexpr( IsExactClass ) {
						daw_json_assert_weak( parse_state.front( ) == '}',
						                      ErrorReason::UnknownMember, parse_state );
						parse_state.remove_prefix( );
					} else {
						(void)parse_state.skip_class( );
						// Yes this must be checked.  We maybe at the end of document. After
						// the 2nd try, give up
					}
					parse_state.trim_left_checked( );
					parse_state.set_class_position( old_class_pos );
				}

				template<bool AllMembersMustExist, typename ParseState,
				         typename OldClassPos>
				struct class_cleanup {
					ParseState &parse_state;
					OldClassPos const &old_class_pos;

					DAW_ATTRIB_INLINE
					CPP20CONSTEXPR ~class_cleanup( ) noexcept( false ) {
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
						if( DAW_IS_CONSTANT_EVALUATED( ) ) {
							class_cleanup_now<AllMembersMustExist>( parse_state,
							                                        old_class_pos );
						} else {
#endif
							if( std::uncaught_exceptions( ) == 0 ) {
								class_cleanup_now<AllMembersMustExist>( parse_state,
								                                        old_class_pos );
							}
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
						}
#endif
					}
				};
			} // namespace

#if not defined( _MSC_VER ) or defined( __clang__ )
			template<typename ParseState, typename... JsonMembers>
			inline constexpr auto
			  known_locations_v = make_locations_info<ParseState, JsonMembers...>( );
#endif

			/***
			 * Parse to the user supplied class.  The parser will run left->right if
			 * it can when the JSON document's order matches that of the order of
			 * the supplied classes ctor.  If there is an order mismatch, store the
			 * start/finish of JSON members we are interested in and return that to
			 * the members parser when needed.
			 */
			template<typename JsonClass, typename... JsonMembers, typename ParseState,
			         std::size_t... Is>
			[[nodiscard]] constexpr json_result<JsonClass>
			parse_json_class( ParseState &parse_state, std::index_sequence<Is...> ) {
				static_assert( is_a_json_type<JsonClass>::value );
				using T = typename JsonClass::base_type;
				using Constructor = typename JsonClass::constructor_t;
				static_assert( has_json_data_contract_trait_v<T>, "Unexpected type" );
				using must_exist = daw::constant<(
				  json_details::all_json_members_must_exist_v<T, ParseState>
				    ? AllMembersMustExist::yes
				    : AllMembersMustExist::no )>;

				parse_state.trim_left( );
				// TODO, use member name
				daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
				                      ErrorReason::InvalidClassStart, parse_state );

				auto const old_class_pos = parse_state.get_class_position( );
				parse_state.set_class_position( );
				parse_state.remove_prefix( );
				parse_state.trim_left( );

				if constexpr( sizeof...( JsonMembers ) == 0 ) {
					// Clang-CL with MSVC has issues if we don't do empties this way
					class_cleanup_now<
					  json_details::all_json_members_must_exist_v<T, ParseState>>(
					  parse_state, old_class_pos );

					if constexpr( use_direct_construction_v<ParseState, JsonClass> ) {
						return T{ };
					} else {
						return construct_value( template_args<T, Constructor>,
						                        parse_state );
					}
				} else {
					using NeedClassPositions = std::bool_constant<(
					  ( without_name<JsonMembers>::must_be_class_member or ... ) )>;

#if defined( _MSC_VER ) and not defined( __clang__ )
					auto known_locations =
					  make_locations_info<ParseState, JsonMembers...>( );
#else
					auto known_locations = known_locations_v<ParseState, JsonMembers...>;
#endif

					if constexpr( use_direct_construction_v<ParseState, JsonClass> ) {
						auto const run_after_parse = class_cleanup<
						  json_details::all_json_members_must_exist_v<T, ParseState>,
						  ParseState, decltype( old_class_pos )>{ parse_state,
						                                          old_class_pos };
						(void)run_after_parse;

						/*
						 * Rather than call directly use apply/tuple to evaluate
						 * left->right
						 */
						if constexpr( force_aggregate_construction_v<T> ) {
							return T{ parse_class_member<
							  Is, traits::nth_type<Is, JsonMembers...>, must_exist::value,
							  NeedClassPositions::value>( parse_state, known_locations )... };
						} else {
							return construct_value_tp<T, Constructor>(
							  parse_state, fwd_pack{ parse_class_member<
							                 Is, traits::nth_type<Is, JsonMembers...>,
							                 must_exist::value, NeedClassPositions::value>(
							                 parse_state, known_locations )... } );
						}
					} else {
						if constexpr( force_aggregate_construction_v<T> ) {
							auto result = T{ parse_class_member<
							  Is, traits::nth_type<Is, JsonMembers...>, must_exist::value,
							  NeedClassPositions::value>( parse_state, known_locations )... };

							class_cleanup_now<
							  json_details::all_json_members_must_exist_v<T, ParseState>>(
							  parse_state, old_class_pos );
							return result;
						} else {
							auto result = construct_value_tp<T, Constructor>(
							  parse_state, fwd_pack{ parse_class_member<
							                 Is, traits::nth_type<Is, JsonMembers...>,
							                 must_exist::value, NeedClassPositions::value>(
							                 parse_state, known_locations )... } );

							class_cleanup_now<
							  json_details::all_json_members_must_exist_v<T, ParseState>>(
							  parse_state, old_class_pos );
							return result;
						}
					}
				}
			}

			/***
			 * Parse to a class where the members are constructed from the values of
			 * a JSON array. Often this is used for geometric types like Point
			 */
			template<typename JsonClass, typename... JsonMembers, typename ParseState>
			[[nodiscard]] static constexpr json_result<JsonClass>
			parse_json_tuple_class( template_params<JsonClass, JsonMembers...>,
			                        ParseState &parse_state ) {
				static_assert( is_a_json_type<JsonClass>::value );
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
				auto const old_class_pos = parse_state.get_class_position( );
				parse_state.set_class_position( );
				parse_state.remove_prefix( );
				parse_state.trim_left( );

				size_t current_idx = 0;

				if constexpr( use_direct_construction_v<ParseState, JsonClass> ) {
					auto const run_after_parse = ordered_class_cleanup<
					  json_details::all_json_members_must_exist_v<T, ParseState>,
					  ParseState, decltype( old_class_pos )>{ parse_state,
					                                          old_class_pos };
					(void)run_after_parse;
					if constexpr( force_aggregate_construction_v<T> ) {
						return T{ parse_ordered_class_member(
						  template_arg<JsonMembers>, current_idx, parse_state )... };
					} else {
						return construct_value_tp<T, Constructor>(
						  parse_state,
						  fwd_pack{ parse_ordered_class_member(
						    template_arg<JsonMembers>, current_idx, parse_state )... } );
					}
				} else {
					auto result = [&] {
						if constexpr( force_aggregate_construction_v<T> ) {
							return T{ parse_ordered_class_member(
							  template_arg<JsonMembers>, current_idx, parse_state )... };
						} else {
							return construct_value_tp<T, Constructor>(
							  parse_state,
							  fwd_pack{ parse_ordered_class_member(
							    template_arg<JsonMembers>, current_idx, parse_state )... } );
						}
					}( );
					if constexpr( json_details::all_json_members_must_exist_v<
					                T, ParseState> ) {
						parse_state.trim_left( );
						daw_json_assert_weak( parse_state.front( ) == ']',
						                      ErrorReason::UnknownMember, parse_state );
						parse_state.remove_prefix( );
						parse_state.trim_left( );
					} else {
						(void)parse_state.skip_array( );
					}
					parse_state.set_class_position( old_class_pos );
					return result;
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
