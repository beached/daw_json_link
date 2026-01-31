// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include <daw/daw_cpp20_concept.h>

#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// @brief This class is used as a way to indicate that a json_data_contract
		/// specialization has not been done for a user class.
		template<typename>
		struct missing_json_data_contract_for_or_unknown_type;

		template<typename T>
		inline constexpr bool is_missing_data_contract_or_unknown_type_v = false;

		template<>
		inline constexpr bool is_missing_data_contract_or_unknown_type_v<void> =
		  true;

		template<typename T>
		inline constexpr bool is_missing_data_contract_or_unknown_type_v<
		  missing_json_data_contract_for_or_unknown_type<T>> = true;

		/// @brief Mapping class for JSON data structures to C++.  It must be
		/// specialized in order to parse to a user class
		/// @tparam T Class to map
		///
		template<typename T, typename = void>
		struct json_data_contract {
			using type = missing_json_data_contract_for_or_unknown_type<T>;
		};

		/// @brief This trait gets us the mapping type from the contract.
		template<typename T>
		using json_data_contract_trait_t = typename json_data_contract<T>::type;

		namespace json_details {
			template<typename T>
			DAW_CPP20_CONCEPT has_json_data_contract_trait_v =
			  not std::is_same_v<missing_json_data_contract_for_or_unknown_type<T>,
			                     json_data_contract_trait_t<T>>;

			DAW_JSON_MAKE_REQ_TRAIT(
			  has_json_to_json_data_v,
			  json_data_contract<T>::to_json_data( std::declval<T &>( ) ) );

			DAW_JSON_MAKE_REQ_TYPE_ALIAS_TRAIT(
			  is_submember_tagged_variant_v,
			  json_data_contract<T>::type::i_am_a_submember_tagged_variant );
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
