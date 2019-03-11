// Code auto generated from json file '/Users/dwright/Downloads/twitter.json'
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include <daw/daw_string_view.h>
#include <daw/json/daw_json_link.h>

struct metadata_t {
	daw::string_view iso_language_code;
	daw::string_view result_type;
}; // metadata_t

inline auto describe_json_class( metadata_t ) {
	using namespace daw::json;
	static constexpr char const iso_language_code[] = "iso_language_code";
	static constexpr char const result_type[] = "result_type";
	return daw::json::class_description_t<json_string<iso_language_code, daw::string_view>,
	                                      json_string<result_type, daw::string_view>>{};
}

inline auto to_json_data( metadata_t const &value ) {
	return std::forward_as_tuple( value.iso_language_code, value.result_type );
}

struct urls_element_t {
	daw::string_view display_url;
	daw::string_view expanded_url;
	std::vector<int64_t> indices;
	daw::string_view url;
}; // urls_element_t

inline auto describe_json_class( urls_element_t ) {
	using namespace daw::json;
	static constexpr char const display_url[] = "display_url";
	static constexpr char const expanded_url[] = "expanded_url";
	static constexpr char const indices[] = "indices";
	static constexpr char const url[] = "url";
	return daw::json::class_description_t<
	  json_string<display_url, daw::string_view>, json_string<expanded_url, daw::string_view>,
	  json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>,
	  json_string<url, daw::string_view>>{};
}

inline auto to_json_data( urls_element_t const &value ) {
	return std::forward_as_tuple( value.display_url, value.expanded_url,
	                              value.indices, value.url );
}

struct url_t {
	std::vector<urls_element_t> urls;
}; // url_t

inline auto describe_json_class( url_t ) {
	using namespace daw::json;
	static constexpr char const urls[] = "urls";
	return daw::json::class_description_t<json_array<
	  urls, std::vector<urls_element_t>, json_class<no_name, urls_element_t>>>{};
}

inline auto to_json_data( url_t const &value ) {
	return std::forward_as_tuple( value.urls );
}

struct description_t {
	std::vector<urls_element_t> urls;
}; // description_t

inline auto describe_json_class( description_t ) {
	using namespace daw::json;
	static constexpr char const urls[] = "urls";
	return daw::json::class_description_t<json_array<
	  urls, std::vector<urls_element_t>, json_class<no_name, urls_element_t>>>{};
}

inline auto to_json_data( description_t const &value ) {
	return std::forward_as_tuple( value.urls );
}

struct entities_t {
	std::optional<description_t> description;
	std::optional<url_t> url;
}; // entities_t

inline auto describe_json_class( entities_t ) {
	using namespace daw::json;
	static constexpr char const description[] = "description";
	static constexpr char const url[] = "url";
	return daw::json::class_description_t<
	  json_nullable<json_class<description, description_t>>,
	  json_nullable<json_class<url, url_t>>>{};
}

inline auto to_json_data( entities_t const &value ) {
	return std::forward_as_tuple( value.description, value.url );
}

struct user_t {
	bool _jsonprotected;
	bool contributors_enabled;
	daw::string_view created_at;
	bool default_profile;
	bool default_profile_image;
	daw::string_view description;
	entities_t entities;
	int64_t favourites_count;
	bool follow_request_sent;
	int64_t followers_count;
	bool following;
	int64_t friends_count;
	bool geo_enabled;
	int64_t id;
	daw::string_view id_str;
	bool is_translation_enabled;
	bool is_translator;
	daw::string_view lang;
	int64_t listed_count;
	daw::string_view location;
	daw::string_view name;
	bool notifications;
	daw::string_view profile_background_color;
	daw::string_view profile_background_image_url;
	daw::string_view profile_background_image_url_https;
	bool profile_background_tile;
	std::optional<daw::string_view> profile_banner_url;
	daw::string_view profile_image_url;
	daw::string_view profile_image_url_https;
	daw::string_view profile_link_color;
	daw::string_view profile_sidebar_border_color;
	daw::string_view profile_sidebar_fill_color;
	daw::string_view profile_text_color;
	bool profile_use_background_image;
	daw::string_view screen_name;
	int64_t statuses_count;
	daw::string_view url;
	bool verified;
}; // user_t

