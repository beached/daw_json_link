// Code auto generated from json file 'twitter.json'

#pragma once

#include <cstdint>
#include <daw/json/daw_json_link.h>
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

struct metadata_t {
	std::string result_type;
	std::string iso_language_code;
}; // metadata_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( metadata_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_string<"result_type">,
	                                      json_string<"iso_language_code">>{};
}
#else
namespace symbols_metadata_t {
	static inline constexpr char const result_type[] = "result_type";
	static inline constexpr char const iso_language_code[] = "iso_language_code";
} // namespace symbols_metadata_t

static inline auto describe_json_class( metadata_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<symbols_metadata_t::result_type>,
	  json_string<symbols_metadata_t::iso_language_code>>{};
}
#endif

static inline auto to_json_data( metadata_t const &value ) {
	return std::forward_as_tuple( value.result_type, value.iso_language_code );
}

struct urls_element_t {
	std::string url;
	std::string expanded_url;
	std::string display_url;
	std::vector<int64_t> indices;
}; // urls_element_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( urls_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<"url">, json_string<"expanded_url">, json_string<"display_url">,
	  json_array<"indices", int64_t>>{};
}
#else
namespace symbols_urls_element_t {
	static inline constexpr char const url[] = "url";
	static inline constexpr char const expanded_url[] = "expanded_url";
	static inline constexpr char const display_url[] = "display_url";
	static inline constexpr char const indices[] = "indices";
} // namespace symbols_urls_element_t
static inline auto describe_json_class( urls_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<symbols_urls_element_t::url>,
	  json_string<symbols_urls_element_t::expanded_url>,
	  json_string<symbols_urls_element_t::display_url>,
	  json_array<symbols_urls_element_t::indices, int64_t>>{};
}
#endif

static inline auto to_json_data( urls_element_t const &value ) {
	return std::forward_as_tuple( value.url, value.expanded_url,
	                              value.display_url, value.indices );
}

struct url_t {
	std::vector<urls_element_t> urls;
}; // url_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( url_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_array<"urls", urls_element_t>>{};
}
#else
namespace symbols_url_t {
	static inline constexpr char const urls[] = "urls";
}
static inline auto describe_json_class( url_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_array<symbols_url_t::urls, urls_element_t>>{};
}
#endif

static inline auto to_json_data( url_t const &value ) {
	return std::forward_as_tuple( value.urls );
}

struct description_t {
	std::vector<urls_element_t> urls;
}; // description_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( description_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_array<"urls", urls_element_t>>{};
}
#else
namespace symbols_description_t {
	static inline constexpr char const urls[] = "urls";
}
static inline auto describe_json_class( description_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_array<symbols_description_t::urls, urls_element_t>>{};
}
#endif

static inline auto to_json_data( description_t const &value ) {
	return std::forward_as_tuple( value.urls );
}

struct entities_t {
	std::optional<url_t> url;
	std::optional<description_t> description;
}; // entities_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( entities_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_nullable<json_class<"url", url_t>>,
	  json_nullable<json_class<"description", description_t>>>{};
}
#else
namespace symbols_entities_t {
	static inline constexpr char const url[] = "url";
	static inline constexpr char const description[] = "description";
} // namespace symbols_entities_t
static inline auto describe_json_class( entities_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_nullable<json_class<symbols_entities_t::url, url_t>>,
	  json_nullable<
	    json_class<symbols_entities_t::description, description_t>>>{};
}
#endif

