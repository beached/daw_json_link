// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_types_base.h"

#include "daw/json/concepts/daw_container_traits.h"
#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_construct_value.h"
#include "daw/json/impl/daw_json_enums.h"
#include "daw/json/impl/daw_json_exec_modes.h"
#include "daw/json/impl/daw_json_link_types_aggregate.h"
#include "daw/json/impl/daw_json_name.h"
#include "daw/json/impl/daw_json_option_bits.h"
#include "daw/json/impl/daw_json_reflection_helpers.h"
#include "daw/json/impl/daw_json_traits.h"
#include "daw/json/impl/daw_json_type_options.h"
#include "daw/json/impl/daw_json_value_fwd.h"

#include <daw/cpp_17.h>
#include <daw/daw_allocator_construct.h>
#include <daw/daw_arith_traits.h>
#include <daw/daw_callable.h>
#include <daw/daw_consteval.h>
#include <daw/daw_cpp20_concept.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_enable_requires.h>
#include <daw/daw_fwd_pack_apply.h>
#include <daw/daw_move.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename T>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL JsonParseTypes
			number_parse_type_impl_test( ) {
				if constexpr( daw::is_floating_point_v<T> ) {
					return JsonParseTypes::Real;
				} else if constexpr( daw::is_signed_v<T> ) {
					return JsonParseTypes::Signed;
				} else {
					static_assert( daw::is_unsigned_v<T> );
					return JsonParseTypes::Unsigned;
				}
			}

			template<typename T>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL JsonParseTypes number_parse_type_test( ) {
				if constexpr( std::is_enum_v<T> ) {
					return number_parse_type_impl_test<std::underlying_type_t<T>>( );
				} else {
					return number_parse_type_impl_test<T>( );
				}
			}

			template<typename T>
			inline constexpr JsonParseTypes number_parse_type_v =
			  number_parse_type_test<T>( );

			template<typename, typename AllowRawString, typename = void>
			struct json_deduced_type_map {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::Unknown;

				static constexpr bool type_map_found = false;
			};

			template<typename JsonType, typename AllowRawString>
			DAW_REQUIRES( is_a_json_type_v<JsonType> )
			struct json_deduced_type_map<
			  JsonType, AllowRawString DAW_ENABLEIF_S( is_a_json_type_v<JsonType> )> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::Unknown;

				using type = JsonType;
				static constexpr bool type_map_found = true;
			};

			template<typename T, typename AllowRawString>
			DAW_REQUIRES( has_json_data_contract_trait_v<T> )
			struct json_deduced_type_map<
			  T, AllowRawString DAW_ENABLEIF_S( has_json_data_contract_trait_v<T> )> {
				static constexpr bool is_null = false;
				using type = typename json_data_contract<T>::type;
				static_assert( is_json_member_list_v<type>,
				               "Expected a JSON member list" );
				static constexpr auto parse_type = JsonParseTypes::Unknown;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<daw::string_view, std::true_type> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::StringRaw;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<daw::string_view, std::false_type> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::StringInsitu;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<std::string_view, std::true_type> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::StringRaw;

				static constexpr bool type_map_found = true;
			};

			template<>
			struct json_deduced_type_map<std::string_view, std::false_type> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::StringInsitu;

				static constexpr bool type_map_found = true;
			};

			/// @brief When serializing, a char is a one character string.  signed
			/// char and unsigned char are distinct types and remain numbers.
			template<>
			struct json_deduced_type_map<char, std::false_type> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::StringInsitu;

				static constexpr bool type_map_found = true;
			};

			template<typename AllowRawString>
			struct json_deduced_type_map<std::string, AllowRawString> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::StringEscaped;

				static constexpr bool type_map_found = true;
			};

			template<typename AllowRawString>
			struct json_deduced_type_map<bool, AllowRawString> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::Bool;
				static constexpr bool type_map_found = true;
			};

			// libc++ has a non-conforming vector<bool>::const_reference as it isn't
			// bool https://bugs.llvm.org/show_bug.cgi?id=16077
#if defined( _LIBCPP_VERSION )
			template<typename AllowRawString>
			struct json_deduced_type_map<typename std::vector<bool>::const_reference,
			                             AllowRawString> {

				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::Bool;

				static constexpr bool type_map_found = true;
			};