inline auto describe_json_class( user_t ) {
	using namespace daw::json;
	static constexpr char const _jsonprotected[] = "protected";
	static constexpr char const contributors_enabled[] = "contributors_enabled";
	static constexpr char const created_at[] = "created_at";
	static constexpr char const default_profile[] = "default_profile";
	static constexpr char const default_profile_image[] = "default_profile_image";
	static constexpr char const description[] = "description";
	static constexpr char const entities[] = "entities";
	static constexpr char const favourites_count[] = "favourites_count";
	static constexpr char const follow_request_sent[] = "follow_request_sent";
	static constexpr char const followers_count[] = "followers_count";
	static constexpr char const following[] = "following";
	static constexpr char const friends_count[] = "friends_count";
	static constexpr char const geo_enabled[] = "geo_enabled";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const is_translation_enabled[] =
	  "is_translation_enabled";
	static constexpr char const is_translator[] = "is_translator";
	static constexpr char const lang[] = "lang";
	static constexpr char const listed_count[] = "listed_count";
	static constexpr char const location[] = "location";
	static constexpr char const name[] = "name";
	static constexpr char const notifications[] = "notifications";
	static constexpr char const profile_background_color[] =
	  "profile_background_color";
	static constexpr char const profile_background_image_url[] =
	  "profile_background_image_url";
	static constexpr char const profile_background_image_url_https[] =
	  "profile_background_image_url_https";
	static constexpr char const profile_background_tile[] =
	  "profile_background_tile";
	static constexpr char const profile_banner_url[] = "profile_banner_url";
	static constexpr char const profile_image_url[] = "profile_image_url";
	static constexpr char const profile_image_url_https[] =
	  "profile_image_url_https";
	static constexpr char const profile_link_color[] = "profile_link_color";
	static constexpr char const profile_sidebar_border_color[] =
	  "profile_sidebar_border_color";
	static constexpr char const profile_sidebar_fill_color[] =
	  "profile_sidebar_fill_color";
	static constexpr char const profile_text_color[] = "profile_text_color";
	static constexpr char const profile_use_background_image[] =
	  "profile_use_background_image";
	static constexpr char const screen_name[] = "screen_name";
	static constexpr char const statuses_count[] = "statuses_count";
	static constexpr char const url[] = "url";
	static constexpr char const verified[] = "verified";
	return daw::json::class_description_t<
	  json_bool<_jsonprotected>, json_bool<contributors_enabled>,
	  json_string<created_at, daw::string_view>, json_bool<default_profile>,
	  json_bool<default_profile_image>, json_string<description, daw::string_view>,
	  json_class<entities, entities_t>, json_number<favourites_count, intmax_t>,
	  json_bool<follow_request_sent>, json_number<followers_count, intmax_t>,
	  json_bool<following>, json_number<friends_count, intmax_t>,
	  json_bool<geo_enabled>, json_number<id, intmax_t>, json_string<id_str, daw::string_view>,
	  json_bool<is_translation_enabled>, json_bool<is_translator>,
	  json_string<lang, daw::string_view>, json_number<listed_count, intmax_t>,
	  json_string<location, daw::string_view>, json_string<name, daw::string_view>, json_bool<notifications>,
	  json_string<profile_background_color, daw::string_view>,
	  json_string<profile_background_image_url, daw::string_view>,
	  json_string<profile_background_image_url_https, daw::string_view>,
	  json_bool<profile_background_tile>,
	  json_nullable<json_string<profile_banner_url, daw::string_view>>,
	  json_string<profile_image_url, daw::string_view>, json_string<profile_image_url_https, daw::string_view>,
	  json_string<profile_link_color, daw::string_view>, json_string<profile_sidebar_border_color, daw::string_view>,
	  json_string<profile_sidebar_fill_color, daw::string_view>, json_string<profile_text_color, daw::string_view>,
	  json_bool<profile_use_background_image>, json_string<screen_name, daw::string_view>,
	  json_number<statuses_count, intmax_t>, json_string<url, daw::string_view>,
	  json_bool<verified>>{};
}

