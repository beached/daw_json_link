// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../../daw_allocator_construct.h"
#include "daw_json_assert.h"
#include "daw_json_enums.h"
#include "daw_json_exec_modes.h"
#include "daw_json_name.h"
#include "daw_json_option_bits.h"
#include "daw_json_traits.h"
#include "daw_json_type_options.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_fwd_pack_apply.h>
#include <daw/daw_move.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {

		struct json_deduce_type;

		template<typename ParseState>
		class basic_json_value;

		namespace json_details {
			template<typename T>
			using ordered_member_subtype_test = typename T::json_member;

			template<typename T>
			using ordered_member_subtype_t =
			  typename daw::detected_or_t<T, ordered_member_subtype_test, T>;

			template<typename T, typename Default>
			inline constexpr auto json_class_constructor =
			  json_class_constructor_t<T, Default>{ };

			template<typename Value, typename Constructor, typename ParseState,
			         typename... Args>
			[[maybe_unused]] DAW_ATTRIB_FLATINLINE static inline constexpr auto
			construct_value( template_params<Value, Constructor>,
			                 ParseState &parse_state, Args &&...args ) {
				// Silence MSVC warning, used in other if constexpr case
				(void)parse_state;
				if constexpr( ParseState::has_allocator ) {
					auto alloc = parse_state.get_allocator_for( template_arg<Value> );
					return daw::try_alloc_construct<Value, Constructor>(
					  DAW_MOVE( alloc ), DAW_FWD( args )... );
				} else {
					static_assert(
					  std::is_invocable_v<Constructor, Args...>,
					  "Unable to construct value with the supplied arguments" );
					return Constructor{ }( DAW_FWD2( Args, args )... );
				}
			}

			template<typename Constructor>
			struct construct_value_tp_invoke_t {
				template<typename... TArgs, std::size_t... Is>
				DAW_ATTRIB_FLATINLINE inline constexpr decltype( auto )
				operator( )( fwd_pack<TArgs...> &&tp,
				             std::index_sequence<Is...> ) const {
					return Constructor{ }( get<Is>( DAW_MOVE( tp ) )... );
				}

				template<typename... TArgs, typename Allocator, std::size_t... Is>
				DAW_ATTRIB_FLATINLINE inline constexpr decltype( auto )
				operator( )( fwd_pack<TArgs...> &&tp, Allocator &alloc,
				             std::index_sequence<Is...> ) const {
					return Constructor{ }( get<Is>( DAW_MOVE( tp ) )...,
					                       DAW_FWD2( Allocator, alloc ) );
				}

				template<typename Alloc, typename... TArgs, std::size_t... Is>
				DAW_ATTRIB_FLATINLINE inline constexpr decltype( auto )
				operator( )( std::allocator_arg_t, Alloc &&alloc,
				             fwd_pack<TArgs...> &&tp,
				             std::index_sequence<Is...> ) const {

					return Constructor{ }( std::allocator_arg, DAW_FWD2( Alloc, alloc ),
					                       get<Is>( DAW_MOVE( tp ) )... );
				}
			};

			template<typename Constructor>
			inline constexpr auto construct_value_tp_invoke =
			  construct_value_tp_invoke_t<Constructor>{ };

			template<typename Value, typename Constructor, typename ParseState,
			         typename... Args>
			[[maybe_unused]] DAW_ATTRIB_FLATINLINE static inline constexpr auto
			construct_value_tp( ParseState &parse_state,
			                    fwd_pack<Args...> &&tp_args ) {
				if constexpr( ParseState::has_allocator ) {
					using alloc_t =
					  typename ParseState::template allocator_type_as<Value>;
					auto alloc = parse_state.get_allocator_for( template_arg<Value> );
					if constexpr( std::is_invocable_v<Constructor, Args..., alloc_t> ) {
						return construct_value_tp_invoke<Constructor>(
						  DAW_MOVE( tp_args ), DAW_MOVE( alloc ),
						  std::index_sequence_for<Args...>{ } );
					} else if constexpr( std::is_invocable_v<Constructor,
					                                         std::allocator_arg_t,
					                                         alloc_t, Args...> ) {
						return construct_value_tp_invoke<Constructor>(
						  std::allocator_arg, DAW_MOVE( alloc ), DAW_MOVE( tp_args ),
						  std::index_sequence_for<Args...>{ } );
					} else {
						static_assert(
						  std::is_invocable_v<Constructor, Args...>,
						  "Unable to construct value with the supplied arguments" );
						return construct_value_tp_invoke<Constructor>(
						  DAW_MOVE( tp_args ), std::index_sequence_for<Args...>{ } );
					}
				} else {
					// Silence MSVC warning, used in other if constexpr case
					(void)parse_state;
					static_assert(
					  std::is_invocable_v<Constructor, Args...>,
					  "Unable to construct value with the supplied arguments" );
					return construct_value_tp_invoke<Constructor>(
					  DAW_MOVE( tp_args ), std::index_sequence_for<Args...>{ } );
				}
			}

			template<typename T>
			inline constexpr bool has_json_data_contract_trait_v =
			  not std::is_same_v<missing_json_data_contract_for<T>,
			                     json_data_contract_trait_t<T>>;

			template<typename T>
			using has_json_data_contract_trait =
			  std::bool_constant<has_json_data_contract_trait_v<T>>;

			template<typename JsonMember>
			using json_result = typename JsonMember::parse_to_t;

			template<typename JsonMember>
			using json_base_type = typename JsonMember::base_type;

			template<typename T>
			using json_parser_to_json_data_t =
			  decltype( json_data_contract<T>::to_json_data( std::declval<T &>( ) ) );

			template<typename T>
			inline constexpr bool has_json_to_json_data_v =
			  daw::is_detected_v<json_parser_to_json_data_t, T>;

			template<typename T>
			using is_submember_tagged_variant_t =
			  typename json_data_contract<T>::type::i_am_a_submember_tagged_variant;

			template<typename T>
			inline constexpr bool is_submember_tagged_variant_v =
			  daw::is_detected_v<is_submember_tagged_variant_t, T>;

			template<typename T>
			using is_json_nullable_test = typename T::i_am_a_json_nullable;

			template<typename T>
			inline constexpr bool is_json_nullable_v =
			  daw::is_detected_v<is_json_nullable_test, T>;

			template<typename T>
			using json_nullable_member_type_t = typename T::member_type;

			/***
			 * Helpers to set options on json_ types
			 */

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t class_opts_set =
			  set_bits( class_opts, CurrentOptions, option, options... );

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t number_opts_set =
			  set_bits( number_opts, CurrentOptions, option, options... );

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t bool_opts_set =
			  set_bits( bool_opts, CurrentOptions, option, options... );

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t string_opts_set =
			  set_bits( string_opts, CurrentOptions, option, options... );

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t string_raw_opts_set =
			  set_bits( string_raw_opts, CurrentOptions, option, options... );

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t json_custom_opts_set =
			  set_bits( json_custom_opts, CurrentOptions, option, options... );

			template<json_options_t CurrentOptions, auto option, auto... options>
			inline constexpr json_options_t tuple_opts_set =
			  set_bits( tuple_opts, CurrentOptions, option, options... );
		} // namespace json_details

		namespace json_base {
			/// @brief Mark a member as nullable
			/// @tparam T type of the value being mapped to(e.g. std::optional<Foo>)
			/// @tparam JsonMember Json Type or type of value when present, deduced
			/// from T if not specified
			/// @tparam Constructor Specify a Constructor type or use
			/// the default nullable_constructor<T>
			template<typename T, typename Constructor = nullable_constructor<T>,
			         typename JsonMember = deduced_type>
			struct json_nullable;

			template<typename T, typename Constructor = default_constructor<T>,
			         json_details::json_options_t Options = class_opts_def>
			struct json_class;

			template<typename T, typename Constructor = nullable_constructor<T>,
			         json_details::json_options_t Options = class_opts_def>
			using json_class_null = json_nullable<
			  T, Constructor,
			  json_class<json_details::unwrapped_t<T>,
			             default_constructor<json_details::unwrapped_t<T>>, Options>>;

			template<typename JsonElement, typename Container,
			         typename Constructor = default_constructor<Container>>
			struct json_array;

			template<typename T, typename FromJsonConverter, typename ToJsonConverter,
			         json_details::json_options_t Options>
			struct json_custom;

			template<typename Variant, typename JsonElements, typename Constructor>
			struct json_variant;

			template<typename T, typename TagMember, typename Switcher,
			         typename JsonElements, typename Constructor>
			struct json_tagged_variant;

			/** Link to a nullable JSON array
			 * @tparam Name name of JSON member to link to
			 * @tparam Container type of C++ container being constructed(e.g.
			 * vector<int>)
			 * @tparam JsonElement Json type being parsed e.g. json_number,
			 * json_string...
			 * @tparam Constructor A callable used to make Container,
			 * default will use the Containers constructor.  Both normal and aggregate
			 * are supported
			 */
			template<typename WrappedContainer,
			         typename Constructor = nullable_constructor<WrappedContainer>,
			         typename JsonElement = typename json_details::unwrapped_t<
			           WrappedContainer>::value_type>
			using json_array_null = json_nullable<
			  WrappedContainer, Constructor,
			  json_array<
			    JsonElement, json_details::unwrapped_t<WrappedContainer>,
			    default_constructor<json_details::unwrapped_t<WrappedContainer>>>>;

			template<typename T,
			         json_details::json_options_t Options = string_raw_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_string_raw;

			template<typename T,
			         json_details::json_options_t Options = string_raw_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_string_raw_null = json_nullable<
			  T, Constructor,
			  json_string_raw<
			    json_details::unwrapped_t<T>, Options,
			    default_constructor<json_string_raw<json_details::unwrapped_t<T>>>>>;

			template<typename T,
			         json_details::json_options_t Options = string_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_string;

			template<typename T,
			         json_details::json_options_t Options = string_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_string_null = json_nullable<
			  T, Constructor,
			  json_string<json_details::unwrapped_t<T>, Options,
			              default_constructor<json_details::unwrapped_t<T>>>>;

			template<typename T, json_details::json_options_t Options = bool_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_bool;

			template<typename T, json_details::json_options_t Options = bool_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_bool_null = json_nullable<
			  T, Constructor,
			  json_bool<json_details::unwrapped_t<T>, Options,
			            default_constructor<json_details::unwrapped_t<T>>>>;

			template<typename T, typename Constructor>
			struct json_date;

			template<typename T,
			         json_details::json_options_t Options = number_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_number;

			template<typename T,
			         json_details::json_options_t Options = number_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_number_null = json_nullable<
			  T, Constructor,
			  json_number<json_details::unwrapped_t<T>, Options,
			              default_constructor<json_details::unwrapped_t<T>>>>;

			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         typename Constructor = default_constructor<Container>>
			struct json_key_value;

			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         typename Constructor = nullable_constructor<Container>>
			using json_key_value_null = json_nullable<
			  Container, Constructor,
			  json_key_value<
			    json_details::unwrapped_t<Container>, JsonValueType, JsonKeyType,
			    default_constructor<json_details::unwrapped_t<Container>>>>;

			template<typename Tuple,
			         typename Constructor = default_constructor<Tuple>,
			         json_details::json_options_t Options = tuple_opts_def,
			         typename JsonTupleTypesList = json_deduce_type>
			struct json_tuple;

			template<typename Tuple,
			         typename Constructor = nullable_constructor<Tuple>,
			         json_details::json_options_t Options = class_opts_def,
			         typename JsonTupleTypesList = json_deduce_type>
			using json_tuple_null = json_nullable<
			  Tuple, Constructor,
			  json_tuple<json_details::unwrapped_t<Tuple>,
			             default_constructor<json_details::unwrapped_t<Tuple>>,
			             Options, JsonTupleTypesList>>;

			/***
			 * json_raw allows for raw JSON access to the member data. It requires a
			 * type that is constructable from (char const *, std::size_t) arguments
			 * and for serialization requires that it can be passed to
			 * std::begin/std::end and the iterator returned has a value_type of char
			 * @tparam T type to hold raw JSON data, defaults to json_value
			 * @tparam Constructor A callable used to construct T.
			 * @tparam Nullable Does the value have to exist in the document or can it
			 * have a null value
			 */
			template<typename T, typename Constructor = default_constructor<T>>
			struct json_raw;

			/***
			 * json_raw_null allows for raw JSON access to the nullable member data.
			 * It requires a type that is constructable from (char const *,
			 * std::size_t) arguments and for serialization requires that it can be
			 * passed to std::begin/std::end and the iterator returned has a
			 * value_type of char
			 * @tparam T type to hold raw JSON data, defaults to json_value
			 * @tparam Constructor A callable used to construct T.
			 */
			template<typename T, typename Constructor = nullable_constructor<T>>
			using json_raw_null = json_nullable<
			  T, Constructor,
			  json_raw<json_details::unwrapped_t<T>,
			           default_constructor<json_details::unwrapped_t<T>>>>;
		} // namespace json_base

		namespace json_details {
			template<typename T>
			inline constexpr JsonParseTypes number_parse_type_impl_v = [] {
				if constexpr( daw::is_floating_point_v<T> ) {
					return JsonParseTypes::Real;
				} else if constexpr( daw::is_signed_v<T> ) {
					return JsonParseTypes::Signed;
				} else {
					static_assert( daw::is_unsigned_v<T> );
					return JsonParseTypes::Unsigned;
				}
			}( );

			template<typename T>
			constexpr auto number_parse_type_test( )
			  -> std::enable_if_t<std::is_enum_v<T>, JsonParseTypes> {

				return number_parse_type_impl_v<std::underlying_type_t<T>>;
			}
			template<typename T>
			constexpr auto number_parse_type_test( )
			  -> std::enable_if_t<not std::is_enum_v<T>, JsonParseTypes> {

				return number_parse_type_impl_v<T>;
			}

			template<typename T>
			inline constexpr JsonParseTypes
			  number_parse_type_v = number_parse_type_test<T>( );

			template<typename, typename = void>
			struct json_deduced_type_map {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Unknown;

				static constexpr bool type_map_found = false;
			};

			template<typename JsonType>
			struct json_deduced_type_map<
			  JsonType, std::enable_if_t<is_a_json_type_v<JsonType>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Unknown;

				using type = JsonType;
				static constexpr bool type_map_found = true;
			};

			template<typename T>
			struct json_deduced_type_map<
			  T, std::enable_if_t<has_json_data_contract_trait_v<T>>> {
				static constexpr bool is_null = false;
				using type = typename json_data_contract<T>::type;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Unknown;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<daw::string_view> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::StringRaw;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<std::string_view> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::StringRaw;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<std::string> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type =
				  JsonParseTypes::StringEscaped;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<bool> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Bool;

				static constexpr bool type_map_found = true;
			};

			// libc++ has a non-conforming vector<bool>::const_reference as it isn't
			// bool https://bugs.llvm.org/show_bug.cgi?id=16077
