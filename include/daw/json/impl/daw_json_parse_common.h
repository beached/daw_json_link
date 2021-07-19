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
#include <string>
#include <string_view>
#include <vector>

#if defined( __cpp_constexpr_dynamic_alloc )
#define CPP20CONSTEXPR constexpr
#else
#define CPP20CONSTEXPR
#endif

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
					using alloc_t =
					  typename ParseState::template allocator_type_as<Value>;
					auto alloc = parse_state.get_allocator_for( template_arg<Value> );
					if constexpr( std::is_invocable<Constructor, Args...,
					                                alloc_t>::value ) {
						return Constructor{ }( DAW_FWD2( Args, args )...,
						                       DAW_MOVE( alloc ) );
					} else if constexpr( daw::traits::is_callable_v<Constructor,
					                                                std::allocator_arg_t,
					                                                alloc_t, Args...> ) {
						return Constructor{ }( std::allocator_arg, DAW_MOVE( alloc ),
						                       DAW_FWD2( Args, args )... );
					} else {
						static_assert(
						  std::is_invocable<Constructor, Args...>::value,
						  "Unable to construct value with the supplied arguments" );
						return Constructor{ }( DAW_FWD2( Args, args )... );
					}
				} else {
					static_assert(
					  std::is_invocable<Constructor, Args...>::value,
					  "Unable to construct value with the supplied arguments" );
					if constexpr( std::is_invocable<Constructor, Args...>::value ) {
						return Constructor{ }( DAW_FWD2( Args, args )... );
					}
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
					if constexpr( std::is_invocable<Constructor, Args...,
					                                alloc_t>::value ) {
						return construct_value_tp_invoke<Constructor>(
						  DAW_MOVE( tp_args ), DAW_MOVE( alloc ),
						  std::index_sequence_for<Args...>{ } );
					} else if constexpr( daw::traits::is_callable_v<Constructor,
					                                                std::allocator_arg_t,
					                                                alloc_t, Args...> ) {
						return construct_value_tp_invoke<Constructor>(
						  std::allocator_arg, DAW_MOVE( alloc ), DAW_MOVE( tp_args ),
						  std::index_sequence_for<Args...>{ } );
					} else {
						static_assert(
						  std::is_invocable<Constructor, Args...>::value,
						  "Unable to construct value with the supplied arguments" );
						return construct_value_tp_invoke<Constructor>(
						  DAW_MOVE( tp_args ), std::index_sequence_for<Args...>{ } );
					}
				} else {
					// Silence MSVC warning, used in other if constexpr case
					(void)parse_state;
					static_assert(
					  std::is_invocable<Constructor, Args...>::value,
					  "Unable to construct value with the supplied arguments" );
					return construct_value_tp_invoke<Constructor>(
					  DAW_MOVE( tp_args ), std::index_sequence_for<Args...>{ } );
				}
			}

			template<typename Allocator>
			using has_allocate_test =
			  decltype( std::declval<Allocator &>( ).allocate( size_t{ 1 } ) );

			template<typename Allocator>
			using has_deallocate_test =
			  decltype( std::declval<Allocator &>( ).deallocate(
			    static_cast<void *>( nullptr ), size_t{ 1 } ) );

			template<typename Allocator>
			inline constexpr bool is_allocator_v = std::conjunction<
			  daw::is_detected<has_allocate_test, Allocator>,
			  daw::is_detected<has_deallocate_test, Allocator>>::value;

			template<typename T>
			using has_json_data_contract_trait =
			  not_trait<std::is_same<missing_json_data_contract_for<T>,
			                         json_data_contract_trait_t<T>>>;
			template<typename T>
			inline constexpr bool has_json_data_contract_trait_v =
			  has_json_data_contract_trait<T>::value;

			template<typename Container, typename Value>
			using detect_push_back = decltype(
			  std::declval<Container &>( ).push_back( std::declval<Value>( ) ) );

			template<typename Container, typename Value>
			using detect_insert_end = decltype( std::declval<Container &>( ).insert(
			  std::end( std::declval<Container &>( ) ), std::declval<Value>( ) ) );

			template<typename Container, typename Value>
			inline constexpr bool has_push_back_v =
			  daw::is_detected<detect_push_back, Container, Value>::value;

			template<typename Container, typename Value>
			inline constexpr bool has_insert_end_v =
			  daw::is_detected<detect_insert_end, Container, Value>::value;

			template<typename JsonMember>
			using json_result = typename JsonMember::parse_to_t;

			template<typename JsonMember>
			using json_base_type = typename JsonMember::base_type;

			template<typename Container>
			struct basic_appender {
				using value_type = typename Container::value_type;
				using reference = value_type;
				using pointer = value_type const *;
				using difference_type = std::ptrdiff_t;
				using iterator_category = std::output_iterator_tag;
				Container *m_container;

				explicit inline constexpr basic_appender( Container &container )
				  : m_container( &container ) {}

				template<typename Value>
				DAW_ATTRIB_FLATINLINE inline constexpr void
				operator( )( Value &&value ) const {
					if constexpr( has_push_back_v<Container,
					                              daw::remove_cvref_t<Value>> ) {
						m_container->push_back( DAW_FWD2( Value, value ) );
					} else if constexpr( has_insert_end_v<Container,
					                                      daw::remove_cvref_t<Value>> ) {
						m_container->insert( std::end( *m_container ),
						                     DAW_FWD2( Value, value ) );
					} else {
						static_assert(
						  has_push_back_v<Container, daw::remove_cvref_t<Value>> or
						    has_insert_end_v<Container, daw::remove_cvref_t<Value>>,
						  "basic_appender requires a Container that either has push_back "
						  "or "
						  "insert with the end iterator as first argument" );
					}
				}

				template<
				  typename Value,
				  std::enable_if_t<
				    not std::is_same<basic_appender, daw::remove_cvref_t<Value>>::value,
				    std::nullptr_t> = nullptr>
				inline constexpr basic_appender &operator=( Value &&v ) {
					operator( )( DAW_FWD2( Value, v ) );
					return *this;
				}

				inline constexpr basic_appender &operator++( ) {
					return *this;
				}

				inline constexpr basic_appender operator++( int ) {
					return *this;
				}

				inline constexpr basic_appender &operator*( ) {
					return *this;
				}
			};

			template<typename T>
			using json_parser_to_json_data_t =
			  decltype( json_data_contract<T>::to_json_data( std::declval<T &>( ) ) );

			template<typename T>
			inline constexpr bool has_json_to_json_data_v =
			  daw::is_detected<json_parser_to_json_data_t, T>::value;

			template<typename T>
			using is_submember_tagged_variant_t =
			  typename json_data_contract<T>::type::i_am_a_submember_tagged_variant;

			template<typename T>
			inline constexpr bool is_submember_tagged_variant_v =
			  daw::is_detected<is_submember_tagged_variant_t, T>::value;

			template<typename>
			inline constexpr std::size_t parse_space_needed_v = 1U;

			template<>
			inline constexpr std::size_t parse_space_needed_v<simd_exec_tag> = 16U;

			template<typename JsonType>
			using is_json_nullable =
			  std::bool_constant<JsonType::expected_type == JsonParseTypes::Null>;

			template<typename JsonType>
			static inline constexpr bool is_json_nullable_v =
			  is_json_nullable<JsonType>::value;

			template<typename T>
			using dereffed_type_impl =
			  daw::remove_cvref_t<decltype( *( std::declval<T &>( ) ) )>;

			template<typename T>
			using dereffed_type =
			  typename daw::detected_or<T, dereffed_type_impl, T>::type;

			template<typename T, JsonNullable Nullable>
			using unwrap_type =
			  typename std::conditional_t<is_nullable_json_value_v<Nullable>,
			                              dereffed_type<T>, T>;

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
			template<typename T, typename Constructor = default_constructor<T>,
			         json_details::json_options_t Options = class_opts_def>
			struct json_class;

			template<typename T, typename Constructor = nullable_constructor<T>,
			         json_details::json_options_t Options = class_opts_def>
			using json_class_null =
			  json_class<T, Constructor,
			             json_details::class_opts_set<Options, JsonNullDefault>>;

			template<typename JsonElement, typename Container,
			         typename Constructor = default_constructor<Container>,
			         JsonNullable Nullable = JsonNullable::MustExist>
			struct json_array;

			template<typename JsonElement, typename Container,
			         typename Constructor = nullable_constructor<Container>>
			using json_array_null =
			  json_array<JsonElement, Container, Constructor, JsonNullDefault>;

			template<typename T,
			         json_details::json_options_t Options = string_raw_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_string_raw;

			template<typename T,
			         json_details::json_options_t Options = string_raw_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_string_raw_null = json_string_raw<
			  T, json_details::string_raw_opts_set<Options, JsonNullDefault>,
			  Constructor>;

			template<typename T,
			         json_details::json_options_t Options = string_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_string;

			template<typename T,
			         json_details::json_options_t Options = string_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_string_null =
			  json_string<T, json_details::string_opts_set<Options, JsonNullDefault>,
			              Constructor>;

			template<typename T, json_details::json_options_t Options = bool_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_bool;

			template<typename T, json_details::json_options_t Options = bool_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_bool_null =
			  json_bool<T, json_details::bool_opts_set<Options, JsonNullDefault>,
			            Constructor>;

			template<typename T,
			         json_details::json_options_t Options = number_opts_def,
			         typename Constructor = default_constructor<T>>
			struct json_number;

			template<typename T,
			         json_details::json_options_t Options = number_opts_def,
			         typename Constructor = nullable_constructor<T>>
			using json_number_null =
			  json_number<T, json_details::number_opts_set<Options, JsonNullDefault>,
			              Constructor>;

			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         typename Constructor = default_constructor<Container>,
			         JsonNullable Nullable = JsonNullable::MustExist>
			struct json_key_value;

			template<typename Container, typename JsonValueType, typename JsonKeyType,
			         typename Constructor = nullable_constructor<Container>>
			using json_key_value_null =
			  json_key_value<Container, JsonValueType, JsonKeyType, Constructor,
			                 JsonNullDefault>;

			template<typename Tuple,
			         typename Constructor = default_constructor<Tuple>,
			         json_details::json_options_t Options = tuple_opts_def,
			         typename JsonTupleTypesList = json_deduce_type>
			struct json_tuple;

			template<typename Tuple,
			         typename Constructor = nullable_constructor<Tuple>,
			         json_details::json_options_t Options = class_opts_def,
			         typename JsonTupleTypesList = json_deduce_type>
			using json_tuple_null =
			  json_tuple<Tuple, Constructor,
			             json_details::tuple_opts_set<Options, JsonNullDefault>,
			             JsonTupleTypesList>;

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
			template<typename T, typename Constructor = default_constructor<T>,
			         JsonNullable Nullable = JsonNullable::MustExist>
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
			using json_raw_null = json_raw<T, Constructor, JsonNullDefault>;
		} // namespace json_base

		namespace json_details {
			template<typename T>
			using can_deref =
			  typename std::bool_constant<daw::is_detected_v<dereffed_type_impl, T>>;

			template<typename T>
			using cant_deref = daw::not_trait<can_deref<T>>;

			template<typename T>
			inline constexpr bool can_deref_v = can_deref<T>::value;

			template<typename T>
			inline constexpr JsonParseTypes number_parse_type_impl_v = [] {
				static_assert( daw::is_arithmetic<T>::value, "Unexpected non-number" );
				if constexpr( daw::is_floating_point<T>::value ) {
					return JsonParseTypes::Real;
				} else if constexpr( daw::is_signed<T>::value ) {
					return JsonParseTypes::Signed;
				} else if constexpr( daw::is_unsigned<T>::value ) {
					return JsonParseTypes::Unsigned;
				}
			}( );

			template<typename T>
			constexpr auto number_parse_type_test( )
			  -> std::enable_if_t<std::is_enum<T>::value, JsonParseTypes> {

				return number_parse_type_impl_v<std::underlying_type_t<T>>;
			}
			template<typename T>
			constexpr auto number_parse_type_test( )
			  -> std::enable_if_t<not std::is_enum<T>::value, JsonParseTypes> {

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
			  JsonType, std::enable_if_t<json_details::is_a_json_type_v<JsonType>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Unknown;

				using type = JsonType;
				static constexpr bool type_map_found = true;
			};

			template<typename T>
			struct json_deduced_type_map<
			  T, std::enable_if_t<json_details::has_json_data_contract_trait_v<T>>> {
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
			  Integer,
			  std::enable_if_t<std::conjunction_v<
			    not_trait<json_details::has_json_data_contract_trait<Integer>>,
			    daw::is_integral<Integer>>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type =
				  daw::is_signed_v<Integer> ? JsonParseTypes::Signed
				                            : JsonParseTypes::Unsigned;

				static constexpr bool type_map_found = true;
			};

			template<typename Enum>
			struct json_deduced_type_map<
			  Enum, std::enable_if_t<std::conjunction_v<
			          not_trait<json_details::has_json_data_contract_trait<Enum>>,
			          std::is_enum<Enum>>>> {
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
			  std::enable_if_t<std::conjunction_v<
			    not_trait<json_details::has_json_data_contract_trait<FloatingPoint>>,
			    daw::is_floating_point<FloatingPoint>>>> {
				static constexpr bool is_null = false;
				static constexpr JsonParseTypes parse_type = JsonParseTypes::Real;

				static constexpr bool type_map_found = true;
			};

			template<typename Tuple>
			struct json_deduced_type_map<
			  Tuple, std::enable_if_t<std::conjunction_v<
			           not_trait<json_details::has_json_data_contract_trait<Tuple>>,
			           is_tuple<Tuple>>>> {
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

				template<typename T>
				inline constexpr bool container_detect_v =
				  std::conjunction_v<daw::is_detected<container_value_type, T>,
				                     daw::is_detected<container_begin_type, T>,
				                     daw::is_detected<container_end_type, T>>;

				template<typename AssociativeContainer>
				using container_key_type = typename AssociativeContainer::key_type;

				template<typename AssociativeContainer>
				using container_mapped_type =
				  typename AssociativeContainer::mapped_type;

				template<typename T>
				using is_value_type_char = std::is_same<char, container_value_type<T>>;

			} // namespace container_detect
			template<typename T>
			using is_container = std::conjunction<
			  daw::is_detected<container_detect::container_value_type, T>,
			  daw::is_detected<container_detect::container_begin_type, T>,
			  daw::is_detected<container_detect::container_end_type, T>>;

			template<typename Container>
			inline constexpr bool is_container_v = is_container<Container>::value;

			template<typename String>
			using is_string =
			  std::conjunction<is_container<String>,
			                   container_detect::is_value_type_char<String>>;

			template<typename String>
			inline constexpr bool is_string_v = is_string<String>::value;

			template<typename AssociativeContainer>
			using is_associative_container = std::conjunction<
			  is_container<AssociativeContainer>,
			  daw::is_detected<container_detect::container_key_type,
			                   AssociativeContainer>,
			  daw::is_detected<container_detect::container_mapped_type,
			                   AssociativeContainer>>;

			template<typename AssociativeContainer>
			inline constexpr bool is_associative_container_v =
			  is_associative_container<AssociativeContainer>::value;

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
			  std::optional<T>,
			  std::enable_if_t<std::conjunction_v<
			    not_trait<has_json_data_contract_trait<std::optional<T>>>,
			    daw::is_detected<json_deduced_type_map, T>>>> {

				static constexpr bool is_null = true;
				using type = json_deduced_type_map<T>;
				static constexpr JsonParseTypes parse_type = type::parse_type;

				static constexpr bool type_map_found = true;
			};

			template<typename T>
			using has_deduced_type_mapping =
			  std::bool_constant<json_deduced_type_map<T>::type_map_found>;

			template<typename T>
			inline constexpr bool has_deduced_type_mapping_v =
			  json_deduced_type_map<T>::type_map_found;

			template<typename Mapped, bool Found = true>
			struct json_link_quick_map_type : std::bool_constant<Found> {
				using mapped_type = Mapped;
			};

			template<typename T>
			inline constexpr auto json_link_quick_map( ) {
				if constexpr( is_a_json_type_v<T> ) {
					return json_link_quick_map_type<T>{ };
				} else if constexpr( has_deduced_type_mapping_v<T> ) {
					using mapped_type_t = json_deduced_type_map<T>;
					constexpr auto mapped_type = mapped_type_t::parse_type;
					constexpr bool is_null = mapped_type_t::is_null;
					if constexpr( mapped_type == JsonParseTypes::Unknown ) {
						if constexpr( is_null ) {
							return json_link_quick_map_type<json_base::json_raw_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_raw<T>>{ };
						}
					} else if constexpr( mapped_type == JsonParseTypes::StringRaw ) {
						if constexpr( is_null ) {
							return json_link_quick_map_type<
							  json_base::json_string_raw_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_string_raw<T>>{ };
						}
					} else if constexpr( mapped_type == JsonParseTypes::StringEscaped ) {
						if constexpr( is_null ) {
							return json_link_quick_map_type<
							  json_base::json_string_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_string<T>>{ };
						}
					} else if constexpr( mapped_type == JsonParseTypes::Bool ) {
						if constexpr( is_null ) {
							return json_link_quick_map_type<json_base::json_bool_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_bool<T>>{ };
						}
					} else if constexpr( ( mapped_type == JsonParseTypes::Signed ) |
					                     ( mapped_type == JsonParseTypes::Unsigned ) |
					                     ( mapped_type == JsonParseTypes::Real ) ) {
						if constexpr( is_null ) {
							return json_link_quick_map_type<
							  json_base::json_number_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_number<T>>{ };
						}
					} else if constexpr( mapped_type == JsonParseTypes::Tuple ) {
						if constexpr( is_null ) {
							return json_link_quick_map_type<json_base::json_tuple_null<T>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_tuple<T>>{ };
						}
					} else if constexpr( mapped_type == JsonParseTypes::KeyValue ) {
						if constexpr( is_null ) {
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
					} else if constexpr( mapped_type == JsonParseTypes::Array ) {
						if constexpr( is_null ) {
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
			using has_json_link_quick_map = decltype( json_link_quick_map<T>( ) );

			template<typename T>
			inline constexpr bool has_json_link_quick_map_v =
			  has_json_link_quick_map<T>::value;

			/***
			 * Get the quick mapped json type for type T
			 */
			template<typename T>
			using json_link_quick_map_t =
			  typename decltype( json_link_quick_map<T>( ) )::mapped_type;

			namespace vector_detect {
				template<typename T>
				struct vector_test {
					struct not_vector {};
					static constexpr bool value = false;
					using type = not_vector;
				};

				template<typename T, typename Alloc>
				struct vector_test<std::vector<T, Alloc>> {
					static constexpr bool value = true;
					using type = T;
				};

				template<typename T>
				using vector_test_t = typename vector_test<T>::type;
			} // namespace vector_detect

			template<typename T>
			using is_vector =
			  std::bool_constant<vector_detect::vector_test<T>::value>;

			template<typename JsonType>
			using is_json_class_map_test = typename JsonType::json_member;

			template<typename JsonType>
			struct json_class_map_type {
				using type = typename JsonType::json_member;
			};

			template<typename JsonType>
			inline constexpr bool is_json_class_map_v =
			  daw::is_detected_v<is_json_class_map_test, JsonType>;

			template<typename T, bool Ordered, bool Contract, bool JsonType,
			         bool QuickMap, bool Container>
			struct json_type_deducer {
				using type = missing_json_data_contract_for<T>;
			};

			template<typename T, /* typename Ordered, */ bool Contract, bool JsonType,
			         bool QuickMap, bool Container>
			struct json_type_deducer<T, true, Contract, JsonType, QuickMap,
			                         Container> {

				using type = T;
			};

			template<typename T, /*typename Contract,*/ bool JsonType, bool QuickMap,
			         bool Container>
			struct json_type_deducer<T, false, true, JsonType, QuickMap, Container> {

				static_assert( not std::is_same_v<T, void> );
				using type = json_base::json_class<
				  T, json_class_constructor_t<T, default_constructor<T>>>;
				static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
				               "Detection failure" );
				static_assert(
				  not std::is_same_v<daw::remove_cvref_t<type>, daw::nonesuch>,
				  "Detection failure" );
			};

			template<typename T,
			         /*bool Contract, bool JsonType,*/ bool QuickMap, bool Container>
			struct json_type_deducer<T, false, false, true, QuickMap, Container> {
				static_assert( not std::is_same_v<T, void> );
				using type =
				  typename std::conditional_t<is_json_class_map_v<T>,
				                              json_class_map_type<T>,
				                              daw::traits::identity<T>>::type;
				static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
				               "Detection failure" );
				static_assert(
				  not std::is_same_v<daw::remove_cvref_t<type>, daw::nonesuch>,
				  "Detection failure" );
			};

			template<typename T /*bool Contract, bool JsonType, bool QuickMap*/,
			         bool Container>
			struct json_type_deducer<T, false, false, false, true, Container> {
				static_assert( not std::is_same_v<T, void> );
				using type = json_link_quick_map_t<T>;
				static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
				               "Detection failure" );
				static_assert(
				  not std::is_same_v<daw::remove_cvref_t<type>, daw::nonesuch>,
				  "Detection failure" );
				static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
				               "Detection failure" );
			};

			template<typename T
			         /*bool Contract, bool JsonType, bool QuickMap,bool Container*/>
			struct json_type_deducer<T, false, false, false, false, true> {
				static_assert( not std::is_same_v<T, void> );
				using type = json_base::json_array<typename T::value_type, T>;
				static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
				               "Detection failure" );
				static_assert(
				  not std::is_same_v<daw::remove_cvref_t<type>, daw::nonesuch>,
				  "Detection failure" );
				static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
				               "Detection failure" );
			};

			template<typename T>
			using json_deduced_type = typename json_type_deducer<
			  T, is_an_ordered_member_v<T>, has_json_data_contract_trait_v<T>,
			  json_details::is_a_json_type_v<T>, has_json_link_quick_map_v<T>,
			  is_container_v<T>>::type;

			template<typename T>
			using has_json_deduced_type = daw::not_trait<
			  std::is_same<json_deduced_type<T>, missing_json_data_contract_for<T>>>;

			template<typename T>
			inline constexpr bool has_unnamed_default_type_mapping_v =
			  has_json_deduced_type<T>::value;

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

			template<typename Constructor, typename... Members>
			inline constexpr bool can_defer_construction_v =
			  std::is_invocable_v<Constructor, typename Members::parse_to_t...>;

			template<typename JsonMember>
			using dependent_member_t = typename JsonMember::dependent_member;

			template<typename JsonMember>
			inline constexpr bool has_dependent_member_v =
			  daw::is_detected_v<dependent_member_t, JsonMember>;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