inline auto to_json_data( user_t const &value ) {
	return std::forward_as_tuple(
	  value._jsonprotected, value.contributors_enabled, value.created_at,
	  value.default_profile, value.default_profile_image, value.description,
	  value.entities, value.favourites_count, value.follow_request_sent,
	  value.followers_count, value.following, value.friends_count,
	  value.geo_enabled, value.id, value.id_str, value.is_translation_enabled,
	  value.is_translator, value.lang, value.listed_count, value.location,
	  value.name, value.notifications, value.profile_background_color,
	  value.profile_background_image_url,
	  value.profile_background_image_url_https, value.profile_background_tile,
	  value.profile_banner_url, value.profile_image_url,
	  value.profile_image_url_https, value.profile_link_color,
	  value.profile_sidebar_border_color, value.profile_sidebar_fill_color,
	  value.profile_text_color, value.profile_use_background_image,
	  value.screen_name, value.statuses_count, value.url, value.verified );
}

struct hashtags_element_t {
	std::vector<int64_t> indices;
	daw::string_view text;
}; // hashtags_element_t

inline auto describe_json_class( hashtags_element_t ) {
	using namespace daw::json;
	static constexpr char const indices[] = "indices";
	static constexpr char const text[] = "text";
	return daw::json::class_description_t<
	  json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>,
	  json_string<text, daw::string_view>>{};
}

inline auto to_json_data( hashtags_element_t const &value ) {
	return std::forward_as_tuple( value.indices, value.text );
}

struct statuses_element_t {
	daw::string_view created_at;
	entities_t entities;
	int64_t favorite_count;
	bool favorited;
	int64_t id;
	daw::string_view id_str;
	std::optional<daw::string_view> in_reply_to_screen_name;
	std::optional<uint64_t> in_reply_to_status_id;
	std::optional<daw::string_view> in_reply_to_status_id_str;
	std::optional<uint64_t> in_reply_to_user_id;
	std::optional<daw::string_view> in_reply_to_user_id_str;
	daw::string_view lang;
	metadata_t metadata;
	std::optional<bool> possibly_sensitive;
	int64_t retweet_count;
	bool retweeted;
	daw::string_view source;
	daw::string_view text;
	bool truncated;
	user_t user;
}; // statuses_element_t

inline auto describe_json_class( statuses_element_t ) {
	using namespace daw::json;
	static constexpr char const created_at[] = "created_at";
	static constexpr char const entities[] = "entities";
	static constexpr char const favorite_count[] = "favorite_count";
	static constexpr char const favorited[] = "favorited";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const in_reply_to_screen_name[] =
	  "in_reply_to_screen_name";
	static constexpr char const in_reply_to_status_id[] = "in_reply_to_status_id";
	static constexpr char const in_reply_to_status_id_str[] =
	  "in_reply_to_status_id_str";
	static constexpr char const in_reply_to_user_id[] = "in_reply_to_user_id";
	static constexpr char const in_reply_to_user_id_str[] =
	  "in_reply_to_user_id_str";
	static constexpr char const lang[] = "lang";
	static constexpr char const metadata[] = "metadata";
	static constexpr char const possibly_sensitive[] = "possibly_sensitive";
	static constexpr char const retweet_count[] = "retweet_count";
	static constexpr char const retweeted[] = "retweeted";
	static constexpr char const source[] = "source";
	static constexpr char const text[] = "text";
	static constexpr char const truncated[] = "truncated";
	static constexpr char const user[] = "user";
	return daw::json::class_description_t<
	  json_string<created_at, daw::string_view>, json_class<entities, entities_t>,
	  json_number<favorite_count, intmax_t>, json_bool<favorited>,
	  json_number<id, intmax_t>, json_string<id_str, daw::string_view>,
	  json_nullable<json_string<in_reply_to_screen_name, daw::string_view>>,
	  json_nullable<json_number<in_reply_to_status_id, uint64_t>>,
	  json_nullable<json_string<in_reply_to_status_id_str, daw::string_view>>,
	  json_nullable<json_number<in_reply_to_user_id, uint64_t>>,
	  json_nullable<json_string<in_reply_to_user_id_str, daw::string_view>>, json_string<lang, daw::string_view>,
	  json_class<metadata, metadata_t>,
	  json_nullable<json_bool<possibly_sensitive>>,
	  json_number<retweet_count, intmax_t>, json_bool<retweeted>,
	  json_string<source, daw::string_view>, json_string<text, daw::string_view>, json_bool<truncated>,
	  json_class<user, user_t>>{};
}

