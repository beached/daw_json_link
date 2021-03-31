// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_json_enums.h"
#include "daw_json_exec_modes.h"
#include "daw_json_name.h"
#include "daw_json_parse_digit.h"
#include "daw_json_parse_string_need_slow.h"
#include "daw_json_parse_string_quote.h"
#include "daw_json_traits.h"

#include <daw/daw_arith_traits.h>
#include <daw/daw_move.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

#include <ciso646>
#include <cstddef>
#include <iterator>

#if defined( __has_include ) and __has_include( <version> )
#include <version>
#endif

#if defined( __cpp_constexpr_dynamic_alloc )
#define CPP20CONSTEXPR constexpr
#define HAS_CPP20CONSTEXPR
#else
#define CPP20CONSTEXPR
#endif

namespace daw::json {
	/***
	 *
	 * Allows specifying an unnamed json mapping where the
	 * result is a tuple
	 */
	template<typename... Members>
	struct tuple_json_mapping {
		std::tuple<typename Members::parse_to_t...> members;

		template<typename... Ts>
		constexpr tuple_json_mapping( Ts &&...values )
		  : members{ DAW_FWD( values )... } {}
	};
} // namespace daw::json

namespace daw::json::json_details {
	template<typename T, bool = false>
	struct ordered_member_subtype {
		using type = T;
	};

	template<typename T>
	struct ordered_member_subtype<T, true> {
		using type = typename T::json_member;
	};

	template<typename T>
	using ordered_member_subtype_t =
	  typename ordered_member_subtype<T, is_an_ordered_member_v<T>>::type;

	template<typename T, bool, bool>
	struct json_data_constract_constructor_impl {
		using type = default_constructor<T>;
	};

	template<typename T>
	using has_json_data_constract_constructor_test =
	  typename json_data_contract_trait_t<T>::constructor;

	template<typename T>
	struct json_data_constract_constructor_impl<T, true, true> {
		using type = typename json_data_contract_trait_t<T>::constructor;
	};

	template<typename T>
	using json_data_contract_constructor_t =
	  typename json_data_constract_constructor_impl<
	    T, daw::is_detected_v<json_data_contract_trait_t, T>,
	    daw::is_detected_v<has_json_data_constract_constructor_test, T>>::type;

	template<typename T>
	using json_class_constructor_t =
	  std::conditional_t<daw::is_detected_v<json_data_contract_constructor_t, T>,
	                     json_data_contract_constructor_t<T>,
	                     default_constructor<T>>;

	template<typename T>
	inline constexpr auto json_class_constructor = json_class_constructor_t<T>{ };

	template<typename Value, typename Constructor, typename Range,
	         typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr auto
	construct_value( Constructor &&ctor, Range &rng, Args &&...args ) {
		if constexpr( Range::has_allocator ) {
			using alloc_t = typename Range::template allocator_type_as<Value>;
			auto alloc = rng.template get_allocator_for<Value>( );
			if constexpr( std::is_invocable_v<Constructor, Args..., alloc_t> ) {
				return ctor( DAW_FWD( args )..., daw::move( alloc ) );
			} else if constexpr( daw::traits::is_callable_v<Constructor,
			                                                std::allocator_arg_t,
			                                                alloc_t, Args...> ) {
				return ctor( std::allocator_arg, daw::move( alloc ),
				             DAW_FWD( args )... );
			} else {
				static_assert( std::is_invocable_v<Constructor, Args...> );
				return ctor( DAW_FWD( args )... );
			}
		} else {
			static_assert( std::is_invocable_v<Constructor, Args...> );
			if constexpr( std::is_invocable_v<Constructor, Args...> ) {
				return ctor( DAW_FWD( args )... );
			}
		}
	}

	template<typename Allocator>
	using has_allocate_test =
	  decltype( std::declval<Allocator>( ).allocate( size_t{ 1 } ) );

	template<typename Allocator>
	using has_deallocate_test = decltype( std::declval<Allocator>( ).deallocate(
	  static_cast<void *>( nullptr ), size_t{ 1 } ) );

	template<typename Allocator>
	inline constexpr bool is_allocator_v =
	  daw::is_detected_v<has_allocate_test, Allocator>
	    and daw::is_detected_v<has_deallocate_test, Allocator>;

	template<typename T>
	struct has_json_data_contract_trait
	  : std::bool_constant<
	      not std::is_same_v<daw::json::missing_json_data_contract_for<T>,
	                         json_data_contract_trait_t<T>>> {};

	template<typename T>
	inline constexpr bool has_json_data_contract_trait_v =
	  has_json_data_contract_trait<T>::value;

	template<typename Container, typename Value>
	using detect_push_back = decltype( std::declval<Container &>( ).push_back(
	  std::declval<Value>( ) ) );

