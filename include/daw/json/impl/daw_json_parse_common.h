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

	template<typename Value, typename Constructor, typename Range,
	         typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr auto
	construct_value_tp( Constructor &&ctor, Range &rng,
	                    std::tuple<Args...> &&tp_args ) {
		if constexpr( Range::has_allocator ) {
			using alloc_t = typename Range::template allocator_type_as<Value>;
			auto alloc = rng.template get_allocator_for<Value>( );
			if constexpr( std::is_invocable_v<Constructor, Args..., alloc_t> ) {
				return std::apply(
				  ctor, std::tuple_cat( daw::move( tp_args ),
				                        std::forward_as_tuple( daw::move( alloc ) ) ) );
			} else if constexpr( daw::traits::is_callable_v<Constructor,
			                                                std::allocator_arg_t,
			                                                alloc_t, Args...> ) {
				return std::apply(
				  ctor, std::tuple_cat( std::forward_as_tuple( std::allocator_arg,
				                                               daw::move( alloc ) ),
				                        daw::move( tp_args ) ) );
			} else {
				static_assert( std::is_invocable_v<Constructor, Args...> );
				return std::apply( ctor, daw::move( tp_args ) );
			}
		} else {
			static_assert( std::is_invocable_v<Constructor, Args...> );
			return std::apply( ctor, daw::move( tp_args ) );
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
	inline constexpr bool has_json_to_json_data_v =
	  daw::is_detected_v<json_parser_to_json_data_t, T>;

	template<typename T>
	using is_submember_tagged_variant_t =
	  typename json_data_contract<T>::type::i_am_a_submember_tagged_variant;

	template<typename T>
	inline constexpr bool is_submember_tagged_variant_v =
	  daw::is_detected_v<is_submember_tagged_variant_t, T>;

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
} // namespace daw::json::json_details

namespace daw::json {
	/**
	 * Link to a JSON class
	 * @tparam Name name of JSON member to link to
	 * @tparam T type that has specialization of
	 * daw::json::json_data_contract
	 * @tparam Constructor A callable used to construct T.  The
	 * default supports normal and aggregate construction
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T,
	         typename Constructor = default_constructor<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_class;

	/**
	 * The member is a number
	 * @tparam Name name of json member
	 * @tparam T type of number(e.g. double, int, unsigned...) to pass to
	 * Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam RangeCheck Check if the value will fit in the result
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = default_constructor<T>,
	         JsonRangeCheck RangeCheck = JsonRangeCheck::Never,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_number;

	/**
	 * The member is a boolean
	 * @tparam Name name of json member
	 * @tparam T result type to pass to Constructor
	 * @tparam LiteralAsString Could this number be embedded in a string
	 * @tparam Constructor Callable used to construct result
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename T = bool,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::Never,
	         typename Constructor = default_constructor<T>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_bool;

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *,
	 * std::size_t )
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename String = std::string,
	         typename Constructor = default_constructor<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_string;

	/**
	 * Member is an escaped string and requires unescaping and escaping of string
	 * data
	 * @tparam Name of json member
	 * @tparam String result type constructed by Constructor
	 * @tparam Constructor a callable taking as arguments ( char const *,
	 * std::size_t )
	 * @tparam EmptyStringNull if string is empty, call Constructor with no
	 * arguments
	 * @tparam EightBitMode Allow filtering of characters with the MSB set
	 * arguments
	 * @tparam Nullable Can the member be missing or have a null value
	 * @tparam AllowEscape Tell parser if we know a \ or escape will be in the
	 * data
	 */
	template<JSONNAMETYPE Name, typename String = std::string,
	         typename Constructor = default_constructor<String>,
	         JsonNullable EmptyStringNull = JsonNullable::Never,
	         EightBitModes EightBitMode = EightBitModes::AllowFull,
	         JsonNullable Nullable = JsonNullable::Never,
	         AllowEscapeCharacter AllowEscape = AllowEscapeCharacter::Allow>
	struct json_string_raw;

} // namespace daw::json

namespace daw::json::json_details {
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
	template<typename>
	struct json_link_basic_type_map {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Unknown;
	};

	template<>
	struct json_link_basic_type_map<daw::string_view> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::StringRaw;
	};

	template<>
	struct json_link_basic_type_map<std::string_view> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::StringRaw;
	};

	template<>
	struct json_link_basic_type_map<std::string> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::StringEscaped;
	};

	template<>
	struct json_link_basic_type_map<bool> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Bool;
	};

	template<>
	struct json_link_basic_type_map<short> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Signed;
	};

	template<>
	struct json_link_basic_type_map<int> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Signed;
	};

	template<>
	struct json_link_basic_type_map<long> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Signed;
	};

	template<>
	struct json_link_basic_type_map<long long> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Signed;
	};

	template<>
	struct json_link_basic_type_map<unsigned short> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Unsigned;
	};

	template<>
	struct json_link_basic_type_map<unsigned int> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Unsigned;
	};

	template<>
	struct json_link_basic_type_map<unsigned long> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Unsigned;
	};

	template<>
	struct json_link_basic_type_map<unsigned long long> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Unsigned;
	};

	template<>
	struct json_link_basic_type_map<float> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Real;
	};

	template<>
	struct json_link_basic_type_map<double> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::Real;
	};
} // namespace daw::json