#endif

			template<typename Integer, typename AllowRawString>
			DAW_REQUIRES(
			  not json_details::has_json_data_contract_trait_v<Integer> and
			  daw::is_integral_v<Integer> )
			struct json_deduced_type_map<
			  Integer, AllowRawString DAW_ENABLEIF_S(
			             not json_details::has_json_data_contract_trait_v<Integer> and
			             daw::is_integral_v<Integer> )> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = daw::is_signed_v<Integer>
				                                     ? JsonParseTypes::Signed
				                                     : JsonParseTypes::Unsigned;

				static constexpr bool type_map_found = true;
			};

			template<typename Enum, typename AllowRawString>
			DAW_REQUIRES( not json_details::has_json_data_contract_trait_v<Enum> and
			              std::is_enum_v<Enum> )
			struct json_deduced_type_map<
			  Enum, AllowRawString DAW_ENABLEIF_S(
			          not json_details::has_json_data_contract_trait_v<Enum> and
			          std::is_enum_v<Enum> )> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type =
				  daw::is_signed_v<std::underlying_type<Enum>>
				    ? JsonParseTypes::Signed
				    : JsonParseTypes::Unsigned;

				static constexpr bool type_map_found = true;
			};

			template<typename FloatingPoint, typename AllowRawString>
			DAW_REQUIRES(
			  not json_details::has_json_data_contract_trait_v<FloatingPoint> and
			  daw::is_floating_point_v<FloatingPoint> )
			struct json_deduced_type_map<
			  FloatingPoint,
			  AllowRawString DAW_ENABLEIF_S(
			    not json_details::has_json_data_contract_trait_v<FloatingPoint> and
			    daw::is_floating_point_v<FloatingPoint> )> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::Real;

				static constexpr bool type_map_found = true;
			};

			template<typename Tuple, typename AllowRawString>
			DAW_REQUIRES( not json_details::has_json_data_contract_trait_v<Tuple> and
			              is_tuple_v<Tuple> )
			struct json_deduced_type_map<
			  Tuple, AllowRawString DAW_ENABLEIF_S(
			           not json_details::has_json_data_contract_trait_v<Tuple> and
			           is_tuple_v<Tuple> )> {
				static constexpr bool is_null = false;
				static constexpr auto parse_type = JsonParseTypes::Tuple;

				static constexpr bool type_map_found = true;
			};

			namespace container_detect {
				template<typename T>
				using is_string_test =
				  decltype( (void)( std::begin( std::declval<T &>( ) ) ),
				            (void)( std::end( std::declval<T &>( ) ) ),
				            std::declval<typename T::value_type>( ) );
			} // namespace container_detect

			template<typename String>
			DAW_CPP20_CONCEPT is_string_v = std::is_same_v<
			  char, daw::detected_t<container_detect::is_string_test, String>>;

			DAW_JSON_MAKE_REQ_TRAIT(
			  is_associative_container_v,
			  ( (void)( std::begin( std::declval<T &>( ) ) ),
			    (void)( std::end( std::declval<T &>( ) ) ),
			    (void)( std::declval<typename T::value_type>( ) ),
			    (void)( std::declval<typename T::key_type>( ) ),
			    (void)( std::declval<typename T::mapped_type>( ) ) ) );

			template<typename AssociativeContainer, typename AllowRawString>
			DAW_REQUIRES( not has_json_data_contract_trait_v<AssociativeContainer> and
			              is_associative_container_v<AssociativeContainer> )
			struct json_deduced_type_map<
			  AssociativeContainer,
			  AllowRawString DAW_ENABLEIF_S(
			    not has_json_data_contract_trait_v<AssociativeContainer> and
			    is_associative_container_v<AssociativeContainer> )> {
				static constexpr bool is_null = false;
				using key = typename AssociativeContainer::key_type;
				using value = typename AssociativeContainer::mapped_type;
				static constexpr auto parse_type = JsonParseTypes::KeyValue;

				static constexpr bool type_map_found = true;
			};

			template<typename T>
			DAW_CPP20_CONCEPT is_deduced_array_v =
			  not has_json_data_contract_trait_v<T> and
			  not is_associative_container_v<T> and concepts::is_container_v<T> and
			  not is_string_v<T>;

			template<typename Container, typename AllowRawString>
			DAW_REQUIRES( is_deduced_array_v<Container> )
			struct json_deduced_type_map<
			  Container,
			  AllowRawString DAW_ENABLEIF_S( is_deduced_array_v<Container> )> {
				static constexpr bool is_null = false;
				using value = typename Container::value_type;
				static constexpr auto parse_type = JsonParseTypes::Array;

				static constexpr bool type_map_found = true;
			};

