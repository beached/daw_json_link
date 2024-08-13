// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_describe
//

#pragma once

#include <daw/daw_consteval.h>
#include <daw/daw_traits.h>
#include <daw/json/daw_json_link.h>

#if not __has_include( <boost/describe.hpp> ) and __has_include( <boost/mp11.hpp> )
#error \
  "In order to use daw_json_link_describe.h Boost.Describe and Boost.Mp11 must be available"
#endif

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <cstddef>
#include <tuple>
#include <utility>

namespace daw::json {
	/// Types that use Boost.Describe need to specialize use_boost_describe_v for
	/// their type with a bool value of true.  This defaults to on, all described
	/// structs, but can be opted-out of if one wants to do a custom mapping of a
	/// Boost.Described struct
	template<typename, typename = void>
	inline constexpr bool use_boost_describe_v = true;

	namespace describe_impl {
		template<typename, typename>
		struct describe_member_impl;

		/// \brief We must use char arrays to store names for NTTP/CNTTP. Building
		/// json_link type too \tparam T Class member being mapped
		template<typename T, std::size_t... Is>
		struct describe_member_impl<T, std::index_sequence<Is...>> {
			static constexpr char const name[sizeof...( Is )]{ T::name[Is]... };
			using type =
			  json_link<name,
			            daw::traits::member_type_of_t<DAW_TYPEOF( T::pointer )>>;
		};

		template<typename T>
		using describe_member =
		  describe_member_impl<T, std::make_index_sequence<
		                            std::char_traits<char>::length( T::name ) + 1>>;

		template<typename>
		struct member_list;

		template<template<typename...> typename List, typename... Ts>
		struct member_list<List<Ts...>> {
			template<typename U>
			using desc_t = typename describe_member<U>::type;

			using type = json_member_list<desc_t<Ts>...>;
		};
	} // namespace describe_impl

	template<typename T>
	DAW_JSON_REQUIRES_20( boost::describe::has_describe_members<T>::value
	                        and use_boost_describe_v<T> )
	struct json_data_contract<
	  T DAW_JSON_REQUIRES_17B( boost::describe::has_describe_members<T>::value
	                             and use_boost_describe_v<T> )> {
	private:
		using pub_desc_t =
		  boost::describe::describe_members<T, boost::describe::mod_public>;
		using pri_desc_t =
		  boost::describe::describe_members<T, boost::describe::mod_private>;
		using pro_desc_t =
		  boost::describe::describe_members<T, boost::describe::mod_protected>;
		static_assert( boost::mp11::mp_empty<pri_desc_t>::value,
		               "Classes with private member variables are not supported. "
		               "Must use a manual mapping." );
		static_assert( boost::mp11::mp_empty<pro_desc_t>::value,
		               "Classes with protected member variables are not supported. "
		               "Must use a manual mapping." );

		template<typename U>
		using desc_t = typename describe_impl::describe_member<U>::type;

		template<template<typename...> typename List, typename... Ts>
		static inline constexpr auto
		to_json_data_impl( T const &value, List<Ts...> const & ) noexcept {
			return std::forward_as_tuple( value.*Ts::pointer... );
		}

	public:
		using type = typename describe_impl::member_list<pub_desc_t>::type;

		static constexpr auto to_json_data( T const &value ) noexcept {
			return to_json_data_impl( value, pub_desc_t{ } );
		}
	};
} // namespace daw::json
