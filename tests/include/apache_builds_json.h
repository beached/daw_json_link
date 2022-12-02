// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "apache_builds.h"

#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <string_view>
#include <tuple>
#include <vector>

namespace daw::json {
	template<>
	struct json_data_contract<apache_builds::jobs_t> {
		static inline constexpr char const name[] = "name";
		static inline constexpr char const url[] = "url";
		static inline constexpr char const color[] = "color";
		using type = json_member_list<json_link<name, std::string_view>,
		                              json_link<url, std::string_view>,
		                              json_link<color, std::string_view>>;

		[[nodiscard]] static constexpr auto
		to_json_data( apache_builds::jobs_t const &value ) {
			return std::forward_as_tuple( value.name, value.url, value.color );
		}
	};
#if defined( DAW_JSON_HAS_CPP20_CX_VECTOR )
	static_assert( from_json<apache_builds::jobs_t>(
	                 R"json({
	"name": "bob",
	"url": "www.example.com",
	"color": "red"
})json" )
	                 .color == "red" );
#endif

	template<>
	struct json_data_contract<apache_builds::views_t> {
		static inline constexpr char const name[] = "name";
		static inline constexpr char const url[] = "url";
		struct Foo {};
		using type = json_member_list<json_link<name, std::string_view>,
		                              json_link<url, std::string_view>>;

		[[nodiscard]] static constexpr auto
		to_json_data( apache_builds::views_t const &value ) {
			return std::forward_as_tuple( value.name, value.url );
		}
	};
#if defined( DAW_JSON_HAS_CPP20_CX_VECTOR )
	static_assert( daw::json::from_json<apache_builds::views_t>( R"json({
	"name": "bob",
	"url": "www.example.com"
})json" )
	                 .url == "www.example.com" );
#endif

	template<>
	struct json_data_contract<apache_builds::apache_builds> {
		static inline constexpr char const mode[] = "mode";
		static inline constexpr char const nodeDescription[] = "nodeDescription";
		static inline constexpr char const nodeName[] = "nodeName";
		static inline constexpr char const numExecutors[] = "numExecutors";
		static inline constexpr char const description[] = "description";
		static inline constexpr char const jobs[] = "jobs";
		static inline constexpr char const primaryView[] = "primaryView";
		static inline constexpr char const quietingDown[] = "quietingDown";
		static inline constexpr char const slaveAgentPort[] = "slaveAgentPort";
		static inline constexpr char const useCrumbs[] = "useCrumbs";
		static inline constexpr char const useSecurity[] = "useSecurity";
		static inline constexpr char const views[] = "views";

		using type = json_member_list<
		  json_link<mode, std::string_view>,
		  json_link<nodeDescription, std::string_view>,
		  json_link<nodeName, std::string_view>, json_number<numExecutors, int64_t>,
		  json_link<description, std::string_view>,
		  json_link<jobs, std::vector<apache_builds::jobs_t>>,
		  json_link<primaryView, apache_builds::views_t>,
		  json_link<quietingDown, bool>, json_link<slaveAgentPort, int64_t>,
		  json_link<useCrumbs, bool>, json_link<useSecurity, bool>,
		  json_link<views, std::vector<apache_builds::views_t>>>;

		[[nodiscard]] static inline DAW_JSON_CX_VECTOR auto
		to_json_data( apache_builds::apache_builds const &value ) {
			return std::forward_as_tuple(
			  value.mode, value.nodeDescription, value.nodeName, value.numExecutors,
			  value.description, value.jobs, value.primaryView, value.quietingDown,
			  value.slaveAgentPort, value.useCrumbs, value.useSecurity, value.views );
		}
	};
//#if defined( DAW_JSON_HAS_CPP20_CX_VECTOR )
//	static_assert( daw::json::from_json<apache_builds::apache_builds>( R"json({
//	"mode": "simple",
//	"nodeDescription": "xyz is a ...",
//	"nodeName": "bobby tables",
//	"numExecutors": 5,
//	"description": "find a new name for bobby",
//	"jobs": [],
//	"primaryView": {
//		"name": "hank",
//		"url": "www.example.com",
//	},
//	"quietingDown": false,
//	"slaveAgentPort": 5555,
//	"useCrumbs": true,
//	"useSecurity": true,
//	"views": []
//})json" )
//	                 .useSecurity );
//#endif
} // namespace daw::json
