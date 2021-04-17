// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "twitter_test_pmr.h"

#include <daw/daw_hide.h>
#include <daw/daw_utility.h>
#include <daw/json/daw_json_link.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>

namespace daw::twitter {
	struct TimestampConverter {
		constexpr twitter_tp operator( )( std::string_view sv ) const {
			daw_json_assert(
			  sv.size( ) >= 26,
			  daw::json::ErrorReason::InvalidTimestamp ); // Date format is always 26
			                                              // characters long
			// Skip Day of Week
			sv.remove_prefix( 4 );
			auto const mo = daw::json::datetime::parse_short_month( sv );
			sv.remove_prefix( 4 );
			auto const dy =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const hr =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const mn =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const se =
			  daw::json::parse_utils::parse_unsigned<uint_least32_t, 2>( sv.data( ) );
			sv.remove_prefix( 3 );
			auto const sign = sv.front( ) == '+' ? 1 : -1;
			sv.remove_prefix( 1 );
			auto const off_hr =
			  daw::json::parse_utils::parse_unsigned<int_least32_t, 2>( sv.data( ) ) *
			  sign;
			sv.remove_prefix( 2 );
			auto const off_mn =
			  daw::json::parse_utils::parse_unsigned<int_least32_t, 2>( sv.data( ) ) *
			  sign;
			sv.remove_prefix( 3 );
			int const yr_sign = [&] {
				if( sv.front( ) == '-' ) {
					sv.remove_prefix( 1 );
					return -1;
				} else if( sv.front( ) == '+' ) {
					sv.remove_prefix( 1 );
				}
				return 1;
			}( );
			auto const yr =
			  yr_sign *
			  daw::json::parse_utils::parse_unsigned2<int_least32_t>( sv.data( ) );

			return daw::json::datetime::civil_to_time_point( yr, mo, dy, hr, mn, se,
			                                                 0 ) +
			       std::chrono::hours( off_hr ) + std::chrono::minutes( off_mn );
		}

		template<typename OutputIterator>
		constexpr OutputIterator operator( )( OutputIterator it,
		                                      twitter_tp tp ) const {

			auto const &[yr, mo, dy, hr, mn, se, ms] =
			  daw::json::datetime::time_point_to_civil( tp );
			// Day of Week
			it = daw::json::utils::copy_to_iterator(
			  it, daw::json::datetime::short_day_of_week( tp ) );
			*it++ = ' ';
			// Month
			it = daw::json::utils::copy_to_iterator(
			  it, daw::json::datetime::month_short_name( mo ) );
			*it++ = ' ';
			it = daw::json::utils::integer_to_string( it, dy );
			*it++ = ' ';
			if( hr < 10 ) {
				*it++ = '0';
			}
			it = daw::json::utils::integer_to_string( it, hr );
			*it++ = ':';
			if( mn < 10 ) {
				*it++ = '0';
			}
			it = daw::json::utils::integer_to_string( it, mn );
			*it++ = ':';
			if( se < 10 ) {
				*it++ = '0';
			}
			it = daw::json::utils::integer_to_string( it, se );
			it = daw::json::utils::copy_to_iterator( it, " +0000 " );
			it = daw::json::utils::integer_to_string( it, yr );
			return it;
		}
	};
} // namespace daw::twitter

