// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "citm_test.h"

#include <daw/json/daw_json_link_types.h>

#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace daw::json {
	template<>
	struct json_data_contract<daw::citm::events_value_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"id", std::int64_t>,
		                   json_string_raw_null<"logo", std::string_view>,
		                   json_string_raw<"name", std::string_view>,
		                   json_array<"subTopicIds", std::int64_t>,
		                   json_array<"topicIds", std::int64_t>>;
#else
		static inline constexpr char const id[] = "id";
		static inline constexpr char const logo[] = "logo";
		static inline constexpr char const name[] = "name";
		static inline constexpr char const subTopicIds[] = "subTopicIds";
		static inline constexpr char const topicIds[] = "topicIds";
		using type = json_member_list<json_number<id, std::int64_t>,
		                              json_string_raw_null<logo, std::string_view>,
		                              json_string_raw<name, std::string_view>,
		                              json_array<subTopicIds, std::int64_t>,
		                              json_array<topicIds, std::int64_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::events_value_t const &value ) {
			return std::forward_as_tuple( value.id, value.logo, value.name,
			                              value.subTopicIds, value.topicIds );
		}
	};

	template<>
	struct json_data_contract<daw::citm::prices_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"amount", std::int64_t>,
		                   json_number<"audienceSubCategoryId", std::int64_t>,
		                   json_number<"seatCategoryId", std::int64_t>>;
#else
		static inline constexpr char const amount[] = "amount";
		static inline constexpr char const audienceSubCategoryId[] =
		  "audienceSubCategoryId";
		static inline constexpr char const seatCategoryId[] = "seatCategoryId";
		using type =
		  json_member_list<json_number<amount, std::int64_t>,
		                   json_number<audienceSubCategoryId, std::int64_t>,
		                   json_number<seatCategoryId, std::int64_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::prices_element_t const &value ) {
			return std::forward_as_tuple( value.amount, value.audienceSubCategoryId,
			                              value.seatCategoryId );
		}
	};

	template<>
	struct json_data_contract<daw::citm::areas_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_number<"areaId", std::int64_t>>;
#else
		static inline constexpr char const areaId[] = "areaId";
		using type = json_member_list<json_number<areaId, std::int64_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::areas_element_t const &value ) {
			return std::forward_as_tuple( value.areaId );
		}
	};

	template<>
	struct json_data_contract<daw::citm::seatCategories_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_array<"areas", daw::citm::areas_element_t>,
		                   json_number<"seatCategoryId", std::int64_t>>;
#else
		static inline constexpr char const areas[] = "areas";
		static inline constexpr char const seatCategoryId[] = "seatCategoryId";
		using type = json_member_list<json_array<areas, daw::citm::areas_element_t>,
		                              json_number<seatCategoryId, std::int64_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::seatCategories_element_t const &value ) {
			return std::forward_as_tuple( value.areas, value.seatCategoryId );
		}
	};

	template<>
	struct json_data_contract<daw::citm::performances_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_number<"eventId", std::int64_t>, json_number<"id", std::int64_t>,
		  json_string_raw_null<"logo", std::string_view>,
		  json_array<"prices", daw::citm::prices_element_t>,
		  json_array<"seatCategories", daw::citm::seatCategories_element_t>,
		  json_number<"start", std::int64_t>,
		  json_string_raw<"venueCode", std::string_view>>;
#else
		static inline constexpr char const eventId[] = "eventId";
		static inline constexpr char const id[] = "id";
		static inline constexpr char const logo[] = "logo";
		static inline constexpr char const prices[] = "prices";
		static inline constexpr char const seatCategories[] = "seatCategories";
		static inline constexpr char const start[] = "start";
		static inline constexpr char const venueCode[] = "venueCode";
		using type = json_member_list<
		  json_number<eventId, std::int64_t>, json_number<id, std::int64_t>,
		  json_string_raw_null<logo, std::string_view>,
		  json_array<prices, daw::citm::prices_element_t>,
		  json_array<seatCategories, daw::citm::seatCategories_element_t>,
		  json_number<start, std::int64_t>,
		  json_string_raw<venueCode, std::string_view>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::performances_element_t const &value ) {
			return std::forward_as_tuple( value.eventId, value.id, value.logo,
			                              value.prices, value.seatCategories,
			                              value.start, value.venueCode );
		}
	};

	template<>
	struct json_data_contract<daw::citm::venueNames_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string_raw<"PLEYEL_PLEYEL", std::string_view>>;