inline auto to_json_data( statuses_element_t const &value ) {
	return std::forward_as_tuple(
	  value.created_at, value.entities, value.favorite_count, value.favorited,
	  value.id, value.id_str, value.in_reply_to_screen_name,
	  value.in_reply_to_status_id, value.in_reply_to_status_id_str,
	  value.in_reply_to_user_id, value.in_reply_to_user_id_str, value.lang,
	  value.metadata, value.possibly_sensitive, value.retweet_count,
	  value.retweeted, value.source, value.text, value.truncated, value.user );
}

struct user_mentions_element_t {
	int64_t id;
	daw::string_view id_str;
	std::vector<int64_t> indices;
	daw::string_view name;
	daw::string_view screen_name;
}; // user_mentions_element_t

inline auto describe_json_class( user_mentions_element_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const indices[] = "indices";
	static constexpr char const name[] = "name";
	static constexpr char const screen_name[] = "screen_name";
	return daw::json::class_description_t<
	  json_number<id, intmax_t>, json_string<id_str, daw::string_view>,
	  json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>,
	  json_string<name, daw::string_view>, json_string<screen_name, daw::string_view>>{};
}

inline auto to_json_data( user_mentions_element_t const &value ) {
	return std::forward_as_tuple( value.id, value.id_str, value.indices,
	                              value.name, value.screen_name );
}

struct medium_t {
	int64_t h;
	daw::string_view resize;
	int64_t w;
}; // medium_t

inline auto describe_json_class( medium_t ) {
	using namespace daw::json;
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	static constexpr char const w[] = "w";
	return daw::json::class_description_t<
	  json_number<h, intmax_t>, json_string<resize, daw::string_view>, json_number<w, intmax_t>>{};
}

inline auto to_json_data( medium_t const &value ) {
	return std::forward_as_tuple( value.h, value.resize, value.w );
}

struct small_t {
	int64_t h;
	daw::string_view resize;
	int64_t w;
}; // small_t

inline auto describe_json_class( small_t ) {
	using namespace daw::json;
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	static constexpr char const w[] = "w";
	return daw::json::class_description_t<
	  json_number<h, intmax_t>, json_string<resize, daw::string_view>, json_number<w, intmax_t>>{};
}

inline auto to_json_data( small_t const &value ) {
	return std::forward_as_tuple( value.h, value.resize, value.w );
}

struct thumb_t {
	int64_t h;
	daw::string_view resize;
	int64_t w;
}; // thumb_t

inline auto describe_json_class( thumb_t ) {
	using namespace daw::json;
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	static constexpr char const w[] = "w";
	return daw::json::class_description_t<
	  json_number<h, intmax_t>, json_string<resize, daw::string_view>, json_number<w, intmax_t>>{};
}

inline auto to_json_data( thumb_t const &value ) {
	return std::forward_as_tuple( value.h, value.resize, value.w );
}

struct large_t {
	int64_t h;
	daw::string_view resize;
	int64_t w;
}; // large_t

inline auto describe_json_class( large_t ) {
	using namespace daw::json;
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	static constexpr char const w[] = "w";
	return daw::json::class_description_t<
	  json_number<h, intmax_t>, json_string<resize, daw::string_view>, json_number<w, intmax_t>>{};
}

inline auto to_json_data( large_t const &value ) {
	return std::forward_as_tuple( value.h, value.resize, value.w );
}