#if defined( _LIBCPP_VERSION )
			template<>
			struct json_deduced_type_map<
			  typename std::vector<bool>::const_reference> {

				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Bool;

				static constexpr bool type_map_found = true;
			};
#endif

			template<typename Integer>
			struct json_deduced_type_map<
			  Integer, std::enable_if_t<
			             not json_details::has_json_data_contract_trait_v<Integer> and
			             daw::is_integral_v<Integer>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type =
				  daw::is_signed_v<Integer> ? JsonParseTypes::Signed
				                            : JsonParseTypes::Unsigned;

				static constexpr bool type_map_found = true;
			};

			template<typename Enum>
			struct json_deduced_type_map<
			  Enum, std::enable_if_t<
			          not json_details::has_json_data_contract_trait_v<Enum> and
			          std::is_enum_v<Enum>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type =
				  daw::is_signed_v<std::underlying_type<Enum>>
				    ? JsonParseTypes::Signed
				    : JsonParseTypes::Unsigned;

				static constexpr bool type_map_found = true;
			};

			template<typename FloatingPoint>
			struct json_deduced_type_map<
			  FloatingPoint,
			  std::enable_if_t<
			    not json_details::has_json_data_contract_trait_v<FloatingPoint> and
			    daw::is_floating_point_v<FloatingPoint>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Real;

				static constexpr bool type_map_found = true;
			};

			template<typename Tuple>
			struct json_deduced_type_map<
			  Tuple, std::enable_if_t<
			           not json_details::has_json_data_contract_trait_v<Tuple> and
			           is_tuple_v<Tuple>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Tuple;

				static constexpr bool type_map_found = true;
			};

			namespace container_detect {
				template<typename T>
				using container_value_type = typename T::value_type;

				template<typename Container>
				using container_begin_type =
				  decltype( std::begin( std::declval<Container const &>( ) ) );

				template<typename Container>
				using container_end_type =
				  decltype( std::end( std::declval<Container const &>( ) ) );

				template<typename AssociativeContainer>
				using container_key_type = typename AssociativeContainer::key_type;

				template<typename AssociativeContainer>
				using container_mapped_type =
				  typename AssociativeContainer::mapped_type;

				template<typename T>
				using is_value_type_char = std::is_same<char, container_value_type<T>>;

			} // namespace container_detect
			template<typename T>
			inline constexpr bool is_container_v =
			  daw::is_detected_v<container_detect::container_value_type, T>
			    and daw::is_detected_v<container_detect::container_begin_type, T>
			      and daw::is_detected_v<container_detect::container_end_type, T>;

			template<typename Container>
			using is_container = std::bool_constant<is_container_v<Container>>;

			template<typename String>
			using is_string =
			  std::conjunction<is_container<String>,
			                   container_detect::is_value_type_char<String>>;

			template<typename T>
			inline constexpr bool is_associative_container_v = std::conjunction_v<
			  is_container<T>,
			  daw::is_detected<container_detect::container_key_type, T>,
			  daw::is_detected<container_detect::container_mapped_type, T>>;

			template<typename T>
			using is_associative_container =
			  std::bool_constant<is_associative_container_v<T>>;

			template<typename AssociativeContainer>
			struct json_deduced_type_map<
			  AssociativeContainer,
			  std::enable_if_t<std::conjunction_v<
			    not_trait<has_json_data_contract_trait<AssociativeContainer>>,
			    is_associative_container<AssociativeContainer>>>> {

				static constexpr bool is_null = false;
				using key = typename AssociativeContainer::key_type;
				using value = typename AssociativeContainer::mapped_type;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::KeyValue;

				static constexpr bool type_map_found = true;
			};

			template<typename Container>
			struct json_deduced_type_map<
			  Container, std::enable_if_t<std::conjunction_v<
			               not_trait<has_json_data_contract_trait<Container>>,
			               not_trait<is_associative_container<Container>>,
			               json_details::is_container<Container>,
			               not_trait<is_string<Container>>>>> {

				static constexpr bool is_null = false;
				using value = typename Container::value_type;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Array;

				static constexpr bool type_map_found = true;
			};

			template<typename T>
			struct json_deduced_type_map<
			  T, std::enable_if_t<is_readable_value_v<T> and
			                      not has_json_data_contract_trait_v<T> and
			                      daw::is_detected_v<json_deduced_type_map,
			                                         readable_value_type_t<T>>>> {

				static constexpr bool is_null = true;
				using sub_type = readable_value_type_t<T>;
				using type = json_deduced_type_map<sub_type>;
				static constexpr JsonParseTypes parse_type = type::parse_type;
				static constexpr bool type_map_found = true;
			};

			template<typename T>
			inline constexpr bool has_deduced_type_mapping_v =
			  json_deduced_type_map<T>::type_map_found;

			template<typename T>
			using has_deduced_type_mapping =
			  bool_constant<has_deduced_type_mapping_v<T>>;

			template<typename Mapped, bool Found = true>
			struct json_link_quick_map_type : std::bool_constant<Found> {
				using mapped_type = Mapped;
			};

			template<JsonParseTypes Value>
			inline constexpr bool is_arithmetic_parse_type_v =
			  daw::traits::equal_to_any_of_v<Value, JsonParseTypes::Signed,
			                                 JsonParseTypes::Unsigned,
			                                 JsonParseTypes::Real>;

			template<typename T>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL auto json_link_quick_map( ) noexcept {
				if constexpr( is_a_json_type_v<T> ) {
					return json_link_quick_map_type<T>{ };
				} else if constexpr( has_deduced_type_mapping_v<T> ) {
					using mapped_type_t = json_deduced_type_map<T>;
					using parse_type = daw::constant<mapped_type_t::parse_type>;
					using is_null = daw::constant<mapped_type_t::is_null>;
					if constexpr( parse_type::value == JsonParseTypes::Unknown ) {
						if constexpr( is_null::value ) {
							if constexpr( has_json_data_contract_trait_v<
							                typename mapped_type_t::sub_type> ) {
								return json_link_quick_map_type<
								  json_base::json_class_null<T>>{ };
							} else {
								return json_link_quick_map_type<json_base::json_raw_null<T>>{ };
							}
						} else {
							return json_link_quick_map_type<json_base::json_raw<T>>{ };
						}
					} else if constexpr( parse_type::value ==
					                     JsonParseTypes::StringRaw ) {
						if constexpr( is_null::value ) {
							return json_link_quick_map_type<
							  json_base::json_string_raw_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_string_raw<T>>{ };
						}
					} else if constexpr( parse_type::value ==
					                     JsonParseTypes::StringEscaped ) {
						if constexpr( is_null::value ) {
							return json_link_quick_map_type<
							  json_base::json_string_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_string<T>>{ };
						}
					} else if constexpr( parse_type::value == JsonParseTypes::Bool ) {
						if constexpr( is_null::value ) {
							return json_link_quick_map_type<json_base::json_bool_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_bool<T>>{ };
						}
					} else if constexpr( is_arithmetic_parse_type_v<parse_type::value> ) {
						if constexpr( is_null::value ) {
							return json_link_quick_map_type<
							  json_base::json_number_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_number<T>>{ };
						}
					} else if constexpr( parse_type::value == JsonParseTypes::Tuple ) {
						if constexpr( is_null::value ) {
							return json_link_quick_map_type<json_base::json_tuple_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_tuple<T>>{ };
						}
					} else if constexpr( parse_type::value == JsonParseTypes::KeyValue ) {
						if constexpr( is_null::value ) {
							using b_t = typename mapped_type_t::base_type;
							using k_t = typename b_t::key;
							using v_t = typename b_t::value;
							return json_link_quick_map_type<
							  json_base::json_key_value_null<T, v_t, k_t>>{ };
						} else {
							using k_t = typename mapped_type_t::key;
							using v_t = typename mapped_type_t::value;
							return json_link_quick_map_type<
							  json_base::json_key_value<T, v_t, k_t>>{ };
						}
					} else if constexpr( parse_type::value == JsonParseTypes::Array ) {
						if constexpr( is_null::value ) {
							using b_t = typename mapped_type_t::base_type;
							using v_t = typename b_t::value;
							return json_link_quick_map_type<
							  json_base::json_array_null<v_t, T>>{ };
						} else {
							using v_t = typename mapped_type_t::value;
							return json_link_quick_map_type<json_base::json_array<v_t, T>>{ };
						}
					} else {
						return json_link_quick_map_type<void, false>{ };
					}
				} else {
					return json_link_quick_map_type<void, false>{ };
				}
			}

			/***
			 * Check if the current type has a quick map specialized for it
			 */
			template<typename T>
			inline constexpr bool has_json_link_quick_map_v =
			  decltype( json_link_quick_map<T>( ) )::value;

			/***
			 * Get the quick mapped json type for type T
			 */
			template<typename T>
			using json_link_quick_map_t =
			  typename decltype( json_link_quick_map<T>( ) )::mapped_type;

			template<typename JsonType>
			struct json_class_map_type {
				using type = typename json_data_contract_trait_t<JsonType>::json_member;
			};

			template<typename>
			struct is_json_class_map : std::false_type {};

			template<typename T>
			inline constexpr bool is_json_class_map_v =
			  std::conjunction_v<has_json_data_contract_trait<T>,
			                     is_json_class_map<json_data_contract_trait_t<T>>>;

			template<typename T>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL auto json_deduced_type_impl( ) noexcept {
				if constexpr( is_an_ordered_member_v<T> ) {
					using type = T;
					return daw::traits::identity<type>{ };
				} else if constexpr( has_json_data_contract_trait_v<T> ) {
					static_assert( not std::is_same_v<T, void> );

					using type = json_base::json_class<
					  T, json_class_constructor_t<T, default_constructor<T>>>;

					static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<remove_cvref_t<type>>,
					               "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( has_json_link_quick_map_v<T> ) {
					static_assert( not std::is_same_v<T, void> );
					using type = json_link_quick_map_t<T>;
					using rcvref_type = remove_cvref_t<type>;
					static_assert( not std::is_same_v<rcvref_type, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<rcvref_type>, "Detection failure" );
					static_assert( not std::is_same_v<rcvref_type, void>,
					               "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( json_details::is_a_json_type_v<T> ) {
					static_assert( not std::is_same_v<T, void> );
					using type =
					  typename std::conditional_t<is_json_class_map_v<T>,
					                              json_class_map_type<T>,
					                              daw::traits::identity<T>>::type;
					static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<remove_cvref_t<type>>,
					               "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( is_container_v<T> ) {
					static_assert( not std::is_same_v<T, void> );
					using type = json_base::json_array<typename T::value_type, T>;
					static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<remove_cvref_t<type>>,
					               "Detection failure" );
					static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
					               "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( is_readable_value_v<T> ) {
					using value_type = readable_value_type_t<T>;
					using sub_type =
					  typename decltype( json_deduced_type_impl<value_type>( ) )::type;
					using type =
					  json_base::json_nullable<T, nullable_constructor<T>, sub_type>;
					return daw::traits::identity<type>{ };
				} else {
					static_assert( is_readable_value_v<T> );
					using type = missing_json_data_contract_for<T>;
					return daw::traits::identity<type>{ };
				}
			}

			template<typename T>
			using json_deduced_type =
			  typename DAW_TYPEOF( json_deduced_type_impl<T>( ) )::type;

			/***
			 * Some types cannot use construct_value or dont needed it.
			 */
			template<typename ParsePolicy, typename JsonMember>
			inline constexpr bool use_direct_construction_v =
			  ParsePolicy::exec_tag_t::always_rvo;
			// TODO DAW implement this so that it work
			/* or
			  ( not ParsePolicy::has_allocator and
			    is_default_constructor_v<
			      typename json_deduced_type<JsonMember>::constructor_t> );*/

			template<typename T>
			inline constexpr bool has_json_deduced_type_v =
			  not std::is_same_v<json_deduced_type<T>,
			                     missing_json_data_contract_for<T>>;

			template<typename T>
			using has_json_deduced_type =
			  std::bool_constant<has_json_deduced_type_v<T>>;

			template<typename T>
			inline constexpr bool has_unnamed_default_type_mapping_v =
			  has_json_deduced_type_v<T>;

			template<typename JsonMember>
			using from_json_result_t = json_result<json_deduced_type<JsonMember>>;

			template<typename Constructor, typename... Members>
			using json_class_parse_result_impl2 = decltype(
			  Constructor{ }( std::declval<typename Members::parse_to_t &&>( )... ) );

			template<typename Constructor, typename... Members>
			using json_class_parse_result_impl =
			  daw::detected_t<json_class_parse_result_impl2, Constructor, Members...>;

			template<typename Constructor, typename... Members>
			using json_class_parse_result_t = daw::remove_cvref_t<
			  json_class_parse_result_impl<Constructor, Members...>>;

			template<typename JsonMember>
			using dependent_member_t = typename JsonMember::dependent_member;

			template<typename JsonMember>
			using nullable_dependent_member_t =
			  typename JsonMember::member_type::dependent_member;

			template<typename JsonMember, typename = void>
			inline constexpr bool has_dependent_member_v =
			  daw::is_detected_v<dependent_member_t, JsonMember>;

			template<typename JsonMember>
			inline constexpr bool has_dependent_member_v<
			  JsonMember, std::enable_if_t<is_json_nullable_v<JsonMember>>> =
			  daw::is_detected_v<nullable_dependent_member_t, JsonMember>;

			template<typename Constructor>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
			construct_nullable_empty( ) {
				if constexpr( std::is_invocable_v<Constructor,
				                                  construct_readable_empty_t> ) {
					return Constructor{ }( construct_readable_empty );
				} else {
					return Constructor{ }( );
				}
			}
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
