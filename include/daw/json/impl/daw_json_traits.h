// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../../daw_readable_value.h"
#include "daw_json_default_constuctor.h"
#include "daw_json_enums.h"

#include <daw/cpp_17.h>
#include <daw/daw_fwd_pack_apply.h>
#include <daw/daw_move.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_traits.h>

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

/***
 * Customization point traits
 *
 */
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename T>
			using has_op_bool_test =
			  decltype( static_cast<bool>( std::declval<T>( ) ) );

			template<typename T>
			inline constexpr bool has_op_bool_v =
			  daw::is_detected_v<has_op_bool_test, T>;

			template<typename T>
			using has_op_star_test = decltype( *std::declval<T>( ) );

			template<typename T>
			inline constexpr bool has_op_star_v =
			  daw::is_detected_v<has_op_star_test, T>;

			template<typename T>
			using has_empty_member_test = decltype( std::declval<T>( ).empty( ) );

			template<typename T>
			inline constexpr bool has_empty_member_v =
			  daw::is_detected_v<has_empty_member_test, T>;

			/*
			template<typename T>
			inline constexpr auto has_value( T const &v )
			  -> std::enable_if_t<is_readable_value_v<T>, bool> {

			  return readable_value_traits<T>::has_value( v );
			}

			template<typename T>
			inline constexpr auto has_value( T const &v )
			  -> std::enable_if_t<not is_readable_value_v<T> and
			has_empty_member_v<T>, bool> { return not v.empty( );
			}*/

			template<typename Constructor, typename... Args>
			struct nullable_constructor_cannot_be_invoked;

			template<typename Constructor, typename... Args>
			struct constructor_cannot_be_invoked;

			// clang-format off
			template<typename Constructor, typename... Args>
			using construction_result =
					std::conditional_t<
						std::is_invocable_v<Constructor, Args...>,
						std::invoke_result<Constructor, Args...>,
						traits::identity<constructor_cannot_be_invoked<Constructor, Args...>>
			  >;
			// clang-format on
		} // namespace json_details

		namespace json_details {
			template<typename JsonMember>
			using without_name = typename JsonMember::without_name;

			template<typename JsonMember, JSONNAMETYPE NewName, bool Cond>
			using copy_name_when = std::conditional_t<
			  Cond, typename JsonMember::template with_name<NewName>, JsonMember>;

			template<typename JsonMember, JSONNAMETYPE NewName>
			using copy_name_when_noname =
			  copy_name_when<JsonMember, NewName, is_no_name_v<JsonMember>>;
		} // namespace json_details
		/***
		 * This class is used as a way to indicate that a json_data_contract
		 * specialization has not been done for a user class.
		 */
		template<typename>
		struct missing_json_data_contract_for;

		/***
		 * Mapping class for JSON data structures to C++.  It must be specialized in
		 * order to parse to a user class
		 * @tparam T Class to map
		 */
		template<typename T, typename = void>
		struct json_data_contract {
			using type = missing_json_data_contract_for<T>;
		};

		/***
		 * This trait gets us the mapping type from the contract.
		 */
		template<typename T>
		using json_data_contract_trait_t = typename json_data_contract<T>::type;

		template<typename T>
		using test_valid_json_data_contract_trait_t =
		  typename json_data_contract_trait_t<T>::i_am_a_json_member_list;

		namespace json_details {
			template<typename T>
			using json_map_alias_test =
			  typename json_data_contract_trait_t<T>::i_am_a_json_map_alias;

			template<typename T>
			inline constexpr bool is_json_map_alias_v =
			  daw::is_detected_v<json_map_alias_test, T>;

			template<typename T>
			using force_aggregate_construction_test =
			  typename json_data_contract<T>::force_aggregate_construction;

			template<typename T>
			using force_aggregate_construction_test2 =
			  decltype( T::force_aggregate_construction );

			template<typename JsonMember>
			using switcher_t = typename JsonMember::switcher;

			template<typename JsonMember>
			inline constexpr bool has_switcher_v =
			  daw::is_detected_v<switcher_t, JsonMember>;
		} // namespace json_details
		/***
		 * This trait can be specialized such that when class being returned has
		 * non-move/copyable members the construction can be done with { } instead
		 * of a callable.  This is a blunt object and probably should not be used
		 * add a type alias named force_aggregate_construction to your
		 * json_data_contract specialization
		 * @tparam T type to specialize
		 */
		template<typename T>
		inline constexpr bool force_aggregate_construction_v =
		  daw::is_detected_v<json_details::force_aggregate_construction_test, T> or
		  daw::is_detected_v<json_details::force_aggregate_construction_test2, T>;

		namespace json_details {
			template<typename T>
			T uneval_func( );
		}

		template<typename... Ts>
		inline constexpr bool is_empty_pack_v = sizeof...( Ts ) == 0;

		template<typename... Ts>
		using is_empty_pack = std::bool_constant<is_empty_pack_v<Ts...>>;

		/***
		 * Can use the fast, pseudo random string iterators.  They are
		 * InputIterators with an operator- that allows for O(1) distance
		 * calculations as we often know the length but cannot provide random
		 * access.  For types that only use InputIterator operations and last -
		 * first for distance calc
		 */
		template<typename>
		inline constexpr bool can_single_allocation_string_v = false;

		template<typename Char, typename CharTrait, typename Allocator>
		inline constexpr bool can_single_allocation_string_v<
		  std::basic_string<Char, CharTrait, Allocator>> = true;

		template<typename T>
		using can_single_allocation_string =
		  std::bool_constant<can_single_allocation_string_v<T>>;

		namespace json_details {
			template<typename T>
			using json_type_t = typename T::i_am_a_json_type;

			template<typename T>
			inline constexpr bool is_a_json_type_v =
			  daw::is_detected_v<json_type_t, T>;

			template<typename T>
			using is_a_json_type = std::bool_constant<is_a_json_type_v<T>>;

			template<typename T>
			using ordered_member_t = typename T::i_am_an_ordered_member;

			template<typename T>
			inline constexpr bool is_an_ordered_member_v =
			  daw::is_detected_v<ordered_member_t, T>;

			template<typename T>
			using is_an_ordered_member =
			  std::bool_constant<is_an_ordered_member_v<T>>;

			template<typename T>
			using is_a_json_tagged_variant_test =
			  typename T::i_am_a_json_tagged_variant;

			template<typename T>
			inline constexpr bool is_a_json_tagged_variant_v =
			  daw::is_detected_v<is_a_json_tagged_variant_test, T>;

			template<typename T>
			using is_a_json_tagged_variant =
			  std::bool_constant<is_a_json_tagged_variant_v<T>>;

			template<typename T, bool, bool>
			struct json_data_contract_constructor_impl {
				using type = default_constructor<T>;
			};

			template<typename T>
			using has_json_data_constract_constructor_test =
			  decltype( decltype( std::declval<
			                      json_data_contract_trait_t<T>> )::constructor );

			template<typename T>
			struct json_data_contract_constructor_impl<T, true, true> {
				using type = typename json_data_contract_trait_t<T>::constructor;
			};

			template<typename T>
			using json_data_contract_constructor =
			  json_data_contract_constructor_impl<
			    T, daw::is_detected_v<json_data_contract_trait_t, T>,
			    daw::is_detected_v<has_json_data_constract_constructor_test, T>>;

			template<typename T>
			using json_data_contract_constructor_t =
			  typename json_data_contract_constructor<T>::type;

			template<typename T>
			using json_class_constructor_t_impl =
			  typename json_data_contract<T>::constructor;

			template<typename T, typename Default>
			using json_class_constructor_t =
			  daw::detected_or_t<Default, json_class_constructor_t_impl, T>;

			namespace is_string_like_impl {
				template<typename T>
				using has_data_test = decltype( std::data( std::declval<T>( ) ) );

				template<typename T>
				using has_size_test = decltype( std::size( std::declval<T>( ) ) );
			} // namespace is_string_like_impl

			template<typename T>
			inline constexpr bool is_string_view_like_v =
			  daw::is_detected_v<is_string_like_impl::has_data_test, T>
			    and daw::is_detected_v<is_string_like_impl::has_size_test, T>;

			static_assert( is_string_view_like_v<std::string_view> );

		} // namespace json_details

		/***
		 * Trait for passively exploiting the zero termination when the type
		 * guarantees it.
		 */
		template<typename>
		inline constexpr bool is_zero_terminated_string_v = false;

		template<typename CharT, typename Traits, typename Alloc>
		inline constexpr bool
		  is_zero_terminated_string_v<std::basic_string<CharT, Traits, Alloc>> =
		    true;

		template<typename T>
		struct is_zero_terminated_string
		  : std::bool_constant<is_zero_terminated_string_v<T>> {};

		namespace json_details {
			template<typename ParsePolicy, auto Option>
			using apply_policy_option_t =
			  typename ParsePolicy::template SetPolicyOptions<Option>;

			template<typename ParsePolicy, typename String, auto Option>
			using apply_zstring_policy_option_t = std::conditional_t<
			  is_zero_terminated_string_v<daw::remove_cvref_t<String>>,
			  apply_policy_option_t<ParsePolicy, Option>, ParsePolicy>;

			template<typename String>
			inline constexpr bool is_mutable_string_v =
			  not std::is_const_v<std::remove_pointer_t<std::remove_reference_t<
			    decltype( std::data( std::declval<String &&>( ) ) )>>>;

			template<typename String>
			constexpr bool is_mutable_string =
			  json_details::is_mutable_string_v<String>;

			template<typename String>
			constexpr bool is_rvalue_string = std::is_rvalue_reference_v<String>;

			template<typename String>
			constexpr bool is_ref_string =
			  not is_rvalue_string<String> and
			  std::is_const_v<std::remove_reference_t<String>>;

			template<typename ParsePolicy, typename String, auto OptionMutable,
			         auto OptionImmutable>
			using apply_mutable_policy = std::conditional_t<
			  ParsePolicy::allow_temporarily_mutating_buffer( ),
			  std::conditional_t<is_mutable_string_v<String>,
			                     apply_policy_option_t<ParsePolicy, OptionMutable>,
			                     apply_policy_option_t<ParsePolicy, OptionImmutable>>,
			  std::conditional_t<
			    (is_rvalue_string<String> and is_mutable_string_v<String>),
			    apply_policy_option_t<ParsePolicy, OptionMutable>,
			    apply_policy_option_t<ParsePolicy, OptionImmutable>>>;
		} // namespace json_details

		/***
		 * Ignore unknown members trait allows the parser to skip unknown members
		 * when the default is exact
		 */
		template<typename>
		struct ignore_unknown_members : std::false_type {};

		namespace json_details {
			template<typename T>
			using has_exact_mapping_trait_in_class_map =
			  typename json_data_contract<T>::exact_class_mapping;

			template<typename T>
			using has_ignore_unknown_members_trait_in_class_map =
			  typename json_data_contract<T>::ignore_unknown_members;
		} // namespace json_details

		template<typename T>
		inline constexpr bool ignore_unknown_members_v = std::disjunction_v<
		  ignore_unknown_members<T>,
		  daw::is_detected<
		    json_details::has_ignore_unknown_members_trait_in_class_map, T>>;

		/***
		 * A trait to specify that this class, when parsed, will describe all
		 * members of the JSON object. Anything not mapped is an error.
		 * Either specialize this variable daw::json::is_exact_class_mapping_v, or
		 * have a type in your json_data_contract named exact_class_mapping for your
		 * type
		 */
		template<typename T>
		inline constexpr bool is_exact_class_mapping_v =
		  daw::is_detected_v<json_details::has_exact_mapping_trait_in_class_map, T>;

		namespace json_details {
			template<typename T, typename ParseState>
			inline constexpr bool all_json_members_must_exist_v =
			  not ignore_unknown_members_v<T> and
			  ( is_exact_class_mapping_v<T> or
			    ParseState::use_exact_mappings_by_default );

			template<JsonNullable ClassNullability, JsonNullable DependentNullability>
			inline constexpr bool is_nullability_compatable_v =
			  ( DependentNullability == JsonNullable::MustExist ) or
			  ( ClassNullability == JsonNullable::Nullable or
			    ClassNullability == JsonNullable::NullVisible );

			template<typename T>
			using element_type_t = typename T::element_type;

			template<typename T>
			using has_element_type = daw::is_detected<element_type_t, T>;
		} // namespace json_details

		/***
		 * is_pointer_like is used in json_array to ensure that to_json_data returns
		 * a Container/View of the data with the size encoded with it.
		 * The std
		 */
		template<typename T>
		inline constexpr bool is_pointer_like_v =
		  std::disjunction_v<std::is_pointer<T>, json_details::has_element_type<T>>;

		/// Allow tuple like types to be used in json_tuple
		/// \tparam Tuple tuple like type to
		template<typename Tuple>
		struct tuple_elements_pack;

		template<typename... Ts>
		struct tuple_elements_pack<std::tuple<Ts...>> {
			using type = std::tuple<Ts...>;

			static constexpr std::size_t size = sizeof...( Ts );

			template<std::size_t Idx>
			using element_t = std::tuple_element_t<Idx, type>;

			template<std::size_t Idx, typename Tuple>
			static constexpr decltype( auto ) get( Tuple &&tp ) {
				return std::get<Idx>( DAW_FWD( tp ) );
			}
		};

		template<typename... Ts>
		struct tuple_elements_pack<daw::fwd_pack<Ts...>> {
			using type = daw::fwd_pack<Ts...>;

			static constexpr std::size_t size = sizeof...( Ts );

			template<std::size_t Idx>
			using element_t = typename daw::tuple_element<Idx, type>::type;

			template<std::size_t Idx, typename Tuple>
			static constexpr decltype( auto ) get( Tuple &&tp ) {
				return DAW_FWD( tp ).template get<Idx>( );
			}
		};

		namespace json_details {
			template<typename T>
			using tuple_test = typename tuple_elements_pack<T>::type;

			/// Detect if T follows the tuple protocol
			/// \tparam T
			template<typename T>
			inline constexpr bool is_tuple_v =
			  daw::is_detected_v<json_details::tuple_test, T>;

			template<typename T>
			using is_tuple = std::bool_constant<is_tuple_v<T>>;

			template<typename T, JsonNullable Nullable = JsonNullable::Nullable>
			using unwrapped_t =
			  typename std::conditional_t<is_nullable_json_value_v<Nullable>,
			                              readable_value_type_t<T>, T>;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
