// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "daw_from_json_fwd.h"
#include "impl/daw_json_parse_class.h"
#include "impl/daw_json_parse_value.h"
#include "impl/daw_json_value.h"

#include <daw/daw_traits.h>

#include <iterator>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified T constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String,
		         auto... PolicyFlags>
		[[nodiscard]] constexpr auto
		from_json( String &&json_data, options::parse_flags_t<PolicyFlags...> ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );
			daw_json_ensure( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			daw_json_ensure( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );

			static_assert(
			  json_details::has_json_deduced_type_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using json_member = json_details::json_deduced_type<JsonMember>;
			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/// If the string is known to have a trailing zero, allow optimization on
			/// that
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/// In cases where we own the buffer or when requested and can, allow
			/// temporarily mutating it to reduce search costs
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;
			auto parse_state =
			  ParseState( std::data( json_data ), daw::data_end( json_data ) );

			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_ensure( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified JSONMember constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String>
		[[nodiscard]] constexpr auto from_json( String &&json_data ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );
			return from_json<JsonMember, KnownBounds>( DAW_FWD( json_data ),
			                                           options::parse_flags<> );
		}

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified T constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String,
		         typename Allocator, auto... PolicyFlags>
		[[nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, Allocator const &alloc,
		                 options::parse_flags_t<PolicyFlags...> ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );
			daw_json_ensure( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_ensure( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );

			using json_member = json_details::json_deduced_type<JsonMember>;

			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );

			char const *f = std::data( json_data );
			char const *l = daw::data_end( json_data );
			Allocator a = alloc;

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/// @brief If the string is known to have a trailing zero, allow
			/// optimization on that
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/// @brief In cases where we own the buffer or when requested and can,
			/// allow temporarily mutating it to reduce search costs
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto parse_state = ParseState::with_allocator( f, l, a );
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_ensure( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/// @brief Construct the JSONMember from the JSON document argument.
		/// @tparam JsonMember any bool, arithmetic, string, string_view,
		/// daw::json::json_data_contract
		/// @param json_data JSON string data
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A reified T constructed from JSON data
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String,
		         typename Allocator>
		[[nodiscard]] constexpr auto from_json_alloc( String &&json_data,
		                                              Allocator const &alloc ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );
			return from_json_alloc<JsonMember, KnownBounds>(
			  DAW_FWD( json_data ), alloc, options::parse_flags<> );
		}

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String,
		         auto... PolicyFlags>
		[[nodiscard]] constexpr auto
		from_json( String &&json_data, std::string_view member_path,
		           options::parse_flags_t<PolicyFlags...> ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );

			daw_json_ensure( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_ensure( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			daw_json_ensure( std::data( member_path ) != nullptr,
			                 ErrorReason::EmptyJSONPath );

			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/// @brief If the string is known to have a trailing zero, allow
			/// optimization on that
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/// @brief In cases where we own the buffer or when requested and can,
			/// allow temporarily mutating it to reduce search costs
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto jv = basic_json_value(
			  ParseState( std::data( json_data ), daw::data_end( json_data ) ) );
			jv = jv.find_member( member_path );

			if constexpr( json_details::is_json_nullable_v<json_member> ) {
				if( not jv ) {
					return json_details::construct_nullable_empty<
					  typename json_member::constructor_t>( );
				}
			} else {
				daw_json_ensure( jv, ErrorReason::JSONPathNotFound );
			}
			auto parse_state = jv.get_raw_state( );
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_ensure( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String>
		[[nodiscard]] constexpr auto from_json( String &&json_data,
		                                        std::string_view member_path ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );

			return from_json<JsonMember, KnownBounds>(
			  DAW_FWD( json_data ), member_path, options::parse_flags<> );
		}

		/// @brief Parse a JSON Member from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain the
		/// whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String,
		         typename Allocator, auto... PolicyFlags>
		[[nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, std::string_view member_path,
		                 Allocator const &alloc,
		                 options::parse_flags_t<PolicyFlags...> ) {

			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );
			daw_json_ensure( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_ensure( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_ensure( std::data( member_path ) != nullptr,
			                 ErrorReason::EmptyJSONPath );

			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/// @brief If the string is known to have a trailing zero, allow
			/// optimization on that
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/// @brief In cases where we own the buffer or when requested and can,
			/// allow temporarily mutating it to reduce search costs
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto first = std::data( json_data );
			auto last = daw::data_end( json_data );

			auto jv = basic_json_value(
			  ParseState( first, last, first, last ).with_allocator( alloc ) );
			jv = jv.find_member( member_path );

			if constexpr( json_details::is_json_nullable_v<json_member> ) {
				if( not jv ) {
					return json_details::construct_nullable_empty<
					  typename json_member::constructor_t>( );
				}
			} else {
				daw_json_ensure( jv, ErrorReason::JSONPathNotFound );
			}
			auto parse_state = jv.get_raw_state( );
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_ensure( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @tparam JsonMember The type of the item being parsed
		/// @param json_data JSON string data
		/// @param member_path A dot separated path of member names, default is
		/// the root.  Array indices are specified with square brackets e.g. [5]
		/// is the 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, typename String,
		         typename Allocator>
		[[nodiscard]] constexpr auto from_json_alloc( String &&json_data,
		                                              std::string_view member_path,
		                                              Allocator const &alloc ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );

			return from_json_alloc<JsonMember, KnownBounds>(
			  DAW_FWD( json_data ), member_path, alloc, options::parse_flags<> );
		}

		/// @brief Parse a value from a json_value
		/// @tparam JsonMember The type of the item being parsed
		/// @param value JSON data, see basic_json_value
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, json_options_t P,
		         typename Allocator, auto... PolicyFlags>
		[[nodiscard]] inline constexpr auto
		from_json( basic_json_value<P, Allocator> value,
		           options::parse_flags_t<PolicyFlags...> ) {
			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using ParsePolicy = typename BasicParsePolicy<
			  P, Allocator>::template SetPolicyOptions<PolicyFlags...>;

			auto const old_parse_state = value.get_raw_state( );
			auto parse_state =
			  ParsePolicy( old_parse_state.first, old_parse_state.last,
			               old_parse_state.class_first, old_parse_state.class_last,
			               old_parse_state.get_allocator( ) );

			return json_details::parse_value<json_member, KnownBounds>(
			  parse_state, ParseTag<json_member::expected_type>{ } );
		}

		/// @brief Parse a value from a json_value
		/// @tparam JsonMember The type of the item being parsed
		/// @param value JSON data, see basic_json_value
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, json_options_t PolicyFlags,
		         typename Allocator>
		[[nodiscard]] inline constexpr auto
		from_json( basic_json_value<PolicyFlags, Allocator> value ) {

			return from_json<JsonMember, KnownBounds>( DAW_MOVE( value ),
			                                           options::parse_flags<> );
		}

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @param value JSON data, see basic_json_value
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam JsonMember The type of the item being parsed
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, json_options_t P,
		         typename Allocator, auto... PolicyFlags>
		[[nodiscard]] constexpr auto
		from_json( basic_json_value<P, Allocator> value,
		           std::string_view member_path,
		           options::parse_flags_t<PolicyFlags...> ) {
			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;
			auto const old_parse_state = value.get_raw_state( );

			auto jv = basic_json_value(
			  ParsePolicy( old_parse_state.first, old_parse_state.last,
			               old_parse_state.class_first, old_parse_state.class_last,
			               old_parse_state.get_allocator( ) ) );

			jv = jv.find_member( member_path );

			if constexpr( json_details::is_json_nullable_v<json_member> ) {
				if( not jv ) {
					return json_details::construct_nullable_empty<
					  typename json_member::constructor_t>( );
				}
			} else {
				daw_json_ensure( jv, ErrorReason::JSONPathNotFound );
			}
			auto parse_state = jv.get_raw_state( );
			return json_details::parse_value<json_member, KnownBounds>(
			  parse_state, ParseTag<json_member::expected_type>{ } );
		}

		/// @brief Parse a JSONMember from the json_data starting at member_path.
		/// @param value JSON data, see basic_json_value
		/// @param member_path A dot separated path of member names, default is the
		/// root.  Array indices are specified with square brackets e.g. [5] is the
		/// 6th item
		/// @tparam JsonMember The type of the item being parsed
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A value reified from the JSON data member
		/// @throws daw::json::json_exception
		template<typename JsonMember, bool KnownBounds, json_options_t PolicyFlags,
		         typename Allocator>
		[[nodiscard]] constexpr auto from_json( basic_json_value<PolicyFlags> value,
		                                        std::string_view member_path ) {
			return from_json<JsonMember, KnownBounds>( DAW_MOVE( value ), member_path,
			                                           options::parse_flags<> );
		}

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String, auto... PolicyFlags>
		requires( json_details::is_string_view_like_v<String> )
		[[nodiscard]] constexpr Container
		from_json_array( String &&json_data,
		                 options::parse_flags_t<PolicyFlags...> ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );

			daw_json_ensure( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_ensure( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using element_type = json_details::json_deduced_type<JsonElement>;
			static_assert( traits::not_same_v<element_type, void>,
			               "Unknown JsonElement type." );

			using parser_t =
			  json_base::json_array<JsonElement, Container, Constructor>;

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/// @brief If the string is known to have a trailing zero, allow
			/// optimization on that
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/// @brief In cases where we own the buffer or when requested and can,
			/// allow temporarily mutating it to reduce search costs
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto parse_state =
			  ParseState{ std::data( json_data ), daw::data_end( json_data ) };

			parse_state.trim_left_unchecked( );
#if defined( _MSC_VER ) and not defined( __clang__ )
			// Work around MSVC ICE
			daw_json_ensure( parse_state.is_opening_bracket_checked( ),
			                 ErrorReason::InvalidArrayStart, parse_state );
#else
			daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
			                      ErrorReason::InvalidArrayStart, parse_state );
#endif
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
				parse_state.trim_left( );
				daw_json_ensure( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
			}
		}

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String>
		requires( json_details::is_string_view_like_v<String> )
		[[nodiscard]] constexpr Container from_json_array( String &&json_data ) {

			return from_json_array<JsonElement, Container, Constructor, KnownBounds>(
			  DAW_FWD( json_data ), options::parse_flags<> );
		}

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @param member_path A dot separated path of member names to start
		/// parsing from. Array indices are specified with square brackets e.g. [5]
		/// is the 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String, auto... PolicyFlags>
		requires( json_details::is_string_view_like_v<String> )
		[[nodiscard]] constexpr Container
		from_json_array( String &&json_data, std::string_view member_path,
		                 options::parse_flags_t<PolicyFlags...> ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );

			daw_json_ensure( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_ensure( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			daw_json_ensure( std::data( member_path ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using element_type = json_details::json_deduced_type<JsonElement>;
			static_assert( traits::not_same_v<element_type, void>,
			               "Unknown JsonElement type." );

			using parser_t =
			  json_base::json_array<JsonElement, Container, Constructor>;

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/// @brief If the string is known to have a trailing zero, allow
			/// optimization on that
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/// @brief In cases where we own the buffer or when requested and can,
			/// allow temporarily mutating it to reduce search costs
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto jv = basic_json_value(
			  ParseState( std::data( json_data ), daw::data_end( json_data ) ) );
			jv = jv.find_member( member_path );

			if constexpr( json_details::is_json_nullable_v<parser_t> ) {
				if( not jv ) {
					return json_details::construct_nullable_empty<
					  typename parser_t::constructor_t>( );
				}
			} else {
				daw_json_ensure( jv, ErrorReason::JSONPathNotFound );
			}
			auto parse_state = jv.get_raw_state( );
			parse_state.trim_left_unchecked( );
#if defined( _MSC_VER ) and not defined( __clang__ )
			// Work around MSVC ICE
			daw_json_ensure( parse_state.is_opening_bracket_checked( ),
			                 ErrorReason::InvalidArrayStart, parse_state );
#else
			daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
			                      ErrorReason::InvalidArrayStart, parse_state );
#endif
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
				parse_state.trim_left( );
				daw_json_ensure( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
			}
		}

		/// @brief Parse JSON data where the root item is an array
		/// @tparam JsonElement The type of each element in array.  Must be one of
		/// the above json_XXX classes.  This version is checked
		/// @tparam Container Container to store values in
		/// @tparam Constructor Callable to construct Container with no arguments
		/// @param json_data JSON string data containing array
		/// @param member_path A dot separated path of member names to start
		/// parsing from. Array indices are specified with square brackets e.g. [5]
		/// is the 6th item
		/// @tparam KnownBounds The bounds of the json_data are known to contain
		/// the whole value
		/// @return A Container containing parsed data from JSON string
		/// @throws daw::json::json_exception
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String>
		requires( json_details::is_string_view_like_v<String> )
		[[nodiscard]] constexpr Container
		from_json_array( String &&json_data, std::string_view member_path ) {
			static_assert(
			  json_details::is_string_view_like_v<String>,
			  "String type must have a be a contiguous range of Characters" );

			return from_json_array<JsonElement, Container, Constructor, KnownBounds>(
			  DAW_FWD( json_data ), member_path, options::parse_flags<> );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
