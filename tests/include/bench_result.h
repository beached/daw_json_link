// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_json_link.h>

#include <chrono>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace daw::bench {
	using timestamp_t = std::chrono::time_point<std::chrono::system_clock,
	                                            std::chrono::milliseconds>;
	struct bench_result {
		std::string name;
		timestamp_t test_time;
		std::size_t data_size;
		std::vector<std::chrono::nanoseconds> run_times{ };
		std::chrono::nanoseconds duration_min;
		std::chrono::nanoseconds duration_25th_percentile;
		std::chrono::nanoseconds duration_50th_percentile;
		std::chrono::nanoseconds duration_75th_percentile;
		std::chrono::nanoseconds duration_max;
		std::string git_revision;
		std::string processor_description;
		std::string os_name;
		std::string os_release;
		std::string os_version;
		std::string os_platform;
		std::string build_type;
		std::string project_name;
		std::string project_subname;
	};
} // namespace daw::bench

struct JSONToNano {
	constexpr std::chrono::nanoseconds operator( )( std::string_view sv ) const {
		auto rng = daw::json::NoCommentSkippingPolicyChecked(
		  sv.data( ), sv.data( ) + sv.size( ) );
		return std::chrono::nanoseconds(
		  daw::json::json_details::parse_value<
		    daw::json::json_number<daw::json::no_name, long long>>(
		    daw::json::ParseTag<daw::json::JsonParseTypes::Signed>{ }, rng ) );
	}

	std::string operator( )( std::chrono::nanoseconds t ) const {
		return std::to_string( t.count( ) );
	}
};

namespace daw::json {
	template<JSONNAMETYPE Name>
	using json_nanosecond =
	  json_custom<Name, std::chrono::nanoseconds, JSONToNano, JSONToNano,
	              CustomJsonTypes::Literal>;

	template<>
	struct json_data_contract<daw::bench::bench_result> {
		static inline constexpr char const name[] = "name";
		static inline constexpr char const test_time[] = "test_time";
		static inline constexpr char const data_size[] = "data_size";
		static inline constexpr char const run_times[] = "run_times_ns";
		static inline constexpr char const duration_min[] = "duration_min";
		static inline constexpr char const duration_25th_percentile[] =
		  "duration_25th_percentile";
		static inline constexpr char const duration_50th_percentile[] =
		  "duration_50th_percentile";
		static inline constexpr char const duration_75th_percentile[] =
		  "duration_75th_percentile";
		static inline constexpr char const duration_max[] = "duration_max";
		static inline constexpr char const git_revision[] = "git_revision";
		static inline constexpr char const processor_description[] =
		  "processor_description";
		static inline constexpr char const os_name[] = "os_name";
		static inline constexpr char const os_release[] = "os_release";
		static inline constexpr char const os_version[] = "os_version";
		static inline constexpr char const os_platform[] = "os_platform";
		static inline constexpr char const build_type[] = "build_type";
		static inline constexpr char const project_name[] = "project_name";
		static inline constexpr char const project_subname[] = "project_subname";
		using type = json_member_list<
		  json_string<name>, json_date<test_time>,
		  json_number<data_size, std::size_t>,
		  json_array<run_times, json_nanosecond<no_name>>,
		  json_nanosecond<duration_min>, json_nanosecond<duration_25th_percentile>,
		  json_nanosecond<duration_50th_percentile>,
		  json_nanosecond<duration_75th_percentile>, json_nanosecond<duration_max>,
		  json_string<git_revision>, json_string<processor_description>,
		  json_string<os_name>, json_string<os_release>, json_string<os_version>,
		  json_string<os_platform>, json_string<build_type>,
		  json_string<project_name>, json_string<project_subname>>;

		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::bench::bench_result const &value ) {
			return std::tie(
			  value.name, value.test_time, value.data_size, value.run_times,
			  value.duration_min, value.duration_25th_percentile,
			  value.duration_50th_percentile, value.duration_75th_percentile,
			  value.duration_max, value.git_revision, value.processor_description,
			  value.os_name, value.os_release, value.os_version, value.os_platform,
			  value.build_type, value.project_name, value.project_subname );
		}
	};
} // namespace daw::json