namespace daw::json::json_details {
	template<typename T>
	inline constexpr bool has_basic_type_map_v =
	  daw::is_detected_v<::daw::json::json_link_basic_type_map, T>;

	template<typename Mapped, bool Found = true>
	struct json_link_quick_map_type : std::bool_constant<Found> {
		using mapped_type = Mapped;
	};

	template<JSONNAMETYPE Name, typename T>
	DAW_ATTRIBUTE_HIDDEN inline constexpr auto json_link_quick_map( ) {
		if constexpr( has_basic_type_map_v<T> ) {
			constexpr auto mapped_type = json_link_basic_type_map<T>::parse_type;
			if constexpr( mapped_type == JsonParseTypes::StringRaw ) {
				return json_link_quick_map_type<json_string_raw<Name, T>>{ };
			} else if constexpr( mapped_type == JsonParseTypes::StringEscaped ) {
				return json_link_quick_map_type<json_string<Name, T>>{ };
			} else if constexpr( mapped_type == JsonParseTypes::Bool ) {
				return json_link_quick_map_type<json_bool<Name, T>>{ };
			} else if constexpr( mapped_type == JsonParseTypes::Signed ) {
				return json_link_quick_map_type<json_number<Name, T>>{ };
			} else if constexpr( mapped_type == JsonParseTypes::Unsigned ) {
				return json_link_quick_map_type<json_number<Name, T>>{ };
			} else if constexpr( mapped_type == JsonParseTypes::Real ) {
				return json_link_quick_map_type<json_number<Name, T>>{ };
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
	  decltype( json_link_quick_map<no_name, T>( ) )::value;

	/***
	 * Get the quick mapped json type for type T
	 */
	template<JSONNAMETYPE Name, typename T>
	using json_link_quick_map_t =
	  typename decltype( json_link_quick_map<Name, T>( ) )::mapped_type;

	namespace vector_detect {
		struct not_vector {};
		template<typename T, typename... Alloc>
		[[maybe_unused]] auto vector_test( daw::tag_t<std::vector<T, Alloc...>> )
		  -> T;

		template<typename T>
		[[maybe_unused]] not_vector vector_test( daw::tag_t<T> );

		template<typename T>
		using detector =
		  std::remove_reference_t<decltype( vector_test( daw::tag<T> ) )>;
	} // namespace vector_detect

	template<typename T>
	using is_vector = daw::not_trait<
	  std::is_same<vector_detect::detector<T>, vector_detect::not_vector>>;
	/** Link to a JSON array that has been detected
	 * @tparam Name name of JSON member to link to
	 * @tparam Container type of C++ container being constructed(e.g.
	 * vector<int>)
	 * @tparam JsonElement Json type being parsed e.g. json_number,
	 * json_string...
	 * @tparam Constructor A callable used to make Container,
	 * default will use the Containers constructor.  Both normal and aggregate
	 * are supported
	 * @tparam Nullable Can the member be missing or have a null value
	 */
	template<JSONNAMETYPE Name, typename JsonElement, typename Container,
	         typename Constructor = default_constructor<Container>,
	         JsonNullable Nullable = JsonNullable::Never>
	struct json_array_detect;

	template<typename T, JSONNAMETYPE Name = no_name>
	using unnamed_default_type_mapping = daw::if_t<
	  json_details::is_a_json_type_v<T>, T,
	  daw::if_t<
	    has_json_data_contract_trait_v<T>,
	    json_class<Name, T, json_class_constructor_t<T>>,
	    daw::if_t<
	      has_json_link_quick_map_v<T>, json_link_quick_map_t<Name, T>,
	      daw::if_t<
	        std::disjunction_v<daw::is_arithmetic<T>, std::is_enum<T>>,
	        json_number<Name, T>,
	        daw::if_t<std::conjunction_v<cant_deref<T>, is_vector<T>>,
	                  json_array_detect<Name, vector_detect::detector<T>, T>,
	                  daw::json::missing_json_data_contract_for<T>>>>>>;

	template<typename T>
	using has_unnamed_default_type_mapping =
	  daw::not_trait<std::is_same<unnamed_default_type_mapping<T>,
	                              daw::json::missing_json_data_contract_for<T>>>;

	template<typename T>
	inline constexpr bool has_unnamed_default_type_mapping_v =
	  has_unnamed_default_type_mapping<T>::value;

	template<typename JsonMember>
	using from_json_result_t =
	  json_result<unnamed_default_type_mapping<JsonMember>>;

	template<typename Constructor, typename... Members>
	using json_class_parse_result_impl2 = decltype(
	  Constructor{ }( std::declval<typename Members::parse_to_t &&>( )... ) );

	template<typename Constructor, typename... Members>
	using json_class_parse_result_impl =
	  daw::detected_t<json_class_parse_result_impl2, Constructor, Members...>;

	template<typename Constructor, typename... Members>
	using json_class_parse_result_t =
	  daw::remove_cvref_t<json_class_parse_result_impl<Constructor, Members...>>;

	template<typename Constructor, typename... Members>
	inline constexpr bool can_defer_construction_v =
	  std::is_invocable_v<Constructor, typename Members::parse_to_t...>;
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
