// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

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
namespace daw::json DAW_ATTRIB_PUBLIC {
	inline namespace DAW_JSON_VER {
		namespace json_details DAW_ATTRIB_HIDDEN {
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

			template<typename T>
			inline constexpr bool is_readable_v =
			  has_op_bool_v<T> or has_empty_member_v<T>;

			template<typename T>
			inline constexpr auto has_value( T const &v )
			  -> std::enable_if_t<is_readable_v<T>, bool> {

				if constexpr( has_op_bool_v<T> ) {
					return static_cast<bool>( v );
				} else if constexpr( has_empty_member_v<T> ) {
					return not v.empty( );
				}
				DAW_UNREACHABLE( );
			}

			template<typename Constructor, typename... Args>
			struct nullable_constructor_cannot_be_invoked;

			template<typename Constructor, typename... Args>
			struct constructor_cannot_be_invoked;

			// clang-format off
			template<bool Nullable, typename Constructor, typename... Args>
			using construction_result =
			  std::conditional_t<
					Nullable,
					std::conditional_t<
						std::is_invocable_v<Constructor, Args...>,
						std::conditional_t<
								std::is_invocable_v<Constructor>,
								std::invoke_result<Constructor>,
								traits::identity<nullable_constructor_cannot_be_invoked<Constructor>>
						>,
						traits::identity<nullable_constructor_cannot_be_invoked<Constructor, Args...>>
					>,
					std::conditional_t<
						std::is_invocable_v<Constructor, Args...>,
						std::invoke_result<Constructor, Args...>,
						traits::identity<constructor_cannot_be_invoked<Constructor, Args...>>
          >
			  >;
			// clang-format on
		} // namespace json_details

		namespace json_details DAW_ATTRIB_HIDDEN {
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

		namespace json_details DAW_ATTRIB_HIDDEN {
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
		using force_aggregate_construction = std::bool_constant<std::disjunction_v<
		  daw::is_detected<json_details::force_aggregate_construction_test, T>,
		  daw::is_detected<json_details::force_aggregate_construction_test2, T>>>;

		template<typename T>
		inline constexpr bool force_aggregate_construction_v =
		  force_aggregate_construction<T>::value;

		namespace json_details DAW_ATTRIB_HIDDEN {
			template<typename T>
			T uneval_func( );
		}

		template<typename... Ts>
		using is_empty_pack = std::bool_constant<( sizeof...( Ts ) == 0 )>;

		template<typename... Ts>
		inline constexpr bool is_empty_pack_v = is_empty_pack<Ts...>::value;

		/***
		 * Default Constructor for a type.  It accounts for aggregate types and uses
		 * brace construction for them
		 * @tparam T type to construct
		 */
		template<typename T>
		struct default_constructor {
			[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr T operator( )( ) const {
				return T{ };
			}

			template<
			  typename... Args,
			  std::enable_if_t<std::conjunction_v<std::is_constructible<T, Args...>,
			                                      not_trait<is_empty_pack<Args...>>>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr T
			operator( )( Args &&...args ) const {

				return T( DAW_FWD2( Args, args )... );
			}

			template<
			  typename... Args,
			  typename std::enable_if_t<
			    std::conjunction_v<daw::not_trait<std::is_constructible<T, Args...>>,
			                       daw::not_trait<is_empty_pack<Args...>>,
			                       daw::traits::is_list_constructible<T, Args...>>,
			    std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr T
			operator( )( Args &&...args ) const
			  noexcept( noexcept( T{ DAW_FWD2( Args, args )... } ) ) {

				return T{ DAW_FWD2( Args, args )... };
			}
		};

		template<typename, typename = void>
		struct is_default_constructor : std::false_type {};

		template<typename T>
		struct is_default_constructor<default_constructor<T>> : std::true_type {};

		template<typename T>
		inline constexpr bool is_default_constructor_v =
		  is_default_constructor<T>::value;

		namespace json_details DAW_ATTRIB_HIDDEN {
			template<typename>
			struct is_std_allocator : std::false_type {};

			template<typename... Ts>
			struct is_std_allocator<std::allocator<Ts...>> : std::true_type {};
		} // namespace json_details

		inline namespace {
			template<typename Iterator>
			struct construct_array_cleanup {
				Iterator &it;

				DAW_ATTRIB_INLINE
				DAW_SG_CXDTOR inline ~construct_array_cleanup( ) noexcept( false ) {
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
					if( DAW_IS_CONSTANT_EVALUATED( ) ) {
						++it;
					} else {
#endif
						if( std::uncaught_exceptions( ) == 0 ) {
							++it;
						}
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
					}
#endif
				}
			};
		} // namespace

		template<typename T, std::size_t Sz>
		struct default_constructor<std::array<T, Sz>> {
			DAW_ATTRIB_INLINE constexpr std::array<T, Sz> operator( )( ) const
			  noexcept( noexcept( std::array<T, Sz>{ } ) ) {
				return { };
			}

			DAW_ATTRIB_INLINE constexpr std::array<T, Sz> &&
			operator( )( std::array<T, Sz> &&v ) const noexcept {
				return DAW_MOVE( v );
			}

			template<typename Iterator, std::size_t... Is>
			DAW_ATTRIB_INLINE static constexpr std::array<T, Sz>
			construct_array( Iterator first, Iterator last,
			                 std::index_sequence<Is...> ) {
				auto const get_result = [&]( std::size_t ) {
					if( first != last ) {
						if constexpr( std::is_move_constructible_v<T> ) {
							auto result = *first;
							++first;
							return result;
						} else {
							auto const run_after_parse = construct_array_cleanup{ first };
							(void)run_after_parse;
							return *first;
						}
					}
					return T{ };
				};
				return std::array<T, Sz>{ get_result( Is )... };
			}

			template<typename Iterator>
			DAW_ATTRIB_INLINE constexpr std::array<T, Sz>
			operator( )( Iterator first, Iterator last ) const {
				return construct_array( first, last, std::make_index_sequence<Sz>{ } );
			}
		};

		template<typename T, typename Alloc>
		struct default_constructor<std::vector<T, Alloc>> {
			// DAW
			DAW_ATTRIB_INLINE inline std::vector<T, Alloc> operator( )( ) const
			  noexcept( noexcept( std::vector<T, Alloc>( ) ) ) {
				return { };
			}

			DAW_ATTRIB_INLINE inline std::vector<T, Alloc> &&
			operator( )( std::vector<T, Alloc> &&v ) const
			  noexcept( noexcept( std::vector<T, Alloc>( v ) ) ) {
				return DAW_MOVE( v );
			}

			template<typename Iterator>
			DAW_ATTRIB_INLINE inline std::vector<T, Alloc>
			operator( )( Iterator first, Iterator last,
			             Alloc const &alloc = Alloc{ } ) const {
				if constexpr( std::is_same_v<std::random_access_iterator_tag,
				                             typename std::iterator_traits<
				                               Iterator>::iterator_category> or
				              not json_details::is_std_allocator<Alloc>::value ) {
					return std::vector<T, Alloc>( first, last, alloc );
				} else {
					constexpr auto reserve_amount = 4096U / ( sizeof( T ) * 8U );
					auto result = std::vector<T, Alloc>( alloc );
					// Lets use a WAG and go for a 4k page size
					result.reserve( reserve_amount );
					result.assign( first, last );
					return result;
				}
			}
		};

		/***
		 * Auto generated constructor for nullable types.
		 * Specializations must accept accept an operator( )( ) that signifies a
		 * JSON null. Any other arguments only need to be valid to construct the
		 * type.
		 */
		template<typename T>
		struct nullable_constructor : default_constructor<T> {};

		template<typename T>
		struct nullable_constructor<std::optional<T>> {
			using value_type = T;

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::optional<T>
			operator( )( ) const noexcept {
				return std::optional<T>( );
			}

			template<typename... Args>
			[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr auto
			operator( )( Args &&...args ) const
			  noexcept( std::is_nothrow_constructible<
			            std::optional<T>, std::in_place_t, Args...>::value )
			    -> std::enable_if_t<
			      ( ( sizeof...( Args ) > 0 ) and
			        std::is_constructible<T, std::in_place_t, Args...>::value ),
			      std::optional<T>> {

				return std::optional<T>( std::in_place, DAW_FWD2( Args, args )... );
			}

			template<typename... Args>
			[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr auto
			operator( )( Args &&...args ) const noexcept(
			  std::conjunction<traits::is_nothrow_list_constructible<T, Args...>,
			                   std::is_nothrow_move_constructible<T>>::value )
			  -> std::enable_if_t<
			    ( ( sizeof...( Args ) > 0 ) and
			      not std::is_constructible<T, std::in_place_t, Args...>::value and
			      traits::is_list_constructible<T, Args...>::value ),
			    std::optional<T>> {

				return std::optional<T>( T{ DAW_FWD2( Args, args )... } );
			}
		};

		template<typename T, typename Deleter>
		struct nullable_constructor<std::unique_ptr<T, Deleter>> {
			using value_type = T;

			DAW_ATTRIB_INLINE inline constexpr std::unique_ptr<T, Deleter>
			operator( )( ) const noexcept {
				return std::unique_ptr<T, Deleter>{ };
			}

			template<typename... Args>
			[[nodiscard]] DAW_ATTRIB_INLINE inline auto
			operator( )( Args &&...args ) const
			  noexcept( std::is_nothrow_constructible<T, Args...>::value )
			    -> std::enable_if_t<( sizeof...( Args ) > 0 and
			                          std::is_constructible<T, Args...>::value ),
			                        std::unique_ptr<T, Deleter>> {

				return std::unique_ptr<T, Deleter>(
				  new T( DAW_FWD2( Args, args )... ) );
			}

			template<typename... Args>
			[[nodiscard]] DAW_ATTRIB_INLINE inline auto
			operator( )( Args &&...args ) const
			  noexcept( traits::is_nothrow_list_constructible<T, Args...>::value )
			    -> std::enable_if_t<
			      ( ( sizeof...( Args ) > 0 ) and
			        not std::is_constructible<T, Args...>::value and
			        traits::is_list_constructible<T, Args...>::value ),
			      std::unique_ptr<T, Deleter>> {

				return std::unique_ptr<T, Deleter>(
				  new T{ DAW_FWD2( Args, args )... } );
			}
		};

		/***
		 * Can use the fast, pseudo random string iterators.  They are
		 * InputIterators with an operator- that allows for O(1) distance
		 * calculations as we often know the length but cannot provide random
		 * access.  For types that only use InputIterator operations and last -
		 * first for distance calc
		 */
		template<typename>
		struct can_single_allocation_string : std::false_type {};

		template<typename Char, typename CharTrait, typename Allocator>
		struct can_single_allocation_string<
		  std::basic_string<Char, CharTrait, Allocator>> : std::true_type {};

		namespace json_details DAW_ATTRIB_HIDDEN {
			template<typename T>
			using json_type_t = typename T::i_am_a_json_type;

			template<typename T>
			using is_a_json_type =
			  std::bool_constant<daw::is_detected_v<json_type_t, T>>;

			template<typename T>
			inline constexpr bool is_a_json_type_v = is_a_json_type<T>::value;

			template<typename T>
			struct ensure_json_type : std::true_type {
				static_assert( is_a_json_type_v<T> );
			};

			template<typename T>
			using ordered_member_t = typename T::i_am_an_ordered_member;

			template<typename T>
			using is_an_ordered_member = daw::is_detected<ordered_member_t, T>;

			template<typename T>
			inline constexpr bool is_an_ordered_member_v =
			  is_an_ordered_member<T>::value;

			template<typename T>
			using is_a_json_tagged_variant_test =
			  typename T::i_am_a_json_tagged_variant;

			template<typename T>
			using is_a_json_tagged_variant =
			  daw::is_detected<is_a_json_tagged_variant_test, T>;

			template<typename T>
			inline constexpr bool is_a_json_tagged_variant_v =
			  is_a_json_tagged_variant<T>::value;

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
			    T, daw::is_detected<json_data_contract_trait_t, T>::value,
			    daw::is_detected<has_json_data_constract_constructor_test, T>::value>;

			template<typename T>
			using json_data_contract_constructor_t =
			  typename json_data_contract_constructor<T>::type;

			// DAW 20210801
			/*template<typename T, typename Default>
			using json_class_constructor_t = typename std::conditional_t<
			  daw::is_detected<json_data_contract_constructor_t, T>::value,
			  json_data_contract_constructor<T>,
			  daw::traits::identity<Default>>::type;*/

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
			using is_string_view_like = std::conjunction<
			  daw::is_detected<is_string_like_impl::has_data_test, T>,
			  daw::is_detected<is_string_like_impl::has_size_test, T>>;

			template<typename T>
			inline constexpr bool is_string_view_like_v =
			  is_string_view_like<T>::value;

			static_assert( is_string_view_like_v<std::string_view> );

		} // namespace json_details

		/***
		 * Trait for passively exploiting the zero termination when the type
		 * guarantees it.
		 */
		template<typename>
		struct is_zero_terminated_string : std::false_type {};

		template<typename CharT, typename Traits, typename Alloc>
		struct is_zero_terminated_string<std::basic_string<CharT, Traits, Alloc>>
		  : std::true_type {};

		template<typename T>
		inline constexpr bool is_zero_terminated_string_v =
		  is_zero_terminated_string<T>::value;

		namespace json_details DAW_ATTRIB_HIDDEN {
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
			  ParsePolicy::allow_temporarily_mutating_buffer,
			  std::conditional_t<is_mutable_string_v<String>,
			                     apply_policy_option_t<ParsePolicy, OptionMutable>,
			                     apply_policy_option_t<ParsePolicy, OptionImmutable>>,
			  std::conditional_t<
			    (is_rvalue_string<String> and is_mutable_string_v<String>),
			    apply_policy_option_t<ParsePolicy, OptionMutable>,
			    apply_policy_option_t<ParsePolicy, OptionImmutable>>>;
		} // namespace json_details

		/***
		 * A trait to specify that this class, when parsed, will describe all
		 * members of the JSON object. Anything not mapped is an error.
		 * Either specialize the class daw::json::is_exact_class_mapping, the
		 * variable is_exact_class_mapping_v, or have a type in your
		 * json_data_contract named exact_class_mapping for your type
		 */
		template<typename>
		struct is_exact_class_mapping : std::false_type {};

		/***
		 * Ignore unknown members trait allows the parser to skip unknown members
		 * when the default is exact
		 */
		template<typename>
		struct ignore_unknown_members : std::false_type {};

		namespace json_details DAW_ATTRIB_HIDDEN {
			template<typename T>
			using has_exact_mapping_trait_in_class_map =
			  typename json_data_contract<T>::exact_class_mapping;

			template<typename T>
			using has_ignore_unknown_members_trait_in_class_map =
			  typename json_data_contract<T>::ignore_unknown_members;
		} // namespace json_details

		template<typename T>
		inline constexpr bool ignore_unknown_members_v = std::disjunction<
		  ignore_unknown_members<T>,
		  daw::is_detected<
		    json_details::has_ignore_unknown_members_trait_in_class_map, T>>::value;

		/***
		 * A trait to specify that this class, when parsed, will describe all
		 * members of the JSON object. Anything not mapped is an error.
		 * Either specialize the class daw::json::is_exact_class_mapping, the
		 * variable is_exact_class_mapping_v, or have a type in your
		 * json_data_contract named exact_class_mapping for your type
		 */
		template<typename T>
		inline constexpr bool is_exact_class_mapping_v = std::disjunction<
		  is_exact_class_mapping<T>,
		  daw::is_detected<json_details::has_exact_mapping_trait_in_class_map,
		                   T>>::value;

		namespace json_details DAW_ATTRIB_HIDDEN {
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
		struct is_pointer_like
		  : std::disjunction<std::is_pointer<T>,
		                     json_details::has_element_type<T>> {};

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

		namespace json_details DAW_ATTRIB_HIDDEN {
			template<typename T>
			using tuple_test = typename tuple_elements_pack<T>::type;

			/// Detect if T follows the tuple protocol
			/// \tparam T
			template<typename T>
			using is_tuple = daw::is_detected<json_details::tuple_test, T>;

			template<typename T>
			inline constexpr bool is_tuple_v = is_tuple<T>::value;

			namespace deref_t_impl {
				template<typename>
				using unable_to_dereference = void;

				template<typename T>
				using deref_type_detect = DAW_TYPEOF( *std::declval<T>( ) );

				template<typename T>
				struct deref_type_impl {
					using type =
					  daw::detected_or_t<unable_to_dereference<T>, deref_type_detect, T>;
				};

				template<typename T>
				struct deref_type_impl<T *> {
					using type = std::remove_cv_t<T>;
				};

				template<typename T>
				struct deref_type_impl<std::unique_ptr<T>> {
					using type = std::remove_cv_t<T>;
				};

				template<typename T>
				struct deref_type_impl<std::optional<T>> {
					using type = std::remove_cv_t<T>;
				};
			} // namespace deref_t_impl
			template<typename T>
			using dereferenced_t = typename deref_t_impl::deref_type_impl<T>::type;

			template<typename T>
			using is_dereferenceable =
			  daw::not_trait<std::is_same<dereferenced_t<T>,
			                              deref_t_impl::unable_to_dereference<T>>>;

			template<typename T>
			inline constexpr bool is_dereferenceable_v = is_dereferenceable<T>::value;

			namespace unwrapped_impl {
				template<typename T>
				using unwrapped_t_impl =
				  std::conditional_t<is_dereferenceable_v<T>, dereferenced_t<T>, T>;
			}

			template<typename T, JsonNullable Nullable>
			using unwrapped_t =
			  typename std::conditional_t<is_nullable_json_value_v<Nullable>,
			                              unwrapped_impl::unwrapped_t_impl<T>, T>;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