static inline auto to_json_data( entities_t const &value ) {
	return std::forward_as_tuple( value.url, value.description );
}

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
	int64_t followers_count;
	int64_t friends_count;
	int64_t listed_count;
	std::string created_at;
	int64_t favourites_count;
	std::optional<int64_t> utc_offset;
	std::optional<std::string> time_zone;
	bool geo_enabled;
	bool verified;
	int64_t statuses_count;
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

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( user_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<"id", int64_t>, json_string<"id_str">, json_string<"name">,
	  json_string<"screen_name">, json_string<"location">,
	  json_string<"description">, json_nullable<json_string<"url">>,
	  json_class<"entities", entities_t>, json_bool<"protected">,
	  json_number<"followers_count", int64_t>,
	  json_number<"friends_count", int64_t>, json_number<"listed_count", int64_t>,
	  json_string<"created_at">, json_number<"favourites_count", int64_t>,
	  json_nullable<json_number<"utc_offset", int64_t>>,
	  json_nullable<json_string<"time_zone">>, json_bool<"geo_enabled">,
	  json_bool<"verified">, json_number<"statuses_count", int64_t>,
	  json_string<"lang">, json_bool<"contributors_enabled">,
	  json_bool<"is_translator">, json_bool<"is_translation_enabled">,
	  json_string<"profile_background_color">,
	  json_string<"profile_background_image_url">,
	  json_string<"profile_background_image_url_https">,
	  json_bool<"profile_background_tile">, json_string<"profile_image_url">,
	  json_string<"profile_image_url_https">,
	  json_nullable<json_string<"profile_banner_url">>,
	  json_string<"profile_link_color">,
	  json_string<"profile_sidebar_border_color">,
	  json_string<"profile_sidebar_fill_color">,
	  json_string<"profile_text_color">,
	  json_bool<"profile_use_background_image">, json_bool<"default_profile">,
	  json_bool<"default_profile_image">, json_bool<"following">,
	  json_bool<"follow_request_sent">, json_bool<"notifications">>{};
}
#else
namespace symbols_user_t {
	static inline constexpr char const id[] = "id";
	static inline constexpr char const id_str[] = "id_str";
	static inline constexpr char const name[] = "name";
	static inline constexpr char const screen_name[] = "screen_name";
	static inline constexpr char const location[] = "location";
	static inline constexpr char const description[] = "description";
	static inline constexpr char const url[] = "url";
	static inline constexpr char const entities[] = "entities";
	static inline constexpr char const _jsonprotected[] = "protected";
	static inline constexpr char const followers_count[] = "followers_count";
	static inline constexpr char const friends_count[] = "friends_count";
	static inline constexpr char const listed_count[] = "listed_count";
	static inline constexpr char const created_at[] = "created_at";
	static inline constexpr char const favourites_count[] = "favourites_count";
	static inline constexpr char const utc_offset[] = "utc_offset";
	static inline constexpr char const time_zone[] = "time_zone";
	static inline constexpr char const geo_enabled[] = "geo_enabled";
	static inline constexpr char const verified[] = "verified";
	static inline constexpr char const statuses_count[] = "statuses_count";
	static inline constexpr char const lang[] = "lang";
	static inline constexpr char const contributors_enabled[] =
	  "contributors_enabled";
	static inline constexpr char const is_translator[] = "is_translator";
	static inline constexpr char const is_translation_enabled[] =
	  "is_translation_enabled";
	static inline constexpr char const profile_background_color[] =
	  "profile_background_color";
	static inline constexpr char const profile_background_image_url[] =
	  "profile_background_image_url";
	static inline constexpr char const profile_background_image_url_https[] =
	  "profile_background_image_url_https";
	static inline constexpr char const profile_background_tile[] =
	  "profile_background_tile";
	static inline constexpr char const profile_image_url[] = "profile_image_url";
	static inline constexpr char const profile_image_url_https[] =
	  "profile_image_url_https";
	static inline constexpr char const profile_banner_url[] =
	  "profile_banner_url";
	static inline constexpr char const profile_link_color[] =
	  "profile_link_color";
	static inline constexpr char const profile_sidebar_border_color[] =
	  "profile_sidebar_border_color";
	static inline constexpr char const profile_sidebar_fill_color[] =
	  "profile_sidebar_fill_color";
	static inline constexpr char const profile_text_color[] =
	  "profile_text_color";
	static inline constexpr char const profile_use_background_image[] =
	  "profile_use_background_image";
	static inline constexpr char const default_profile[] = "default_profile";
	static inline constexpr char const default_profile_image[] =
	  "default_profile_image";
	static inline constexpr char const following[] = "following";
	static inline constexpr char const follow_request_sent[] =
	  "follow_request_sent";
	static inline constexpr char const notifications[] = "notifications";
} // namespace symbols_user_t

