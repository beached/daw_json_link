// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_serialize_impl.h"
#include "impl/daw_json_traits.h"

#include <daw/daw_attributes.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_visit.h>

#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/**
		 *
		 * @tparam JsonMembers JSON classes that map the relation ship from the json
		 * data to the classes constructor
		 */
		template<typename... JsonMembers>
		struct json_member_list {
			using i_am_a_json_member_list = daw::fwd_pack<JsonMembers...>;
			static_assert(
			  std::conjunction_v<json_details::is_a_json_type<JsonMembers>...>,
			  "Only JSON Link mapping types can appear in a "
			  "json_member_list(e.g. json_number, json_string...)" );

			static_assert(
			  not std::disjunction_v<json_details::is_no_name<JsonMembers>...>,
			  "All members must have a name and not no_name in a "
			  "json_member_list" );
			/**
			 * Serialize a C++ class to JSON data
			 * @tparam OutputIterator An output iterator with a char value_type
			 * @tparam Args  tuple of values that map to the JSON members of v
			 * @tparam Value Value type being serialized
			 * @param it OutputIterator to append string data to
			 * @param args members from C++ class
			 * @param v value to be serialized as JSON object
			 * @return the OutputIterator it at final position
			 */
			template<typename OutputIterator, typename Value,
			         template<class...> class Tuple, typename... Ts>
			[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
			serialize( OutputIterator it, Tuple<Ts...> const &args, Value const &v ) {
				static_assert(
				  sizeof...( Ts ) == sizeof...( JsonMembers ),
				  "The method to_json_data in the json_data_contract does not match "
				  "the mapping.  The number of members is not the same." );
				static_assert(
				  ( (not std::is_rvalue_reference_v<Ts>)and... ),
				  "The Tuple contains rvalue references.  The values "
				  "passed are now dangling.  daw::forward_nonrvalue_as_tuple in "
				  "<daw/daw_tuple_forward.h> can forward only non-rvalue refs and "
				  "store the temporaries" );
				return json_details::serialize_json_class<JsonMembers...>(
				  it, args, v, std::index_sequence_for<Ts...>{ } );
			}

			template<typename Constructor>
			using result_type =
			  json_details::json_class_parse_result_t<Constructor, JsonMembers...>;
			/**
			 *
			 * Parse JSON data and construct a C++ class.  This is used by parse_value
			 * to get back into a mode with a JsonMembers...
			 * @tparam T The result of parsing json_class
			 * @tparam ParseState Input range type
			 * @param parse_state JSON data to parse
			 * @return A T object
			 */
			template<typename JsonClass, typename ParseState>
			[[maybe_unused,
			  nodiscard]] DAW_ATTRIB_FLATTEN static inline constexpr json_details::
			  json_result<JsonClass>
			  parse_to_class( ParseState &parse_state,
			                  template_param<JsonClass> = template_arg<JsonClass> ) {

				static_assert( json_details::is_no_name_v<JsonClass> );
				static_assert( json_details::is_a_json_type_v<JsonClass> );
				static_assert( json_details::has_json_data_contract_trait_v<
				                 typename JsonClass::parse_to_t>,
				               "Unexpected type" );
				return json_details::parse_json_class<JsonClass, JsonMembers...>(
				  parse_state, std::index_sequence_for<JsonMembers...>{ } );
			}
		};

		///
		/// Deduce the json type mapping based on common types and types already
		/// mapped.
		template<JSONNAMETYPE Name, typename T>
		using json_link = typename json_details::ensure_mapped_t<
		  json_details::json_deduced_type<T>>::template with_name<Name>;

		///
		/// Deduce the json type mapping based on common types and types already
		/// mapped. This version is for when a name is unneeded, such as array
		/// element types.
		template<typename T>
		using json_link_no_name =
		  json_details::ensure_mapped_t<json_details::json_deduced_type<T>>;

		/***
		 * @brief Allow the JsonMember type to parse like JsonMember.  This is
		 * required to be aliased to type in a json_data_contract specialization.
		 * Assuming T is the specialized type, it's constructor must have an
		 * overload for that of what would be expected for the JsonMember's
		 * parse_to_t
		 * @note using type = json_type_alias<json_class<T>>;
		 * @tparam JsonMember This is the json_ type to be aliased
		 */
		template<typename JsonMember>
		struct json_type_alias {
			using i_am_a_json_member_list = void;
			using i_am_a_json_map_alias = void;
			using json_member = json_details::json_deduced_type<JsonMember>;
			static_assert( json_details::is_a_json_type_v<json_member>,
			               "Only JSON Link mapping types can appear in a "
			               "json_type_alias(e.g. json_number, json_string...)" );

			static_assert(
			  json_details::is_no_name_v<json_member>,
			  "The JSONMember cannot be named, it does not make sense in "
			  "this context" );

			template<typename OutputIterator, typename Member, typename Value>
			[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
			serialize( OutputIterator it, Member const &m, Value const & ) {
				return json_details::member_to_string( template_arg<json_member>, it,
				                                       m );
			}

			template<typename Constructor>
			using result_type =
			  json_details::json_class_parse_result_t<Constructor, json_member>;

			template<typename JsonClass, typename ParseState>
			[[maybe_unused,
			  nodiscard]] DAW_ATTRIB_FLATTEN static constexpr json_details::
			  json_result<JsonClass>
			  parse_to_class( ParseState &parse_state, template_param<JsonClass> ) {
				static_assert( json_details::is_a_json_type_v<JsonClass> );
				static_assert( json_details::has_json_data_contract_trait_v<
				                 typename JsonClass::parse_to_t>,
				               "Unexpected type" );
				// Using construct_value here as the result of aliased type is used to
				// construct our result and the Constructor maybe different.  This
				// happens with BigInt and string.
				using Constructor = typename JsonClass::constructor_t;
				return json_details::construct_value(
				  template_args<JsonClass, Constructor>, parse_state,
				  json_details::parse_value<json_member, false>(
				    parse_state, ParseTag<json_member::expected_type>{ } ) );
			}
		};

		template<typename JsonType>
		struct json_details::is_json_class_map<json_type_alias<JsonType>>
		  : std::true_type {};

		/***
		 *
		 * Allows specifying an unnamed json mapping where the
		 * result is a tuple
		 */
		template<typename... Members>
		struct tuple_json_mapping {
			std::tuple<typename Members::parse_to_t...> members;

			template<typename... Ts>
			explicit constexpr tuple_json_mapping( Ts &&...values )
			  : members{ DAW_FWD2( Ts, values )... } {}
		};

		template<typename... Members>
		struct json_data_contract<tuple_json_mapping<Members...>> {
			using type = json_member_list<Members...>;

			[[nodiscard, maybe_unused]] static inline auto const &
			to_json_data( tuple_json_mapping<Members...> const &value ) {
				return value.members;
			}
		};

		/***
		 * In a json_tuple_member_list, this allows specifying the position in the
		 * array to parse this member from
		 * @tparam Index Position in array where member is
		 * @tparam JsonMember type of value( e.g. int, json_string )
		 */
		template<std::size_t Index, typename JsonMember>
		struct json_tuple_member {
			using i_am_an_ordered_member = void;
			using i_am_a_json_type = void;
			static constexpr std::size_t member_index = Index;
			static_assert(
			  json_details::has_json_deduced_type_v<JsonMember>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );
			using json_member = json_details::json_deduced_type<JsonMember>;
			using parse_to_t = typename json_member::parse_to_t;
			using base_type = parse_to_t;
		};

		template<std::size_t Index, typename JsonMember>
		using ordered_json_member
		  [[deprecated( "Use json_tuple_member, removal in v4" )]] =
		    json_tuple_member<Index, JsonMember>;

		namespace json_details {
			template<typename JsonMember>
			using json_tuple_member_wrapper =
			  std::conditional_t<is_an_ordered_member_v<JsonMember>, JsonMember,
			                     json_deduced_type<JsonMember>>;
		} // namespace json_details

		/***
		 * Allow extracting elements from a JSON array and constructing from it.
		 * Members can be either normal C++ no_name members, or an ordered_member
		 * with a position. All ordered members must have a value greater than the
		 * previous.  The first element in the list, unless it is specified as an
		 * ordered_member, is 0.  A non-ordered_member item will be 1 more than the
		 * previous item in the list.  All items must have an index greater than the
		 * previous. In Javascript these are also called tuples.
		 * @tparam JsonMembers A list of json_TYPE mappings or a json_TYPE mapping
		 * wrapped into a json_tuple_member
		 */
		template<typename... JsonMembers>
		struct json_tuple_member_list {
			using i_am_a_json_member_list = daw::fwd_pack<JsonMembers...>;
			using i_am_a_json_tuple_member_list = void;
			static_assert( ( json_details::is_no_name_v<JsonMembers> and ... ),
			               "Json Tuple class members are unnamed" );
			/**
			 * Serialize a C++ class to JSON data
			 * @tparam OutputIterator An output iterator with a char value_type
			 * @tparam Args  tuple of values that map to the JSON members
			 * @param it OutputIterator to append string data to
			 * @param args members from C++ class
			 * @return the OutputIterator it
			 */
			template<typename OutputIterator, typename Value,
			         template<class...> class Tuple, typename... Ts>
			[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
			serialize( OutputIterator it, Tuple<Ts...> const &args, Value const &v ) {
				static_assert( sizeof...( Ts ) == sizeof...( JsonMembers ),
				               "Argument count is incorrect" );
				static_assert(
				  ( (not std::is_rvalue_reference_v<Ts>)and... ),
				  "The Tuple contains rvalue references.  The values "
				  "passed are now dangling.  daw::forward_nonrvalue_as_tuple in "
				  "<daw/daw_tuple_forward.h> can forward only non-rvalue refs and "
				  "store the temporaries" );
				static_assert(
				  std::conjunction_v<json_details::is_a_json_type<
				    json_details::json_tuple_member_wrapper<JsonMembers>>...>,
				  "Only value JSON types can be used" );
				return json_details::serialize_ordered_json_class<
				  json_details::json_tuple_member_wrapper<JsonMembers>...>(
				  it, args, v, std::index_sequence_for<Ts...>{ } );
			}

			template<typename Constructor>
			using result_type = json_details::json_class_parse_result_t<
			  Constructor, json_details::ordered_member_subtype_t<
			                 json_details::json_deduced_type<JsonMembers>>...>;
			/**
			 *
			 * Parse JSON data and construct a C++ class.  This is used by parse_value
			 * to get back into a mode with a JsonMembers...
			 * @tparam T The result of parsing json_class
			 * @tparam ParseState Input range type
			 * @param parse_state JSON data to parse
			 * @return A T object
			 */
			template<typename JsonClass, typename ParseState>
			[[maybe_unused,
			  nodiscard]] static inline constexpr json_details::json_result<JsonClass>
			parse_to_class( ParseState &parse_state, template_param<JsonClass> ) {
				static_assert( json_details::is_a_json_type_v<JsonClass> );
				static_assert( json_details::has_json_data_contract_trait_v<
				                 typename JsonClass::base_type>,
				               "Unexpected type" );

				return json_details::parse_json_tuple_class(
				  template_args<
				    JsonClass, json_details::json_tuple_member_wrapper<JsonMembers>...>,
				  parse_state );
			}
		};

		template<typename... JsonMembers>
		using json_ordered_member_list
		  [[deprecated( "Use json_tuple_member_list, removal in v4" )]] =
		    json_tuple_member_list<JsonMembers...>;

		/***
		 * Parse a tagged variant like class where the tag member is in the same
		 * class that is being discriminated.  The container type, that will
		 * specialize json_data_construct on, must support the get_if, get_index
		 * @tparam TagMember JSON element to pass to Switcher. Does not have to be
		 * declared in member list
		 * @tparam Switcher A callable that returns an index into JsonClasses when
		 * passed the TagMember object
		 * @tparam JsonClasses List of alternative classes that are mapped via a
		 * json_data_contract
		 */
		template<typename TagMember, typename Switcher, typename... JsonClasses>
		struct json_submember_tagged_variant {
			using i_am_a_json_member_list = void;
			using i_am_a_submember_tagged_variant = void;

			/**
			 * Serialize a C++ class to JSON data
			 * @tparam OutputIterator An output iterator with a char value_type
			 * @param it OutputIterator to append string data to
			 * @return the OutputIterator it
			 */
			template<typename OutputIterator, typename Value>
			[[maybe_unused, nodiscard]] static inline constexpr OutputIterator
			serialize( OutputIterator it, Value const &v ) {

				return daw::visit_nt( v, [&]( auto const &alternative ) {
					using Alternative = DAW_TYPEOF( alternative );
					static_assert(
					  std::disjunction_v<std::is_same<Alternative, JsonClasses>...>,
					  "Unexpected alternative type" );
					static_assert( json_details::has_json_to_json_data_v<Alternative>,
					               "Alternative type does not have a to_json_data_member "
					               "in it's json_data_contract specialization" );

					return json_details::member_to_string(
					  template_arg<json_base::json_class<Alternative>>, it, alternative );
				} );
			}

			template<typename Constructor>
			using result_type = json_details::json_class_parse_result_t<
			  Constructor, json_details::json_deduced_type<
			                 daw::traits::first_type<JsonClasses...>>>;
			/**
			 *
			 * Parse JSON data and construct a C++ class.  This is used by parse_value
			 * to get back into a mode with a JsonMembers...
			 * @tparam T The result of parsing json_class
			 * @tparam ParseState Input range type
			 * @param parse_state JSON data to parse
			 * @return A T object
			 */
			template<typename JsonClass, typename ParseState>
			[[maybe_unused,
			  nodiscard]] DAW_ATTRIB_FLATTEN static inline constexpr json_details::
			  from_json_result_t<JsonClass>
			  parse_to_class( ParseState &parse_state, template_param<JsonClass> ) {
				static_assert( json_details::is_a_json_type_v<JsonClass> );
				static_assert( json_details::has_json_data_contract_trait_v<
				                 typename JsonClass::base_type>,
				               "Unexpected type" );
				using tag_class_t = tuple_json_mapping<TagMember>;

				std::size_t const idx = [parse_state]( ) mutable {
					return Switcher{ }( std::get<0>(
					  json_details::parse_value<json_base::json_class<tag_class_t>>(
					    parse_state, ParseTag<JsonParseTypes::Class>{ } )
					    .members ) );
				}( );
				daw_json_assert_weak( idx < sizeof...( JsonClasses ),
				                      ErrorReason::UnexpectedJSONVariantType );
				return json_details::parse_nth_class<
				  0, JsonClass, false, json_base::json_class<JsonClasses>...>(
				  idx, parse_state );
			}
		};

		/**************************************************
		 * Member types - These are the mapping classes to
		 * describe the constructor of your classes
		 **************************************************/

		namespace json_base {
			template<typename T, json_details::json_options_t Options,
			         typename Constructor>
			struct json_number {
				using i_am_a_json_type = void;
				using wrapped_type = T;
				static constexpr bool must_be_class_member = false;

				static_assert( daw::is_arithmetic_v<T>,
				               "json_number requires an arithmetic type" );

				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<T>, Constructor>;
				using parse_to_t =
				  typename json_details::construction_result<constructor_t, T>::type;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type =
				  json_details::number_parse_type_v<T>;

				static constexpr LiteralAsStringOpt literal_as_string =
				  json_details::get_bits_for<LiteralAsStringOpt>( number_opts,
				                                                  Options );

				static constexpr JsonRangeCheck range_check =
				  json_details::get_bits_for<JsonRangeCheck>( number_opts, Options );

				static constexpr FPOutputFormat fp_output_format =
				  json_details::get_bits_for<FPOutputFormat>( number_opts, Options );

				static constexpr JsonNumberErrors allow_number_errors =
				  json_details::get_bits_for<JsonNumberErrors>( number_opts, Options );

				static_assert( allow_number_errors == JsonNumberErrors::None or
				                 literal_as_string != LiteralAsStringOpt::Never,
				               "Cannot allow NaN/Inf/-Inf when numbers cannot be "
				               "serialized/parsed as a string" );

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Number;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_number<NewName, T, Options, Constructor>;
			};

		} // namespace json_base

		/**
		 * The member is a range checked number
		 * @tparam Name name of json member
		 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
		 * Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 */
		template<JSONNAMETYPE Name, typename T,
		         json_details::json_options_t Options, typename Constructor>
		struct json_number : json_base::json_number<T, Options, Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_number<T, Options, Constructor>;
		};

		template<typename T = double,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = use_default>
		using json_number_no_name = json_base::json_number<T, Options, Constructor>;

		template<typename T = std::optional<double>,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = use_default>
		using json_number_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_number<json_details::unwrapped_t<T>, Options>>;

		/**
		 * The member is a range checked number
		 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
		 * Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 */
		template<typename T = double,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = use_default>
		using json_checked_number_no_name = json_base::json_number<
		  T,
		  json_details::number_opts_set<Options, JsonRangeCheck::CheckForNarrowing>,
		  Constructor>;

		/**
		 * The member is a nullable range checked number
		 * @tparam Name name of json member
		 * @tparam T type of number(e.g. optional<double>, optional<int>,
		 * optional<unsigned>...) to pass to Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 */
		template<typename T = std::optional<double>,
		         json_details::json_options_t Options = number_opts_def,
		         typename Constructor = use_default>
		using json_checked_number_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_number<json_details::unwrapped_t<T>,
		                         json_details::number_opts_set<
		                           Options, JsonRangeCheck::CheckForNarrowing>>>;

		namespace json_base {
			template<typename T, json_details::json_options_t Options,
			         typename Constructor>
			struct json_bool {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;

				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<T>, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, T>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t = std::invoke_result_t<constructor_t, T>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Bool;

				static constexpr LiteralAsStringOpt literal_as_string =
				  json_details::get_bits_for<LiteralAsStringOpt>( bool_opts, Options );

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Bool;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_bool<NewName, T, Options, Constructor>;
			};
		} // namespace json_base

		/**
		 * The member is a nullable boolean
		 * @tparam Name name of json member
		 * @tparam T result type to pass to Constructor
		 * @tparam LiteralAsString Could this number be embedded in a string
		 * @tparam Constructor Callable used to construct result
		 */
		template<JSONNAMETYPE Name, typename T,
		         json_details::json_options_t Options, typename Constructor>
		struct json_bool : json_base::json_bool<T, Options, Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_bool<T, Options, Constructor>;
		};

		template<typename T = bool,
		         json_details::json_options_t Options = bool_opts_def,
		         typename Constructor = use_default>
		using json_bool_no_name = json_base::json_bool<T, Options, Constructor>;

		template<typename T = std::optional<bool>,
		         json_details::json_options_t Options = bool_opts_def,
		         typename Constructor = use_default>
		using json_bool_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_bool<json_details::unwrapped_t<T>, Options>>;

		namespace json_base {
			/// String - A raw string as is.  Escapes are left in.
			template<typename String, json_details::json_options_t Options,
			         typename Constructor>
			struct json_string_raw {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;
				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<String>, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, String>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t = std::invoke_result_t<constructor_t, String>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::StringRaw;

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::String;

				static constexpr EmptyStringNull empty_is_null =
				  json_details::get_bits_for<EmptyStringNull>( string_raw_opts,
				                                               Options );
				static constexpr EightBitModes eight_bit_mode =
				  json_details::get_bits_for<EightBitModes>( string_raw_opts, Options );

				static constexpr AllowEscapeCharacter allow_escape_character =
				  json_details::get_bits_for<AllowEscapeCharacter>( string_raw_opts,
				                                                    Options );
				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_string_raw<NewName, String, Options, Constructor>;
			};
		} // namespace json_base

		/**
		 * String - A raw string as is.  Escapes are left in.
		 * @tparam Name of json member
		 * @tparam String result type constructed by Constructor
		 * @tparam Constructor a callable taking as arguments ( char const *,
		 * std::size_t )
		 * @tparam EmptyStringNull if string is empty, call Constructor with no
		 * arguments
		 * @tparam EightBitMode Allow filtering of characters with the MSB set
		 * arguments
		 * @tparam AllowEscape Tell parser if we know a \ or escape will be in the
		 * data
		 */
		template<JSONNAMETYPE Name, typename String,
		         json_details::json_options_t Options, typename Constructor>
		struct json_string_raw
		  : json_base::json_string_raw<String, Options, Constructor> {
			using i_am_a_json_type = void;

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_string_raw<String, Options, Constructor>;
		};

		template<typename T = std::string,
		         json_details::json_options_t Options = string_raw_opts_def,
		         typename Constructor = use_default>
		using json_string_raw_no_name =
		  json_base::json_string_raw<T, Options, Constructor>;

		template<typename T = std::optional<std::string>,
		         json_details::json_options_t Options = string_raw_opts_def,
		         typename Constructor = use_default>
		using json_string_raw_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_string_raw<json_details::unwrapped_t<T>, Options>>;

		namespace json_base {
			template<typename String, json_details::json_options_t Options,
			         typename Constructor>
			struct json_string {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;

				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<String>, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, String>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t = std::invoke_result_t<constructor_t, String>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::StringEscaped;
				static constexpr EmptyStringNull empty_is_null =
				  json_details::get_bits_for<EmptyStringNull>( string_opts, Options );
				static constexpr EightBitModes eight_bit_mode =
				  json_details::get_bits_for<EightBitModes>( string_opts, Options );

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::String;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_string<NewName, String, Options, Constructor>;
			};
		} // namespace json_base

		/**
		 * Member is an escaped string and requires unescaping and escaping of
		 * string data
		 * @tparam Name of json member
		 * @tparam String result type constructed by Constructor
		 * @tparam Constructor a callable taking as arguments ( InputIterator,
		 * InputIterator ).  If others are needed use the Constructor callable
		 * convert
		 * @tparam EmptyStringNull if string is empty, call Constructor with no
		 * arguments
		 * @tparam EightBitMode Allow filtering of characters with the MSB set
		 */
		template<JSONNAMETYPE Name, typename String,
		         json_details::json_options_t Options, typename Constructor>
		struct json_string : json_base::json_string<String, Options, Constructor> {
			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_string<String, Options, Constructor>;
		};

		template<typename T = std::string,
		         json_details::json_options_t Options = string_opts_def,
		         typename Constructor = use_default>
		using json_string_no_name = json_base::json_string<T, Options, Constructor>;

		template<typename T = std::optional<std::string>,
		         json_details::json_options_t Options = string_opts_def,
		         typename Constructor = use_default>
		using json_string_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_string<json_details::unwrapped_t<T>, Options>>;

		namespace json_base {
			template<typename T, typename Constructor>
			struct json_date {
				using i_am_a_json_type = void;
				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     construct_from_iso8601_timestamp, Constructor>;

				using wrapped_type = T;
				static constexpr bool must_be_class_member = false;

				using parse_to_t = T;
				using base_type = T;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Date;
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::String;

				template<JSONNAMETYPE NewName>
				using with_name = daw::json::json_date<NewName, T, Constructor>;
			};
		} // namespace json_base

		/**
		 * Link to a JSON string representing a nullable date
		 * @tparam Name name of JSON member to link to
		 * @tparam T C++ type to construct, by default is a time_point
		 * @tparam Constructor A Callable used to construct a T.
		 * Must accept a char pointer and size as argument to the date/time string.
		 */
		template<JSONNAMETYPE Name, typename T, typename Constructor>
		struct json_date : json_base::json_date<T, Constructor> {
			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_date<T, Constructor>;
		};

		template<typename T, typename Constructor = use_default>
		using json_date_no_name = json_base::json_date<T, Constructor>;

		template<typename T, typename Constructor = use_default>
		using json_date_null_no_name = json_base::json_nullable<
		  T, Constructor, json_base::json_date<json_details::unwrapped_t<T>>>;

		namespace json_base {
			/// @brief Mark a member as nullable
			/// @tparam T type of the value being mapped to(e.g. std::optional<Foo>)
			/// @tparam JsonMember Json Type or type of value when present, deduced
			/// from T if not specified
			/// @tparam Constructor Specify a Constructor type or use
			/// the default nullable_constructor<T>
			template<typename T, typename Constructor, typename JsonMember,
			         JsonNullable NullableType>
			struct json_nullable {
				using i_am_a_json_type = void;
				using i_am_a_json_nullable = void;
				static constexpr bool must_be_class_member = true;
				static constexpr JsonNullable nullable = NullableType;
				using wrapped_type = T;
				using parse_to_t = T;
				using raw_member_type = typename std::conditional_t<
				  not std::is_same_v<JsonMember, deduced_type>,
				  traits::identity<JsonMember>,
				  std::conditional_t<
				    is_readable_value_v<T>,
				    json_details::ident_trait<readable_value_type_t, T>,
				    std::conditional<json_details::has_op_star_v<T>,
				                     json_details::unwrapped_t<T>, T>>>::type;

				using member_type = typename std::conditional_t<
				  json_details::is_a_json_type_v<raw_member_type>,
				  traits::identity<raw_member_type>,
				  json_details::ident_trait<json_details::json_deduced_type,
				                            raw_member_type>>::type;

				static_assert( json_details::is_no_name_v<member_type>,
				               "JsonMember template paramater must be noname type" );
				using base_type = json_details::json_result<member_type>;
				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     nullable_constructor<T>, Constructor>;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Null;

				static constexpr JsonParseTypes base_expected_type =
				  member_type::expected_type;

				static constexpr JsonBaseParseTypes underlying_json_type =
				  member_type::underlying_json_type;

				template<JSONNAMETYPE NewName>
				using with_name = daw::json::json_nullable<NewName, T, Constructor,
				                                           JsonMember, NullableType>;
			};

		} // namespace json_base

		template<JSONNAMETYPE Name, typename T, typename Constructor,
		         typename JsonMember, JsonNullable NullableType>
		struct json_nullable
		  : json_base::json_nullable<T, Constructor, JsonMember, NullableType> {

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_nullable<T, Constructor, JsonMember, NullableType>;
		};

		namespace json_base {
			template<typename T, typename Constructor>
			struct json_class {
				using i_am_a_json_type = void;
				using wrapped_type = T;
				static constexpr bool must_be_class_member = false;
				using constructor_t =
				  json_details::json_class_constructor_t<T, Constructor>;

				using json_member_list = json_data_contract_trait_t<T>;

				using data_contract = json_data_contract_trait_t<T>;

				using parse_to_t = typename std::conditional_t<
				  force_aggregate_construction_v<T>, daw::traits::identity<T>,
				  daw::traits::identity<
				    typename data_contract::template result_type<constructor_t>>>::type;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Class;

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Class;

				template<JSONNAMETYPE NewName>
				using with_name = daw::json::json_class<NewName, T, Constructor>;
			};
		} // namespace json_base

		/**
		 * Link to a JSON class
		 * @tparam Name name of JSON member to link to
		 * @tparam T type that has specialization of
		 * daw::json::json_data_contract
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<JSONNAMETYPE Name, typename T, typename Constructor>
		struct json_class : json_base::json_class<T, Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_class<T, Constructor>;
		};

		template<typename T, typename Constructor = use_default>
		using json_class_no_name = json_base::json_class<T, Constructor>;

		template<typename T, typename Constructor = use_default>
		using json_class_null_no_name = json_base::json_nullable<
		  T, Constructor, json_base::json_class<json_details::unwrapped_t<T>>>;

		/***
		 * A type to hold the types for parsing variants.
		 * @tparam JsonElements Up to one of a JsonElement that is a JSON number,
		 * string, object, or array
		 */
		template<typename... JsonElements>
		using json_tagged_variant_type_list
		  [[deprecated( "Use json_variant_type_list, removal in v4" )]] =
		    json_variant_type_list<JsonElements...>;

		template<typename>
		struct non_discriminated_variant_base_map;

		/// This ensures that we only map to up to one of each of the basic JSON
		/// types(Number, Bool, String, Array, Class) plus being optionally
		/// nullable.
		/// \tparam JsonElements The types we are mapping to
		template<typename... JsonElements>
		struct non_discriminated_variant_base_map<
		  json_variant_type_list<JsonElements...>> {
			static_assert(
			  sizeof...( JsonElements ) <= 5U,
			  "There can be at most 5 items, one for each JsonBaseParseTypes" );

			static_assert(
			  std::conjunction_v<
			    json_details::has_json_deduced_type<JsonElements>...>,
			  "Missing specialization of daw::json::json_data_contract for class "
			  "mapping or specialization of daw::json::json_link_basic_type_map" );

			static constexpr std::size_t base_map[5] = {
			  json_details::find_json_element<JsonBaseParseTypes::Number>(
			    { json_details::json_deduced_type<
			      JsonElements>::underlying_json_type... } ),
			  json_details::find_json_element<JsonBaseParseTypes::Bool>(
			    { json_details::json_deduced_type<
			      JsonElements>::underlying_json_type... } ),
			  json_details::find_json_element<JsonBaseParseTypes::String>(
			    { json_details::json_deduced_type<
			      JsonElements>::underlying_json_type... } ),
			  json_details::find_json_element<JsonBaseParseTypes::Class>(
			    { json_details::json_deduced_type<
			      JsonElements>::underlying_json_type... } ),
			  json_details::find_json_element<JsonBaseParseTypes::Array>(
			    { json_details::json_deduced_type<
			      JsonElements>::underlying_json_type... } ) };
		};

		namespace json_base {
			template<typename Variant, typename JsonElements, typename Constructor>
			struct json_variant {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;

				using json_elements = typename std::conditional_t<
				  std::is_same_v<JsonElements, use_default>,
				  json_details::ident_trait<json_details::variant_alternatives_list,
				                            Variant>,
				  daw::traits::identity<JsonElements>>::type;
				static_assert(
				  std::is_same_v<typename json_elements::i_am_variant_type_list, void>,
				  "Expected a json_variant_type_list or could not deduce alternatives "
				  "from Variant" );

				using constructor_t =
				  json_details::json_class_constructor_t<Variant, Constructor>;
				using parse_to_t = Variant;
				using base_type = parse_to_t;
				static constexpr JsonParseTypes expected_type = JsonParseTypes::Variant;

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::None;
				using base_map = non_discriminated_variant_base_map<json_elements>;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_variant<NewName, Variant, JsonElements, Constructor>;
			};
		} // namespace json_base

		/***
		 * Link to a nullable JSON variant
		 * @tparam Name name of JSON member to link to
		 * @tparam T type that has specialization of
		 * daw::json::json_data_contract
		 * @tparam JsonElements a json_variant_type_list
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<JSONNAMETYPE Name, typename Variant, typename JsonElements,
		         typename Constructor>
		struct json_variant
		  : json_base::json_variant<Variant, JsonElements, Constructor> {
			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_variant<Variant, JsonElements, Constructor>;
		};

		template<typename Variant, typename JsonElements = use_default,
		         typename Constructor = use_default>
		using json_variant_no_name =
		  json_base::json_variant<Variant, JsonElements, Constructor>;

		template<typename Variant, typename JsonElements = use_default,
		         typename Constructor = use_default>
		using json_variant_null_no_name = json_base::json_nullable<
		  Variant, Constructor,
		  json_base::json_variant<json_details::unwrapped_t<Variant>,
		                          JsonElements>>;

		namespace json_base {
			template<typename T, typename TagMember, typename Switcher,
			         typename JsonElements, typename Constructor>
			struct json_tagged_variant {
				using i_am_a_json_type = void;
				using i_am_a_tagged_variant = void;
				static constexpr bool must_be_class_member = false;

				using json_elements = typename std::conditional_t<
				  std::is_same_v<JsonElements, use_default>,
				  json_details::variant_alternatives_list<T>,
				  daw::traits::identity<JsonElements>>::type;
				static_assert(
				  std::is_same_v<typename json_elements::i_am_variant_type_list, void>,
				  "Expected a json_variant_type_list or could not deduce alternatives "
				  "from Variant" );

				static_assert( json_details::is_a_json_type_v<TagMember>,
				               "The TagMember type must have a name and be a "
				               "member of the same object as this" );

				using dependent_member = TagMember;

				static_assert(
				  std::disjunction_v<
				    json_details::is_an_ordered_member<dependent_member>,
				    std::conjunction<
				      json_details::is_a_json_type<dependent_member>,
				      daw::not_trait<json_details::is_no_name<TagMember>>>>,
				  "Must specify the location in tuple or name of member "
				  "for TagMember" );

				using tag_member = typename std::conditional_t<
				  json_details::is_a_json_type_v<TagMember>,
				  daw::traits::identity<TagMember>,
				  daw::traits::identity<json_details::json_deduced_type<TagMember>>>::
				  type;

				using tag_member_class_wrapper = std::conditional_t<
				  json_details::is_an_ordered_member_v<tag_member>,
				  json_tuple<std::tuple<typename tag_member::parse_to_t>,
				             json_tuple_types_list<tag_member>>,
				  json_details::json_deduced_type<tuple_json_mapping<tag_member>>>;

				using switcher = Switcher;
				using constructor_t =
				  json_details::json_class_constructor_t<T, Constructor>;

				using parse_to_t = T;
				using base_type = parse_to_t;
				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::VariantTagged;

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::None;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_tagged_variant<NewName, T, TagMember, Switcher,
				                                 JsonElements, Constructor>;
			};
		} // namespace json_base

		/***
		 * Link to a nullable variant like data type that is discriminated via
		 * another member.
		 * @tparam Name name of JSON member to link to
		 * @tparam T type of value to construct
		 * @tparam TagMember JSON element to pass to Switcher. Does not have to be
		 * declared in member list
		 * @tparam Switcher A callable that returns an index into JsonElements when
		 * passed the TagMember object in parent member list
		 * @tparam JsonElements a json_tagged_variant_type_list, defaults to type
		 * elements of T when T is a std::variant and they are all auto mappable
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<JSONNAMETYPE Name, typename T, typename TagMember,
		         typename Switcher, typename JsonElements, typename Constructor>
		struct json_tagged_variant
		  : json_base::json_tagged_variant<T, TagMember, Switcher, JsonElements,
		                                   Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_tagged_variant<T, TagMember, Switcher, JsonElements,
			                                 Constructor>;
		};

		template<typename T, typename TagMember, typename Switcher,
		         typename JsonElements = use_default,
		         typename Constructor = use_default>
		using json_tagged_variant_no_name =
		  json_base::json_tagged_variant<T, TagMember, Switcher, JsonElements,
		                                 Constructor>;

		template<typename T, typename TagMember, typename Switcher,
		         typename JsonElements = use_default,
		         typename Constructor = use_default>
		using json_tagged_variant_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_tagged_variant<json_details::unwrapped_t<T>, TagMember,
		                                 Switcher, JsonElements>>;

		namespace json_base {
			template<typename T, typename FromJsonConverter, typename ToJsonConverter,
			         json_details::json_options_t Options>
			struct json_custom {
				using i_am_a_json_type = void;
				using to_converter_t = ToJsonConverter;
				using from_converter_t = FromJsonConverter;
				using constructor_t = FromJsonConverter;
				static constexpr bool must_be_class_member = false;

				static_assert(
				  std::is_invocable_v<FromJsonConverter, std::string_view>,
				  "Constructor must support construction from std::string_view" );
				using parse_to_t =
				  std::invoke_result_t<FromJsonConverter, std::string_view>;
				using base_type = parse_to_t;

				static_assert(
				  std::is_invocable_v<ToJsonConverter, parse_to_t> or
				    std::is_invocable_r_v<char *, ToJsonConverter, char *, parse_to_t>,
				  "ToConverter must be callable with T or T and and OutputIterator" );

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Custom;
				static constexpr JsonParseTypes base_expected_type =
				  JsonParseTypes::Custom;

				static constexpr JsonCustomTypes custom_json_type =
				  json_details::get_bits_for<JsonCustomTypes>( json_custom_opts,
				                                               Options );

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::String;

				template<JSONNAMETYPE NewName>
				using with_name = daw::json::json_custom<NewName, T, FromJsonConverter,
				                                         ToJsonConverter, Options>;
			};
		} // namespace json_base

		/**
		 * Allow parsing of a nullable type that does not fit
		 * @tparam Name Name of JSON member to link to
		 * @tparam T type of value being constructed
		 * @tparam FromJsonConverter Callable that accepts a std::string_view of the
		 * range to parse
		 * @tparam ToJsonConverter Returns a string from the value
		 * @tparam JsonRawType JSON type value is encoded as literal/string
		 */
		template<JSONNAMETYPE Name, typename T, typename FromJsonConverter,
		         typename ToJsonConverter, json_details::json_options_t Options>
		struct json_custom
		  : json_base::json_custom<T, FromJsonConverter, ToJsonConverter, Options> {

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_custom<T, FromJsonConverter, ToJsonConverter, Options>;
		};

		template<typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         json_details::json_options_t Options = json_custom_opts_def>
		using json_custom_no_name =
		  json_base::json_custom<T, FromJsonConverter, ToJsonConverter, Options>;

		template<typename T,
		         typename FromJsonConverter = default_from_json_converter_t<T>,
		         typename ToJsonConverter = default_to_json_converter_t<T>,
		         json_details::json_options_t Options = json_custom_opts_def>
		using json_custom_lit_no_name = json_base::json_custom<
		  T, FromJsonConverter, ToJsonConverter,
		  json_details::json_custom_opts_set<Options, JsonCustomTypes::Literal>>;

		template<typename T,
		         typename FromJsonConverter =
		           default_from_json_converter_t<json_details::unwrapped_t<T>>,
		         typename ToJsonConverter =
		           default_to_json_converter_t<json_details::unwrapped_t<T>>,
		         json_details::json_options_t Options = json_custom_opts_def,
		         typename Constructor = use_default>
		using json_custom_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_custom<json_details::unwrapped_t<T>, FromJsonConverter,
		                         ToJsonConverter, Options>>;

		template<typename T,
		         typename FromJsonConverter =
		           default_from_json_converter_t<json_details::unwrapped_t<T>>,
		         typename ToJsonConverter =
		           default_to_json_converter_t<json_details::unwrapped_t<T>>,
		         json_details::json_options_t Options = json_custom_opts_def,
		         typename Constructor = use_default>
		using json_custom_lit_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_custom<
		    json_details::unwrapped_t<T>, FromJsonConverter, ToJsonConverter,
		    json_details::json_custom_opts_set<Options, JsonCustomTypes::Literal>>>;

		namespace json_base {
			template<typename JsonElement, typename Container, typename Constructor>
			struct json_array {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;
				static_assert(
				  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
				  "Missing specialization of daw::json::json_data_contract for class "
				  "mapping or specialization of daw::json::json_link_basic_type_map" );
				using json_element_t = json_details::json_deduced_type<JsonElement>;
				using json_element_parse_to_t = typename json_element_t::parse_to_t;

				static_assert( not std::is_same_v<json_element_t, void>,
				               "Unknown JsonElement type." );
				static_assert( json_details::is_no_name_v<json_element_t>,
				               "Array elements must not have names" );
				static_assert( json_details::is_a_json_type_v<json_element_t>,
				               "Error determining element type" );
				using container_t =
				  std::conditional_t<std::is_same_v<Container, use_default>,
				                     std::vector<json_element_parse_to_t>, Container>;

				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<container_t>, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, json_element_parse_to_t const *,
				                      json_element_parse_to_t const *>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t =
				  std::invoke_result_t<constructor_t, json_element_parse_to_t const *,
				                       json_element_parse_to_t const *>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Array;

				static_assert( json_details::is_no_name_v<json_element_t>,
				               "All elements of json_array must be have no_name" );
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Array;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_array<NewName, JsonElement, Container, Constructor>;
			};
		} // namespace json_base

		/** Link to a JSON array
		 * @tparam Name name of JSON member to link to
		 * @tparam Container type of C++ container being constructed(e.g.
		 * vector<int>)
		 * @tparam JsonElement Json type being parsed e.g. json_number,
		 * json_string...
		 * @tparam Constructor A callable used to make Container,
		 * default will use the Containers constructor.  Both normal and aggregate
		 * are supported
		 */
		template<JSONNAMETYPE Name, typename JsonElement, typename Container,
		         typename Constructor>
		struct json_array
		  : json_base::json_array<JsonElement, Container, Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_array<JsonElement, Container, Constructor>;
		};

		template<typename JsonElement, typename Container = use_default,
		         typename Constructor = use_default>
		using json_array_no_name =
		  json_base::json_array<JsonElement, Container, Constructor>;

		template<typename JsonElement, typename WrappedContainer,
		         typename Constructor = use_default>
		using json_array_null_no_name = json_base::json_nullable<
		  WrappedContainer, Constructor,
		  json_base::json_array<JsonElement,
		                        json_details::unwrapped_t<WrappedContainer>>>;

		namespace json_base {
			template<typename JsonElement, typename SizeMember,
			         typename Container = use_default,
			         typename Constructor = use_default>
			struct json_sized_array {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = true;
				static_assert(
				  json_details::has_unnamed_default_type_mapping_v<JsonElement>,
				  "Missing specialization of daw::json::json_data_contract for class "
				  "mapping or specialization of daw::json::json_link_basic_type_map" );
				using json_element_t = json_details::json_deduced_type<JsonElement>;
				static_assert( not std::is_same_v<json_element_t, void>,
				               "Unknown JsonElement type." );
				static_assert( json_details::is_a_json_type_v<json_element_t>,
				               "Error determining element type" );

				using json_element_parse_to_t = typename json_element_t::parse_to_t;

				using container_t =
				  std::conditional_t<std::is_same_v<Container, use_default>,
				                     std::vector<json_element_parse_to_t>, Container>;
				using constructor_t =
				  std::conditional_t<std::is_same_v<Constructor, use_default>,
				                     default_constructor<container_t>, Constructor>;

				static_assert( json_details::is_a_json_type_v<SizeMember>,
				               "The SizeMember type must have a name and be a "
				               "member of the same object as this" );
				static_assert( SizeMember::expected_type != JsonParseTypes::Null,
				               "The SizeMember cannot be a nullable type" );
				using dependent_member = SizeMember;

				static_assert(
				  std::is_invocable_v<constructor_t, json_element_parse_to_t const *,
				                      json_element_parse_to_t const *, std::size_t>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t =
				  std::invoke_result_t<constructor_t, json_element_parse_to_t const *,
				                       json_element_parse_to_t const *, std::size_t>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::SizedArray;

				static_assert( json_details::is_no_name_v<json_element_t>,
				               "All elements of json_array must be have	no_name" );
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Array;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_sized_array<NewName, JsonElement, SizeMember,
				                              Container, Constructor>;
			};
		} // namespace json_base

		template<JSONNAMETYPE Name, typename JsonElement, typename SizeMember,
		         typename Container, typename Constructor>
		struct json_sized_array
		  : json_base::json_sized_array<JsonElement, SizeMember, Container,
		                                Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_sized_array<JsonElement, SizeMember,
			                                                 Container, Constructor>;
		};

		template<typename JsonElement, typename SizeMember,
		         typename Container = use_default,
		         typename Constructor = use_default>
		using json_sized_array_no_name =
		  json_base::json_sized_array<JsonElement, SizeMember, Container,
		                              Constructor>;

		template<typename JsonElement, typename SizeMember,
		         typename WrappedContainer, typename Constructor = use_default>
		using json_sized_array_null_no_name = json_base::json_nullable<
		  WrappedContainer, Constructor,
		  json_base::json_sized_array<JsonElement, SizeMember,
		                              json_details::unwrapped_t<WrappedContainer>>>;

		namespace json_base {
			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         typename Constructor>
			struct json_key_value {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;

				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<Container>, Constructor>;

				static_assert(
				  json_details::has_unnamed_default_type_mapping_v<JsonValueType>,
				  "Missing specialization of daw::json::json_data_contract for class "
				  "mapping or specialization of daw::json::json_link_basic_type_map" );

				using json_element_t = json_details::json_deduced_type<JsonValueType>;

				static_assert( not std::is_same_v<json_element_t, void>,
				               "Unknown JsonValueType type." );

				static_assert( json_details::is_no_name_v<json_element_t>,
				               "Value member name must be the default no_name" );
				static_assert(
				  json_details::has_unnamed_default_type_mapping_v<JsonKeyType>,
				  "Missing specialization of daw::json::json_data_contract for class "
				  "mapping or specialization of daw::json::json_link_basic_type_map" );

				using json_key_t = json_details::json_deduced_type<JsonKeyType>;

				static_assert( not std::is_same_v<json_key_t, void>,
				               "Unknown JsonKeyType type." );
				static_assert( json_details::is_no_name_v<json_key_t>,
				               "Key member name must be the default no_name" );

				using parse_to_t = Container;
				using base_type = parse_to_t;
				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::KeyValue;
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Class;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_key_value<NewName, Container, JsonValueType,
				                            JsonKeyType, Constructor>;
			};
		} // namespace json_base

		/** Map a KV type json class { "Key StringRaw": ValueType, ... }
		 *  to a c++ class.  Keys are Always string like and the destination
		 *  needs to be constructable with a pointer, size
		 *  @tparam Name name of JSON member to link to
		 *  @tparam Container type to put values in
		 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
		 *  json_string, ... ). It also supports basic types like numbers, bool, and
		 * mapped classes and enums(mapped to numbers)
		 *  @tparam JsonKeyType type of key in kv pair.  As with value it supports
		 * basic types too
		 *  @tparam Constructor A callable used to make Container, default will use
		 * the Containers constructor.  Both normal and aggregate are supported
		 */
		template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
		         typename JsonKeyType, typename Constructor>
		struct json_key_value
		  : json_base::json_key_value<Container, JsonValueType, JsonKeyType,
		                              Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_key_value<Container, JsonValueType,
			                                               JsonKeyType, Constructor>;
		};

		template<typename Container,
		         typename JsonValueType = typename Container::mapped_type,
		         typename JsonKeyType = typename Container::key_type,
		         typename Constructor = use_default>
		using json_key_value_no_name =
		  json_base::json_key_value<Container, JsonValueType, JsonKeyType,
		                            Constructor>;

		template<typename WrappedContainer,
		         typename JsonValueType = typename json_details::unwrapped_t<
		           WrappedContainer>::mapped_type,
		         typename JsonKeyType =
		           typename json_details::unwrapped_t<WrappedContainer>::key_type,
		         typename Constructor = use_default>
		using json_key_value_null_no_name = json_base::json_nullable<
		  WrappedContainer, Constructor,
		  json_base::json_key_value<json_details::unwrapped_t<WrappedContainer>,
		                            JsonValueType, JsonKeyType>>;

		namespace json_base {
			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         typename Constructor = use_default>
			struct json_key_value_array {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;

				using constructor_t =
				  std::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<Container>, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, Container>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t = std::invoke_result_t<constructor_t, Container>;
				using base_type = parse_to_t;

				using json_key_t = json_details::copy_name_when_noname<
				  json_details::json_deduced_type<JsonKeyType>,
				  json_details::default_key_name>;

				static_assert( not std::is_same_v<json_key_t, void>,
				               "Unknown JsonKeyType type." );
				static_assert( not json_details::is_no_name_v<json_key_t>,
				               "Must supply a valid key member name" );
				using json_value_t = json_details::copy_name_when_noname<
				  json_details::json_deduced_type<JsonValueType>,
				  json_details::default_value_name>;

				using json_class_t =
				  json_class<tuple_json_mapping<json_key_t, json_value_t>>;

				static_assert( not std::is_same_v<json_value_t, void>,
				               "Unknown JsonValueType type." );
				static_assert( not json_details::is_no_name_v<json_value_t>,
				               "Must supply a valid value member name" );
				static_assert( json_key_t::name != json_value_t::name,
				               "Key and Value member names cannot be the same" );
				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::KeyValueArray;
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Array;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_key_value_array<NewName, Container, JsonValueType,
				                                  JsonKeyType, Constructor>;
			};
		} // namespace json_base

		/**
		 * Map a KV type json array [ {"key": ValueOfKeyType, "value":
		 * ValueOfValueType},... ] to a c++ class. needs to be constructable with a
		 * pointer, size
		 *  @tparam Name name of JSON member to link to
		 *  @tparam Container type to put values in
		 *  @tparam JsonValueType Json type of value in kv pair( e.g. json_number,
		 *  json_string, ... ).  If specific json member type isn't specified, the
		 * member name defaults to "value"
		 *  @tparam JsonKeyType type of key in kv pair.  If specific json member
		 * type isn't specified, the key name defaults to "key"
		 *  @tparam Constructor A callable used to make Container, default will use
		 * the Containers constructor.  Both normal and aggregate are supported
		 */
		template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
		         typename JsonKeyType, typename Constructor>
		struct json_key_value_array
		  : json_base::json_key_value_array<Container, JsonValueType, JsonKeyType,
		                                    Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_key_value_array<Container, JsonValueType, JsonKeyType,
			                                  Constructor>;
		};

		template<typename Container,
		         typename JsonValueType = typename Container::mapped_type,
		         typename JsonKeyType = typename Container::key_type,
		         typename Constructor = use_default>
		using json_key_value_array_no_name =
		  json_base::json_key_value_array<Container, JsonValueType, JsonKeyType,
		                                  Constructor>;

		template<typename WrappedContainer,
		         typename JsonValueType = typename json_details::unwrapped_t<
		           WrappedContainer>::mapped_type,
		         typename JsonKeyType =
		           typename json_details::unwrapped_t<WrappedContainer>::key_type,
		         typename Constructor = use_default>
		using json_key_value_array_null_no_name =
		  json_base::json_nullable<WrappedContainer, Constructor,
		                           json_base::json_key_value_array<
		                             json_details::unwrapped_t<WrappedContainer>,
		                             JsonValueType, JsonKeyType>>;

		namespace json_base {
			template<typename Tuple, typename JsonTupleTypesList,
			         typename Constructor>
			struct json_tuple {
				using i_am_a_json_type = void;

				using wrapped_type = Tuple;
				static constexpr bool must_be_class_member = false;
				static constexpr bool force_aggregate_construction = false;

				using sub_member_list = typename std::conditional_t<
				  std::is_same_v<JsonTupleTypesList, use_default>,
				  daw::traits::identity<json_details::tuple_types_list<Tuple>>,
				  daw::traits::identity<
				    json_details::tuple_types_list<JsonTupleTypesList>>>::type::types;

				using constructor_t =
				  json_details::json_class_constructor_t<Tuple, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, Tuple>,
				  "Constructor must support copy and/or move construction" );
				using parse_to_t = std::invoke_result_t<constructor_t, Tuple>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Tuple;
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::Array;

				template<JSONNAMETYPE NewName>
				using with_name =
				  daw::json::json_tuple<NewName, Tuple, JsonTupleTypesList,
				                        Constructor>;
			};
		} // namespace json_base

		template<JSONNAMETYPE Name, typename Tuple, typename JsonTupleTypesList,
		         typename Constructor>
		struct json_tuple
		  : json_base::json_tuple<Tuple, JsonTupleTypesList, Constructor> {
			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_tuple<Tuple, JsonTupleTypesList, Constructor>;
		};

		template<typename Tuple, typename Constructor = use_default,
		         typename JsonTupleTypesList = use_default>
		using json_tuple_no_name =
		  json_base::json_tuple<Tuple, Constructor, JsonTupleTypesList>;

		template<typename WrappedTuple, typename Constructor = use_default,
		         typename JsonTupleTypesList = use_default>
		using json_tuple_null_no_name = json_base::json_nullable<
		  WrappedTuple, Constructor,
		  json_base::json_tuple<json_details::unwrapped_t<WrappedTuple>>>;

		namespace json_base {
			template<typename Variant, typename TagMember, typename Switcher,
			         typename JsonElements = use_default,
			         typename Constructor = use_default>
			struct json_intrusive_variant {
				using i_am_a_json_type = void;
				using i_am_a_tagged_variant = void;
				static constexpr bool must_be_class_member = false;

				using json_elements = typename std::conditional_t<
				  std::is_same_v<JsonElements, use_default>,
				  json_details::variant_alternatives_list<Variant>,
				  daw::traits::identity<JsonElements>>::type;

				static_assert(
				  std::is_same_v<typename json_elements::i_am_variant_type_list, void>,
				  "Expected a json_variant_type_list or could not deduce alternatives "
				  "from Variant" );

				using tag_submember = typename std::conditional_t<
				  json_details::is_an_ordered_member_v<TagMember>,
				  daw::traits::identity<TagMember>,
				  daw::traits::identity<json_details::json_deduced_type<TagMember>>>::
				  type;

				static_assert( json_details::is_a_json_type_v<tag_submember>,
				               "The TagMember type must have a name and be a "
				               "member of the same object as this" );

				static_assert(
				  std::is_invocable_v<Switcher, typename tag_submember::parse_to_t>,
				  "There is a mismatch between the Switcher and the TagMember's parsed "
				  "result" );

				using switcher = Switcher;

				using tag_submember_class_wrapper = std::conditional_t<
				  json_details::is_an_ordered_member_v<tag_submember>,
				  json_tuple<std::tuple<typename tag_submember::parse_to_t>,
				             use_default, json_tuple_types_list<tag_submember>>,
				  json_details::json_deduced_type<tuple_json_mapping<tag_submember>>>;

				using constructor_t =
				  json_details::json_class_constructor_t<Variant, Constructor>;

				using parse_to_t = Variant;
				using base_type = parse_to_t;
				static constexpr JsonParseTypes expected_type =
				  JsonParseTypes::VariantIntrusive;
				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::None;

				using json_container_type = std::conditional_t<
				  std::disjunction_v<json_details::is_an_ordered_member<
				                       json_details::json_deduced_type<tag_submember>>,
				                     json_details::is_no_name<
				                       json_details::json_deduced_type<tag_submember>>>,
				  json_tuple<std::tuple<typename json_details::json_deduced_type<
				               tag_submember>::parse_to_t>,
				             json_tuple_types_list<tag_submember>>,
				  tuple_json_mapping<tag_submember>>;

				template<JSONNAMETYPE NewName>
				using with_name = daw::json::json_intrusive_variant<
				  NewName, Variant, TagMember, Switcher, JsonElements, Constructor>;
			};
		} // namespace json_base
		/***
		 * Link to a nullable variant like data type that is discriminated via
		 * another member.
		 * @tparam Name name of JSON member to link to
		 * @tparam T type of value to construct
		 * @tparam TagMember JSON element to pass to Switcher. Does not have to be
		 * declared in member list
		 * @tparam Switcher A callable that returns an index into JsonElements when
		 * passed the TagMember object in parent member list
		 * @tparam JsonElements a json_tagged_variant_type_list, defaults to type
		 * elements of T when T is a std::variant and they are all auto mappable
		 * @tparam Constructor A callable used to construct T.  The
		 * default supports normal and aggregate construction
		 */
		template<JSONNAMETYPE Name, typename T, typename TagMember,
		         typename Switcher, typename JsonElements, typename Constructor>
		struct json_intrusive_variant
		  : json_base::json_intrusive_variant<T, TagMember, Switcher, JsonElements,
		                                      Constructor> {

			static constexpr daw::string_view name = Name;

			using without_name =
			  json_base::json_intrusive_variant<T, TagMember, Switcher, JsonElements,
			                                    Constructor>;
		};

		template<typename T, typename TagMember, typename Switcher,
		         typename JsonElements = use_default,
		         typename Constructor = use_default>
		using json_intrusive_variant_no_name =
		  json_base::json_intrusive_variant<T, TagMember, Switcher, JsonElements,
		                                    Constructor>;

		template<typename T, typename TagMember, typename Switcher,
		         typename JsonElements = use_default,
		         typename Constructor = use_default>
		using json_intrusive_variant_null_no_name = json_base::json_nullable<
		  T, Constructor,
		  json_base::json_intrusive_variant<json_details::unwrapped_t<T>, TagMember,
		                                    Switcher, JsonElements>>;

		/***
		 * An untyped JSON value
		 */
		using json_value = basic_json_value<DefaultParsePolicy>;

		/***
		 * A name/value pair of string_view/json_value.  This is used for iterating
		 * class members in a json_value
		 */
		using json_pair = basic_json_pair<DefaultParsePolicy>;

		/***
		 * json_raw allows for raw JSON access to the member data. It requires a
		 * type that is constructable from (char const *, std::size_t) arguments and
		 * for serialization requires that it can be passed to std::begin/std::end
		 * and the iterator returned has a value_type of char
		 * @tparam Name json member name
		 * @tparam T type to hold raw JSON data, defaults to json_value
		 * @tparam Constructor A callable used to construct T.
		 * have a null value
		 */
		template<JSONNAMETYPE Name, typename T = json_value,
		         typename Constructor = use_default>
		struct json_raw;

		namespace json_base {
			/***
			 * json_raw allows for raw JSON access to the member data. It requires a
			 * type that is constructable from (char const *, std::size_t) arguments
			 * and for serialization requires that it can be passed to
			 * std::begin/std::end and the iterator returned has a value_type of char.
			 * Any whitespace surrounding the value may not be preserved.  The default
			 * T json_value allows for delaying the parsing of this member until later
			 * @tparam T type to hold raw JSON data, defaults to json_value
			 * @tparam Constructor A callable used to construct T.
			 */
			template<typename T, typename Constructor>
			struct json_raw {
				using i_am_a_json_type = void;
				static constexpr bool must_be_class_member = false;
				using wrapped_type = json_value;
				using constructor_t =
				  json_details::json_class_constructor_t<T, Constructor>;

				static_assert(
				  std::is_invocable_v<constructor_t, char const *, std::size_t>,
				  "Constructor must be constructible from char const *, std::size_t" );
				using parse_to_t =
				  std::invoke_result_t<constructor_t, char const *, std::size_t>;
				using base_type = parse_to_t;

				static constexpr JsonParseTypes expected_type = JsonParseTypes::Unknown;
				static constexpr JsonParseTypes base_expected_type =
				  JsonParseTypes::Unknown;

				static constexpr JsonBaseParseTypes underlying_json_type =
				  JsonBaseParseTypes::None;

				template<JSONNAMETYPE NewName>
				using with_name = daw::json::json_raw<NewName, T, Constructor>;
			};
		} // namespace json_base

		/***
		 * json_raw allows for raw JSON access to the member data. It requires a
		 * type that is constructable from (char const *, std::size_t) arguments and
		 * for serialization requires that it can be passed to std::begin/std::end
		 * and the iterator returned has a value_type of char. Any whitespace
		 * surrounding the value may not be preserved.  The default T json_value
		 * allows for delaying the parsing of this member until later
		 * @tparam Name json member name
		 * @tparam T type to hold raw JSON data, defaults to json_value
		 * @tparam Constructor A callable used to construct T.
		 */
		template<JSONNAMETYPE Name, typename T, typename Constructor>
		struct json_raw : json_base::json_raw<T, Constructor> {
			static constexpr daw::string_view name = Name;

			using without_name = json_base::json_raw<T, Constructor>;
		};

		template<JSONNAMETYPE Name, typename T = json_value,
		         typename Constructor = use_default, auto...>
		using json_delayed
		  [[deprecated( "Was renamed to json_raw, will be removed in v4" )]] =
		    json_raw<Name, T, Constructor>;

		/***
		 * json_raw_null allows for raw JSON access to the nullable member data.
		 * It requires a type that is constructable from (char const *,
		 * std::size_t) arguments and for serialization requires that it can be
		 * passed to std::begin/std::end and the iterator returned has a
		 * value_type of char. Any whitespace
		 * surrounding the value may not be preserved.  The default T json_value
		 * allows for delaying the parsing of this member until later
		 * @tparam Name json member name
		 * @tparam T type to hold raw JSON data, defaults to json_value
		 * @tparam Constructor A callable used to construct T.
		 */
		template<JSONNAMETYPE Name, typename T = std::optional<json_value>,
		         typename Constructor = use_default>
		using json_raw_null =
		  json_nullable<Name, T, Constructor,
		                json_base::json_raw<json_details::unwrapped_t<T>>>;

		template<JSONNAMETYPE Name, typename T = std::optional<json_value>,
		         typename Constructor = use_default>
		using json_delayed_null
		  [[deprecated( "Was renamed to json_raw_null, will be removed in v4" )]] =
		    json_raw_null<Name, T, Constructor>;

		/***
		 * json_raw allows for raw JSON access to the member data. It requires a
		 * type that is constructable from (char const *, std::size_t) arguments and
		 * for serialization requires that it can be passed to std::begin/std::end
		 * and the iterator returned has a value_type of char. Any whitespace
		 * surrounding the value may not be preserved.  The default T json_value
		 * allows for delaying the parsing of this member until later
		 * @tparam T type to hold raw JSON data, defaults to json_value
		 * @tparam Constructor A callable used to construct T.
		 */
		template<typename T = json_value, typename Constructor = use_default>
		using json_raw_no_name = json_base::json_raw<T, Constructor>;

		/***
		 * json_raw_null allows for raw JSON access to the nullable member data.
		 * It requires a type that is constructable from (char const *,
		 * std::size_t) arguments and for serialization requires that it can be
		 * passed to std::begin/std::end and the iterator returned has a
		 * value_type of char. Any whitespace
		 * surrounding the value may not be preserved.  The default T json_value
		 * allows for delaying the parsing of this member until later
		 * @tparam T type to hold raw JSON data, defaults to json_value
		 * @tparam Constructor A callable used to construct T.
		 */
		template<typename T = std::optional<json_value>,
		         typename Constructor = use_default>
		using json_raw_null_no_name = json_base::json_nullable<
		  T, Constructor, json_base::json_raw<json_details::unwrapped_t<T>>>;

		template<typename ParseState>
		struct json_data_contract<basic_json_value<ParseState>> {
			using type =
			  json_type_alias<json_raw_no_name<basic_json_value<ParseState>>>;
		};

	} // namespace DAW_JSON_VER
} // namespace daw::json
