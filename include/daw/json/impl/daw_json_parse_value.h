// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_assert.h"
#include "daw_json_parse_array_iterator.h"
#include "daw_json_parse_kv_array_iterator.h"
#include "daw_json_parse_kv_class_iterator.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_real.h"
#include "daw_json_parse_std_string.h"
#include "daw_json_parse_string_need_slow.h"
#include "daw_json_parse_string_quote.h"
#include "daw_json_parse_unsigned_int.h"
#include "daw_json_parse_value_fwd.h"

#include <daw/daw_attributes.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/***
			 * Depending on the type of literal, it may always be quoted, sometimes,
			 * or never.  This method handles the always and sometimes cases.
			 * In checked input, ensures State has more data.
			 * @tparam literal_as_string Is the literal being parsed enclosed in
			 * quotes.
			 * @tparam ParseState ParseState idiom
			 * @param parse_state Current parsing state
			 */
			template<LiteralAsStringOpt literal_as_string, typename ParseState>
			DAW_ATTRIB_INLINE inline constexpr void
			skip_quote_when_literal_as_string( ParseState &parse_state ) {
				if constexpr( literal_as_string == LiteralAsStringOpt::Always ) {
					daw_json_assert_weak( parse_state.is_quotes_checked( ),
					                      ErrorReason::InvalidNumberUnexpectedQuoting,
					                      parse_state );
					parse_state.remove_prefix( );
				} else if constexpr( literal_as_string == LiteralAsStringOpt::Maybe ) {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					if( parse_state.front( ) == '"' ) {
						parse_state.remove_prefix( );
					}
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Real> ) {
				using constructor_t = typename JsonMember::constructor_t;
				using element_t = typename JsonMember::base_type;

				if constexpr( KnownBounds ) {
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  parse_real<element_t, true>( parse_state ) );
				} else {
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					daw_json_assert_weak(
					  parse_state.has_more( ) and
					    parse_policy_details::is_number_start( parse_state.front( ) ),
					  ErrorReason::InvalidNumberStart, parse_state );

					auto result = construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  parse_real<element_t, false>( parse_state ) );
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					daw_json_assert_weak(
					  parse_policy_details::at_end_of_item( parse_state.front( ) ),
					  ErrorReason::InvalidEndOfValue, parse_state );
					return result;
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Signed> ) {
				using constructor_t = typename JsonMember::constructor_t;
				using element_t = typename JsonMember::base_type;
				using int_type =
				  typename std::conditional_t<std::is_enum_v<element_t>,
				                              std::underlying_type<element_t>,
				                              daw::traits::identity<element_t>>::type;

				static_assert( daw::is_signed<int_type>::value,
				               "Expected signed type" );
				if constexpr( KnownBounds ) {
					daw_json_assert_weak(
					  parse_policy_details::is_number_start( parse_state.front( ) ),
					  ErrorReason::InvalidNumberStart, parse_state );
				} else {
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					} else if constexpr( not ParseState::is_zero_terminated_string ) {
						daw_json_assert_weak( parse_state.has_more( ),
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
					}
				}
				int_type const sign = static_cast<int_type>(
				  parse_policy_details::validate_signed_first( parse_state ) );

				if constexpr( KnownBounds ) {
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  static_cast<element_t>(
					    sign * static_cast<int_type>(
					             unsigned_parser<element_t, JsonMember::range_check,
					                             KnownBounds>( ParseState::exec_tag,
					                                           parse_state ) ) ) );
				} else {
					auto result = construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  static_cast<element_t>(
					    sign * static_cast<int_type>(
					             unsigned_parser<element_t, JsonMember::range_check,
					                             KnownBounds>( ParseState::exec_tag,
					                                           parse_state ) ) ) );
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					parse_state.trim_left( );
					daw_json_assert_weak(
					  parse_policy_details::at_end_of_item( parse_state.front( ) ),
					  ErrorReason::InvalidEndOfValue, parse_state );
					return result;
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::Unsigned> ) {
				using constructor_t = typename JsonMember::constructor_t;
				using element_t = typename JsonMember::base_type;

				if constexpr( KnownBounds ) {
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					parse_policy_details::validate_unsigned_first( parse_state );

					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
					    ParseState::exec_tag, parse_state ) );
				} else {
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
						if constexpr( not ParseState::is_zero_terminated_string ) {
							daw_json_assert_weak( parse_state.has_more( ),
							                      ErrorReason::UnexpectedEndOfData,
							                      parse_state );
						}
					} else if constexpr( not ParseState::is_zero_terminated_string ) {
						daw_json_assert_weak( parse_state.has_more( ),
						                      ErrorReason::UnexpectedEndOfData,
						                      parse_state );
					}
					daw_json_assert_weak(
					  parse_policy_details::is_number( parse_state.front( ) ),
					  ErrorReason::InvalidNumber, parse_state );
					auto result = construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
					    ParseState::exec_tag, parse_state ) );
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
						if constexpr( not ParseState::is_zero_terminated_string ) {
							daw_json_assert_weak( parse_state.has_more( ),
							                      ErrorReason::UnexpectedEndOfData,
							                      parse_state );
						}
					}
					daw_json_assert_weak(
					  parse_policy_details::at_end_of_item( parse_state.front( ) ),
					  ErrorReason::InvalidEndOfValue, parse_state );
					return result;
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Null> ) {

				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( KnownBounds ) {
					// skip_value will leave a null parse_state
					if( parse_state.is_null( ) ) {
						if constexpr( JsonMember::nullable == JsonNullable::Nullable ) {
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state );
						} else {
							daw_json_error( missing_member(
							  std::string_view( std::data( JsonMember::name ),
							                    std::size( JsonMember::name ) ) ) );
						}
					}
					return parse_value<JsonMember, true>(
					  parse_state, ParseTag<JsonMember::base_expected_type>{ } );
				} else if constexpr( ParseState::is_unchecked_input ) {
					if( not parse_state.has_more( ) ) {
						if constexpr( JsonMember::nullable == JsonNullable::Nullable ) {
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state );
						} else {
							daw_json_error( missing_member(
							  std::string_view( std::data( JsonMember::name ),
							                    std::size( JsonMember::name ) ) ) );
						}
					} else if( parse_state.front( ) == 'n' ) {
						parse_state.remove_prefix( 4 );
						parse_state.trim_left_unchecked( );
						parse_state.remove_prefix( );
						return construct_value(
						  template_args<json_result<JsonMember>, constructor_t>,
						  parse_state );
					}
					return parse_value<JsonMember>(
					  parse_state, ParseTag<JsonMember::base_expected_type>{ } );
				} else {
					if( parse_state.starts_with( "null" ) ) {
						parse_state.remove_prefix( 4 );
						daw_json_assert_weak(
						  parse_policy_details::at_end_of_item( parse_state.front( ) ),
						  ErrorReason::InvalidLiteral, parse_state );
						parse_state.trim_left_checked( );
						return construct_value(
						  template_args<json_result<JsonMember>, constructor_t>,
						  parse_state );
					}
					return parse_value<JsonMember>(
					  parse_state, ParseTag<JsonMember::base_expected_type>{ } );
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Bool> ) {
				using constructor_t = typename JsonMember::constructor_t;

				if constexpr( KnownBounds ) {
					// We have already checked if it is a true/false
					if constexpr( ParseState::is_unchecked_input ) {
						return static_cast<bool>( parse_state.counter );
					} else {
						switch( parse_state.front( ) ) {
						case 't':
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state, true );
						case 'f':
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state, false );
						}
						daw_json_error( ErrorReason::InvalidLiteral, parse_state );
					}
				} else {
					// Beginning quotes
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					bool result = false;
					if constexpr( ParseState::is_unchecked_input ) {
						if( parse_state.front( ) == 't' ) /* true */ {
							result = true;
							parse_state.remove_prefix( 4 );
						} else /* false */ {
							parse_state.remove_prefix( 5 );
						}
					} else {
						if( parse_state.starts_with( "true" ) ) {
							parse_state.remove_prefix( 4 );
							result = true;
						} else if( parse_state.starts_with( "false" ) ) {
							parse_state.remove_prefix( 5 );
						} else {
							daw_json_error( ErrorReason::InvalidLiteral, parse_state );
						}
					}
					// Trailing quotes
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					parse_state.trim_left( );
					daw_json_assert_weak(
					  parse_policy_details::at_end_of_item( parse_state.front( ) ),
					  ErrorReason::InvalidEndOfValue, parse_state );
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  result );
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::StringRaw> ) {

				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( KnownBounds ) {
					if constexpr( JsonMember::empty_is_null == EmptyStringNull::yes ) {
						if( parse_state.empty( ) ) {
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state );
						}
					}
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  std::data( parse_state ), std::size( parse_state ) );
				} else {
					if constexpr( JsonMember::allow_escape_character ==
					              AllowEscapeCharacter::Allow ) {
						auto const str = skip_string( parse_state );
						if constexpr( JsonMember::empty_is_null == EmptyStringNull::yes ) {
							if( str.empty( ) ) {
								return construct_value(
								  template_args<json_result<JsonMember>, constructor_t>,
								  parse_state );
							}
						}
						return construct_value(
						  template_args<json_result<JsonMember>, constructor_t>,
						  parse_state, std::data( str ), std::size( str ) );
					} else {
						parse_state.remove_prefix( );

						char const *const first = parse_state.first;
						parse_state.template move_to_next_of<'"'>( );
						char const *const last = parse_state.first;
						parse_state.remove_prefix( );
						if constexpr( JsonMember::empty_is_null == EmptyStringNull::yes ) {
							if( first == last ) {
								return construct_value(
								  template_args<json_result<JsonMember>, constructor_t>,
								  parse_state );
							}
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state, first, static_cast<std::size_t>( last - first ) );
						} else {
							return construct_value(
							  template_args<json_result<JsonMember>, constructor_t>,
							  parse_state, first, static_cast<std::size_t>( last - first ) );
						}
					}
				}
			}

			/***
			 * We know that we are constructing a std::string or
			 * std::optional<std::string> We can take advantage of this and reduce
			 * the allocator time by presizing the string up front and then using a
			 * pointer to the data( ).
			 */
			template<typename JsonMember>
			struct can_parse_to_stdstring_fast
			  : std::disjunction<
			      can_single_allocation_string<json_result<JsonMember>>,
			      can_single_allocation_string<json_base_type<JsonMember>>> {};

			template<typename T>
			using json_member_constructor_t = typename T::constructor_t;

			template<typename T>
			using json_member_parse_to_t = typename T::parse_to_t;

			template<typename T>
			inline constexpr bool has_json_member_constructor_v =
			  daw::is_detected<json_member_constructor_t, T>::value;

			template<typename T>
			inline constexpr bool has_json_member_parse_to_v =
			  daw::is_detected<json_member_constructor_t, T>::value;

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::StringEscaped> ) {
				static_assert( has_json_member_constructor_v<JsonMember> );
				static_assert( has_json_member_parse_to_v<JsonMember> );

				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( can_parse_to_stdstring_fast<JsonMember>::value ) {
					constexpr bool AllowHighEightbits =
					  JsonMember::eight_bit_mode != EightBitModes::DisallowHigh;
					auto parse_state2 =
					  KnownBounds ? parse_state : skip_string( parse_state );
					// FIXME this needs std::string, fix
					if( not AllowHighEightbits or needs_slow_path( parse_state2 ) ) {
						// There are escapes in the string
						return parse_string_known_stdstring<AllowHighEightbits, JsonMember,
						                                    true>( parse_state2 );
					}
					// There are no escapes in the string, we can just use the ptr/size
					// ctor
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  std::data( parse_state2 ), daw::data_end( parse_state2 ) );
				} else {
					auto parse_state2 =
					  KnownBounds ? parse_state : skip_string( parse_state );
					constexpr bool AllowHighEightbits =
					  JsonMember::eight_bit_mode != EightBitModes::DisallowHigh;
					if( not AllowHighEightbits or needs_slow_path( parse_state2 ) ) {
						// There are escapes in the string
						return parse_string_known_stdstring<AllowHighEightbits, JsonMember,
						                                    true>( parse_state2 );
					}
					// There are no escapes in the string, we can just use the ptr/size
					// ctor
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  std::data( parse_state2 ), daw::data_end( parse_state2 ) );
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Date> ) {

				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				auto str = skip_string( parse_state );
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value(
				  template_args<json_result<JsonMember>, constructor_t>, parse_state,
				  std::data( str ), std::size( str ) );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Custom> ) {

				if( ( ( parse_state.front( ) != '"' ) &
				      ( parse_state.class_first != nullptr ) ) &
				    ( ( parse_state.first > parse_state.class_first ) and
				      ( *std::prev( parse_state.first ) == '"' ) ) ) {
					parse_state.first = std::prev( parse_state.first );
				}
				auto const str = skip_value( parse_state );

				using constructor_t = typename JsonMember::from_converter_t;
				return construct_value(
				  template_args<json_result<JsonMember>, constructor_t>, parse_state,
				  std::string_view( std::data( str ), std::size( str ) ) );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Class> ) {

				using element_t = typename JsonMember::base_type;
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );

				if constexpr( is_guaranteed_rvo_v<ParseState> ) {
					// This relies on non-trivial dtor's being allowed.  So C++20
					// constexpr or not in a constant expression.  It does allow for
					// construction of classes without move/copy special members
					if constexpr( not KnownBounds ) {
						auto const run_after_parse =
						  daw::on_exit_success( [&] { parse_state.trim_left_checked( ); } );
						(void)run_after_parse;
						return json_data_contract_trait_t<element_t>::parse_to_class(
						  parse_state, template_arg<JsonMember> );
					} else {
						return json_data_contract_trait_t<element_t>::parse_to_class(
						  parse_state, template_arg<JsonMember> );
					}
				} else {
					if constexpr( KnownBounds ) {
						return json_data_contract_trait_t<element_t>::parse_to_class(
						  parse_state, template_arg<JsonMember> );
					} else {
						if constexpr( force_aggregate_construction_v<element_t> ) {
							auto const oe = daw::on_exit_success(
							  [&] { parse_state.trim_left_checked( ); } );
							return json_data_contract_trait_t<element_t>::parse_to_class(
							  parse_state, template_arg<JsonMember> );
						} else {
							auto result =
							  json_data_contract_trait_t<element_t>::parse_to_class(
							    parse_state, template_arg<JsonMember> );
							// TODO: make trim_left
							parse_state.trim_left_checked( );
							return result;
						}
					}
				}
			}

			/**
			 * Parse a key_value pair encoded as a json object where the keys are
			 * the member names
			 * @tparam JsonMember json_key_value type
			 * @tparam ParseState Input range type
			 * @param parse_state ParseState of input to parse
			 * @return Constructed key_value container
			 */
			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::KeyValue> ) {

				static_assert( JsonMember::expected_type == JsonParseTypes::KeyValue,
				               "Expected a json_key_value" );
				daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
				                      ErrorReason::ExpectedKeyValueToStartWithBrace,
				                      parse_state );

				parse_state.remove_prefix( );
				parse_state.trim_left( );