static inline auto describe_json_class( user_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_user_t::id, int64_t>,
	  json_string<symbols_user_t::id_str>, json_string<symbols_user_t::name>,
	  json_string<symbols_user_t::screen_name>,
	  json_string<symbols_user_t::location>,
	  json_string<symbols_user_t::description>,
	  json_nullable<json_string<symbols_user_t::url>>,
	  json_class<symbols_user_t::entities, entities_t>,
	  json_bool<symbols_user_t::_jsonprotected>,
	  json_number<symbols_user_t::followers_count, int64_t>,
	  json_number<symbols_user_t::friends_count, int64_t>,
	  json_number<symbols_user_t::listed_count, int64_t>,
	  json_string<symbols_user_t::created_at>,
	  json_number<symbols_user_t::favourites_count, int64_t>,
	  json_nullable<json_number<symbols_user_t::utc_offset, int64_t>>,
	  json_nullable<json_string<symbols_user_t::time_zone>>,
	  json_bool<symbols_user_t::geo_enabled>, json_bool<symbols_user_t::verified>,
	  json_number<symbols_user_t::statuses_count, int64_t>,
	  json_string<symbols_user_t::lang>,
	  json_bool<symbols_user_t::contributors_enabled>,
	  json_bool<symbols_user_t::is_translator>,
	  json_bool<symbols_user_t::is_translation_enabled>,
	  json_string<symbols_user_t::profile_background_color>,
	  json_string<symbols_user_t::profile_background_image_url>,
	  json_string<symbols_user_t::profile_background_image_url_https,
	              std::string>,
	  json_bool<symbols_user_t::profile_background_tile>,
	  json_string<symbols_user_t::profile_image_url>,
	  json_string<symbols_user_t::profile_image_url_https>,
	  json_nullable<json_string<symbols_user_t::profile_banner_url>>,
	  json_string<symbols_user_t::profile_link_color>,
	  json_string<symbols_user_t::profile_sidebar_border_color>,
	  json_string<symbols_user_t::profile_sidebar_fill_color>,
	  json_string<symbols_user_t::profile_text_color>,
	  json_bool<symbols_user_t::profile_use_background_image>,
	  json_bool<symbols_user_t::default_profile>,
	  json_bool<symbols_user_t::default_profile_image>,
	  json_bool<symbols_user_t::following>,
	  json_bool<symbols_user_t::follow_request_sent>,
	  json_bool<symbols_user_t::notifications>>{};
}
#endif

static inline auto to_json_data( user_t const &value ) {
	return std::forward_as_tuple(
	  value.id, value.id_str, value.name, value.screen_name, value.location,
	  value.description, value.url, value.entities, value._jsonprotected,
	  value.followers_count, value.friends_count, value.listed_count,
	  value.created_at, value.favourites_count, value.utc_offset, value.time_zone,
	  value.geo_enabled, value.verified, value.statuses_count, value.lang,
	  value.contributors_enabled, value.is_translator,
	  value.is_translation_enabled, value.profile_background_color,
	  value.profile_background_image_url,
	  value.profile_background_image_url_https, value.profile_background_tile,
	  value.profile_image_url, value.profile_image_url_https,
	  value.profile_banner_url, value.profile_link_color,
	  value.profile_sidebar_border_color, value.profile_sidebar_fill_color,
	  value.profile_text_color, value.profile_use_background_image,
	  value.default_profile, value.default_profile_image, value.following,
	  value.follow_request_sent, value.notifications );
}

struct hashtags_element_t {
	std::string text;
	std::vector<int64_t> indices;
}; // hashtags_element_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( hashtags_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_string<"text">,
	                                      json_array<"indices", int64_t>>{};
}
#else
namespace symbols_hashtags_element_t {
	static inline constexpr char const text[] = "text";
	static inline constexpr char const indices[] = "indices";
} // namespace symbols_hashtags_element_t

