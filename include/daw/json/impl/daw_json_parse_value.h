// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

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
#include "version.h"

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<LiteralAsStringOpt literal_as_string, bool KnownBounds = false,
			         typename ParseState>
			DAW_ONLY_INLINE inline constexpr void
			skip_quote_when_literal_as_string( ParseState &parse_state ) {
				if constexpr( literal_as_string == LiteralAsStringOpt::Always ) {
					daw_json_assert_weak( parse_state.is_quotes_checked( ),
					                      ErrorReason::InvalidNumberUnexpectedQuoting,
					                      parse_state );
					parse_state.remove_prefix( );
					if constexpr( KnownBounds ) {
						parse_state.last = std::prev( parse_state.last );
					}
				} else if constexpr( literal_as_string == LiteralAsStringOpt::Maybe ) {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					if( parse_state.front( ) == '"' ) {
						parse_state.remove_prefix( );
						if constexpr( KnownBounds ) {
							parse_state.last = std::prev( parse_state.last );
						}
					}
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Real>, ParseState &parse_state ) {
				using constructor_t = typename JsonMember::constructor_t;
				using element_t = typename JsonMember::base_type;

				if constexpr( KnownBounds ) {
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state,
					  parse_real<element_t, true>( parse_state ) );
				} else {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					daw_json_assert_weak(
					  parse_policy_details::is_number_start( parse_state.front( ) ),
					  ErrorReason::InvalidNumberStart, parse_state );

					// TODO allow for guaranteed copy elision
					auto result = construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state,
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
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Signed>, ParseState &parse_state ) {
				using constructor_t = typename JsonMember::constructor_t;
				using element_t = typename JsonMember::base_type;
				static_assert( daw::is_signed_v<element_t>, "Expected signed type" );
				if constexpr( KnownBounds ) {
					daw_json_assert_weak(
					  parse_policy_details::is_number_start( parse_state.front( ) ),
					  ErrorReason::InvalidNumberStart, parse_state );
				} else {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					daw_json_assert_weak(
					  parse_policy_details::is_number_start( parse_state.front( ) ),
					  ErrorReason::InvalidNumberStart, parse_state );
				}
				element_t sign = 1;
				if( parse_state.front( ) == '-' ) {
					parse_state.remove_prefix( );
					sign = -1;
				}

				if constexpr( KnownBounds ) {

					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state,
					  sign *
					    unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
					      ParseState::exec_tag, parse_state ) );
				} else {
					auto result = construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state,
					  sign *
					    unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
					      ParseState::exec_tag, parse_state ) );
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
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Unsigned>,
			             ParseState &parse_state ) {
				using constructor_t = typename JsonMember::constructor_t;
				using element_t = typename JsonMember::base_type;

				if constexpr( KnownBounds ) {
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					daw_json_assert_weak(
					  parse_policy_details::is_number( parse_state.front( ) ),
					  ErrorReason::InvalidNumber, parse_state );
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state,
					  unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
					    ParseState::exec_tag, parse_state ) );
				} else {
					daw_json_assert_weak( parse_state.has_more( ),
					                      ErrorReason::UnexpectedEndOfData, parse_state );
					skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
					  parse_state );
					daw_json_assert_weak(
					  parse_policy_details::is_number( parse_state.front( ) ),
					  ErrorReason::InvalidNumber, parse_state );
					auto result = construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state,
					  unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
					    ParseState::exec_tag, parse_state ) );
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
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Null>, ParseState &parse_state ) {

				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( KnownBounds ) {
					if( parse_state.is_null( ) ) {
						return construct_value<json_result<JsonMember>>( constructor_t{ },
						                                                 parse_state );
					}
					return parse_value<JsonMember, true>(
					  ParseTag<JsonMember::base_expected_type>{ }, parse_state );
				} else if constexpr( ParseState::is_unchecked_input ) {
					if( not parse_state.has_more( ) ) {
						return construct_value<json_result<JsonMember>>( constructor_t{ },
						                                                 parse_state );
					} else if( parse_state.front( ) == 'n' ) {
						parse_state.remove_prefix( 4 );
						parse_state.trim_left_unchecked( );
						parse_state.remove_prefix( );
						return construct_value<json_result<JsonMember>>( constructor_t{ },
						                                                 parse_state );
					}
					return parse_value<JsonMember>(
					  ParseTag<JsonMember::base_expected_type>{ }, parse_state );
				} else {
					if( parse_state.starts_with( "null" ) ) {
						parse_state.remove_prefix( 4 );
						daw_json_assert_weak(
						  parse_policy_details::at_end_of_item( parse_state.front( ) ),
						  ErrorReason::InvalidLiteral, parse_state );
						parse_state.trim_left_checked( );
						return construct_value<json_result<JsonMember>>( constructor_t{ },
						                                                 parse_state );
					}
					return parse_value<JsonMember>(
					  ParseTag<JsonMember::base_expected_type>{ }, parse_state );
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Bool>, ParseState &parse_state ) {
				daw_json_assert_weak( std::size( parse_state ) >= 4,
				                      ErrorReason::InvalidLiteral, parse_state );

				using constructor_t = typename JsonMember::constructor_t;

				if constexpr( KnownBounds ) {
					// We have already checked if it is a true/false
					if constexpr( ParseState::is_unchecked_input ) {
						return parse_state.front( ) == 't';
					} else {
						switch( parse_state.front( ) ) {
						case 't':
							return construct_value<json_result<JsonMember>>(
							  constructor_t{ }, parse_state, true );
						case 'f':
							return construct_value<json_result<JsonMember>>(
							  constructor_t{ }, parse_state, false );
						}
						daw_json_error( ErrorReason::InvalidLiteral, parse_state );
					}
				} else {
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
					if constexpr( JsonMember::literal_as_string !=
					              LiteralAsStringOpt::Never ) {
						skip_quote_when_literal_as_string<JsonMember::literal_as_string>(
						  parse_state );
					}
					parse_state.trim_left( );
					daw_json_assert_weak(
					  parse_policy_details::at_end_of_item( parse_state.front( ) ),
					  ErrorReason::InvalidEndOfValue, parse_state );
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state, result );
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::StringRaw>,
			             ParseState &parse_state ) {

				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( KnownBounds ) {
					if constexpr( JsonMember::empty_is_null == JsonNullable::Nullable ) {
						if( parse_state.empty( ) ) {
							return construct_value<json_result<JsonMember>>( constructor_t{ },
							                                                 parse_state );
						}
					}
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state, std::data( parse_state ),
					  std::size( parse_state ) );
				} else {
					if constexpr( JsonMember::allow_escape_character ==
					              AllowEscapeCharacter::Allow ) {
						auto const str = skip_string( parse_state );
						if constexpr( JsonMember::empty_is_null ==
						              JsonNullable::Nullable ) {
							if( str.empty( ) ) {
								return construct_value<json_result<JsonMember>>(
								  constructor_t{ }, parse_state );
							}
						}
						return construct_value<json_result<JsonMember>>(
						  constructor_t{ }, parse_state, std::data( str ),
						  std::size( str ) );
					} else {
						parse_state.remove_prefix( );

						char const *const first = parse_state.first;
						parse_state.template move_to_next_of<'"'>( );
						char const *const last = parse_state.first;
						parse_state.remove_prefix( );
						if constexpr( JsonMember::empty_is_null ==
						              JsonNullable::Nullable ) {
							if( first == last ) {
								return construct_value<json_result<JsonMember>>(
								  constructor_t{ }, parse_state );
							}
							return construct_value<json_result<JsonMember>>(
							  constructor_t{ }, parse_state, first,
							  static_cast<std::size_t>( last - first ) );
						} else {
							return construct_value<json_result<JsonMember>>(
							  constructor_t{ }, parse_state, first,
							  static_cast<std::size_t>( last - first ) );
						}
					}
				}
			}

			namespace {
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
			} // namespace

			template<typename T>
			using json_member_constructor_t = typename T::constructor_t;

			template<typename T>
			using json_member_parse_to_t = typename T::parse_to_t;

			template<typename T>
			inline constexpr bool has_json_member_constructor_v =
			  daw::is_detected_v<json_member_constructor_t, T>;

			template<typename T>
			inline constexpr bool has_json_member_parse_to_v =
			  daw::is_detected_v<json_member_constructor_t, T>;

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::StringEscaped>,
			             ParseState &parse_state ) {
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
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state, std::data( parse_state2 ),
					  daw::data_end( parse_state2 ) );
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
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state, std::data( parse_state2 ),
					  daw::data_end( parse_state2 ) );
				}
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Date>, ParseState &parse_state ) {

				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );
				auto str = skip_string( parse_state );
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value<json_result<JsonMember>>(
				  constructor_t{ }, parse_state, std::data( str ), std::size( str ) );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Custom>, ParseState &parse_state ) {

				if( ( ( parse_state.front( ) != '"' ) &
				      ( parse_state.class_first != nullptr ) ) &
				    ( ( parse_state.first > parse_state.class_first ) and
				      ( *std::prev( parse_state.first ) == '"' ) ) ) {
					parse_state.first = std::prev( parse_state.first );
				}
				auto const str = skip_value( parse_state );

				using constructor_t = typename JsonMember::from_converter_t;
				return construct_value<json_result<JsonMember>>(
				  constructor_t{ }, parse_state,
				  std::string_view( std::data( str ), std::size( str ) ) );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Class>, ParseState &parse_state ) {

				using element_t = typename JsonMember::base_type;
				daw_json_assert_weak( parse_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, parse_state );

				if constexpr( is_guaranteed_rvo_v<ParseState> ) {
					// This relies on non-trivial dtor's being allowed.  So C++20
					// constexpr or not in a constant expression.  It does allow for
					// construction of classes without move/copy special members
					if constexpr( not KnownBounds ) {
						auto const oe =
						  daw::on_exit_success( [&] { parse_state.trim_left_checked( ); } );
						return json_data_contract_trait_t<
						  element_t>::template parse_to_class<JsonMember>( parse_state );
					} else {
						return json_data_contract_trait_t<
						  element_t>::template parse_to_class<JsonMember>( parse_state );
					}
				} else {
					if constexpr( KnownBounds ) {
						return json_data_contract_trait_t<
						  element_t>::template parse_to_class<JsonMember>( parse_state );
					} else {
						auto result = json_data_contract_trait_t<
						  element_t>::template parse_to_class<JsonMember>( parse_state );
						// TODO: make trim_left
						parse_state.trim_left_checked( );
						return result;
					}
				}
			}

			/**
			 * Parse a key_value pair encoded as a json object where the keys are the
			 * member names
			 * @tparam JsonMember json_key_value type
			 * @tparam ParseState Input range type
			 * @param parse_state ParseState of input to parse
			 * @return Constructed key_value container
			 */
			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::KeyValue>,
			             ParseState &parse_state ) {

				static_assert( JsonMember::expected_type == JsonParseTypes::KeyValue,
				               "Expected a json_key_value" );
				daw_json_assert_weak( parse_state.is_opening_brace_checked( ),
				                      ErrorReason::ExpectedKeyValueToStartWithBrace,
				                      parse_state );

				parse_state.remove_prefix( );
				// We are inside a KV map, we can expected a quoted name next
				parse_state.template move_to_next_of<'"'>( );

#if defined( __GNUC__ ) or defined( __clang__ )
				using iter_t =
				  json_parse_kv_class_iterator<JsonMember, ParseState, KnownBounds>;
#else
				using iter_t =
				  json_parse_kv_class_iterator<JsonMember, ParseState, false>;
#endif
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value<json_result<JsonMember>>(
				  constructor_t{ }, parse_state, iter_t( parse_state ), iter_t( ) );
			}

			/**
			 * Parse a key_value pair encoded as a json object where the keys are the
			 * member names
			 * @tparam JsonMember json_key_value type
			 * @tparam ParseState Input ParseState type
			 * @param parse_state ParseState of input to parse
			 * @return Constructed key_value container
			 */
			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::KeyValueArray>,
			             ParseState &parse_state ) {

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
				return construct_value<json_result<JsonMember>>(
				  constructor_t{ }, parse_state, iter_t( parse_state ), iter_t( ) );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Array>, ParseState &parse_state ) {
				parse_state.trim_left( );
				daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, parse_state );
				parse_state.remove_prefix( );
				parse_state.trim_left_unchecked( );
				using iterator_t =
				  json_parse_array_iterator<JsonMember, ParseState, KnownBounds>;
				using constructor_t = typename JsonMember::constructor_t;
				return construct_value<json_result<JsonMember>>(
				  constructor_t{ }, parse_state, iterator_t( parse_state ),
				  iterator_t( ) );
			}

			template<JsonBaseParseTypes BPT, typename JsonMembers,
			         typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_FLATTEN constexpr json_result<JsonMembers>
			parse_variant_value( ParseState &parse_state ) {

				using element_t = typename JsonMembers::json_elements;
				constexpr std::size_t idx =
				  element_t::base_map[static_cast<int_fast8_t>( BPT )];

				if constexpr( idx <
				              std::tuple_size_v<typename element_t::element_map_t> ) {
					using JsonMember =
					  std::tuple_element_t<idx, typename element_t::element_map_t>;
					return parse_value<JsonMember>(
					  ParseTag<JsonMember::base_expected_type>{ }, parse_state );
				} else {
					daw_json_error( ErrorReason::UnexpectedJSONVariantType );
				}
			}

			template<typename JsonMember, bool /*KnownBounds*/, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_INLINE constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Variant>,
			             ParseState &parse_state ) {

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
			DAW_ATTRIBUTE_FLATTEN constexpr Result
			parse_visit( std::size_t idx, ParseState &parse_state ) {
				if( idx == pos ) {
					using JsonMember = std::tuple_element_t<pos, TypeList>;
					return { parse_value<JsonMember>(
					  ParseTag<JsonMember::expected_type>{ }, parse_state ) };
				}
				if constexpr( pos + 1 < std::tuple_size_v<TypeList> ) {
					return parse_visit<Result, TypeList, pos + 1>( idx, parse_state );
				} else {
					daw_json_error( ErrorReason::MissingMemberNameOrEndOfClass,
					                parse_state );
				}
			}

			template<typename JsonMember, bool /*KnownBounds*/, typename ParseState>
			[[nodiscard,
			  maybe_unused]] DAW_ONLY_INLINE inline constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::VariantTagged>,
			             ParseState &parse_state ) {

				using tag_member = typename JsonMember::tag_member;
				auto [is_found, parse_state2] = find_range<ParseState>(
				  daw::string_view( parse_state.class_first,
				                    static_cast<std::size_t>(
				                      parse_state.last - parse_state.class_first ) ),
				  tag_member::name );

				daw_json_assert( is_found, ErrorReason::TagMemberNotFound,
				                 parse_state );
				auto index = typename JsonMember::switcher{ }( parse_value<tag_member>(
				  ParseTag<tag_member::expected_type>{ }, parse_state2 ) );

				return parse_visit<json_result<JsonMember>,
				                   typename JsonMember::json_elements::element_map_t>(
				  index, parse_state );
			}

			template<typename JsonMember, bool KnownBounds, std::size_t N,
			         typename ParseState, bool B>
			[[nodiscard]] constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::VariantTagged>,
			             ParseState &parse_state ) {
				using tag_member = typename JsonMember::tag_member;
				auto [is_found, parse_state2] = find_range<ParseState>(
				  daw::string_view( parse_state.class_first,
				                    static_cast<std::size_t>(
				                      parse_state.last - parse_state.class_first ) ),
				  tag_member::name );

				daw_json_assert( is_found, ErrorReason::TagMemberNotFound,
				                 parse_state );
				auto index = typename JsonMember::switcher{ }( parse_value<tag_member>(
				  ParseTag<tag_member::expected_type>{ }, parse_state2 ) );

				return parse_visit<json_result<JsonMember>,
				                   typename JsonMember::json_elements::element_map_t>(
				  index, parse_state );
			}

			template<typename JsonMember, bool KnownBounds, typename ParseState>
			DAW_ATTRIBUTE_FLATTEN constexpr json_result<JsonMember>
			parse_value( ParseTag<JsonParseTypes::Unknown>,
			             ParseState &parse_state ) {
				using constructor_t = typename JsonMember::constructor_t;
				if constexpr( KnownBounds ) {
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state, std::data( parse_state ),
					  std::size( parse_state ) );
				} else {
					auto value_parse_state = skip_value( parse_state );
					return construct_value<json_result<JsonMember>>(
					  constructor_t{ }, parse_state, std::data( value_parse_state ),
					  std::size( value_parse_state ) );
				}
			}

			template<std::size_t N, typename JsonClass, bool KnownBounds,
			         typename... JsonClasses, typename ParseState>
			DAW_ONLY_FLATTEN constexpr json_result<JsonClass>
			parse_nth_class( std::size_t idx, ParseState &parse_state ) {
				if constexpr( sizeof...( JsonClasses ) >= N + 8 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 5: {
						using cur_json_class_t = traits::nth_element<N + 5, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 6: {
						using cur_json_class_t = traits::nth_element<N + 6, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 7: {
						using cur_json_class_t = traits::nth_element<N + 7, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					default:
						if constexpr( sizeof...( JsonClasses ) > N + 7 ) {
							return parse_nth_class<N + 8, JsonClass, KnownBounds,
							                       JsonClasses...>( idx, parse_state );
						} else {
							DAW_UNREACHABLE( );
							DAW_UNREACHABLE( );
						}
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 7 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 5: {
						using cur_json_class_t = traits::nth_element<N + 5, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 6: {
						using cur_json_class_t = traits::nth_element<N + 6, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					default:
						DAW_UNREACHABLE( );
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 6 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 5: {
						using cur_json_class_t = traits::nth_element<N + 5, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					default:
						DAW_UNREACHABLE( );
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 5 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 4: {
						using cur_json_class_t = traits::nth_element<N + 4, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					default:
						DAW_UNREACHABLE( );
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 4 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 3: {
						using cur_json_class_t = traits::nth_element<N + 3, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					default:
						DAW_UNREACHABLE( );
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 3 ) {
					switch( idx ) {
					case N + 0: {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 1: {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					case N + 2: {
						using cur_json_class_t = traits::nth_element<N + 2, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
					default:
						DAW_UNREACHABLE( );
					}
				} else if constexpr( sizeof...( JsonClasses ) == N + 2 ) {
					if( idx == N ) {
						using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					} else {
						using cur_json_class_t = traits::nth_element<N + 1, JsonClasses...>;
						return parse_value<cur_json_class_t>(
						  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
					}
				} else {
					using cur_json_class_t = traits::nth_element<N + 0, JsonClasses...>;
					return parse_value<cur_json_class_t>(
					  ParseTag<cur_json_class_t::base_expected_type>{ }, parse_state );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
