// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace daw::citm {
	struct events_value_t {
		std::int64_t id;
		std::optional<std::string_view> logo;
		std::string_view name;
		std::vector<std::int64_t> subTopicIds;
		std::vector<std::int64_t> topicIds;
	}; // events_value_t

	struct prices_element_t {
		std::int64_t amount;
		std::int64_t audienceSubCategoryId;
		std::int64_t seatCategoryId;
	}; // prices_element_t

	struct areas_element_t {
		std::int64_t areaId;
	}; // areas_element_t

	struct seatCategories_element_t {
		std::vector<areas_element_t> areas;
		std::int64_t seatCategoryId;
	}; // seatCategories_element_t

	struct performances_element_t {
		std::int64_t eventId;
		std::int64_t id;
		std::optional<std::string_view> logo;
		std::vector<prices_element_t> prices;
		std::vector<seatCategories_element_t> seatCategories;
		std::int64_t start;
		std::string_view venueCode;
	}; // performances_element_t

	struct venueNames_t {
		std::string_view pleyel_pleyel;
	}; // venueNames_t

	struct citm_object_t {
		std::unordered_map<std::int64_t, std::string_view> areaNames;
		std::unordered_map<std::int64_t, std::string_view> audienceSubCategoryNames;
		std::unordered_map<std::int64_t, events_value_t> events;
		std::vector<performances_element_t> performances;
		std::unordered_map<std::string_view, std::string_view> seatCategoryNames;
		std::unordered_map<std::string_view, std::string_view> subTopicNames;
		std::unordered_map<std::string_view, std::string_view> topicNames;
		std::unordered_map<std::string_view, std::vector<std::int64_t>>
		  topicSubTopics;
		std::optional<venueNames_t> venueNames;
	}; // citm_object_t
} // namespace daw::citm

