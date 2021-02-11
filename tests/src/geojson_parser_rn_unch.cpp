// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include "geojson_json.h"
#include "json_firewall.h"

#include <daw/json/daw_json_link.h>

using policy_t =
  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::runtime_exec_tag>;

using result1_t = daw::geojson::Polygon;
template<>
result1_t daw::parse_json_data<result1_t, policy_t>( std::string_view json_doc,
                                                     std::string_view path ) {
	return daw::json::from_json<result1_t, policy_t>( json_doc, path );
}
using result2_t = daw::geojson::FeatureCollection;
template<>
result2_t daw::parse_json_data<result2_t, policy_t>( std::string_view json_doc,
                                                     std::string_view path ) {
	return daw::json::from_json<result2_t, policy_t>( json_doc, path );
}