#if defined( __GNUC__ ) or defined( __clang__ )
				using iter_t =
				  json_parse_kv_class_iterator<JsonMember, ParseState, KnownBounds>;
#else
				using iter_t =
				  json_parse_kv_class_iterator<JsonMember, ParseState, false>;
#endif
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value(
				  template_args<json_result<JsonMember>, constructor_t>, parse_state,
				  iter_t( parse_state ), iter_t( ) );
			}

			/**
			 * Parse a key_value pair encoded as a json object where the keys are
			 * the member names
			 * @tparam JsonMember json_key_value type
			 * @tparam ParseState Input ParseState type
			 * @param parse_state ParseState of input to parse
			 * @return Constructed key_value container
			 */
			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::KeyValueArray> ) {

				static_assert( JsonMember::expected_type ==
				                 JsonParseTypes::KeyValueArray,
				               "Expected a json_key_value" );
				daw_json_assert_weak(
				  parse_state.is_opening_bracket_checked( ),
				  ErrorReason::ExpectedKeyValueArrayToStartWithBracket, parse_state );

				parse_state.remove_prefix( );

				using iter_t =
				  json_parse_kv_array_iterator<JsonMember, ParseState, KnownBounds>;
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value(
				  template_args<json_result<JsonMember>, constructor_t>, parse_state,
				  iter_t( parse_state ), iter_t( ) );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state, ParseTag<JsonParseTypes::Array> ) {
				parse_state.trim_left( );
				daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, parse_state );
				parse_state.remove_prefix( );
				parse_state.trim_left_unchecked( );
				// TODO: add parse option to disable random access iterators. This is
				// coding to the implementations
				using iterator_t =
				  json_parse_array_iterator<JsonMember, ParseState, KnownBounds>;
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value(
				  template_args<json_result<JsonMember>, constructor_t>, parse_state,
				  iterator_t( parse_state ), iterator_t( ) );
			}

			template<JsonBaseParseTypes BPT, typename JsonMembers,
			         typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_FLATTEN constexpr json_result<JsonMembers>
			parse_variant_value( ParseState &parse_state ) {
				using element_t = typename JsonMembers::json_elements;
				constexpr std::size_t idx =
				  element_t::base_map[static_cast<int_fast8_t>( BPT )];

				if constexpr( idx < pack_size_v<typename element_t::element_map_t> ) {
					using JsonMember =
					  pack_element_t<idx, typename element_t::element_map_t>;
					return parse_value<JsonMember>(
					  parse_state, ParseTag<JsonMember::base_expected_type>{ } );
				} else {
					if constexpr( ParseState::is_unchecked_input ) {
						DAW_UNREACHABLE( );
					} else {
						daw_json_error( ErrorReason::UnexpectedJSONVariantType );
					}
				}
			}

			template<typename JsonMember, bool /*KnownBounds*/, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::Variant> ) {
				switch( parse_state.front( ) ) {
				case '{':
					return parse_variant_value<JsonBaseParseTypes::Class, JsonMember>(
					  parse_state );
				case '[':
					return parse_variant_value<JsonBaseParseTypes::Array, JsonMember>(
					  parse_state );
				case 't':
				case 'f':
					return parse_variant_value<JsonBaseParseTypes::Bool, JsonMember>(
					  parse_state );
				case '"':
					return parse_variant_value<JsonBaseParseTypes::String, JsonMember>(
					  parse_state );
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '+':
				case '-':
					return parse_variant_value<JsonBaseParseTypes::Number, JsonMember>(
					  parse_state );
				}
				if constexpr( ParseState::is_unchecked_input ) {
					DAW_UNREACHABLE( );
				} else {
					daw_json_error( ErrorReason::InvalidStartOfValue, parse_state );
				}
			}

			template<typename Result, typename TypeList, std::size_t pos = 0,
			         typename ParseState>
			DAW_ATTRIB_FLATINLINE constexpr Result
			parse_visit( std::size_t idx, ParseState &parse_state ) {
				if( idx == pos ) {
					using JsonMember = pack_element_t<pos, TypeList>;
					if constexpr( std::is_same_v<json_result<JsonMember>, Result> ) {
						return parse_value<JsonMember>(
						  parse_state, ParseTag<JsonMember::expected_type>{ } );
					} else {
						return { parse_value<JsonMember>(
						  parse_state, ParseTag<JsonMember::expected_type>{ } ) };
					}
				}
				if constexpr( pos + 1 < pack_size_v<TypeList> ) {
					return parse_visit<Result, TypeList, pos + 1>( idx, parse_state );
				} else {
					if constexpr( ParseState::is_unchecked_input ) {
						DAW_UNREACHABLE( );
					} else {
						daw_json_error( ErrorReason::MissingMemberNameOrEndOfClass,
						                parse_state );
					}
				}
			}

			template<typename JsonMember, bool /*KnownBounds*/, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ATTRIB_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::VariantTagged> ) {
				using tag_member = typename JsonMember::tag_member;
				auto [is_found, parse_state2] = find_range<ParseState>(
				  ParseState( parse_state.class_first, parse_state.last ),
				  tag_member::name );

				daw_json_assert( is_found, ErrorReason::TagMemberNotFound,
				                 parse_state );
				auto index = typename JsonMember::switcher{ }( parse_value<tag_member>(
				  parse_state2, ParseTag<tag_member::expected_type>{ } ) );

				return parse_visit<json_result<JsonMember>,
				                   typename JsonMember::json_elements::element_map_t>(
				  index, parse_state );
			}

			template<typename JsonMember, bool KnownBounds, std::size_t N,
			         typename ParseState, bool B>
			[[nodiscard]] constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::VariantTagged> ) {
				using tag_member = typename JsonMember::tag_member;
				auto [is_found, parse_state2] = find_range<ParseState>(
				  ParseState( parse_state.class_first, parse_state.last ),
				  tag_member::name );

				daw_json_assert( is_found, ErrorReason::TagMemberNotFound,
				                 parse_state );
				auto index = typename JsonMember::switcher{ }( parse_value<tag_member>(
				  parse_state2, ParseTag<tag_member::expected_type>{ } ) );

				return parse_visit<json_result<JsonMember>,
				                   typename JsonMember::json_elements::element_map_t>(
				  index, parse_state );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			DAW_ATTRIB_FLATINLINE constexpr json_result<JsonMember>
			parse_value( ParseState &parse_state,
			             ParseTag<JsonParseTypes::Unknown> ) {
				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( KnownBounds ) {
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  std::data( parse_state ), std::size( parse_state ) );
				} else {
					auto value_parse_state = skip_value( parse_state );
					return construct_value(
					  template_args<json_result<JsonMember>, constructor_t>, parse_state,
					  std::data( value_parse_state ), std::size( value_parse_state ) );
				}
			}

			template<std::size_t N, typename JsonClass, bool KnownBounds,
			         typename... JsonClasses, typename ParseState>
			DAW_ATTRIB_FLATTEN constexpr json_result<JsonClass>
			parse_nth_class( std::size_t idx, ParseState &parse_state ) {
				if constexpr( sizeof...( JsonClasses ) >= N + 8 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 5: {
						using cur_json_class_t = traits::nth_element<N + 5, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 6: {
						using cur_json_class_t = traits::nth_element<N + 6, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 7: {
						using cur_json_class_t = traits::nth_element<N + 7, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					default:
						if constexpr( sizeof...( JsonClasses ) >= N + 8 ) {
							return parse_nth_class<N + 8, JsonClass, KnownBounds,
							                       JsonClasses...>( idx, parse_state );
						} else {
							DAW_UNREACHABLE( );
						}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 7 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 5: {
						using cur_json_class_t = traits::nth_element<N + 5, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 6: {
						using cur_json_class_t = traits::nth_element<N + 6, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 6 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 5: {
						using cur_json_class_t = traits::nth_element<N + 5, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 5 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 4 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 3 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 2 ) {
					if( idx == N ) {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					} else {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
					}
				} else {
					using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
					return parse_value<cur_json_class_t>(
					  parse_state, ParseTag<cur_json_class_t::base_expected_type>{ } );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
