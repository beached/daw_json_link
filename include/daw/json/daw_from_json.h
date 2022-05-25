// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_from_json_fwd.h"
#include "impl/daw_json_parse_class.h"
#include "impl/daw_json_parse_value.h"
#include "impl/daw_json_value.h"
#include "impl/version.h"

#include <daw/daw_traits.h>

#include <iterator>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/**
		 * Construct the JSONMember from the JSON document argument.
		 * @tparam JsonMember any bool, arithmetic, string, string_view,
		 * daw::json::json_data_contract
		 * @param json_data JSON string data
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A reified T constructed from JSON data
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( String &&json_data, options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {

			daw_json_assert( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			daw_json_assert( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );

			static_assert(
			  json_details::has_json_deduced_type_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using json_member = json_details::json_deduced_type<JsonMember>;
			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;
			/***
			 * If the string is known to have a trailing zero, allow optimization on
			 * that
			 */
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/***
			 * In cases where we own the buffer or when requested and can, allow
			 * temporarily mutating it to reduce search costs
			 */
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;
			auto parse_state =
			  ParseState( std::data( json_data ), daw::data_end( json_data ) );

			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_assert( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/**
		 * Construct the JSONMember from the JSON document argument.
		 * @tparam JsonMember any bool, arithmetic, string, string_view,
		 * daw::json::json_data_contract
		 * @param json_data JSON string data
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A reified JSONMember constructed from JSON data
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String>
		[[maybe_unused, nodiscard]] constexpr auto from_json( String &&json_data )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {
			return from_json<JsonMember, KnownBounds, Result>(
			  DAW_FWD( json_data ), options::parse_flags<> );
		}

		/**
		 * Construct the JSONMember from the JSON document argument.
		 * @tparam JsonMember any bool, arithmetic, string, string_view,
		 * daw::json::json_data_contract
		 * @param json_data JSON string data
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A reified T constructed from JSON data
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String, typename Allocator, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, Allocator const &alloc,
		                 options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {

			daw_json_assert( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_assert( std::data( json_data ) != nullptr,
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
			/***
			 * If the string is known to have a trailing zero, allow optimization on
			 * that
			 */
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/***
			 * In cases where we own the buffer or when requested and can, allow
			 * temporarily mutating it to reduce search costs
			 */
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto parse_state = ParseState::with_allocator( f, l, a );
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_assert( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/**
		 * Construct the JSONMember from the JSON document argument.
		 * @tparam JsonMember any bool, arithmetic, string, string_view,
		 * daw::json::json_data_contract
		 * @param json_data JSON string data
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A reified T constructed from JSON data
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String, typename Allocator>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, Allocator const &alloc )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {
			return from_json_alloc<JsonMember, KnownBounds, Result>(
			  DAW_FWD( json_data ), alloc, options::parse_flags<> );
		}

		/***
		 * Parse a JSONMember from the json_data starting at member_path.
		 * @tparam JsonMember The type of the item being parsed
		 * @param json_data JSON string data
		 * @param member_path A dot separated path of member names, default is the
		 * root.  Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( String &&json_data, std::string_view member_path,
		           options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {

			daw_json_assert( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_assert( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			daw_json_assert( std::data( member_path ) != nullptr,
			                 ErrorReason::EmptyJSONPath );

			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;

			/***
			 * If the string is known to have a trailing zero, allow optimization on
			 * that
			 */
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/***
			 * In cases where we own the buffer or when requested and can, allow
			 * temporarily mutating it to reduce search costs
			 */
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto [is_found, parse_state] = json_details::find_range<ParseState>(
			  json_data, { std::data( member_path ), std::size( member_path ) } );
			if constexpr( json_details::is_json_nullable_v<JsonMember> ) {
				if( not is_found ) {
					return json_details::construct_nullable_empty<
					  typename json_member::constructor_t>( );
				}
			} else {
				daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
			}

			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_assert( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/***
		 * Parse a JSONMember from the json_data starting at member_path.
		 * @tparam JsonMember The type of the item being parsed
		 * @param json_data JSON string data
		 * @param member_path A dot separated path of member names, default is the
		 * root.  Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json( String &&json_data, std::string_view member_path )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {
			return from_json<JsonMember, KnownBounds, Result>(
			  DAW_FWD( json_data ), member_path, options::parse_flags<> );
		}

		/***
		 * Parse a JSONMember from the json_data starting at member_path.
		 * @tparam JsonMember The type of the item being parsed
		 * @param json_data JSON string data
		 * @param member_path A dot separated path of member names, default is the
		 * root.  Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String, typename Allocator, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, std::string_view member_path,
		                 Allocator const &alloc,
		                 options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {

			daw_json_assert( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_assert( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_assert( std::data( member_path ) != nullptr,
			                 ErrorReason::EmptyJSONPath );

			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			Allocator a = alloc;

			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;
			/***
			 * If the string is known to have a trailing zero, allow optimization on
			 * that
			 */
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/***
			 * In cases where we own the buffer or when requested and can, allow
			 * temporarily mutating it to reduce search costs
			 */
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto [is_found, parse_state] = json_details::find_range<ParseState>(
			  json_data, { std::data( member_path ), std::size( member_path ) }, a );
			if constexpr( json_details::is_json_nullable_v<json_member> ) {
				if( not is_found ) {
					return json_details::construct_nullable_empty<
					  typename json_member::constructor_t>( );
				}
			} else {
				daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
			}

			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
				parse_state.trim_left( );
				daw_json_assert( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<json_member, KnownBounds>(
				  parse_state, ParseTag<json_member::expected_type>{ } );
			}
		}

		/***
		 * Parse a JSONMember from the json_data starting at member_path.
		 * @tparam JsonMember The type of the item being parsed
		 * @param json_data JSON string data
		 * @param member_path A dot separated path of member names, default is the
		 * root.  Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename String, typename Allocator>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_alloc( String &&json_data, std::string_view member_path,
		                 Allocator const &alloc )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>, Result> {

			return from_json_alloc<JsonMember, KnownBounds, Result>(
			  DAW_FWD( json_data ), member_path, alloc, options::parse_flags<> );
		}

		/***
		 * Parse a value from a json_value
		 * @tparam JsonMember The type of the item being parsed
		 * @param value JSON data, see basic_json_value
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename ParseState, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] inline constexpr Result
		from_json( basic_json_value<ParseState> value,
		           options::parse_flags_t<PolicyFlags...> ) {
			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using ParsePolicy =
			  typename ParseState::template SetPolicyOptions<PolicyFlags...>;

			auto const json_data = value.get_string_view( );
			auto parse_state =
			  ParsePolicy( std::data( json_data ), daw::data_end( json_data ) );

			return json_details::parse_value<json_member, KnownBounds>(
			  parse_state, ParseTag<json_member::expected_type>{ } );
		}

		/***
		 * Parse a value from a json_value
		 * @tparam JsonMember The type of the item being parsed
		 * @param value JSON data, see basic_json_value
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename ParseState>
		[[maybe_unused, nodiscard]] inline constexpr Result
		from_json( basic_json_value<ParseState> value ) {

			return from_json<JsonMember, KnownBounds, Result>(
			  DAW_MOVE( value ), options::parse_flags<> );
		}

		/***
		 * Parse a JSONMember from the json_data starting at member_path.
		 * @param value JSON data, see basic_json_value
		 * @param member_path A dot separated path of member names, default is the
		 * root.  Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam JsonMember The type of the item being parsed
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename ParseState, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result
		from_json( basic_json_value<ParseState> value, std::string_view member_path,
		           options::parse_flags_t<PolicyFlags...> ) {
			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert(
			  json_details::has_unnamed_default_type_mapping_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using ParsePolicy =
			  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;
			auto json_data = value.get_state( );
			auto [is_found, parse_state] = json_details::find_range<ParsePolicy>(
			  json_data, { std::data( member_path ), std::size( member_path ) } );
			if constexpr( json_details::is_json_nullable_v<json_member> ) {
				if( not is_found ) {
					return json_details::construct_nullable_empty<
					  typename json_member::constructor_t>( );
				}
			} else {
				daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
			}
			return json_details::parse_value<json_member, KnownBounds>(
			  parse_state, ParseTag<json_member::expected_type>{ } );
		}

		/***
		 * Parse a JSONMember from the json_data starting at member_path.
		 * @param value JSON data, see basic_json_value
		 * @param member_path A dot separated path of member names, default is the
		 * root.  Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam JsonMember The type of the item being parsed
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A value reified from the JSON data member
		 * @throws daw::json::json_exception
		 */
		template<typename JsonMember, bool KnownBounds, typename Result,
		         typename ParseState>
		[[maybe_unused, nodiscard]] constexpr Result
		from_json( basic_json_value<ParseState> value,
		           std::string_view member_path ) {
			return from_json<JsonMember, KnownBounds, Result>(
			  DAW_MOVE( value ), member_path, options::parse_flags<> );
		}

		/*********************************************************/

		/**
		 * Parse JSON data where the root item is an array
		 * @tparam JsonElement The type of each element in array.  Must be one of
		 * the above json_XXX classes.  This version is checked
		 * @tparam Container Container to store values in
		 * @tparam Constructor Callable to construct Container with no arguments
		 * @param json_data JSON string data containing array
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A Container containing parsed data from JSON string
		 * @throws daw::json::json_exception
		 */
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data,
		                 options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container> {

			daw_json_assert( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_assert( std::data( json_data ) != nullptr,
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

			/***
			 * If the string is known to have a trailing zero, allow optimization on
			 * that
			 */
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/***
			 * In cases where we own the buffer or when requested and can, allow
			 * temporarily mutating it to reduce search costs
			 */
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto parse_state =
			  ParseState{ std::data( json_data ), daw::data_end( json_data ) };

			parse_state.trim_left_unchecked( );
#if defined( _MSC_VER ) and not defined( __clang__ )
			// Work around MSVC ICE
			daw_json_assert( parse_state.is_opening_bracket_checked( ),
			                 ErrorReason::InvalidArrayStart, parse_state );
#else
			daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
			                      ErrorReason::InvalidArrayStart, parse_state );
#endif
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
				parse_state.trim_left( );
				daw_json_assert( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
			}
		}

		/**
		 * Parse JSON data where the root item is an array
		 * @tparam JsonElement The type of each element in array.  Must be one of
		 * the above json_XXX classes.  This version is checked
		 * @tparam Container Container to store values in
		 * @tparam Constructor Callable to construct Container with no arguments
		 * @param json_data JSON string data containing array
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A Container containing parsed data from JSON string
		 * @throws daw::json::json_exception
		 */
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container> {

			return from_json_array<JsonElement, Container, Constructor, KnownBounds>(
			  DAW_FWD( json_data ), options::parse_flags<> );
		}

		/**
		 * Parse JSON data where the root item is an array
		 * @tparam JsonElement The type of each element in array.  Must be one of
		 * the above json_XXX classes.  This version is checked
		 * @tparam Container Container to store values in
		 * @tparam Constructor Callable to construct Container with no arguments
		 * @param json_data JSON string data containing array
		 * @param member_path A dot separated path of member names to start parsing
		 * from. Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A Container containing parsed data from JSON string
		 * @throws daw::json::json_exception
		 */
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String, auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data, std::string_view member_path,
		                 options::parse_flags_t<PolicyFlags...> )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container> {

			daw_json_assert( std::size( json_data ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			daw_json_assert( std::data( json_data ) != nullptr,
			                 ErrorReason::EmptyJSONPath );
			daw_json_assert( std::data( member_path ) != nullptr,
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

			/***
			 * If the string is known to have a trailing zero, allow optimization on
			 * that
			 */
			using policy_zstring_t = json_details::apply_zstring_policy_option_t<
			  ParsePolicy, String, options::ZeroTerminatedString::yes>;

			/***
			 * In cases where we own the buffer or when requested and can, allow
			 * temporarily mutating it to reduce search costs
			 */
			using ParseState = json_details::apply_mutable_policy<
			  policy_zstring_t, String, options::TemporarilyMutateBuffer::yes,
			  options::TemporarilyMutateBuffer::no>;

			auto [is_found, parse_state] = json_details::find_range<ParseState>(
			  json_data, { std::data( member_path ), std::size( member_path ) } );

			if constexpr( json_details::is_json_nullable_v<parser_t> ) {
				if( not is_found ) {
					return json_details::construct_nullable_empty<
					  typename parser_t::constructor_t>( );
				}
			} else {
				daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
			}
			parse_state.trim_left_unchecked( );
#if defined( _MSC_VER ) and not defined( __clang__ )
			// Work around MSVC ICE
			daw_json_assert( parse_state.is_opening_bracket_checked( ),
			                 ErrorReason::InvalidArrayStart, parse_state );
#else
			daw_json_assert_weak( parse_state.is_opening_bracket_checked( ),
			                      ErrorReason::InvalidArrayStart, parse_state );
#endif
			if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
				auto result = json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
				parse_state.trim_left( );
				daw_json_assert( parse_state.empty( ), ErrorReason::InvalidEndOfValue,
				                 parse_state );
				return result;
			} else {
				return json_details::parse_value<parser_t, KnownBounds>(
				  parse_state, ParseTag<JsonParseTypes::Array>{ } );
			}
		}

		/**
		 * Parse JSON data where the root item is an array
		 * @tparam JsonElement The type of each element in array.  Must be one of
		 * the above json_XXX classes.  This version is checked
		 * @tparam Container Container to store values in
		 * @tparam Constructor Callable to construct Container with no arguments
		 * @param json_data JSON string data containing array
		 * @param member_path A dot separated path of member names to start parsing
		 * from. Array indices are specified with square brackets e.g. [5] is the
		 * 6th item
		 * @tparam KnownBounds The bounds of the json_data are known to contain the
		 * whole value
		 * @return A Container containing parsed data from JSON string
		 * @throws daw::json::json_exception
		 */
		template<typename JsonElement, typename Container, typename Constructor,
		         bool KnownBounds, typename String>
		[[maybe_unused, nodiscard]] constexpr auto
		from_json_array( String &&json_data, std::string_view member_path )
		  -> std::enable_if_t<json_details::is_string_view_like_v<String>,
		                      Container> {

			return from_json_array<JsonElement, Container, Constructor, KnownBounds>(
			  DAW_FWD( json_data ), member_path, options::parse_flags<> );
		}

	} // namespace DAW_JSON_VER
} // namespace daw::json
