// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace daw::citm {
	struct events_value_t {
		int64_t id;
		std::optional<std::string_view> logo;
		std::string_view name;
		std::vector<int64_t> subTopicIds;
		std::vector<int64_t> topicIds;
	}; // events_value_t

	struct prices_element_t {
		int64_t amount;
		int64_t audienceSubCategoryId;
		int64_t seatCategoryId;
	}; // prices_element_t

	struct areas_element_t {
		int64_t areaId;
	}; // areas_element_t

	struct seatCategories_element_t {
		std::vector<areas_element_t> areas;
		int64_t seatCategoryId;
	}; // seatCategories_element_t

	struct performances_element_t {
		int64_t eventId;
		int64_t id;
		std::optional<std::string_view> logo;
		std::vector<prices_element_t> prices;
		std::vector<seatCategories_element_t> seatCategories;
		int64_t start;
		std::string_view venueCode;
	}; // performances_element_t

	struct venueNames_t {
		std::string_view pleyel_pleyel;
	}; // venueNames_t

	struct citm_object_t {
		std::unordered_map<intmax_t, std::string_view> areaNames;
		std::unordered_map<intmax_t, std::string_view> audienceSubCategoryNames;
		std::unordered_map<intmax_t, events_value_t> events;
		std::vector<performances_element_t> performances;
		std::unordered_map<std::string_view, std::string_view> seatCategoryNames;
		std::unordered_map<std::string_view, std::string_view> subTopicNames;
		std::unordered_map<std::string_view, std::string_view> topicNames;
		std::unordered_map<std::string_view, std::vector<int64_t>> topicSubTopics;
		std::optional<venueNames_t> venueNames;
	}; // citm_object_t
} // namespace daw::citm

namespace daw::json {
	template<>
	struct json_data_contract<daw::citm::events_value_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"id", int64_t>,
		                   json_string_raw_null<"logo", std::string_view>,
		                   json_string_raw<"name", std::string_view>,
		                   json_array<"subTopicIds", int64_t>,
		                   json_array<"topicIds", int64_t>>;
#else
		static inline constexpr char const id[] = "id";
		static inline constexpr char const logo[] = "logo";
		static inline constexpr char const name[] = "name";
		static inline constexpr char const subTopicIds[] = "subTopicIds";
		static inline constexpr char const topicIds[] = "topicIds";
		using type = json_member_list<
		  json_number<id, int64_t>, json_string_raw_null<logo, std::string_view>,
		  json_string_raw<name, std::string_view>, json_array<subTopicIds, int64_t>,
		  json_array<topicIds, int64_t>>;
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
		using type = json_member_list<json_number<"amount", int64_t>,
		                              json_number<"audienceSubCategoryId", int64_t>,
		                              json_number<"seatCategoryId", int64_t>>;
#else
		static inline constexpr char const amount[] = "amount";
		static inline constexpr char const audienceSubCategoryId[] =
		  "audienceSubCategoryId";
		static inline constexpr char const seatCategoryId[] = "seatCategoryId";
		using type = json_member_list<json_number<amount, int64_t>,
		                              json_number<audienceSubCategoryId, int64_t>,
		                              json_number<seatCategoryId, int64_t>>;
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
		using type = json_member_list<json_number<"areaId", int64_t>>;
#else
		static inline constexpr char const areaId[] = "areaId";
		using type = json_member_list<json_number<areaId, int64_t>>;
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
		                   json_number<"seatCategoryId", int64_t>>;
#else
		static inline constexpr char const areas[] = "areas";
		static inline constexpr char const seatCategoryId[] = "seatCategoryId";
		using type = json_member_list<json_array<areas, daw::citm::areas_element_t>,
		                              json_number<seatCategoryId, int64_t>>;
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
		  json_number<"eventId", int64_t>, json_number<"id", int64_t>,
		  json_string_raw_null<"logo", std::string_view>,
		  json_array<"prices", daw::citm::prices_element_t>,
		  json_array<"seatCategories", daw::citm::seatCategories_element_t>,
		  json_number<"start", int64_t>,
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
		  json_number<eventId, int64_t>, json_number<id, int64_t>,
		  json_string_raw_null<logo, std::string_view>,
		  json_array<prices, daw::citm::prices_element_t>,
		  json_array<seatCategories, daw::citm::seatCategories_element_t>,
		  json_number<start, int64_t>,
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
		    "areaNames", std::unordered_map<intmax_t, std::string_view>,
		    json_string_raw<no_name, std::string_view>,
		    json_number<no_name, intmax_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    "audienceSubCategoryNames",
		    std::unordered_map<intmax_t, std::string_view>,
		    json_string_raw<no_name, std::string_view>,
		    json_number<no_name, intmax_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    "events", std::unordered_map<intmax_t, daw::citm::events_value_t>,
		    json_class<no_name, daw::citm::events_value_t>,
		    json_number<no_name, intmax_t, LiteralAsStringOpt::Always>>,
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
		  json_key_value<"topicSubTopics",
		                 std::unordered_map<std::string_view, std::vector<int64_t>>,
		                 std::vector<int64_t>, std::string_view>,
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
		    areaNames, std::unordered_map<intmax_t, std::string_view>,
		    std::string_view,
		    json_number<no_name, intmax_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    audienceSubCategoryNames,
		    std::unordered_map<intmax_t, std::string_view>, std::string_view,
		    json_number<no_name, intmax_t, LiteralAsStringOpt::Always>>,
		  json_key_value<
		    events, std::unordered_map<intmax_t, daw::citm::events_value_t>,
		    json_class<no_name, daw::citm::events_value_t>,
		    json_number<no_name, intmax_t, LiteralAsStringOpt::Always>>,
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
		  json_key_value<topicSubTopics,
		                 std::unordered_map<std::string_view, std::vector<int64_t>>,
		                 std::vector<int64_t>, std::string_view>,
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