#if defined( DAW_JSON_USE_CONCEPTS )
			template<typename T, typename AllowRawString>
			inline constexpr bool detected_deduced_type_map_v = requires {
				requires json_deduced_type_map<T, AllowRawString>::type_map_found;
			};
#else
			template<typename, typename, typename = void>
			inline constexpr bool detected_deduced_type_map_v = false;

			template<typename T, typename AllowRawString>
			inline constexpr bool detected_deduced_type_map_v<
			  T, AllowRawString,
			  std::void_t<decltype( json_deduced_type_map<
			                        T, AllowRawString>::type_map_found )>> =
			  json_deduced_type_map<T, AllowRawString>::type_map_found;
#endif
			template<typename T, typename AllowRawString>
			DAW_CPP20_CONCEPT has_nullable_type_map_v =
			  concepts::is_nullable_value_v<T> and
			  not has_json_data_contract_trait_v<T> and
			  detected_deduced_type_map_v<concepts::nullable_value_type_t<T>,
			                              AllowRawString>;

			template<typename T, typename AllowRawString>
			DAW_REQUIRES( has_nullable_type_map_v<T, AllowRawString> )
			struct json_deduced_type_map<
			  T, AllowRawString DAW_ENABLEIF_S(
			       has_nullable_type_map_v<T, AllowRawString> )> {
				static constexpr bool is_null = true;
				using sub_type = concepts::nullable_value_type_t<T>;
				using type = json_deduced_type_map<sub_type, AllowRawString>;
				static constexpr auto parse_type = type::parse_type;
				static constexpr bool type_map_found = true;
			};

			template<typename T, typename AllowRawString = std::true_type>
			DAW_CPP20_CONCEPT has_deduced_type_mapping_v =
			  json_deduced_type_map<T, AllowRawString>::type_map_found;

			template<typename... Ts>
			DAW_CPP20_CONCEPT are_deduced_type_mapped_v =
			  ( has_deduced_type_mapping_v<Ts> and ... );

			template<typename Mapped, bool Found = true>
			struct json_link_quick_map_type {
				static constexpr bool value = Found;
				using mapped_type = Mapped;
			};

			template<JsonParseTypes Value>
			DAW_CPP20_CONCEPT is_arithmetic_parse_type_v =
			  daw::traits::equal_to_any_of_v<Value, JsonParseTypes::Signed,
			                                 JsonParseTypes::Unsigned,
			                                 JsonParseTypes::Real>;

			template<typename T, typename AllowRawString = std::true_type>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL auto json_deduced_type_impl( ) noexcept;

			template<typename T, typename AllowRawString>
			struct deduced_element {
				using type =
				  typename decltype( json_deduced_type_impl<T,
				                                            AllowRawString>( ) )::type;
			};

			/// @brief The element type passed to a deduced container/tuple mapping.
			/// The mapping types deduce their elements with raw strings allowed, so
			/// when they are not the element is deduced here and passed as a JSON
			/// type.  When they are, T is passed as is so the mapping is unchanged.
			template<typename T, typename AllowRawString>
			using deduced_element_t =
			  typename daw::conditional_t<AllowRawString::value,
			                              daw::traits::identity<T>,
			                              deduced_element<T, AllowRawString>>::type;

			template<typename Tuple, typename AllowRawString,
			         typename = std::make_index_sequence<std::tuple_size_v<Tuple>>>
			struct deduced_tuple_types;

			template<typename Tuple, typename AllowRawString, std::size_t... Is>
			struct deduced_tuple_types<Tuple, AllowRawString,
			                           std::index_sequence<Is...>> {
				using type = std::tuple<deduced_element_t<
				  daw::remove_cvref_t<std::tuple_element_t<Is, Tuple>>,
				  AllowRawString>...>;
			};

			/// @brief The tuple types list passed to a deduced json_tuple, see
			/// deduced_element_t
			template<typename Tuple, typename AllowRawString>
			using deduced_tuple_types_t = typename daw::conditional_t<
			  AllowRawString::value, daw::traits::identity<use_default>,
			  deduced_tuple_types<Tuple, AllowRawString>>::type;

			template<typename T, typename AllowRawString = std::true_type>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL auto json_link_quick_map( ) noexcept {
				if constexpr( is_a_json_type_v<T> ) {
					return json_link_quick_map_type<T>{ };
				} else if constexpr( has_deduced_type_mapping_v<T, AllowRawString> ) {
					using mapped_type_t = json_deduced_type_map<T, AllowRawString>;
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
					} else if constexpr( parse_type::value ==
					                     JsonParseTypes::StringInsitu ) {
						if constexpr( is_null::value ) {
							return json_link_quick_map_type<
							  json_base::json_string_insitu_null<T>>{ };
						} else {
							return json_link_quick_map_type<
							  json_base::json_string_insitu<T>>{ };
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
							using tp_t = json_details::unwrapped_t<T>;
							return json_link_quick_map_type<json_base::json_tuple_null<
							  T,
							  deduced_tuple_types_t<tp_t, AllowRawString>>>{ };
						} else {
							return json_link_quick_map_type<json_base::json_tuple<
							  T,
							  deduced_tuple_types_t<T, AllowRawString>>>{ };
						}
					} else if constexpr( parse_type::value == JsonParseTypes::KeyValue ) {
						if constexpr( is_null::value ) {
							using b_t = typename mapped_type_t::type;
							using k_t = deduced_element_t<typename b_t::key, AllowRawString>;
							using v_t =
							  deduced_element_t<typename b_t::value, AllowRawString>;
							return json_link_quick_map_type<
							  json_base::json_key_value_null<T, v_t, k_t>>{ };
						} else {
							using k_t =
							  deduced_element_t<typename mapped_type_t::key, AllowRawString>;
							using v_t = deduced_element_t<typename mapped_type_t::value,
							                              AllowRawString>;
							return json_link_quick_map_type<
							  json_base::json_key_value<T, v_t, k_t>>{ };
						}
					} else if constexpr( parse_type::value == JsonParseTypes::Array ) {
						if constexpr( is_null::value ) {
							using b_t = typename mapped_type_t::type;
							using v_t =
							  deduced_element_t<typename b_t::value, AllowRawString>;
							return json_link_quick_map_type<json_base::json_nullable<
							  T,
							  json_base::json_array<v_t, json_details::unwrapped_t<T>>>>{ };
						} else {
							using v_t = deduced_element_t<typename mapped_type_t::value,
							                              AllowRawString>;
							return json_link_quick_map_type<json_base::json_array<v_t, T>>{ };
						}
#if defined( DAW_JSON_HAS_REFLECTION )
					} else if constexpr( refl_details::PotentiallyReflectable<T> ) {
						return json_link_quick_map_type<
						  json_base::json_reflected_class<T>>{ };
#endif
					} else {
						return json_link_quick_map_type<void, false>{ };
					}
#if defined( DAW_JSON_HAS_REFLECTION )
				} else if constexpr( refl_details::PotentiallyReflectable<T> ) {
					return json_link_quick_map_type<
					  json_base::json_reflected_class<T>>{ };
#endif
				} else {
					return json_link_quick_map_type<void, false>{ };
				}
			}

			/// Check if the current type has a quick map specialized for it
			template<typename T, typename AllowRawString = std::true_type>
			DAW_CPP20_CONCEPT has_json_link_quick_map_v =
			  decltype( json_link_quick_map<T, AllowRawString>( ) )::value;

			/// @brief Get the quick mapped json type for type T
			template<typename T, typename AllowRawString = std::true_type>
			using json_link_quick_map_t =
			  typename decltype( json_link_quick_map<
			                     T, AllowRawString>( ) )::mapped_type;

			template<typename JsonType>
			struct json_class_map_type {
				using type = typename json_data_contract_trait_t<JsonType>::json_member;
			};

			/// @brief A mapping parsed from a JSON object by its members
			template<typename JsonMember>
			inline constexpr bool is_json_class_mapping_v =
			  JsonMember::expected_type == JsonParseTypes::Class
