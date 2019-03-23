// Code auto generated from json file '/Users/dwright/Downloads/twitter.json'

#pragma once

#include <tuple>
#include <optional>
#include <cstdint>
#include <string_view>
#include <vector>
#include <daw/json/daw_json_link.h>

struct metadata_t {
	std::string_view result_type;
	std::string_view iso_language_code;
};	// metadata_t

inline auto describe_json_class( metadata_t ) {
	using namespace daw::json;
	static constexpr char const result_type[] = "result_type";
	static constexpr char const iso_language_code[] = "iso_language_code";
	return daw::json::class_description_t<
		json_string<result_type, std::string_view>
		,json_string<iso_language_code, std::string_view>
>{};
}

inline auto to_json_data( metadata_t const & value ) {
	return std::forward_as_tuple( value.result_type, value.iso_language_code );
}

struct urls_element_t {
	std::string_view url;
	std::string_view expanded_url;
	std::string_view display_url;
	std::vector<int64_t> indices;
};	// urls_element_t

inline auto describe_json_class( urls_element_t ) {
	using namespace daw::json;
	static constexpr char const url[] = "url";
	static constexpr char const expanded_url[] = "expanded_url";
	static constexpr char const display_url[] = "display_url";
	static constexpr char const indices[] = "indices";
	return daw::json::class_description_t<
		json_string<url, std::string_view>
		,json_string<expanded_url, std::string_view>
		,json_string<display_url, std::string_view>
		,json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( urls_element_t const & value ) {
	return std::forward_as_tuple( value.url, value.expanded_url, value.display_url, value.indices );
}

struct url_t {
	std::vector<urls_element_t> urls;
};	// url_t

inline auto describe_json_class( url_t ) {
	using namespace daw::json;
	static constexpr char const urls[] = "urls";
	return daw::json::class_description_t<
		json_array<urls, std::vector<urls_element_t>, json_class<no_name, urls_element_t>>
>{};
}

inline auto to_json_data( url_t const & value ) {
	return std::forward_as_tuple( value.urls );
}

struct description_t {
	std::optional<std::vector<urls_element_t>> urls;
};	// description_t

inline auto describe_json_class( description_t ) {
	using namespace daw::json;
	static constexpr char const urls[] = "urls";
	return daw::json::class_description_t<
		json_nullable<json_array<urls, std::vector<urls_element_t>, json_class<no_name, urls_element_t>>>
>{};
}

inline auto to_json_data( description_t const & value ) {
	return std::forward_as_tuple( value.urls );
}

struct entities_t {
	std::optional<url_t> url;
	std::optional<description_t> description;
};	// entities_t

inline auto describe_json_class( entities_t ) {
	using namespace daw::json;
	static constexpr char const url[] = "url";
	static constexpr char const description[] = "description";
	return daw::json::class_description_t<
		json_nullable<json_class<url, url_t>>
		,json_nullable<json_class<description, description_t>>
>{};
}

inline auto to_json_data( entities_t const & value ) {
	return std::forward_as_tuple( value.url, value.description );
}

struct user_t {
	int64_t id;
	std::string_view id_str;
	std::string_view name;
	std::string_view screen_name;
	std::string_view location;
	std::string_view description;
	std::optional<std::string_view> url;
	entities_t entities;
	bool _jsonprotected;
	int64_t followers_count;
	int64_t friends_count;
	int64_t listed_count;
	std::string_view created_at;
	int64_t favourites_count;
	std::optional<int64_t> utc_offset;
	std::optional<std::string_view> time_zone;
	bool geo_enabled;
	bool verified;
	int64_t statuses_count;
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
};	// user_t

inline auto describe_json_class( user_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const name[] = "name";
	static constexpr char const screen_name[] = "screen_name";
	static constexpr char const location[] = "location";
	static constexpr char const description[] = "description";
	static constexpr char const url[] = "url";
	static constexpr char const entities[] = "entities";
	static constexpr char const _jsonprotected[] = "protected";
	static constexpr char const followers_count[] = "followers_count";
	static constexpr char const friends_count[] = "friends_count";
	static constexpr char const listed_count[] = "listed_count";
	static constexpr char const created_at[] = "created_at";
	static constexpr char const favourites_count[] = "favourites_count";
	static constexpr char const utc_offset[] = "utc_offset";
	static constexpr char const time_zone[] = "time_zone";
	static constexpr char const geo_enabled[] = "geo_enabled";
	static constexpr char const verified[] = "verified";
	static constexpr char const statuses_count[] = "statuses_count";
	static constexpr char const lang[] = "lang";
	static constexpr char const contributors_enabled[] = "contributors_enabled";
	static constexpr char const is_translator[] = "is_translator";
	static constexpr char const is_translation_enabled[] = "is_translation_enabled";
	static constexpr char const profile_background_color[] = "profile_background_color";
	static constexpr char const profile_background_image_url[] = "profile_background_image_url";
	static constexpr char const profile_background_image_url_https[] = "profile_background_image_url_https";
	static constexpr char const profile_background_tile[] = "profile_background_tile";
	static constexpr char const profile_image_url[] = "profile_image_url";
	static constexpr char const profile_image_url_https[] = "profile_image_url_https";
	static constexpr char const profile_banner_url[] = "profile_banner_url";
	static constexpr char const profile_link_color[] = "profile_link_color";
	static constexpr char const profile_sidebar_border_color[] = "profile_sidebar_border_color";
	static constexpr char const profile_sidebar_fill_color[] = "profile_sidebar_fill_color";
	static constexpr char const profile_text_color[] = "profile_text_color";
	static constexpr char const profile_use_background_image[] = "profile_use_background_image";
	static constexpr char const default_profile[] = "default_profile";
	static constexpr char const default_profile_image[] = "default_profile_image";
	static constexpr char const following[] = "following";
	static constexpr char const follow_request_sent[] = "follow_request_sent";
	static constexpr char const notifications[] = "notifications";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<id_str, std::string_view>
		,json_string<name, std::string_view>
		,json_string<screen_name, std::string_view>
		,json_string<location, std::string_view>
		,json_string<description, std::string_view>
		,json_nullable<json_string<url, std::string_view>>
		,json_class<entities, entities_t>
		,json_bool<_jsonprotected>
		,json_number<followers_count, intmax_t>
		,json_number<friends_count, intmax_t>
		,json_number<listed_count, intmax_t>
		,json_string<created_at, std::string_view>
		,json_number<favourites_count, intmax_t>
		,json_nullable<json_number<utc_offset, intmax_t>>
		,json_nullable<json_string<time_zone, std::string_view>>
		,json_bool<geo_enabled>
		,json_bool<verified>
		,json_number<statuses_count, intmax_t>
		,json_string<lang, std::string_view>
		,json_bool<contributors_enabled>
		,json_bool<is_translator>
		,json_bool<is_translation_enabled>
		,json_string<profile_background_color, std::string_view>
		,json_string<profile_background_image_url, std::string_view>
		,json_string<profile_background_image_url_https, std::string_view>
		,json_bool<profile_background_tile>
		,json_string<profile_image_url, std::string_view>
		,json_string<profile_image_url_https, std::string_view>
		,json_nullable<json_string<profile_banner_url, std::string_view>>
		,json_string<profile_link_color, std::string_view>
		,json_string<profile_sidebar_border_color, std::string_view>
		,json_string<profile_sidebar_fill_color, std::string_view>
		,json_string<profile_text_color, std::string_view>
		,json_bool<profile_use_background_image>
		,json_bool<default_profile>
		,json_bool<default_profile_image>
		,json_bool<following>
		,json_bool<follow_request_sent>
		,json_bool<notifications>
>{};
}

inline auto to_json_data( user_t const & value ) {
	return std::forward_as_tuple( value.id, value.id_str, value.name, value.screen_name, value.location, value.description, value.url, value.entities, value._jsonprotected, value.followers_count, value.friends_count, value.listed_count, value.created_at, value.favourites_count, value.utc_offset, value.time_zone, value.geo_enabled, value.verified, value.statuses_count, value.lang, value.contributors_enabled, value.is_translator, value.is_translation_enabled, value.profile_background_color, value.profile_background_image_url, value.profile_background_image_url_https, value.profile_background_tile, value.profile_image_url, value.profile_image_url_https, value.profile_banner_url, value.profile_link_color, value.profile_sidebar_border_color, value.profile_sidebar_fill_color, value.profile_text_color, value.profile_use_background_image, value.default_profile, value.default_profile_image, value.following, value.follow_request_sent, value.notifications );
}

struct hashtags_element_t {
	std::string_view text;
	std::vector<int64_t> indices;
};	// hashtags_element_t

inline auto describe_json_class( hashtags_element_t ) {
	using namespace daw::json;
	static constexpr char const text[] = "text";
	static constexpr char const indices[] = "indices";
	return daw::json::class_description_t<
		json_string<text, std::string_view>
		,json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( hashtags_element_t const & value ) {
	return std::forward_as_tuple( value.text, value.indices );
}

struct statuses_element_t {
	metadata_t metadata;
	std::string_view created_at;
	int64_t id;
	std::string_view id_str;
	std::string_view text;
	std::string_view source;
	bool truncated;
	std::optional<int64_t> in_reply_to_status_id;
	std::optional<std::string_view> in_reply_to_status_id_str;
	std::optional<int64_t> in_reply_to_user_id;
	std::optional<std::string_view> in_reply_to_user_id_str;
	std::optional<std::string_view> in_reply_to_screen_name;
	user_t user;
	int64_t retweet_count;
	int64_t favorite_count;
	entities_t entities;
	bool favorited;
	bool retweeted;
	std::optional<bool> possibly_sensitive;
	std::string_view lang;
};	// statuses_element_t

inline auto describe_json_class( statuses_element_t ) {
	using namespace daw::json;
	static constexpr char const metadata[] = "metadata";
	static constexpr char const created_at[] = "created_at";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const text[] = "text";
	static constexpr char const source[] = "source";
	static constexpr char const truncated[] = "truncated";
	static constexpr char const in_reply_to_status_id[] = "in_reply_to_status_id";
	static constexpr char const in_reply_to_status_id_str[] = "in_reply_to_status_id_str";
	static constexpr char const in_reply_to_user_id[] = "in_reply_to_user_id";
	static constexpr char const in_reply_to_user_id_str[] = "in_reply_to_user_id_str";
	static constexpr char const in_reply_to_screen_name[] = "in_reply_to_screen_name";
	static constexpr char const user[] = "user";
	static constexpr char const retweet_count[] = "retweet_count";
	static constexpr char const favorite_count[] = "favorite_count";
	static constexpr char const entities[] = "entities";
	static constexpr char const favorited[] = "favorited";
	static constexpr char const retweeted[] = "retweeted";
	static constexpr char const possibly_sensitive[] = "possibly_sensitive";
	static constexpr char const lang[] = "lang";
	return daw::json::class_description_t<
		json_class<metadata, metadata_t>
		,json_string<created_at, std::string_view>
		,json_number<id, intmax_t>
		,json_string<id_str, std::string_view>
		,json_string<text, std::string_view>
		,json_string<source, std::string_view>
		,json_bool<truncated>
		,json_nullable<json_number<in_reply_to_status_id, intmax_t>>
		,json_nullable<json_string<in_reply_to_status_id_str, std::string_view>>
		,json_nullable<json_number<in_reply_to_user_id, intmax_t>>
		,json_nullable<json_string<in_reply_to_user_id_str, std::string_view>>
		,json_nullable<json_string<in_reply_to_screen_name, std::string_view>>
		,json_class<user, user_t>
		,json_number<retweet_count, intmax_t>
		,json_number<favorite_count, intmax_t>
		,json_class<entities, entities_t>
		,json_bool<favorited>
		,json_bool<retweeted>
		,json_nullable<json_bool<possibly_sensitive>>
		,json_string<lang, std::string_view>
>{};
}

inline auto to_json_data( statuses_element_t const & value ) {
	return std::forward_as_tuple( value.metadata, value.created_at, value.id, value.id_str, value.text, value.source, value.truncated, value.in_reply_to_status_id, value.in_reply_to_status_id_str, value.in_reply_to_user_id, value.in_reply_to_user_id_str, value.in_reply_to_screen_name, value.user, value.retweet_count, value.favorite_count, value.entities, value.favorited, value.retweeted, value.possibly_sensitive, value.lang );
}

struct user_mentions_element_t {
	std::string_view screen_name;
	std::string_view name;
	int64_t id;
	std::string_view id_str;
	std::vector<int64_t> indices;
};	// user_mentions_element_t

inline auto describe_json_class( user_mentions_element_t ) {
	using namespace daw::json;
	static constexpr char const screen_name[] = "screen_name";
	static constexpr char const name[] = "name";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const indices[] = "indices";
	return daw::json::class_description_t<
		json_string<screen_name, std::string_view>
		,json_string<name, std::string_view>
		,json_number<id, intmax_t>
		,json_string<id_str, std::string_view>
		,json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( user_mentions_element_t const & value ) {
	return std::forward_as_tuple( value.screen_name, value.name, value.id, value.id_str, value.indices );
}

struct medium_t {
	int64_t w;
	int64_t h;
	std::string_view resize;
};	// medium_t

inline auto describe_json_class( medium_t ) {
	using namespace daw::json;
	static constexpr char const w[] = "w";
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	return daw::json::class_description_t<
		json_number<w, intmax_t>
		,json_number<h, intmax_t>
		,json_string<resize, std::string_view>
>{};
}

inline auto to_json_data( medium_t const & value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct small_t {
	int64_t w;
	int64_t h;
	std::string_view resize;
};	// small_t

inline auto describe_json_class( small_t ) {
	using namespace daw::json;
	static constexpr char const w[] = "w";
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	return daw::json::class_description_t<
		json_number<w, intmax_t>
		,json_number<h, intmax_t>
		,json_string<resize, std::string_view>
>{};
}

inline auto to_json_data( small_t const & value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct thumb_t {
	int64_t w;
	int64_t h;
	std::string_view resize;
};	// thumb_t

inline auto describe_json_class( thumb_t ) {
	using namespace daw::json;
	static constexpr char const w[] = "w";
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	return daw::json::class_description_t<
		json_number<w, intmax_t>
		,json_number<h, intmax_t>
		,json_string<resize, std::string_view>
>{};
}

inline auto to_json_data( thumb_t const & value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct large_t {
	int64_t w;
	int64_t h;
	std::string_view resize;
};	// large_t

inline auto describe_json_class( large_t ) {
	using namespace daw::json;
	static constexpr char const w[] = "w";
	static constexpr char const h[] = "h";
	static constexpr char const resize[] = "resize";
	return daw::json::class_description_t<
		json_number<w, intmax_t>
		,json_number<h, intmax_t>
		,json_string<resize, std::string_view>
>{};
}

inline auto to_json_data( large_t const & value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct sizes_t {
	medium_t medium;
	small_t small;
	thumb_t thumb;
	large_t large;
};	// sizes_t

inline auto describe_json_class( sizes_t ) {
	using namespace daw::json;
	static constexpr char const medium[] = "medium";
	static constexpr char const small[] = "small";
	static constexpr char const thumb[] = "thumb";
	static constexpr char const large[] = "large";
	return daw::json::class_description_t<
		json_class<medium, medium_t>
		,json_class<small, small_t>
		,json_class<thumb, thumb_t>
		,json_class<large, large_t>
>{};
}

inline auto to_json_data( sizes_t const & value ) {
	return std::forward_as_tuple( value.medium, value.small, value.thumb, value.large );
}

struct media_element_t {
	int64_t id;
	std::string_view id_str;
	std::vector<int64_t> indices;
	std::string_view media_url;
	std::string_view media_url_https;
	std::string_view url;
	std::string_view display_url;
	std::string_view expanded_url;
	std::string_view type;
	sizes_t sizes;
};	// media_element_t

inline auto describe_json_class( media_element_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const indices[] = "indices";
	static constexpr char const media_url[] = "media_url";
	static constexpr char const media_url_https[] = "media_url_https";
	static constexpr char const url[] = "url";
	static constexpr char const display_url[] = "display_url";
	static constexpr char const expanded_url[] = "expanded_url";
	static constexpr char const type[] = "type";
	static constexpr char const sizes[] = "sizes";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<id_str, std::string_view>
		,json_array<indices, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_string<media_url, std::string_view>
		,json_string<media_url_https, std::string_view>
		,json_string<url, std::string_view>
		,json_string<display_url, std::string_view>
		,json_string<expanded_url, std::string_view>
		,json_string<type, std::string_view>
		,json_class<sizes, sizes_t>
>{};
}

inline auto to_json_data( media_element_t const & value ) {
	return std::forward_as_tuple( value.id, value.id_str, value.indices, value.media_url, value.media_url_https, value.url, value.display_url, value.expanded_url, value.type, value.sizes );
}

struct retweeted_status_t {
	metadata_t metadata;
	std::string_view created_at;
	int64_t id;
	std::string_view id_str;
	std::string_view text;
	std::string_view source;
	bool truncated;
	std::optional<int64_t> in_reply_to_status_id;
	std::optional<std::string_view> in_reply_to_status_id_str;
	std::optional<int64_t> in_reply_to_user_id;
	std::optional<std::string_view> in_reply_to_user_id_str;
	std::optional<std::string_view> in_reply_to_screen_name;
	user_t user;
	int64_t retweet_count;
	int64_t favorite_count;
	entities_t entities;
	bool favorited;
	bool retweeted;
	std::optional<bool> possibly_sensitive;
	std::string_view lang;
};	// retweeted_status_t

inline auto describe_json_class( retweeted_status_t ) {
	using namespace daw::json;
	static constexpr char const metadata[] = "metadata";
	static constexpr char const created_at[] = "created_at";
	static constexpr char const id[] = "id";
	static constexpr char const id_str[] = "id_str";
	static constexpr char const text[] = "text";
	static constexpr char const source[] = "source";
	static constexpr char const truncated[] = "truncated";
	static constexpr char const in_reply_to_status_id[] = "in_reply_to_status_id";
	static constexpr char const in_reply_to_status_id_str[] = "in_reply_to_status_id_str";
	static constexpr char const in_reply_to_user_id[] = "in_reply_to_user_id";
	static constexpr char const in_reply_to_user_id_str[] = "in_reply_to_user_id_str";
	static constexpr char const in_reply_to_screen_name[] = "in_reply_to_screen_name";
	static constexpr char const user[] = "user";
	static constexpr char const retweet_count[] = "retweet_count";
	static constexpr char const favorite_count[] = "favorite_count";
	static constexpr char const entities[] = "entities";
	static constexpr char const favorited[] = "favorited";
	static constexpr char const retweeted[] = "retweeted";
	static constexpr char const possibly_sensitive[] = "possibly_sensitive";
	static constexpr char const lang[] = "lang";
	return daw::json::class_description_t<
		json_class<metadata, metadata_t>
		,json_string<created_at, std::string_view>
		,json_number<id, intmax_t>
		,json_string<id_str, std::string_view>
		,json_string<text, std::string_view>
		,json_string<source, std::string_view>
		,json_bool<truncated>
		,json_nullable<json_number<in_reply_to_status_id, intmax_t>>
		,json_nullable<json_string<in_reply_to_status_id_str, std::string_view>>
		,json_nullable<json_number<in_reply_to_user_id, intmax_t>>
		,json_nullable<json_string<in_reply_to_user_id_str, std::string_view>>
		,json_nullable<json_string<in_reply_to_screen_name, std::string_view>>
		,json_class<user, user_t>
		,json_number<retweet_count, intmax_t>
		,json_number<favorite_count, intmax_t>
		,json_class<entities, entities_t>
		,json_bool<favorited>
		,json_bool<retweeted>
		,json_nullable<json_bool<possibly_sensitive>>
		,json_string<lang, std::string_view>
>{};
}

inline auto to_json_data( retweeted_status_t const & value ) {
	return std::forward_as_tuple( value.metadata, value.created_at, value.id, value.id_str, value.text, value.source, value.truncated, value.in_reply_to_status_id, value.in_reply_to_status_id_str, value.in_reply_to_user_id, value.in_reply_to_user_id_str, value.in_reply_to_screen_name, value.user, value.retweet_count, value.favorite_count, value.entities, value.favorited, value.retweeted, value.possibly_sensitive, value.lang );
}

struct search_metadata_t {
	double completed_in;
	int64_t max_id;
	std::string_view max_id_str;
	std::string_view next_results;
	std::string_view query;
	std::string_view refresh_url;
	int64_t count;
	int64_t since_id;
	std::string_view since_id_str;
};	// search_metadata_t

inline auto describe_json_class( search_metadata_t ) {
	using namespace daw::json;
	static constexpr char const completed_in[] = "completed_in";
	static constexpr char const max_id[] = "max_id";
	static constexpr char const max_id_str[] = "max_id_str";
	static constexpr char const next_results[] = "next_results";
	static constexpr char const query[] = "query";
	static constexpr char const refresh_url[] = "refresh_url";
	static constexpr char const count[] = "count";
	static constexpr char const since_id[] = "since_id";
	static constexpr char const since_id_str[] = "since_id_str";
	return daw::json::class_description_t<
		json_number<completed_in>
		,json_number<max_id, intmax_t>
		,json_string<max_id_str, std::string_view>
		,json_string<next_results, std::string_view>
		,json_string<query, std::string_view>
		,json_string<refresh_url, std::string_view>
		,json_number<count, intmax_t>
		,json_number<since_id, intmax_t>
		,json_string<since_id_str, std::string_view>
>{};
}

inline auto to_json_data( search_metadata_t const & value ) {
	return std::forward_as_tuple( value.completed_in, value.max_id, value.max_id_str, value.next_results, value.query, value.refresh_url, value.count, value.since_id, value.since_id_str );
}

struct twitter_object_t {
	std::vector<statuses_element_t> statuses;
	search_metadata_t search_metadata;
};	// root_object_t

inline auto describe_json_class( twitter_object_t ) {
	using namespace daw::json;
	static constexpr char const statuses[] = "statuses";
	static constexpr char const search_metadata[] = "search_metadata";
	return daw::json::class_description_t<
		json_array<statuses, std::vector<statuses_element_t>, json_class<no_name, statuses_element_t>>
		,json_class<search_metadata, search_metadata_t>
>{};
}

inline auto to_json_data( twitter_object_t const & value ) {
	return std::forward_as_tuple( value.statuses, value.search_metadata );
}

