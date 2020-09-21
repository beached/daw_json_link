// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <string_view>
#include <tuple>
#include <vector>

#ifdef __GNUC__
#define DAW_HIDDEN __attribute__( ( visibility( "hidden" ) ) )
#else
#define DAW_HIDDEN
#endif

namespace apache_builds {
	struct jobs_t {
		std::string_view name;
		std::string_view url;
		std::string_view color;
	};

	struct views_t {
		std::string_view name;
		std::string_view url;
	};

	struct apache_builds {
		std::string_view mode;
		std::string_view nodeDescription;
		std::string_view nodeName;
		int64_t numExecutors;
		std::string_view description;
		std::vector<jobs_t> jobs;
		views_t primaryView;
		bool quietingDown;
		int64_t slaveAgentPort;
		bool useCrumbs;
		bool useSecurity;
		std::vector<views_t> views;
	};
} // namespace apache_builds

namespace daw::json {
	template<>
	struct DAW_HIDDEN json_data_contract<apache_builds::jobs_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_string_raw<"name", std::string_view>,
		                              json_string_raw<"url", std::string_view>,
		                              json_string_raw<"color", std::string_view>>;
#else
		static inline constexpr char const name[] = "name";
		static inline constexpr char const url[] = "url";
		static inline constexpr char const color[] = "color";
		using type = json_member_list<json_string_raw<name, std::string_view>,
		                              json_string_raw<url, std::string_view>,
		                              json_string_raw<color, std::string_view>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( apache_builds::jobs_t const &value ) {
			return std::forward_as_tuple( value.name, value.url, value.color );
		}
	};

	template<>
	struct DAW_HIDDEN json_data_contract<apache_builds::views_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_string_raw<"name", std::string_view>,
		                              json_string_raw<"url", std::string_view>>;
#else
		static inline constexpr char const name[] = "name";
		static inline constexpr char const url[] = "url";
		using type = json_member_list<json_string_raw<name, std::string_view>,
		                              json_string_raw<url, std::string_view>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( apache_builds::views_t const &value ) {
			return std::forward_as_tuple( value.name, value.url );
		}
	};

	template<>
	struct DAW_HIDDEN json_data_contract<apache_builds::apache_builds> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string_raw<"mode", std::string_view>,
		                   json_string_raw<"nodeDescription", std::string_view>,
		                   json_string_raw<"nodeName", std::string_view>,
		                   json_number<"numExecutors", int64_t>,
		                   json_string_raw<"description", std::string_view>,
		                   json_array<"jobs", apache_builds::jobs_t>,
		                   json_class<"primaryView", apache_builds::views_t>,
		                   json_bool<"quietingDown">,
		                   json_number<"slaveAgentPort", int64_t>,
		                   json_bool<"useCrumbs">, json_bool<"useSecurity">,
		                   json_array<"views", apache_builds::views_t>>;
#else
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
		  json_string_raw<mode, std::string_view>,
		  json_string_raw<nodeDescription, std::string_view>,
		  json_string_raw<nodeName, std::string_view>,
		  json_number<numExecutors, int64_t>,
		  json_string_raw<description, std::string_view>,
		  json_array<jobs, apache_builds::jobs_t>,
		  json_class<primaryView, apache_builds::views_t>, json_bool<quietingDown>,
		  json_number<slaveAgentPort, int64_t>, json_bool<useCrumbs>,
		  json_bool<useSecurity>, json_array<views, apache_builds::views_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( apache_builds::apache_builds const &value ) {
			return std::forward_as_tuple(
			  value.mode, value.nodeDescription, value.nodeName, value.numExecutors,
			  value.description, value.jobs, value.primaryView, value.quietingDown,
			  value.slaveAgentPort, value.useCrumbs, value.useSecurity, value.views );
		}
	};

} // namespace daw::json
