// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_from_json_fwd.h>

#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

template<typename T>
struct opt_construct_a {
	constexpr std::optional<T> operator( )( ) const {
		return daw::construct_a<std::optional<T>>( );
	}

	template<typename... Args>
	constexpr std::optional<T> operator( )( Args &&...args ) const {
		return std::optional<T>(
		  daw::construct_a<T>( std::forward<Args>( args )... ) );
	}
};
namespace daw::twitter2 {
	struct metadata_t {
		std::string_view result_type;
		std::string_view iso_language_code;
	}; // metadata_t

	struct urls_element_t {
		std::string_view url;
		std::string_view expanded_url;
		std::string_view display_url;
		std::vector<std::string_view> indices;
	}; // urls_element_t

	struct url_t {
		std::vector<urls_element_t> urls;
	}; // url_t

	struct description_t {
		std::vector<urls_element_t> urls;
	}; // description_t

	struct entities_t {
		std::optional<url_t> url;
		std::optional<description_t> description;
	}; // entities_t

	struct user_t {
		std::string_view id;
		std::string_view id_str;
		std::string_view name;
		std::string_view screen_name;
		std::string_view location;
		std::string_view description;
		std::optional<std::string_view> url;
		entities_t entities;
		bool _jsonprotected;
		std::string_view followers_count;
		std::string_view friends_count;
		std::string_view listed_count;
		std::string_view created_at;
		std::string_view favourites_count;
		std::optional<std::string_view> utc_offset;
		std::optional<std::string_view> time_zone;
		bool geo_enabled;
		bool verified;
		std::string_view statuses_count;
		std::string_view lang;
		bool contributors_enabled;
		bool is_translator;
		bool is_translation_enabled;
		std::string_view profile_background_color;
		std::string_view profile_background_image_url;
		std::string_view profile_background_image_url_https;
		bool profile_background_tile;
		std::string_view profile_image_url;
		std::string_view profile_image_url_https;
		std::optional<std::string_view> profile_banner_url;
		std::string_view profile_link_color;
		std::string_view profile_sidebar_border_color;
		std::string_view profile_sidebar_fill_color;
		std::string_view profile_text_color;
		bool profile_use_background_image;
		bool default_profile;
		bool default_profile_image;
		bool following;
		bool follow_request_sent;
		bool notifications;
	}; // user_t

	struct hashtags_element_t {
		std::string_view text;
		std::vector<std::string_view> indices;
	}; // hashtags_element_t

	struct tweet_object_t {
		metadata_t metadata;
		std::string_view created_at;
		std::string_view id;
		std::string_view id_str;
		std::string_view text;
		std::string_view source;
		bool truncated;
		std::optional<std::string_view> in_reply_to_status_id;
		std::optional<std::string_view> in_reply_to_status_id_str;
		std::optional<std::string_view> in_reply_to_user_id;
		std::optional<std::string_view> in_reply_to_user_id_str;
		std::optional<std::string_view> in_reply_to_screen_name;
		user_t user;
		std::string_view retweet_count;
		std::string_view favorite_count;
		entities_t entities;
		bool favorited;
		bool retweeted;
		std::optional<bool> possibly_sensitive;
		std::string_view lang;
	}; // statuses_element_t

	struct user_mentions_element_t {
		std::string_view screen_name;
		std::string_view name;
		std::string_view id;
		std::string_view id_str;
		std::vector<std::string_view> indices;
	}; // user_mentions_element_t

	struct medium_t {
		std::string_view w;
		std::string_view h;
		std::string_view resize;
	}; // medium_t

	struct small_t {
		std::string_view w;
		std::string_view h;
		std::string_view resize;
	}; // small_t

	struct thumb_t {
		std::string_view w;
		std::string_view h;
		std::string_view resize;
	}; // thumb_t

	struct large_t {
		std::string_view w;
		std::string_view h;
		std::string_view resize;
	}; // large_t

	struct sizes_t {
		medium_t medium;
		small_t small_;
		thumb_t thumb;
		large_t large;
	}; // sizes_t

	struct media_element_t {
		std::string_view id;
		std::string_view id_str;
		std::vector<std::string_view> indices;
		std::string_view media_url;
		std::string_view media_url_https;
		std::string_view url;
		std::string_view display_url;
		std::string_view expanded_url;
		std::string_view type;
		sizes_t sizes;
	}; // media_element_t

	struct retweeted_status_t {
		metadata_t metadata;
		std::string_view created_at;
		std::string_view id;
		std::string_view id_str;
		std::string_view text;
		std::string_view source;
		bool truncated;
		std::optional<std::string_view> in_reply_to_status_id;
		std::optional<std::string_view> in_reply_to_status_id_str;
		std::optional<std::string_view> in_reply_to_user_id;
		std::optional<std::string_view> in_reply_to_user_id_str;
		std::optional<std::string_view> in_reply_to_screen_name;
		user_t user;
		std::string_view retweet_count;
		std::string_view favorite_count;
		entities_t entities;
		bool favorited;
		bool retweeted;
		std::optional<bool> possibly_sensitive;
		std::string_view lang;
	}; // retweeted_status_t

	struct search_metadata_t {
		std::string_view completed_in;
		std::string_view max_id;
		std::string_view max_id_str;
		std::string_view next_results;
		std::string_view query;
		std::string_view refresh_url;
		std::string_view count;
		std::string_view since_id;
		std::string_view since_id_str;
	}; // search_metadata_t

	struct twitter_object_t {
		std::vector<tweet_object_t> statuses;
		search_metadata_t search_metadata;
	}; // twitter_object_t
} // namespace daw::twitter2

namespace daw::json {
	extern template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyChecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data,
	                                             std::string_view path );

	extern template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data,
	                                             std::string_view path );

	extern template daw::twitter2::twitter_object_t from_json<
	  daw::twitter2::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>>(
	  std::string_view json_data, std::string_view path );

	extern template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::runtime_exec_tag>>( std::string_view json_data,
	                                           std::string_view path );

	extern template daw::twitter2::twitter_object_t from_json<
	  daw::twitter2::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data, std::string_view path );

	extern template daw::twitter2::twitter_object_t from_json<
	  daw::twitter2::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data, std::string_view path );

	extern template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyChecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data );

	extern template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::constexpr_exec_tag>>( std::string_view json_data );

	extern template daw::twitter2::twitter_object_t from_json<
	  daw::twitter2::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::runtime_exec_tag>>(
	  std::string_view json_data );

	extern template daw::twitter2::twitter_object_t
	from_json<daw::twitter2::twitter_object_t,
	          daw::json::SIMDNoCommentSkippingPolicyUnchecked<
	            daw::json::runtime_exec_tag>>( std::string_view json_data );

	extern template daw::twitter2::twitter_object_t from_json<
	  daw::twitter2::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyChecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data );

	extern template daw::twitter2::twitter_object_t from_json<
	  daw::twitter2::twitter_object_t,
	  daw::json::SIMDNoCommentSkippingPolicyUnchecked<daw::json::simd_exec_tag>>(
	  std::string_view json_data );
} // namespace daw::json
