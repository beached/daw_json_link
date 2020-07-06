// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

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

} // namespace daw::json