struct sizes_t {
	large_t large;
	medium_t medium;
	small_t small;
	thumb_t thumb;
}; // sizes_t

inline auto describe_json_class( sizes_t ) {
	using namespace daw::json;
	static constexpr char const large[] = "large";
	static constexpr char const medium[] = "medium";
	static constexpr char const small[] = "small";
	static constexpr char const thumb[] = "thumb";
	return daw::json::class_description_t<
	  json_class<large, large_t>, json_class<medium, medium_t>,
	  json_class<small, small_t>, json_class<thumb, thumb_t>>{};
}

inline auto to_json_data( sizes_t const &value ) {
	return std::forward_as_tuple( value.large, value.medium, value.small,
	                              value.thumb );
}

struct media_element_t {
	daw::string_view display_url;
	daw::string_view expanded_url;
	int64_t id;
	daw::string_view id_str;
	std::vector<int64_t> indices;
	daw::string_view media_url;
	daw::string_view media_url_https;
	sizes_t sizes;
	daw::string_view type;
	daw::string_view url;
}; // media_element_t

inline auto describe_json_class( media_element_t ) {
	using namespace daw::json;
	static constexpr char const display_url[] = "display_url";
	static constexpr char const expanded_url[] = "expanded_url";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const indices[] = "indices";
	static constexpr char const media_url[] = "media_url";
	static constexpr char const media_url_https[] = "media_url_https";
	static constexpr char const sizes[] = "sizes";
	static constexpr char const type[] = "type";
	static constexpr char const url[] = "url";
	return daw::json::class_description_t<
	  json_string<display_url, daw::string_view>, json_string<expanded_url, daw::string_view>,
	  json_number<id, intmax_t>, json_string<id_str, daw::string_view>,
	  json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>,
	  json_string<media_url, daw::string_view>, json_string<media_url_https, daw::string_view>,
	  json_class<sizes, sizes_t>, json_string<type, daw::string_view>, json_string<url, daw::string_view>>{};
}

inline auto to_json_data( media_element_t const &value ) {
	return std::forward_as_tuple( value.display_url, value.expanded_url, value.id,
	                              value.id_str, value.indices, value.media_url,
	                              value.media_url_https, value.sizes, value.type,
	                              value.url );
}

struct retweeted_status_t {
	daw::string_view created_at;
	entities_t entities;
	int64_t favorite_count;
	bool favorited;
	int64_t id;
	daw::string_view id_str;
	std::optional<daw::string_view> in_reply_to_screen_name;
	std::optional<uint64_t> in_reply_to_status_id;
	std::optional<daw::string_view> in_reply_to_status_id_str;
	std::optional<uint64_t> in_reply_to_user_id;
	std::optional<daw::string_view> in_reply_to_user_id_str;
	daw::string_view lang;
	metadata_t metadata;
	std::optional<bool> possibly_sensitive;
	int64_t retweet_count;
	bool retweeted;
	daw::string_view source;
	daw::string_view text;
	bool truncated;
	user_t user;
}; // retweeted_status_t

