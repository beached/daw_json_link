// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <type_traits>

/***
 * Public traits that can be opted into
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

	/***
	 * This trait can be specialized such that when class being returned has
	 * non-move/copyiable members the construction can be done with { } instead of
	 * a callable.  This is a blunt object and probably should not be used
	 * @tparam T type to specialize
	 */
	template<typename T>
	struct force_aggregate_constrution : std::false_type {};

} // namespace daw::json