static inline auto describe_json_class( hashtags_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<symbols_hashtags_element_t::text>,
	  json_array<symbols_hashtags_element_t::indices, int64_t>>{};
}
#endif
static inline auto to_json_data( hashtags_element_t const &value ) {
	return std::forward_as_tuple( value.text, value.indices );
}

struct statuses_element_t {
	metadata_t metadata;
	std::string created_at;
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
	int64_t retweet_count;
	int64_t favorite_count;
	entities_t entities;
	bool favorited;
	bool retweeted;
	std::optional<bool> possibly_sensitive;
	std::string lang;
}; // statuses_element_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( statuses_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_class<"metadata", metadata_t>, json_string<"created_at">,
	  json_number<"id", int64_t>, json_string<"id_str">, json_string<"text">,
	  json_string<"source">, json_bool<"truncated">,
	  json_nullable<json_number<"in_reply_to_status_id", int64_t>>,
	  json_nullable<json_string<"in_reply_to_status_id_str">>,
	  json_nullable<json_number<"in_reply_to_user_id", int64_t>>,
	  json_nullable<json_string<"in_reply_to_user_id_str">>,
	  json_nullable<json_string<"in_reply_to_screen_name">>,
	  json_class<"user", user_t>, json_number<"retweet_count", int64_t>,
	  json_number<"favorite_count", int64_t>, json_class<"entities", entities_t>,
	  json_bool<"favorited">, json_bool<"retweeted">,
	  json_nullable<json_bool<"possibly_sensitive">>, json_string<"lang">>{};
}
#else
namespace symbols_statuses_element_t {
	static inline constexpr char const metadata[] = "metadata";
	static inline constexpr char const created_at[] = "created_at";
	static inline constexpr char const id[] = "id";
	static inline constexpr char const id_str[] = "id_str";
	static inline constexpr char const text[] = "text";
	static inline constexpr char const source[] = "source";
	static inline constexpr char const truncated[] = "truncated";
	static inline constexpr char const in_reply_to_status_id[] =
	  "in_reply_to_status_id";
	static inline constexpr char const in_reply_to_status_id_str[] =
	  "in_reply_to_status_id_str";
	static inline constexpr char const in_reply_to_user_id[] =
	  "in_reply_to_user_id";
	static inline constexpr char const in_reply_to_user_id_str[] =
	  "in_reply_to_user_id_str";
	static inline constexpr char const in_reply_to_screen_name[] =
	  "in_reply_to_screen_name";
	static inline constexpr char const user[] = "user";
	static inline constexpr char const retweet_count[] = "retweet_count";
	static inline constexpr char const favorite_count[] = "favorite_count";
	static inline constexpr char const entities[] = "entities";
	static inline constexpr char const favorited[] = "favorited";
	static inline constexpr char const retweeted[] = "retweeted";
	static inline constexpr char const possibly_sensitive[] =
	  "possibly_sensitive";
	static inline constexpr char const lang[] = "lang";
} // namespace symbols_statuses_element_t

static inline auto describe_json_class( statuses_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_class<symbols_statuses_element_t::metadata, metadata_t>,
	  json_string<symbols_statuses_element_t::created_at>,
	  json_number<symbols_statuses_element_t::id, int64_t>,
	  json_string<symbols_statuses_element_t::id_str>,
	  json_string<symbols_statuses_element_t::text>,
	  json_string<symbols_statuses_element_t::source>,
	  json_bool<symbols_statuses_element_t::truncated>,
	  json_nullable<
	    json_number<symbols_statuses_element_t::in_reply_to_status_id, int64_t>>,
	  json_nullable<
	    json_string<symbols_statuses_element_t::in_reply_to_status_id_str>>,
	  json_nullable<
	    json_number<symbols_statuses_element_t::in_reply_to_user_id, int64_t>>,
	  json_nullable<
	    json_string<symbols_statuses_element_t::in_reply_to_user_id_str>>,
	  json_nullable<
	    json_string<symbols_statuses_element_t::in_reply_to_screen_name>>,
	  json_class<symbols_statuses_element_t::user, user_t>,
	  json_number<symbols_statuses_element_t::retweet_count, int64_t>,
	  json_number<symbols_statuses_element_t::favorite_count, int64_t>,
	  json_class<symbols_statuses_element_t::entities, entities_t>,
	  json_bool<symbols_statuses_element_t::favorited>,
	  json_bool<symbols_statuses_element_t::retweeted>,
	  json_nullable<json_bool<symbols_statuses_element_t::possibly_sensitive>>,
	  json_string<symbols_statuses_element_t::lang>>{};
}
#endif
static inline auto to_json_data( statuses_element_t const &value ) {
	return std::forward_as_tuple(
	  value.metadata, value.created_at, value.id, value.id_str, value.text,
	  value.source, value.truncated, value.in_reply_to_status_id,
	  value.in_reply_to_status_id_str, value.in_reply_to_user_id,
	  value.in_reply_to_user_id_str, value.in_reply_to_screen_name, value.user,
	  value.retweet_count, value.favorite_count, value.entities, value.favorited,
	  value.retweeted, value.possibly_sensitive, value.lang );
}

