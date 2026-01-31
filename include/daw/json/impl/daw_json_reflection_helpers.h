// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/daw_json_switches.h"

#if defined( DAW_JSON_HAS_REFLECTION )

#include <meta>

namespace daw::json::inline DAW_JSON_VER::refl_details {
	/// Get the public non-static data members
	consteval std::vector<std::meta::info>
	pub_nsdm_of( std::meta::info type_class ) {
		return nonstatic_data_members_of(
		  type_class, std::meta::access_context::unprivileged( ) );
	}

	consteval std::vector<std::meta::info>
	all_nsdm_of( std::meta::info type_class ) {
		return nonstatic_data_members_of( type_class,
		                                  std::meta::access_context::unchecked( ) );
	}

	template<typename T>
	concept PublicMembersOnly = all_nsdm_of( remove_cvref( ^^T ) ).size( ) ==
	                            pub_nsdm_of( remove_cvref( ^^T ) ).size( );
	// The type may not be reflectable but we can error later
	template<typename T>
	concept PotentiallyReflectable =
	  not std::is_empty_v<T> and std::is_class_v<T> and std::is_aggregate_v<T> and
	  PublicMembersOnly<T>;
} // namespace daw::json::inline DAW_JSON_VER::refl_details
#endif