#else
		static inline constexpr char const PLEYEL_PLEYEL[] = "PLEYEL_PLEYEL";
		using type =
		  json_member_list<json_string_raw<PLEYEL_PLEYEL, std::string_view>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::venueNames_t const &value ) {
			return std::forward_as_tuple( value.pleyel_pleyel );
		}
	};

	template<>
	struct json_data_contract<daw::citm::citm_object_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_key_value<
		    "areaNames", std::unordered_map<std::int64_t, std::string_view>,
		    json_string_raw<no_name, std::string_view>,
		    json_number<no_name, std::int64_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    "audienceSubCategoryNames",
		    std::unordered_map<std::int64_t, std::string_view>,
		    json_string_raw<no_name, std::string_view>,
		    json_number<no_name, std::int64_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    "events", std::unordered_map<std::int64_t, daw::citm::events_value_t>,
		    json_class<no_name, daw::citm::events_value_t>,
		    json_number<no_name, std::int64_t, LiteralAsStringOpt::Always>>,
		  json_array<"performances", daw::citm::performances_element_t>,
		  json_key_value<"seatCategoryNames",
		                 std::unordered_map<std::string_view, std::string_view>,
		                 std::string_view, std::string_view>,
		  json_key_value<"subTopicNames",
		                 std::unordered_map<std::string_view, std::string_view>,
		                 std::string_view, std::string_view>,
		  json_key_value<"topicNames",
		                 std::unordered_map<std::string_view, std::string_view>,
		                 std::string_view, std::string_view>,
		  json_key_value<
		    "topicSubTopics",
		    std::unordered_map<std::string_view, std::vector<std::int64_t>>,
		    std::vector<std::int64_t>, std::string_view>,
		  json_class_null<"venueNames", std::optional<daw::citm::venueNames_t>>>;
#else
		static inline constexpr char const areaNames[] = "areaNames";
		static inline constexpr char const audienceSubCategoryNames[] =
		  "audienceSubCategoryNames";
		static inline constexpr char const events[] = "events";
		static inline constexpr char const performances[] = "performances";
		static inline constexpr char const seatCategoryNames[] =
		  "seatCategoryNames";
		static inline constexpr char const subTopicNames[] = "subTopicNames";
		static inline constexpr char const topicNames[] = "topicNames";
		static inline constexpr char const topicSubTopics[] = "topicSubTopics";
		static inline constexpr char const venueNames[] = "venueNames";

		using type = json_member_list<
		  json_key_value<
		    areaNames, std::unordered_map<std::int64_t, std::string_view>,
		    std::string_view,
		    json_number<no_name, std::int64_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    audienceSubCategoryNames,
		    std::unordered_map<std::int64_t, std::string_view>, std::string_view,
		    json_number<no_name, std::int64_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    events, std::unordered_map<std::int64_t, daw::citm::events_value_t>,
		    json_class<no_name, daw::citm::events_value_t>,
		    json_number<no_name, std::int64_t, LiteralAsStringOpt::Always>>,
		  json_array<performances, daw::citm::performances_element_t>,
		  json_key_value<seatCategoryNames,
		                 std::unordered_map<std::string_view, std::string_view>,
		                 std::string_view, std::string_view>,
		  json_key_value<subTopicNames,
		                 std::unordered_map<std::string_view, std::string_view>,
		                 std::string_view, std::string_view>,
		  json_key_value<topicNames,
		                 std::unordered_map<std::string_view, std::string_view>,
		                 std::string_view, std::string_view>,
		  json_key_value<
		    topicSubTopics,
		    std::unordered_map<std::string_view, std::vector<std::int64_t>>,
		    std::vector<std::int64_t>, std::string_view>,
		  json_class_null<venueNames, std::optional<daw::citm::venueNames_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::citm::citm_object_t const &value ) {
			return std::forward_as_tuple(
			  value.areaNames, value.audienceSubCategoryNames, value.events,
			  value.performances, value.seatCategoryNames, value.subTopicNames,
			  value.topicNames, value.topicSubTopics, value.venueNames );
		}
	};
} // namespace daw::json
