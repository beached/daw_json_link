// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/daw_tuple_forward.h"
#include "daw_json_link.h"

#include <daw/daw_attributes.h>
#include <daw/daw_consteval.h>

#include <boost/pfr.hpp>
#include <daw/stdinc/tuple_traits.h>
#include <type_traits>
#include <utility>

namespace daw::json {
	/// \brief Enable Boost PFR mappings for a type
	/// \tparam T
	template<typename T>
	requires( std::is_aggregate_v<T> and std::is_class_v<T> and
	          not std::is_empty_v<T> ) //
	  inline constexpr bool use_boost_pfr = false;

	namespace pfr_details {
		template<typename T, std::size_t Idx>
		DAW_CONSTEVAL auto get_member_name( ) {
			constexpr auto name = boost::pfr::get_name<Idx, T>( );
			static_assert( not name.empty( ) );
			return json_name<name.size( ) + 1>(
			  name.data( ), std::make_index_sequence<name.size( ) + 1>{ } );
		}

		template<typename, typename>
		struct make_data_contract;

		template<typename T, std::size_t... Is>
		struct make_data_contract<T, std::index_sequence<Is...>> {
		private:
			using member_types = DAW_TYPEOF(
			  boost::pfr::structure_to_tuple( std::declval<T const &>( ) ) );

		public:
			using type =
			  json_member_list<json_link<get_member_name<T, Is>( ),
			                             std::tuple_element_t<Is, member_types>>...>;

			DAW_ATTRIB_INLINE static constexpr auto to_json_data( T const &value ) {
				return boost::pfr::structure_tie( value );
			}
		};
	} // namespace pfr_details

	template<typename T>
	requires use_boost_pfr<T> //
	  struct json_data_contract<T>
	  : pfr_details::make_data_contract<
	      T, std::make_index_sequence<boost::pfr::tuple_size_v<T>>> {};
} // namespace daw::json
