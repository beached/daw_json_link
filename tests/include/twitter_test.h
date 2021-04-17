// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_from_json_fwd.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace daw::twitter {
	using twitter_tp = std::chrono::time_point<std::chrono::system_clock,
	                                           std::chrono::milliseconds>;

	struct metadata_t {
		std::string result_type;
		std::string iso_language_code;
	}; // metadata_t

	struct urls_element_t {
		std::string url;
		std::string expanded_url;
		std::string display_url;
		std::vector<int32_t> indices;
	}; // urls_element_t

	struct url_t {
		std::vector<urls_element_t> urls;
	}; // url_t

	struct entities_t {
		std::optional<url_t> url;
		std::optional<url_t> description;
	}; // entities_t

	struct user_t {
		int64_t id;
		std::string id_str;
		std::string name;
		std::string screen_name;
		std::string location;
		std::string description;
		std::optional<std::string> url;
		entities_t entities;
		bool _jsonprotected;
		int32_t followers_count;
		int32_t friends_count;
		int32_t listed_count;
		twitter_tp created_at;
		int32_t favourites_count;
		bool geo_enabled;
		bool verified;
		int32_t statuses_count;
		std::string lang;
		bool contributors_enabled;
		bool is_translator;
		bool is_translation_enabled;
		std::string profile_background_color;
		std::string profile_background_image_url;
		std::string profile_background_image_url_https;
		bool profile_background_tile;
		std::string profile_image_url;
		std::string profile_image_url_https;
		std::optional<std::string> profile_banner_url;
		std::string profile_link_color;
		std::string profile_sidebar_border_color;
		std::string profile_sidebar_fill_color;
		std::string profile_text_color;
		bool profile_use_background_image;
		bool default_profile;
		bool default_profile_image;
		bool following;
		bool follow_request_sent;
		bool notifications;
	}; // user_t

	struct hashtags_element_t {
		std::string text;
		std::vector<int32_t> indices;
	}; // hashtags_element_t

	struct tweet_object_t {
		metadata_t metadata;
		twitter_tp created_at;
		int64_t id;
		std::string id_str;
		std::string text;
		std::string source;
		bool truncated;
		std::optional<int64_t> in_reply_to_status_id;
		std::optional<std::string> in_reply_to_status_id_str;
		std::optional<int64_t> in_reply_to_user_id;
		std::optional<std::string> in_reply_to_user_id_str;
		std::optional<std::string> in_reply_to_screen_name;
		user_t user;
		int32_t retweet_count;
		std::optional<int32_t> favorite_count;
		entities_t entities;
		bool favorited;
		bool retweeted;
		std::optional<bool> possibly_sensitive;
		std::string lang;
	}; // statuses_element_t

	struct user_mentions_element_t {
		std::string screen_name;
		std::string name;
		int64_t id;
		std::string id_str;
		std::vector<int32_t> indices;
	}; // user_mentions_element_t

	struct size_item_t {
		int64_t w;
		int64_t h;
		std::string resize;
	}; // size_item_t

	struct sizes_t {
		size_item_t medium;
		size_item_t small_;
		size_item_t thumb;
		size_item_t large;
	}; // sizes_t

	struct media_element_t {
		int64_t id;
		std::string id_str;
		std::vector<int32_t> indices;
		std::string media_url;
		std::string media_url_https;
		std::string url;
		std::string display_url;
		std::string expanded_url;
		std::string type;
		sizes_t sizes;
	}; // media_element_t

	struct retweeted_status_t {
		metadata_t metadata;
		twitter_tp created_at;
		int64_t id;
		std::string id_str;
		std::string text;
		std::string source;
		bool truncated;
		std::optional<int64_t> in_reply_to_status_id;
		std::optional<std::string> in_reply_to_status_id_str;
		std::optional<int64_t> in_reply_to_user_id;
		std::optional<std::string> in_reply_to_user_id_str;
		std::optional<std::string> in_reply_to_screen_name;
		user_t user;
		int32_t retweet_count;
		std::optional<int32_t> favorite_count;
		entities_t entities;
		bool favorited;
		bool retweeted;
		std::optional<bool> possibly_sensitive;
		std::string lang;
	}; // retweeted_status_t

	struct search_metadata_t {
		double completed_in;
		int64_t max_id;
		std::string max_id_str;
		std::string next_results;
		std::string query;
		std::string refresh_url;
		int64_t count;
		int64_t since_id;
		std::string since_id_str;
	}; // search_metadata_t

	struct twitter_object_t {
		std::vector<tweet_object_t> statuses;
		search_metadata_t search_metadata;
	}; // twitter_object_t
} // namespace daw::twitter

namespace daw::json {
	extern template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyChecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data,
	                                             std::string_view path );

	extern template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data,
	                                             std::string_view path );

	extern template daw::twitter::twitter_object_t from_json<
	  daw::twitter::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>>(
	  std::string_view json_data, std::string_view path );

	extern template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::runtime_exec_tag>>( std::string_view json_data,
	                                           std::string_view path );

	extern template daw::twitter::twitter_object_t from_json<
	  daw::twitter::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data, std::string_view path );

	extern template daw::twitter::twitter_object_t from_json<
	  daw::twitter::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data, std::string_view path );

	extern template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyChecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data );

	extern template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data );

	extern template daw::twitter::twitter_object_t from_json<
	  daw::twitter::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>>(
	  std::string_view json_data );

	extern template daw::twitter::twitter_object_t
	from_json<daw::twitter::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::runtime_exec_tag>>( std::string_view json_data );

	extern template daw::twitter::twitter_object_t from_json<
	  daw::twitter::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data );

	extern template daw::twitter::twitter_object_t from_json<
	  daw::twitter::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data );
} // namespace daw::json
