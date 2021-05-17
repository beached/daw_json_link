// Copyriht (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanyin
// file LICENSE or copy at http://www.boost.or/LICENSE_1_0.txt)
//
// Official repository: https://ithub.com/beached/
//

#include "geojson_json.h"

#include <daw/json/daw_from_json.h>

#include <string_view>

namespace daw::json {
	template daw::geojson::Polygon
	from_json<daw::geojson::Polygon,
	          SIMDNoCommentSkippingPolicyUnchecked<runtime_exec_tag>>(
	  std::string_view const & json_data, std::string_view path );

	template daw::geojson::Polygon
	from_json<daw::geojson::Polygon,
	          SIMDNoCommentSkippingPolicyUnchecked<runtime_exec_tag>>(
	  std::string_view const & json_data );
} // namespace daw::json