	template<typename Container, typename Value>
	using detect_insert_end = decltype( std::declval<Container &>( ).insert(
	  std::end( std::declval<Container &>( ) ), std::declval<Value>( ) ) );

	template<typename Container, typename Value>
	inline constexpr bool has_push_back_v =
	  daw::is_detected_v<detect_push_back, Container, Value>;

	template<typename Container, typename Value>
	inline constexpr bool has_insert_end_v =
	  daw::is_detected_v<detect_insert_end, Container, Value>;

	template<typename JsonMember>
	using json_result = typename JsonMember::parse_to_t;

	template<typename JsonMember>
	using json_base_type = typename JsonMember::base_type;

	template<std::size_t I, typename... JsonMembers>
	using json_result_n =
	  json_result<daw::traits::nth_element<I, JsonMembers...>>;

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
		inline constexpr void operator( )( Value &&value ) const {
			if constexpr( has_push_back_v<Container, daw::remove_cvref_t<Value>> ) {
				m_container->push_back( DAW_FWD( value ) );
			} else if constexpr( has_insert_end_v<Container,
			                                      daw::remove_cvref_t<Value>> ) {
				m_container->insert( std::end( *m_container ), DAW_FWD( value ) );
			} else {
				static_assert(
				  has_push_back_v<Container, daw::remove_cvref_t<Value>> or
				    has_insert_end_v<Container, daw::remove_cvref_t<Value>>,
				  "basic_appender requires a Container that either has push_back or "
				  "insert with the end iterator as first argument" );
			}
		}

		template<typename Value,
		         std::enable_if_t<
		           not std::is_same_v<basic_appender, daw::remove_cvref_t<Value>>,
		           std::nullptr_t> = nullptr>
		inline constexpr basic_appender &operator=( Value &&v ) {
			operator( )( DAW_FWD( v ) );
			return *this;
		}

		inline constexpr basic_appender &operator++( ) {
			return *this;
		}

		inline constexpr basic_appender operator++( int ) const {
			return *this;
		}

		inline constexpr basic_appender &operator*( ) {
			return *this;
		}
	};

	template<typename T>
	using json_parser_to_json_data_t =
	  decltype( daw::json::json_data_contract<T>::to_json_data(
	    std::declval<T &>( ) ) );

	template<typename T>
	static inline constexpr bool has_json_to_json_data_v =
	  daw::is_detected_v<json_parser_to_json_data_t, T>;

	template<typename T>
	using is_submember_tagged_variant_t =
	  typename json_data_contract<T>::type::i_am_a_submember_tagged_variant;

	template<typename T>
	static inline constexpr bool is_submember_tagged_variant_v =
	  daw::is_detected_v<is_submember_tagged_variant_t, T>;
} // namespace daw::json::json_details

namespace daw::json::json_details {
	template<typename>
	inline constexpr std::size_t parse_space_needed_v = 1U;

	template<>
	inline constexpr std::size_t parse_space_needed_v<simd_exec_tag> = 16U;

	template<typename JsonType>
	static inline constexpr bool is_json_nullable_v =
	  JsonType::expected_type == JsonParseTypes::Null;

	template<typename T>
	[[maybe_unused]] auto dereffed_type_impl( daw::tag_t<T> )
	  -> decltype( *( std::declval<T &>( ) ) );

	template<typename T>
	using dereffed_type =
	  daw::remove_cvref_t<decltype( dereffed_type_impl( daw::tag<T> ) )>;

	template<typename T, JsonNullable Nullable>
	using unwrap_type = std::conditional_t<
	  std::conjunction_v<std::bool_constant<Nullable == JsonNullable::Nullable>,
	                     daw::is_detected<dereffed_type, T>>,
	  daw::detected_t<dereffed_type, T>, T>;

	template<typename T>
	using can_deref = daw::is_detected<dereffed_type, T>;

	template<typename T>
	using cant_deref = daw::not_trait<can_deref<T>>;

	template<typename T>
	inline constexpr bool can_deref_v = can_deref<T>::value;

	template<typename T>
	inline constexpr JsonParseTypes number_parse_type_impl_v = [] {
		static_assert( daw::is_arithmetic_v<T>, "Unexpected non-number" );
		if constexpr( daw::is_floating_point_v<T> ) {
			return JsonParseTypes::Real;
		} else if constexpr( daw::is_signed_v<T> ) {
			return JsonParseTypes::Signed;
		} else if constexpr( daw::is_unsigned_v<T> ) {
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

} // namespace daw::json::json_details

namespace daw::json {
	template<typename T>
	struct TestInputIteratorType {
		using iterator_category = std::input_iterator_tag;
		using value_type = T;
		using reference = T &;
		using pointer = T *;
		using difference_type = std::ptrdiff_t;
	};
	static_assert(
	  std::is_same_v<
	    typename std::iterator_traits<TestInputIteratorType<int>>::value_type,
	    int> );
} // namespace daw::json
