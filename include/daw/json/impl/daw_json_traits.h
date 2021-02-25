// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_move.h>
#include <daw/daw_traits.h>

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

/***
 * Customization point traits
 *
 */
namespace daw::json {

	/***
	 * This class is used as a way to indicate that a json_data_contract
	 * specialization has not been done for a user class.
	 */
	template<typename>
	struct missing_json_data_contract_for {};

	/***
	 * Mapping class for JSON data structures to C++.  It must be specialized in
	 * order to parse to a user class
	 * @tparam T Class to map
	 */
	template<typename T>
	struct json_data_contract {
		using type = missing_json_data_contract_for<T>;
	};

	template<typename T>
	using json_data_contract_trait_t =
	  typename daw::json::json_data_contract<T>::type;

	/***
	 * This trait can be specialized such that when class being returned has
	 * non-move/copyable members the construction can be done with { } instead of
	 * a callable.  This is a blunt object and probably should not be used
	 * @tparam T type to specialize
	 */
	template<typename T>
	struct force_aggregate_construction : std::false_type {};

	/***
	 * Default Constructor for a type.  It accounts for aggregate types and uses
	 * brace construction for them
	 * @tparam T type to construct
	 */
	template<typename T>
	struct default_constructor {
		template<typename... Args>
		[[nodiscard]] inline constexpr auto operator( )( Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<T, Args...> )
		    -> std::enable_if_t<std::is_constructible_v<T, Args...>, T> {

			return T( DAW_FWD( args )... );
		}

		template<typename... Args>
		[[nodiscard]] inline constexpr auto operator( )( Args &&...args ) const
		  noexcept( daw::traits::is_nothrow_list_constructible_v<T, Args...> )
		    -> std::enable_if_t<(not std::is_constructible_v<T, Args...> and
		                         daw::traits::is_list_constructible_v<T, Args...>),
		                        T> {

			return T{ DAW_FWD( args )... };
		}
	};

	template<typename K, typename V, typename H, typename E, typename Alloc>
	struct default_constructor<std::unordered_map<K, V, H, E, Alloc>> {
		inline std::unordered_map<K, V, H, E, Alloc> operator( )( ) const
		  noexcept( noexcept( std::unordered_map<K, V, H, E, Alloc>( ) ) ) {
			return { };
		}

		template<typename Iterator>
		inline std::unordered_map<K, V, H, E, Alloc>
		operator( )( Iterator first, Iterator last,
		             Alloc const &alloc = Alloc{ } ) const
		  noexcept( noexcept( std::unordered_map<K, V, H, E, Alloc>( first, last, 0,
		                                                             H{ }, E{ },
		                                                             alloc ) ) ) {
			return std::unordered_map<K, V, H, E, Alloc>( first, last, 0, H{ }, E{ },
			                                              alloc );
		}
	};

	/***
	 * Auto generated constructor for nullable types.
	 * Specializations must accept accept an operator( )( ) that signifies a JSON
	 * null. Any other arguments only need to be valid to construct the type.
	 */
	template<typename T>
	struct nullable_constructor : default_constructor<T> {};

	template<typename T>
	struct nullable_constructor<std::optional<T>> {
		using value_type = T;

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN constexpr std::optional<T>
		operator( )( ) const noexcept {
			return std::optional<T>( );
		}

		template<typename... Args>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr auto
		operator( )( Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<std::optional<T>,
		                                            std::in_place_t, Args...> )
		    -> std::enable_if_t<
		      (( sizeof...( Args ) > 0 ) and
		       std::is_constructible_v<T, std::in_place_t, Args...>),
		      std::optional<T>> {

			return std::optional<T>( std::in_place, DAW_FWD( args )... );
		}

		template<typename... Args>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr auto
		operator( )( Args &&...args ) const
		  noexcept( daw::traits::is_nothrow_list_constructible_v<T, Args...>
		              and std::is_nothrow_move_constructible_v<T> )
		    -> std::enable_if_t<
		      (( sizeof...( Args ) > 0 ) and
		       not std::is_constructible_v<T, std::in_place_t, Args...> and
		       daw::traits::is_list_constructible_v<T, Args...>),
		      std::optional<T>> {

			return std::optional<T>( T{ DAW_FWD( args )... } );
		}
	};

	template<typename T, typename Deleter>
	struct nullable_constructor<std::unique_ptr<T, Deleter>> {
		using value_type = T;

		constexpr std::unique_ptr<T, Deleter> operator( )( ) const noexcept {
			return std::unique_ptr<T, Deleter>{ };
		}

		template<typename... Args>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline auto
		operator( )( Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<T, Args...> )
		    -> std::enable_if_t<(sizeof...( Args ) > 0 and
		                         std::is_constructible_v<T, Args...>),
		                        std::unique_ptr<T, Deleter>> {

			return std::unique_ptr<T, Deleter>( new T( DAW_FWD( args )... ) );
		}

		template<typename... Args>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline auto
		operator( )( Args &&...args ) const
		  noexcept( daw::traits::is_nothrow_list_constructible_v<T, Args...> )
		    -> std::enable_if_t<(( sizeof...( Args ) > 0 ) and
		                         not std::is_constructible_v<T, Args...> and
		                         daw::traits::is_list_constructible_v<T, Args...>),
		                        std::unique_ptr<T, Deleter>> {

			return std::unique_ptr<T, Deleter>( new T{ DAW_FWD( args )... } );
		}
	};

	/***
	 * Can use the fast, pseudo random string iterators.  They are InputIterators
	 * with an operator- that allows for O(1) distance calculations as we often
	 * know the length but cannot provide random access.  For types that only use
	 * InputIterator operations and last - first for distance calc
	 */
	template<typename>
	struct can_single_allocation_string : std::false_type {};

	template<typename Char, typename CharTrait, typename Allocator>
	struct can_single_allocation_string<
	  std::basic_string<Char, CharTrait, Allocator>> : std::true_type {};

	namespace json_details {
		template<typename T>
		using json_type_t = typename T::i_am_a_json_type;

		template<typename T>
		inline constexpr bool is_a_json_type_v = daw::is_detected_v<json_type_t, T>;

		template<typename T>
		using ordered_member_t = typename T::i_am_an_ordered_member;

		template<typename T>
		inline constexpr bool is_an_ordered_member_v =
		  daw::is_detected_v<ordered_member_t, T>;

		template<typename T>
		using is_a_json_tagged_variant_test =
		  typename T::i_am_a_json_tagged_variant;

		template<typename T>
		inline constexpr bool is_a_json_tagged_variant_v =
		  daw::is_detected_v<is_a_json_tagged_variant_test, T>;

		template<typename T>
		using must_verify_end_of_data_is_valid_t =
		  typename T::must_verify_end_of_data_is_valid;

		/// If the parse policy has a type alias `must_verify_end_of_data_is_valid`
		/// this trait is true.  This is used to ensure that after the JSON value is
		/// parse that only whitespace or nothing follows \tparam ParsePolicy Parser
		/// Policy Type
		template<typename ParsePolicy>
		inline constexpr bool must_verify_end_of_data_is_valid_v =
		  not ParsePolicy::is_unchecked_input and
		  daw::is_detected_v<must_verify_end_of_data_is_valid_t, ParsePolicy>;
	} // namespace json_details
} // namespace daw::json