struct user_mentions_element_t {
	std::string screen_name;
	std::string name;
	int64_t id;
	std::string id_str;
	std::vector<int64_t> indices;
}; // user_mentions_element_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( user_mentions_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<"screen_name">, json_string<"name">, json_number<"id", int64_t>,
	  json_string<"id_str">, json_array<"indices", int64_t>>{};
}
#else
namespace symbols_user_mentions_element_t {
	static inline constexpr char const screen_name[] = "screen_name";
	static inline constexpr char const name[] = "name";
	static inline constexpr char const id[] = "id";
	static inline constexpr char const id_str[] = "id_str";
	static inline constexpr char const indices[] = "indices";
} // namespace symbols_user_mentions_element_t

static inline auto describe_json_class( user_mentions_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_string<symbols_user_mentions_element_t::screen_name>,
	  json_string<symbols_user_mentions_element_t::name>,
	  json_number<symbols_user_mentions_element_t::id, int64_t>,
	  json_string<symbols_user_mentions_element_t::id_str>,
	  json_array<symbols_user_mentions_element_t::indices, int64_t>>{};
}
#endif

static inline auto to_json_data( user_mentions_element_t const &value ) {
	return std::forward_as_tuple( value.screen_name, value.name, value.id,
	                              value.id_str, value.indices );
}

struct medium_t {
	int64_t w;
	int64_t h;
	std::string resize;
}; // medium_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( medium_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_number<"w", int64_t>,
	                                      json_number<"h", int64_t>,
	                                      json_string<"resize">>{};
}
#else
namespace symbols_medium_t {
	static inline constexpr char const w[] = "w";
	static inline constexpr char const h[] = "h";
	static inline constexpr char const resize[] = "resize";
} // namespace symbols_medium_t

static inline auto describe_json_class( medium_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_medium_t::w, int64_t>,
	  json_number<symbols_medium_t::h, int64_t>,
	  json_string<symbols_medium_t::resize>>{};
}
#endif

static inline auto to_json_data( medium_t const &value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct small_t {
	int64_t w;
	int64_t h;
	std::string resize;
}; // small_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( small_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_number<"w", int64_t>,
	                                      json_number<"h", int64_t>,
	                                      json_string<"resize">>{};
}
#else
namespace symbols_small_t {
	static inline constexpr char const w[] = "w";
	static inline constexpr char const h[] = "h";
	static inline constexpr char const resize[] = "resize";
} // namespace symbols_small_t

static inline auto describe_json_class( small_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_small_t::w, int64_t>,
	  json_number<symbols_small_t::h, int64_t>,
	  json_string<symbols_small_t::resize>>{};
}
#endif

static inline auto to_json_data( small_t const &value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct thumb_t {
	int64_t w;
	int64_t h;
	std::string resize;
}; // thumb_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( thumb_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_number<"w", int64_t>,
	                                      json_number<"h", int64_t>,
	                                      json_string<"resize">>{};
}
#else
namespace symbols_thumb_t {
	static inline constexpr char const w[] = "w";
	static inline constexpr char const h[] = "h";
	static inline constexpr char const resize[] = "resize";
} // namespace symbols_thumb_t