inline auto describe_json_class( retweeted_status_t ) {
	using namespace daw::json;
	static constexpr char const created_at[] = "created_at";
	static constexpr char const entities[] = "entities";
	static constexpr char const favorite_count[] = "favorite_count";
	static constexpr char const favorited[] = "favorited";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const in_reply_to_screen_name[] =
	  "in_reply_to_screen_name";
	static constexpr char const in_reply_to_status_id[] = "in_reply_to_status_id";
	static constexpr char const in_reply_to_status_id_str[] =
	  "in_reply_to_status_id_str";
	static constexpr char const in_reply_to_user_id[] = "in_reply_to_user_id";
	static constexpr char const in_reply_to_user_id_str[] =
	  "in_reply_to_user_id_str";
	static constexpr char const lang[] = "lang";
	static constexpr char const metadata[] = "metadata";
	static constexpr char const possibly_sensitive[] = "possibly_sensitive";
	static constexpr char const retweet_count[] = "retweet_count";
	static constexpr char const retweeted[] = "retweeted";
	static constexpr char const source[] = "source";
	static constexpr char const text[] = "text";
	static constexpr char const truncated[] = "truncated";
	static constexpr char const user[] = "user";
	return daw::json::class_description_t<
	  json_string<created_at, daw::string_view>, json_class<entities, entities_t>,
	  json_number<favorite_count, intmax_t>, json_bool<favorited>,
	  json_number<id, intmax_t>, json_string<id_str, daw::string_view>,
	  json_nullable<json_string<in_reply_to_screen_name, daw::string_view>>,
	  json_nullable<json_number<in_reply_to_status_id, uint64_t>>,
	  json_nullable<json_string<in_reply_to_status_id_str, daw::string_view>>,
	  json_nullable<json_number<in_reply_to_user_id, uint64_t>>,
	  json_nullable<json_string<in_reply_to_user_id_str, daw::string_view>>, json_string<lang, daw::string_view>,
	  json_class<metadata, metadata_t>,
	  json_nullable<json_bool<possibly_sensitive>>,
	  json_number<retweet_count, intmax_t>, json_bool<retweeted>,
	  json_string<source, daw::string_view>, json_string<text, daw::string_view>, json_bool<truncated>,
	  json_class<user, user_t>>{};
}

inline auto to_json_data( retweeted_status_t const &value ) {
	return std::forward_as_tuple(
	  value.created_at, value.entities, value.favorite_count, value.favorited,
	  value.id, value.id_str, value.in_reply_to_screen_name,
	  value.in_reply_to_status_id, value.in_reply_to_status_id_str,
	  value.in_reply_to_user_id, value.in_reply_to_user_id_str, value.lang,
	  value.metadata, value.possibly_sensitive, value.retweet_count,
	  value.retweeted, value.source, value.text, value.truncated, value.user );
}

struct search_metadata_t {
	double completed_in;
	int64_t count;
	int64_t max_id;
	daw::string_view max_id_str;
	daw::string_view next_results;
	daw::string_view query;
	daw::string_view refresh_url;
	int64_t since_id;
	daw::string_view since_id_str;
}; // search_metadata_t

inline auto describe_json_class( search_metadata_t ) {
	using namespace daw::json;
	static constexpr char const completed_in[] = "completed_in";
	static constexpr char const count[] = "count";
	static constexpr char const max_id[] = "max_id";
	static constexpr char const max_id_str[] = "max_id_str";
	static constexpr char const next_results[] = "next_results";
	static constexpr char const query[] = "query";
	static constexpr char const refresh_url[] = "refresh_url";
	static constexpr char const since_id[] = "since_id";
	static constexpr char const since_id_str[] = "since_id_str";
	return daw::json::class_description_t<
	  json_number<completed_in>, json_number<count, intmax_t>,
	  json_number<max_id, intmax_t>, json_string<max_id_str, daw::string_view>,
	  json_string<next_results, daw::string_view>, json_string<query, daw::string_view>, json_string<refresh_url, daw::string_view>,
	  json_number<since_id, intmax_t>, json_string<since_id_str, daw::string_view>>{};
}

inline auto to_json_data( search_metadata_t const &value ) {
	return std::forward_as_tuple( value.completed_in, value.count, value.max_id,
	                              value.max_id_str, value.next_results,
	                              value.query, value.refresh_url, value.since_id,
	                              value.since_id_str );
}

struct root_object_t {
	search_metadata_t search_metadata;
	std::vector<statuses_element_t> statuses;
}; // root_object_t

inline auto describe_json_class( root_object_t ) {
	using namespace daw::json;
	static constexpr char const search_metadata[] = "search_metadata";
	static constexpr char const statuses[] = "statuses";
	return daw::json::class_description_t<
	  json_class<search_metadata, search_metadata_t>,
	  json_array<statuses, std::vector<statuses_element_t>,
	             json_class<no_name, statuses_element_t>>>{};
}

inline auto to_json_data( root_object_t const &value ) {
	return std::forward_as_tuple( value.search_metadata, value.statuses );
}
