// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "fixed_alloc.h"

#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace daw::citm {
	template<typename T>
	using Vector = std::vector<T, fixed_allocator<T>>;
	template<typename K, typename V>
	using Map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>,
	                               fixed_allocator<std::pair<K const, V>>>;

	struct events_value_t {
		std::int64_t id;
		std::optional<std::string_view> logo;
		std::string_view name;
		Vector<std::int64_t> subTopicIds;
		Vector<std::int64_t> topicIds;
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
		Vector<areas_element_t> areas;
		std::int64_t seatCategoryId;
	}; // seatCategories_element_t

	struct performances_element_t {
		std::int64_t eventId;
		std::int64_t id;
		std::optional<std::string_view> logo;
		Vector<prices_element_t> prices;
		Vector<seatCategories_element_t> seatCategories;
		std::int64_t start;
		std::string_view venueCode;
	}; // performances_element_t

	struct venueNames_t {
		std::string_view pleyel_pleyel;
	}; // venueNames_t

	struct citm_object_t {
		Map<std::int64_t, std::string_view> areaNames;
		Map<std::int64_t, std::string_view> audienceSubCategoryNames;
		Map<std::int64_t, events_value_t> events;
		Vector<performances_element_t> performances;
		Map<std::string_view, std::string_view> seatCategoryNames;
		Map<std::string_view, std::string_view> subTopicNames;
		Map<std::string_view, std::string_view> topicNames;
		Map<std::string_view, Vector<std::int64_t>> topicSubTopics;
		std::optional<venueNames_t> venueNames;
	}; // citm_object_t
} // namespace daw::citm