static inline auto describe_json_class( thumb_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_thumb_t::w, int64_t>,
	  json_number<symbols_thumb_t::h, int64_t>,
	  json_string<symbols_thumb_t::resize>>{};
}
#endif

static inline auto to_json_data( thumb_t const &value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct large_t {
	int64_t w;
	int64_t h;
	std::string resize;
}; // large_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( large_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<json_number<"w", int64_t>,
	                                      json_number<"h", int64_t>,
	                                      json_string<"resize">>{};
}
#else
namespace symbols_large_t {
	static inline constexpr char const w[] = "w";
	static inline constexpr char const h[] = "h";
	static inline constexpr char const resize[] = "resize";
} // namespace symbols_large_t

static inline auto describe_json_class( large_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_large_t::w, int64_t>,
	  json_number<symbols_large_t::h, int64_t>,
	  json_string<symbols_large_t::resize>>{};
}
#endif

static inline auto to_json_data( large_t const &value ) {
	return std::forward_as_tuple( value.w, value.h, value.resize );
}

struct sizes_t {
	medium_t medium;
	small_t small_;
	thumb_t thumb;
	large_t large;
}; // sizes_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( sizes_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_class<"medium", medium_t>, json_class<"small", small_t>,
	  json_class<"thumb", thumb_t>, json_class<"large", large_t>>{};
}
#else
namespace symbols_sizes_t {
	static inline constexpr char const medium[] = "medium";
	static inline constexpr char const small_[] = "small";
	static inline constexpr char const thumb[] = "thumb";
	static inline constexpr char const large[] = "large";
} // namespace symbols_sizes_t

static inline auto describe_json_class( sizes_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_class<symbols_sizes_t::medium, medium_t>,
	  json_class<symbols_sizes_t::small_, small_t>,
	  json_class<symbols_sizes_t::thumb, thumb_t>,
	  json_class<symbols_sizes_t::large, large_t>>{};
}
#endif

static inline auto to_json_data( sizes_t const &value ) {
	return std::forward_as_tuple( value.medium, value.small_, value.thumb,
	                              value.large );
}

struct media_element_t {
	int64_t id;
	std::string id_str;
	std::vector<int64_t> indices;
	std::string media_url;
	std::string media_url_https;
	std::string url;
	std::string display_url;
	std::string expanded_url;
	std::string type;
	sizes_t sizes;
}; // media_element_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( media_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<"id", int64_t>, json_string<"id_str">,
	  json_array<"indices", int64_t>, json_string<"media_url">,
	  json_string<"media_url_https">, json_string<"url">,
	  json_string<"display_url">, json_string<"expanded_url">,
	  json_string<"type">, json_class<"sizes", sizes_t>>{};
}
#else
namespace symbols_media_element_t {
	static inline constexpr char const id[] = "id";
	static inline constexpr char const id_str[] = "id_str";
	static inline constexpr char const indices[] = "indices";
	static inline constexpr char const media_url[] = "media_url";
	static inline constexpr char const media_url_https[] = "media_url_https";
	static inline constexpr char const url[] = "url";
	static inline constexpr char const display_url[] = "display_url";
	static inline constexpr char const expanded_url[] = "expanded_url";
	static inline constexpr char const type[] = "type";
	static inline constexpr char const sizes[] = "sizes";
} // namespace symbols_media_element_t

static inline auto describe_json_class( media_element_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_media_element_t::id, int64_t>,
	  json_string<symbols_media_element_t::id_str>,
	  json_array<symbols_media_element_t::indices, int64_t>,
	  json_string<symbols_media_element_t::media_url>,
	  json_string<symbols_media_element_t::media_url_https>,
	  json_string<symbols_media_element_t::url>,
	  json_string<symbols_media_element_t::display_url>,
	  json_string<symbols_media_element_t::expanded_url>,
	  json_string<symbols_media_element_t::type>,
	  json_class<symbols_media_element_t::sizes, sizes_t>>{};
}
#endif