#if defined( DAW_JSON_HAS_REFLECTION )
			  or JsonMember::expected_type == JsonParseTypes::ReflectedClass
#endif
			  ;

			/// @brief The members of a class mapping.  A mapping can give its own,
			/// as json_reflected_class does, otherwise they are its
			/// json_data_contract's
			template<typename JsonMember, typename = void>
			struct json_class_member_list {
				using type =
				  json_data_contract_trait_t<typename JsonMember::wrapped_type>;
			};

			template<typename JsonMember>
			struct json_class_member_list<
			  JsonMember, std::void_t<typename JsonMember::json_member_list_t>> {
				using type = typename JsonMember::json_member_list_t;
			};

			template<typename JsonMember>
			using json_class_member_list_t =
			  typename json_class_member_list<JsonMember>::type;

			template<typename>
			struct is_json_class_map : std::false_type {};

			// This maybe specialized
			template<typename T>
			inline constexpr bool is_json_class_map_v =
			  has_json_data_contract_trait_v<T> and
			  is_json_class_map_v<json_data_contract_trait_t<T>>;

			/// @brief char *, char const * - a nul terminated string
			template<typename T>
			inline constexpr bool is_char_pointer_v =
			  std::is_pointer_v<T> and
			  std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>, char>;

			/// @brief char[N], char const[N] - a string literal or buffer
			template<typename T>
			inline constexpr bool is_char_array_v =
			  std::rank_v<T> == 1 and
			  std::is_same_v<std::remove_cv_t<std::remove_extent_t<T>>, char>;

			template<typename T, typename AllowRawString>
			DAW_ATTRIB_INLINE DAW_CONSTEVAL auto json_deduced_type_impl( ) noexcept {
				if constexpr( is_char_array_v<T> and not AllowRawString::value ) {
					return daw::traits::identity<
					  json_base::json_string_insitu<daw::string_view>>{ };
				} else if constexpr( is_a_basic_json_value<T> ) {
					return daw::traits::identity<json_base::json_raw<T>>{ };
#if defined( DAW_JSON_HAS_REFLECTION )
				} else if constexpr( refl_details::HasTypeLevelAnnotation<T> ) {
					return daw::traits::identity<
					  refl_details::type_annotation_mapping_t<T>>{ };
#endif
				} else if constexpr( is_an_ordered_member_v<T> ) {
					using type = T;
					return daw::traits::identity<type>{ };
				} else if constexpr( has_json_data_contract_trait_v<T> ) {
					static_assert( not std::is_same_v<T, void> );

					using type = json_base::json_class<T>;

					static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<remove_cvref_t<type>>,
					               "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( has_json_link_quick_map_v<T, AllowRawString> ) {
					static_assert( not std::is_same_v<T, void> );
					using type = json_link_quick_map_t<T, AllowRawString>;
					using rcvref_type = remove_cvref_t<type>;
					static_assert( not std::is_same_v<rcvref_type, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<rcvref_type>, "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( is_a_json_type_v<T> ) {
					static_assert( not std::is_same_v<T, void> );
					using type =
					  typename daw::conditional_t<is_json_class_map_v<T>,
					                              json_class_map_type<T>,
					                              daw::traits::identity<T>>::type;
					static_assert( not std::is_same_v<daw::remove_cvref_t<type>, void>,
					               "Detection failure" );
					static_assert( not is_nonesuch_v<remove_cvref_t<type>>,
					               "Detection failure" );
					return daw::traits::identity<type>{ };
				} else if constexpr( concepts::is_nullable_value_v<T> ) {
					using value_type = concepts::nullable_value_type_t<T>;
					using sub_type = typename decltype( json_deduced_type_impl<
					                                    value_type,
					                                    AllowRawString>( ) )::type;
					using type = json_base::json_nullable<T, sub_type>;
					return daw::traits::identity<type>{ };
				} else if constexpr( concepts::is_container_v<T> ) {
					using type = json_base::json_array<
					  deduced_element_t<typename T::value_type, AllowRawString>,
					  T>;
					return daw::traits::identity<type>{ };
				} else if constexpr( std::is_empty_v<T> and
				                     std::is_default_constructible_v<T> ) {
					// Allow empty/default constructible types to work without mapping
					using type = json_details::json_empty_class<T>;
					return daw::traits::identity<type>{ };
#if not defined( DAW_JSON_HAS_REFLECTION )
				} else if constexpr( can_convert_to_tuple_v<T> ) {
					using type = json_base::json_tuple<T>;
					return daw::traits::identity<type>{ };
#endif
				} else {
					static_assert( daw::deduced_false_v<T>,
					               "Could not deduced data contract type and there is no "
					               "json_data_contract_specialization" );
				}
			}

			template<typename T, typename AllowRawString = std::true_type>
			using json_deduced_type = typename DAW_TYPEOF(
			  json_deduced_type_impl<T, AllowRawString>( ) )::type;

			/// @brief The JSON type used by the serialization only interfaces,
			/// to_json and json_writer.  When JsonClass is use_default or a C++
			/// type, it is deduced without raw strings as the value did not come
			/// from parsing.  An explicit JSON type is used as is.
			template<typename JsonClass, typename Value>
			using json_serialize_deduced_type = typename daw::conditional_t<
			  std::is_same_v<use_default, JsonClass>,
			  ident_trait<json_deduced_type, Value, std::false_type>,
			  daw::conditional_t<
			    is_a_json_type_v<JsonClass>,
			    ident_trait<json_deduced_type, JsonClass>,
			    ident_trait<json_deduced_type, JsonClass, std::false_type>>>::type;

			template<typename T, typename AllowRawString = std::true_type>
			DAW_CPP20_CONCEPT has_json_deduced_type_v =
			  not std::is_same_v<json_deduced_type<T, AllowRawString>,
			                     missing_json_data_contract_for_or_unknown_type<T>>;

			template<typename... Ts>
			DAW_CPP20_CONCEPT all_have_deduced_type_v =
			  ( has_json_deduced_type_v<Ts> and ... );

			template<typename JsonElement, typename Container, typename Constructor>
			struct json_constructor<
			  json_base::json_array<JsonElement, Container, Constructor>> {
				using json_element_t = json_deduced_type<JsonElement>;
				using json_element_parse_to_t = json_result_t<json_element_t>;

				using container_t =
				  daw::conditional_t<std::is_same_v<Container, use_default>,
				                     std::vector<json_element_parse_to_t>, Container>;

				using type =
				  daw::conditional_t<std::is_same_v<use_default, Constructor>,
				                     default_constructor<container_t>, Constructor>;

				static_assert(
				  daw::is_callable_v<type, json_element_parse_to_t const *,
				                     json_element_parse_to_t const *>,
				  "Constructor must support copy and/or move construction" );
			};

			template<typename JsonElement, typename Container, typename Constructor>
			struct json_result<
			  json_base::json_array<JsonElement, Container, Constructor>> {
				using constructor_t = typename json_constructor<
				  json_base::json_array<JsonElement, Container, Constructor>>::type;
				using json_element_t = json_deduced_type<JsonElement>;
				using json_element_parse_to_t =
				  typename json_result<json_element_t>::type;
				using type =
				  std::invoke_result_t<constructor_t, json_element_parse_to_t const *,
				                       json_element_parse_to_t const *>;
			};

			template<typename T>
			DAW_CPP20_CONCEPT has_unnamed_default_type_mapping_v =
			  has_json_deduced_type_v<T>;

			template<typename JsonMember>
			using from_json_result_t = json_result_t<json_deduced_type<JsonMember>>;

			template<typename Constructor, typename... Members>
			using json_class_parse_result_impl2 =
			  std::invoke_result_t<Constructor, json_result_t<Members>...>;

			template<typename Constructor, typename... Members>
			using json_class_parse_result_impl =
			  daw::detected_t<json_class_parse_result_impl2, Constructor, Members...>;

			template<typename Constructor, typename... Members>
			struct could_not_construct_from_members_error;

			template<typename Constructor, typename... Members>
			using json_class_parse_result_t = typename daw::conditional_t<
			  daw::is_callable_v<Constructor, json_result_t<Members>...>,
			  std::invoke_result<Constructor, json_result_t<Members>...>,
			  daw::traits::identity<could_not_construct_from_members_error<
			    Constructor, Members...>>>::type;

			template<typename JsonMember>
			using dependent_member_t = typename JsonMember::dependent_member;

			template<typename JsonMember, typename = void>
			inline constexpr bool has_dependent_member_v = false;

			template<typename JsonMember>
			inline constexpr bool has_dependent_member_v<
			  JsonMember, std::void_t<dependent_member_t<JsonMember>>> = true;

			DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT( has_nullable_dependent_member_v,
			                                    T::member_type::dependent_member );

			template<typename JsonMember>
			DAW_REQUIRES( is_json_nullable_v<JsonMember> )
			inline constexpr bool has_dependent_member_v<
			  JsonMember DAW_ENABLEIF_S( is_json_nullable_v<JsonMember> )> =
			  has_nullable_dependent_member_v<JsonMember>;

			template<typename Constructor>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
			construct_nullable_empty( ) {
				if constexpr( daw::is_callable_v<
				                Constructor,
				                concepts::construct_nullable_with_empty_t> ) {
					return Constructor{ }( concepts::construct_nullable_with_empty );
				} else {
					return Constructor{ }( );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
