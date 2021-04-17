// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "fixed_alloc.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace daw::twitter {
	template<typename T>
	using Vector = std::vector<T, daw::fixed_allocator<T>>;
	using String =
	  std::basic_string<char, std::char_traits<char>, daw::fixed_allocator<char>>;
	using OptString = std::optional<String>;

	using twitter_tp = std::chrono::time_point<std::chrono::system_clock,
	                                           std::chrono::milliseconds>;

	struct metadata_t {
		String result_type;
		String iso_language_code;
	}; // metadata_t

	struct urls_element_t {
		String url;
		String expanded_url;
		String display_url;
		Vector<int32_t> indices;
	}; // urls_element_t

	struct url_t {
		Vector<urls_element_t> urls;
	}; // url_t

	struct description_t {
		Vector<urls_element_t> urls;
	}; // description_t

	struct entities_t {
		std::optional<url_t> url;
		std::optional<description_t> description;
	}; // entities_t

	struct user_t {
		int64_t id;
		String id_str;
		String name;
		String screen_name;
		String location;
		String description;
		OptString url;
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
		String lang;
		bool contributors_enabled;
		bool is_translator;
		bool is_translation_enabled;
		String profile_background_color;
		String profile_background_image_url;
		String profile_background_image_url_https;
		bool profile_background_tile;
		String profile_image_url;
		String profile_image_url_https;
		OptString profile_banner_url;
		String profile_link_color;
		String profile_sidebar_border_color;
		String profile_sidebar_fill_color;
		String profile_text_color;
		bool profile_use_background_image;
		bool default_profile;
		bool default_profile_image;
		bool following;
		bool follow_request_sent;
		bool notifications;
	}; // user_t

	struct hashtags_element_t {
		String text;
		Vector<int32_t> indices;
	}; // hashtags_element_t

	struct tweet_object_t {
		metadata_t metadata;
		twitter_tp created_at;
		int64_t id;
		String id_str;
		String text;
		String source;
		bool truncated;
		std::optional<int64_t> in_reply_to_status_id;
		OptString in_reply_to_status_id_str;
		std::optional<int64_t> in_reply_to_user_id;
		OptString in_reply_to_user_id_str;
		OptString in_reply_to_screen_name;
		user_t user;
		int32_t retweet_count;
		std::optional<int32_t> favorite_count;
		entities_t entities;
		bool favorited;
		bool retweeted;
		std::optional<bool> possibly_sensitive;
		String lang;
	}; // statuses_element_t

	struct user_mentions_element_t {
		String screen_name;
		String name;
		int64_t id;
		String id_str;
		Vector<int32_t> indices;
	}; // user_mentions_element_t

	struct medium_t {
		int64_t w;
		int64_t h;
		String resize;
	}; // medium_t

	struct small_t {
		int64_t w;
		int64_t h;
		String resize;
	}; // small_t

	struct thumb_t {
		int64_t w;
		int64_t h;
		String resize;
	}; // thumb_t

	struct large_t {
		int64_t w;
		int64_t h;
		String resize;
	}; // large_t

	struct sizes_t {
		medium_t medium;
		small_t small_;
		thumb_t thumb;
		large_t large;
	}; // sizes_t

	struct media_element_t {
		int64_t id;
		String id_str;
		Vector<int32_t> indices;
		String media_url;
		String media_url_https;
		String url;
		String display_url;
		String expanded_url;
		String type;
		sizes_t sizes;
	}; // media_element_t

	struct retweeted_status_t {
		metadata_t metadata;
		twitter_tp created_at;
		int64_t id;
		String id_str;
		String text;
		String source;
		bool truncated;
		std::optional<int64_t> in_reply_to_status_id;
		OptString in_reply_to_status_id_str;
		std::optional<int64_t> in_reply_to_user_id;
		OptString in_reply_to_user_id_str;
		OptString in_reply_to_screen_name;
		user_t user;
		int32_t retweet_count;
		std::optional<int32_t> favorite_count;
		entities_t entities;
		bool favorited;
		bool retweeted;
		std::optional<bool> possibly_sensitive;
		String lang;
	}; // retweeted_status_t

	struct search_metadata_t {
		double completed_in;
		int64_t max_id;
		String max_id_str;
		String next_results;
		String query;
		String refresh_url;
		int64_t count;
		int64_t since_id;
		String since_id_str;
	}; // search_metadata_t

	struct twitter_object_t {
		Vector<tweet_object_t> statuses;
		search_metadata_t search_metadata;
	}; // twitter_object_t
} // namespace daw::twitter