static inline auto to_json_data( media_element_t const &value ) {
	return std::forward_as_tuple( value.id, value.id_str, value.indices,
	                              value.media_url, value.media_url_https,
	                              value.url, value.display_url,
	                              value.expanded_url, value.type, value.sizes );
}

struct retweeted_status_t {
	metadata_t metadata;
	std::string created_at;
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
	int64_t retweet_count;
	int64_t favorite_count;
	entities_t entities;
	bool favorited;
	bool retweeted;
	std::optional<bool> possibly_sensitive;
	std::string lang;
}; // retweeted_status_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( retweeted_status_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_class<"metadata", metadata_t>, json_string<"created_at">,
	  json_number<"id", int64_t>, json_string<"id_str">, json_string<"text">,
	  json_string<"source">, json_bool<"truncated">,
	  json_nullable<json_number<"in_reply_to_status_id", int64_t>>,
	  json_nullable<json_string<"in_reply_to_status_id_str">>,
	  json_nullable<json_number<"in_reply_to_user_id", int64_t>>,
	  json_nullable<json_string<"in_reply_to_user_id_str">>,
	  json_nullable<json_string<"in_reply_to_screen_name">>,
	  json_class<"user", user_t>, json_number<"retweet_count", int64_t>,
	  json_number<"favorite_count", int64_t>, json_class<"entities", entities_t>,
	  json_bool<"favorited">, json_bool<"retweeted">,
	  json_nullable<json_bool<"possibly_sensitive">>, json_string<"lang">>{};
}
#else
namespace symbols_retweeted_status_t {
	static inline constexpr char const metadata[] = "metadata";
	static inline constexpr char const created_at[] = "created_at";
	static inline constexpr char const id[] = "id";
	static inline constexpr char const id_str[] = "id_str";
	static inline constexpr char const text[] = "text";
	static inline constexpr char const source[] = "source";
	static inline constexpr char const truncated[] = "truncated";
	static inline constexpr char const in_reply_to_status_id[] =
	  "in_reply_to_status_id";
	static inline constexpr char const in_reply_to_status_id_str[] =
	  "in_reply_to_status_id_str";
	static inline constexpr char const in_reply_to_user_id[] =
	  "in_reply_to_user_id";
	static inline constexpr char const in_reply_to_user_id_str[] =
	  "in_reply_to_user_id_str";
	static inline constexpr char const in_reply_to_screen_name[] =
	  "in_reply_to_screen_name";
	static inline constexpr char const user[] = "user";
	static inline constexpr char const retweet_count[] = "retweet_count";
	static inline constexpr char const favorite_count[] = "favorite_count";
	static inline constexpr char const entities[] = "entities";
	static inline constexpr char const favorited[] = "favorited";
	static inline constexpr char const retweeted[] = "retweeted";
	static inline constexpr char const possibly_sensitive[] =
	  "possibly_sensitive";
	static inline constexpr char const lang[] = "lang";
} // namespace symbols_retweeted_status_t

static inline auto describe_json_class( retweeted_status_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_class<symbols_retweeted_status_t::metadata, metadata_t>,
	  json_string<symbols_retweeted_status_t::created_at>,
	  json_number<symbols_retweeted_status_t::id, int64_t>,
	  json_string<symbols_retweeted_status_t::id_str>,
	  json_string<symbols_retweeted_status_t::text>,
	  json_string<symbols_retweeted_status_t::source>,
	  json_bool<symbols_retweeted_status_t::truncated>,
	  json_nullable<
	    json_number<symbols_retweeted_status_t::in_reply_to_status_id, int64_t>>,
	  json_nullable<
	    json_string<symbols_retweeted_status_t::in_reply_to_status_id_str>>,
	  json_nullable<
	    json_number<symbols_retweeted_status_t::in_reply_to_user_id, int64_t>>,
	  json_nullable<
	    json_string<symbols_retweeted_status_t::in_reply_to_user_id_str>>,
	  json_nullable<
	    json_string<symbols_retweeted_status_t::in_reply_to_screen_name>>,
	  json_class<symbols_retweeted_status_t::user, user_t>,
	  json_number<symbols_retweeted_status_t::retweet_count, int64_t>,
	  json_number<symbols_retweeted_status_t::favorite_count, int64_t>,
	  json_class<symbols_retweeted_status_t::entities, entities_t>,
	  json_bool<symbols_retweeted_status_t::favorited>,
	  json_bool<symbols_retweeted_status_t::retweeted>,
	  json_nullable<json_bool<symbols_retweeted_status_t::possibly_sensitive>>,
	  json_string<symbols_retweeted_status_t::lang>>{};
}
#endif