namespace daw::json {
	template<>
	struct json_link_basic_type_map<daw::twitter::String> {
		constexpr static JsonParseTypes parse_type = JsonParseTypes::StringEscaped;
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::metadata_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string<"result_type", daw::twitter::String>,
		                   json_string<"iso_language_code", daw::twitter::String>>;
#else
		static inline constexpr char const result_type[] = "result_type";
		static inline constexpr char const iso_language_code[] =
		  "iso_language_code";
		using type =
		  json_member_list<json_string<result_type, daw::twitter::String>,
		                   json_string<iso_language_code, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::metadata_t const &value ) {
			return std::forward_as_tuple( value.result_type,
			                              value.iso_language_code );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::urls_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_string<"url", daw::twitter::String>,
		  json_string<"expanded_url", daw::twitter::String>,
		  json_string<"display_url", daw::twitter::String>,
		  json_array<"indices", int32_t, daw::twitter::Vector<int32_t>>>;
#else
		static inline constexpr char const url[] = "url";
		static inline constexpr char const expanded_url[] = "expanded_url";
		static inline constexpr char const display_url[] = "display_url";
		static inline constexpr char const indices[] = "indices";
		using type = json_member_list<
		  json_string<url, daw::twitter::String>,
		  json_string<expanded_url, daw::twitter::String>,
		  json_string<display_url, daw::twitter::String>,
		  json_array<indices, int32_t, daw::twitter::Vector<int32_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::urls_element_t const &value ) {
			return std::forward_as_tuple( value.url, value.expanded_url,
			                              value.display_url, value.indices );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::url_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_array<"urls", daw::twitter::urls_element_t,
		             daw::twitter::Vector<daw::twitter::urls_element_t>>>;
#else
		static inline constexpr char const urls[] = "urls";
		using type = json_member_list<
		  json_array<urls, daw::twitter::urls_element_t,
		             daw::twitter::Vector<daw::twitter::urls_element_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::url_t const &value ) {
			return std::forward_as_tuple( value.urls );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::description_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_array<"urls", daw::twitter::urls_element_t,
		             daw::twitter::Vector<daw::twitter::urls_element_t>>>;
#else
		static inline constexpr char const urls[] = "urls";
		using type = json_member_list<
		  json_array<urls, daw::twitter::urls_element_t,
		             daw::twitter::Vector<daw::twitter::urls_element_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::description_t const &value ) {
			return std::forward_as_tuple( value.urls );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::entities_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_class_null<"url", std::optional<daw::twitter::url_t>>,
		  json_class_null<"description",
		                  std::optional<daw::twitter::description_t>>>;
#else
		static inline constexpr char const url[] = "url";
		static inline constexpr char const description[] = "description";
		using type = json_member_list<
		  json_class_null<url, std::optional<daw::twitter::url_t>>,
		  json_class_null<description, std::optional<daw::twitter::description_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::entities_t const &value ) {
			return std::forward_as_tuple( value.url, value.description );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::user_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_number<"id", int64_t>, json_string<"id_str", daw::twitter::String>,
		  json_string<"name", daw::twitter::String>,
		  json_string<"screen_name", daw::twitter::String>,
		  json_string<"location", daw::twitter::String>,
		  json_string<"description", daw::twitter::String>,
		  json_string_null<"url", daw::twitter::OptString>,
		  json_class<"entities", daw::twitter::entities_t>, json_bool<"protected">,
		  json_number<"followers_count", int32_t>,
		  json_number<"friends_count", int32_t>,
		  json_number<"listed_count", int32_t>,
		  json_custom<"created_at", daw::twitter::twitter_tp,
		              daw::twitter::TimestampConverter,
		              daw::twitter::TimestampConverter>,
		  json_number<"favourites_count", int32_t>, json_bool<"geo_enabled">,
		  json_bool<"verified">, json_number<"statuses_count", int32_t>,
		  json_string<"lang", daw::twitter::String>,
		  json_bool<"contributors_enabled">, json_bool<"is_translator">,
		  json_bool<"is_translation_enabled">,
		  json_string<"profile_background_color", daw::twitter::String>,
		  json_string<"profile_background_image_url", daw::twitter::String>,
		  json_string<"profile_background_image_url_https", daw::twitter::String>,
		  json_bool<"profile_background_tile">,
		  json_string<"profile_image_url", daw::twitter::String>,
		  json_string<"profile_image_url_https", daw::twitter::String>,
		  json_string_null<"profile_banner_url", daw::twitter::OptString>,
		  json_string<"profile_link_color", daw::twitter::String>,
		  json_string<"profile_sidebar_border_color", daw::twitter::String>,
		  json_string<"profile_sidebar_fill_color", daw::twitter::String>,
		  json_string<"profile_text_color", daw::twitter::String>,
		  json_bool<"profile_use_background_image">, json_bool<"default_profile">,
		  json_bool<"default_profile_image">, json_bool<"following">,
		  json_bool<"follow_request_sent">, json_bool<"notifications">>;
#else
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
		static inline constexpr char const profile_image_url[] =
		  "profile_image_url";
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
		using type = json_member_list<
		  json_number<id, int64_t>, json_string<id_str, daw::twitter::String>,
		  json_string<name, daw::twitter::String>,
		  json_string<screen_name, daw::twitter::String>,
		  json_string<location, daw::twitter::String>,
		  json_string<description, daw::twitter::String>,
		  json_string_null<url, daw::twitter::OptString>,
		  json_class<entities, daw::twitter::entities_t>, json_bool<_jsonprotected>,
		  json_number<followers_count, int32_t>,
		  json_number<friends_count, int32_t>, json_number<listed_count, int32_t>,
		  json_custom<created_at, daw::twitter::twitter_tp,
		              daw::twitter::TimestampConverter,
		              daw::twitter::TimestampConverter>,
		  json_number<favourites_count, int32_t>, json_bool<geo_enabled>,
		  json_bool<verified>, json_number<statuses_count, int32_t>,
		  json_string<lang, daw::twitter::String>, json_bool<contributors_enabled>,
		  json_bool<is_translator>, json_bool<is_translation_enabled>,
		  json_string<profile_background_color, daw::twitter::String>,
		  json_string<profile_background_image_url, daw::twitter::String>,
		  json_string<profile_background_image_url_https, daw::twitter::String>,
		  json_bool<profile_background_tile>,
		  json_string<profile_image_url, daw::twitter::String>,
		  json_string<profile_image_url_https, daw::twitter::String>,
		  json_string_null<profile_banner_url, daw::twitter::OptString>,
		  json_string<profile_link_color, daw::twitter::String>,
		  json_string<profile_sidebar_border_color, daw::twitter::String>,
		  json_string<profile_sidebar_fill_color, daw::twitter::String>,
		  json_string<profile_text_color, daw::twitter::String>,
		  json_bool<profile_use_background_image>, json_bool<default_profile>,
		  json_bool<default_profile_image>, json_bool<following>,
		  json_bool<follow_request_sent>, json_bool<notifications>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::user_t const &value ) {
			return std::forward_as_tuple(
			  value.id, value.id_str, value.name, value.screen_name, value.location,
			  value.description, value.url, value.entities, value._jsonprotected,
			  value.followers_count, value.friends_count, value.listed_count,
			  value.created_at, value.favourites_count, value.geo_enabled,
			  value.verified, value.statuses_count, value.lang,
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
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN
	  json_data_contract<daw::twitter::hashtags_element_t> {
		using constructor = daw::construct_a_t<daw::twitter::hashtags_element_t>;
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_string<"text", daw::twitter::String>,
		  json_array<"indices", int32_t, daw::twitter::Vector<int32_t>>>;
#else
		static inline constexpr char const text[] = "text";
		static inline constexpr char const indices[] = "indices";
		using type = json_member_list<
		  json_string<text, daw::twitter::String>,
		  json_array<indices, int32_t, daw::twitter::Vector<int32_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::hashtags_element_t const &value ) {
			return std::forward_as_tuple( value.text, value.indices );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::tweet_object_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_class<"metadata", daw::twitter::metadata_t>,
		  json_custom<"created_at", daw::twitter::twitter_tp,
		              daw::twitter::TimestampConverter,
		              daw::twitter::TimestampConverter>,
		  json_number<"id", int64_t>, json_string<"id_str", daw::twitter::String>,
		  json_string<"text", daw::twitter::String>,
		  json_string<"source", daw::twitter::String>, json_bool<"truncated">,
		  json_number_null<"in_reply_to_status_id", std::optional<int64_t>>,
		  json_string_null<"in_reply_to_status_id_str", daw::twitter::OptString>,
		  json_number_null<"in_reply_to_user_id", std::optional<int64_t>>,
		  json_string_null<"in_reply_to_user_id_str", daw::twitter::OptString>,
		  json_string_null<"in_reply_to_screen_name", daw::twitter::OptString>,
		  json_class<"user", daw::twitter::user_t>,
		  json_number<"retweet_count", int32_t>,
		  json_number_null<"favorite_count", std::optional<int32_t>>,
		  json_class<"entities", daw::twitter::entities_t>, json_bool<"favorited">,
		  json_bool<"retweeted">, json_bool_null<"possibly_sensitive">,
		  json_string<"lang", daw::twitter::String>>;
#else
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
		using type = json_member_list<
		  json_class<metadata, daw::twitter::metadata_t>,
		  json_custom<created_at, daw::twitter::twitter_tp,
		              daw::twitter::TimestampConverter,
		              daw::twitter::TimestampConverter>,
		  json_number<id, int64_t>, json_string<id_str, daw::twitter::String>,
		  json_string<text, daw::twitter::String>,
		  json_string<source, daw::twitter::String>, json_bool<truncated>,
		  json_number_null<in_reply_to_status_id, std::optional<int64_t>>,
		  json_string_null<in_reply_to_status_id_str, daw::twitter::OptString>,
		  json_number_null<in_reply_to_user_id, std::optional<int64_t>>,
		  json_string_null<in_reply_to_user_id_str, daw::twitter::OptString>,
		  json_string_null<in_reply_to_screen_name, daw::twitter::OptString>,
		  json_class<user, daw::twitter::user_t>,
		  json_number<retweet_count, int32_t>,
		  json_number_null<favorite_count, std::optional<int32_t>>,
		  json_class<entities, daw::twitter::entities_t>, json_bool<favorited>,
		  json_bool<retweeted>, json_bool_null<possibly_sensitive>,
		  json_string<lang, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::tweet_object_t const &value ) {
			return std::forward_as_tuple(
			  value.metadata, value.created_at, value.id, value.id_str, value.text,
			  value.source, value.truncated, value.in_reply_to_status_id,
			  value.in_reply_to_status_id_str, value.in_reply_to_user_id,
			  value.in_reply_to_user_id_str, value.in_reply_to_screen_name,
			  value.user, value.retweet_count, value.favorite_count, value.entities,
			  value.favorited, value.retweeted, value.possibly_sensitive,
			  value.lang );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN
	  json_data_contract<daw::twitter::user_mentions_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_string<"screen_name", daw::twitter::String>,
		  json_string<"name", daw::twitter::String>, json_number<"id", int64_t>,
		  json_string<"id_str", daw::twitter::String>,
		  json_array<"indices", int64_t, daw::twitter::Vector<int64_t>>>;
#else
		static inline constexpr char const screen_name[] = "screen_name";
		static inline constexpr char const name[] = "name";
		static inline constexpr char const id[] = "id";
		static inline constexpr char const id_str[] = "id_str";
		static inline constexpr char const indices[] = "indices";
		using type = json_member_list<
		  json_string<screen_name, daw::twitter::String>,
		  json_string<name, daw::twitter::String>, json_number<id, int64_t>,
		  json_string<id_str, daw::twitter::String>,
		  json_array<indices, int64_t, daw::twitter::Vector<int64_t>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::user_mentions_element_t const &value ) {
			return std::forward_as_tuple( value.screen_name, value.name, value.id,
			                              value.id_str, value.indices );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::medium_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"w", int64_t>, json_number<"h", int64_t>,
		                   json_string<"resize", daw::twitter::String>>;
#else
		static inline constexpr char const w[] = "w";
		static inline constexpr char const h[] = "h";
		static inline constexpr char const resize[] = "resize";
		using type =
		  json_member_list<json_number<w, int64_t>, json_number<h, int64_t>,
		                   json_string<resize, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::medium_t const &value ) {
			return std::forward_as_tuple( value.w, value.h, value.resize );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::small_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"w", int64_t>, json_number<"h", int64_t>,
		                   json_string<"resize", daw::twitter::String>>;
#else
		static inline constexpr char const w[] = "w";
		static inline constexpr char const h[] = "h";
		static inline constexpr char const resize[] = "resize";
		using type =
		  json_member_list<json_number<w, int64_t>, json_number<h, int64_t>,
		                   json_string<resize, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::small_t const &value ) {
			return std::forward_as_tuple( value.w, value.h, value.resize );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::thumb_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"w", int64_t>, json_number<"h", int64_t>,
		                   json_string<"resize", daw::twitter::String>>;
#else
		static inline constexpr char const w[] = "w";
		static inline constexpr char const h[] = "h";
		static inline constexpr char const resize[] = "resize";
		using type =
		  json_member_list<json_number<w, int64_t>, json_number<h, int64_t>,
		                   json_string<resize, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::thumb_t const &value ) {
			return std::forward_as_tuple( value.w, value.h, value.resize );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::large_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_number<"w", int64_t>, json_number<"h", int64_t>,
		                   json_string<"resize", daw::twitter::String>>;
#else
		static inline constexpr char const w[] = "w";
		static inline constexpr char const h[] = "h";
		static inline constexpr char const resize[] = "resize";
		using type =
		  json_member_list<json_number<w, int64_t>, json_number<h, int64_t>,
		                   json_string<resize, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::large_t const &value ) {
			return std::forward_as_tuple( value.w, value.h, value.resize );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN json_data_contract<daw::twitter::sizes_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_class<"medium", daw::twitter::medium_t>,
		                              json_class<"small", daw::twitter::small_t>,
		                              json_class<"thumb", daw::twitter::thumb_t>,
		                              json_class<"large", daw::twitter::large_t>>;
#else
		static inline constexpr char const medium[] = "medium";
		static inline constexpr char const small_[] = "small";
		static inline constexpr char const thumb[] = "thumb";
		static inline constexpr char const large[] = "large";
		using type = json_member_list<json_class<medium, daw::twitter::medium_t>,
		                              json_class<small_, daw::twitter::small_t>,
		                              json_class<thumb, daw::twitter::thumb_t>,
		                              json_class<large, daw::twitter::large_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::sizes_t const &value ) {
			return std::forward_as_tuple( value.medium, value.small_, value.thumb,
			                              value.large );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN
	  json_data_contract<daw::twitter::media_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_number<"id", int64_t>, json_string<"id_str", daw::twitter::String>,
		  json_array<indices, int64_t, daw::twitter::Vector<int64_t>>,
		  json_string<"media_url", daw::twitter::String>,
		  json_string<"media_url_https", daw::twitter::String>,
		  json_string<"url", daw::twitter::String>,
		  json_string<"display_url", daw::twitter::String>,
		  json_string<"expanded_url", daw::twitter::String>,
		  json_string<"type", daw::twitter::String>,
		  json_class<"sizes", daw::twitter::sizes_t>>;
#else
		static inline constexpr char const id[] = "id";
		static inline constexpr char const id_str[] = "id_str";
		static inline constexpr char const indices[] = "indices";
		static inline constexpr char const media_url[] = "media_url";
		static inline constexpr char const media_url_https[] = "media_url_https";
		static inline constexpr char const url[] = "url";
		static inline constexpr char const display_url[] = "display_url";
		static inline constexpr char const expanded_url[] = "expanded_url";
		static inline constexpr char const _jsontype[] = "type";
		static inline constexpr char const sizes[] = "sizes";
		using type = json_member_list<
		  json_number<id, int64_t>, json_string<id_str, daw::twitter::String>,
		  json_array<indices, int64_t, daw::twitter::Vector<int64_t>>,
		  json_string<media_url, daw::twitter::String>,
		  json_string<media_url_https, daw::twitter::String>,
		  json_string<url, daw::twitter::String>,
		  json_string<display_url, daw::twitter::String>,
		  json_string<expanded_url, daw::twitter::String>,
		  json_string<_jsontype, daw::twitter::String>,
		  json_class<sizes, daw::twitter::sizes_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::media_element_t const &value ) {
			return std::forward_as_tuple(
			  value.id, value.id_str, value.indices, value.media_url,
			  value.media_url_https, value.url, value.display_url, value.expanded_url,
			  value.type, value.sizes );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN
	  json_data_contract<daw::twitter::retweeted_status_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_class<"metadata", daw::twitter::metadata_t>,
		  json_custom<"created_at", daw::twitter::twitter_tp,
		              daw::twitter::TimestampConverter,
		              daw::twitter::TimestampConverter>,
		  json_number<"id", int64_t>, json_string<"id_str", daw::twitter::String>,
		  json_string<"text", daw::twitter::String>,
		  json_string<"source", daw::twitter::String>, json_bool<"truncated">,
		  json_number_null<"in_reply_to_status_id", std::optional<int64_t>>,
		  json_string_null<"in_reply_to_status_id_str", daw::twitter::OptString>,
		  json_number_null<"in_reply_to_user_id", std::optional<int64_t>>,
		  json_string_null<"in_reply_to_user_id_str", daw::twitter::OptString>,
		  json_string_null<"in_reply_to_screen_name", daw::twitter::OptString>,
		  json_class<"user", daw::twitter::user_t>,
		  json_number<"retweet_count", int32_t>,
		  json_number_null<"favorite_count", std::optional<int32_t>>,
		  json_class<"entities", daw::twitter::entities_t>, json_bool<"favorited">,
		  json_bool<"retweeted">, json_bool_null<"possibly_sensitive">,
		  json_string<"lang", daw::twitter::String>>;
#else
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
		using type = json_member_list<
		  json_class<metadata, daw::twitter::metadata_t>,
		  json_custom<created_at, daw::twitter::twitter_tp,
		              daw::twitter::TimestampConverter,
		              daw::twitter::TimestampConverter>,
		  json_number<id, int64_t>, json_string<id_str, daw::twitter::String>,
		  json_string<text, daw::twitter::String>,
		  json_string<source, daw::twitter::String>, json_bool<truncated>,
		  json_number_null<in_reply_to_status_id, std::optional<int64_t>>,
		  json_string_null<in_reply_to_status_id_str, daw::twitter::OptString>,
		  json_number_null<in_reply_to_user_id, std::optional<int64_t>>,
		  json_string_null<in_reply_to_user_id_str, daw::twitter::OptString>,
		  json_string_null<in_reply_to_screen_name, daw::twitter::OptString>,
		  json_class<user, daw::twitter::user_t>,
		  json_number<retweet_count, int32_t>,
		  json_number_null<favorite_count, std::optional<int32_t>>,
		  json_class<entities, daw::twitter::entities_t>, json_bool<favorited>,
		  json_bool<retweeted>, json_bool_null<possibly_sensitive>,
		  json_string<lang, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::retweeted_status_t const &value ) {
			return std::forward_as_tuple(
			  value.metadata, value.created_at, value.id, value.id_str, value.text,
			  value.source, value.truncated, value.in_reply_to_status_id,
			  value.in_reply_to_status_id_str, value.in_reply_to_user_id,
			  value.in_reply_to_user_id_str, value.in_reply_to_screen_name,
			  value.user, value.retweet_count, value.favorite_count, value.entities,
			  value.favorited, value.retweeted, value.possibly_sensitive,
			  value.lang );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN
	  json_data_contract<daw::twitter::search_metadata_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_number<"completed_in">, json_number<"max_id", int64_t>,
		  json_string<"max_id_str", daw::twitter::String>,
		  json_string<"next_results", daw::twitter::String>,
		  json_string<"query", daw::twitter::String>,
		  json_string<"refresh_url", daw::twitter::String>,
		  json_number<"count", int64_t>, json_number<"since_id", int64_t>,
		  json_string<"since_id_str", daw::twitter::String>>;
#else
		static inline constexpr char const completed_in[] = "completed_in";
		static inline constexpr char const max_id[] = "max_id";
		static inline constexpr char const max_id_str[] = "max_id_str";
		static inline constexpr char const next_results[] = "next_results";
		static inline constexpr char const query[] = "query";
		static inline constexpr char const refresh_url[] = "refresh_url";
		static inline constexpr char const count[] = "count";
		static inline constexpr char const since_id[] = "since_id";
		static inline constexpr char const since_id_str[] = "since_id_str";
		using type =
		  json_member_list<json_number<completed_in>, json_number<max_id, int64_t>,
		                   json_string<max_id_str, daw::twitter::String>,
		                   json_string<next_results, daw::twitter::String>,
		                   json_string<query, daw::twitter::String>,
		                   json_string<refresh_url, daw::twitter::String>,
		                   json_number<count, int64_t>,
		                   json_number<since_id, int64_t>,
		                   json_string<since_id_str, daw::twitter::String>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::search_metadata_t const &value ) {
			return std::forward_as_tuple( value.completed_in, value.max_id,
			                              value.max_id_str, value.next_results,
			                              value.query, value.refresh_url, value.count,
			                              value.since_id, value.since_id_str );
		}
	};

	template<>
	struct DAW_ATTRIBUTE_HIDDEN
	  json_data_contract<daw::twitter::twitter_object_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_array<"statuses", daw::twitter::tweet_object_t,
		             daw::twitter::Vector<daw::twitter::tweet_object_t>>,
		  json_class<"search_metadata", daw::twitter::search_metadata_t>>;
#else
		static inline constexpr char const statuses[] = "statuses";
		static inline constexpr char const search_metadata[] = "search_metadata";
		using type = json_member_list<
		  json_array<statuses, daw::twitter::tweet_object_t,
		             daw::twitter::Vector<daw::twitter::tweet_object_t>>,
		  json_class<search_metadata, daw::twitter::search_metadata_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::twitter::twitter_object_t const &value ) {
			return std::forward_as_tuple( value.statuses, value.search_metadata );
		}
	};
} // namespace daw::json
