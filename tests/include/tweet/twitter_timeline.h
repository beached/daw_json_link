// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "tweet.h"
#include "twitter_user.h"

#include "daw/json/daw_json_link.h"

namespace daw::json {
	template<>
	struct json_data_contract<daw::twitter::twitter_user> {
		static constexpr char id[] = "id";
		static constexpr char screen_name[] = "screen_name";

		using type =
		  json_member_list<json_string_raw<screen_name, std::string_view>,
		                   json_number<id, std::uint64_t>>;
	};

	template<>
	struct json_data_contract<daw::twitter::tweet> {
		static constexpr char id[] = "id";
		static constexpr char text[] = "text";
		static constexpr char created_at[] = "created_at";
		static constexpr char in_reply_to_status_id[] = "in_reply_to_status_id";
		static constexpr char retweet_count[] = "retweet_count";
		static constexpr char favorite_count[] = "favorite_count";
		static constexpr char user[] = "user";
		using type =
		  json_member_list<json_number<retweet_count, std::uint32_t>,
		                   json_class<user, daw::twitter::twitter_user>,
		                   json_string_raw<created_at, std::string_view>,
		                   json_number_null<in_reply_to_status_id, std::uint64_t>,
		                   json_number_null<favorite_count, std::uint32_t>,
		                   json_number<id, std::uint64_t>,
		                   json_string_raw<text, std::string_view>>;
	};
} // namespace daw::json