static inline auto to_json_data( retweeted_status_t const &value ) {
	return std::forward_as_tuple(
	  value.metadata, value.created_at, value.id, value.id_str, value.text,
	  value.source, value.truncated, value.in_reply_to_status_id,
	  value.in_reply_to_status_id_str, value.in_reply_to_user_id,
	  value.in_reply_to_user_id_str, value.in_reply_to_screen_name, value.user,
	  value.retweet_count, value.favorite_count, value.entities, value.favorited,
	  value.retweeted, value.possibly_sensitive, value.lang );
}

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

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( search_metadata_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<"completed_in">, json_number<"max_id", int64_t>,
	  json_string<"max_id_str">, json_string<"next_results">,
	  json_string<"query">, json_string<"refresh_url">,
	  json_number<"count", int64_t>, json_number<"since_id", int64_t>,
	  json_string<"since_id_str">>{};
}
#else
namespace symbols_search_metadata_t {
	static inline constexpr char const completed_in[] = "completed_in";
	static inline constexpr char const max_id[] = "max_id";
	static inline constexpr char const max_id_str[] = "max_id_str";
	static inline constexpr char const next_results[] = "next_results";
	static inline constexpr char const query[] = "query";
	static inline constexpr char const refresh_url[] = "refresh_url";
	static inline constexpr char const count[] = "count";
	static inline constexpr char const since_id[] = "since_id";
	static inline constexpr char const since_id_str[] = "since_id_str";
} // namespace symbols_search_metadata_t

static inline auto describe_json_class( search_metadata_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_number<symbols_search_metadata_t::completed_in>,
	  json_number<symbols_search_metadata_t::max_id, int64_t>,
	  json_string<symbols_search_metadata_t::max_id_str>,
	  json_string<symbols_search_metadata_t::next_results>,
	  json_string<symbols_search_metadata_t::query>,
	  json_string<symbols_search_metadata_t::refresh_url>,
	  json_number<symbols_search_metadata_t::count, int64_t>,
	  json_number<symbols_search_metadata_t::since_id, int64_t>,
	  json_string<symbols_search_metadata_t::since_id_str>>{};
}
#endif

static inline auto to_json_data( search_metadata_t const &value ) {
	return std::forward_as_tuple( value.completed_in, value.max_id,
	                              value.max_id_str, value.next_results,
	                              value.query, value.refresh_url, value.count,
	                              value.since_id, value.since_id_str );
}

struct twitter_object_t {
	std::vector<statuses_element_t> statuses;
	search_metadata_t search_metadata;
}; // twitter_object_t

#ifdef __cpp_nontype_template_parameter_class
static inline auto describe_json_class( twitter_object_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_array<"statuses", statuses_element_t>,
	  json_class<"search_metadata", search_metadata_t>>{};
}
#else
namespace symbols_twitter_object_t {
	static inline constexpr char const statuses[] = "statuses";
	static inline constexpr char const search_metadata[] = "search_metadata";
} // namespace symbols_twitter_object_t

static inline auto describe_json_class( twitter_object_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
	  json_array<symbols_twitter_object_t::statuses, statuses_element_t>,
	  json_class<symbols_twitter_object_t::search_metadata, search_metadata_t>>{};
}
#endif

static inline auto to_json_data( twitter_object_t const &value ) {
	return std::forward_as_tuple( value.statuses, value.search_metadata );
}
