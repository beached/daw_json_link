// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_parse_common.h"
#include "daw_json_value.h"

#include <ciso646>

namespace daw::json::json_details {
	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringRaw>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringEscaped>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>, Range & );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValueArray>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard]] static constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Variant>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	[[nodiscard, maybe_unused]] constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>, Range &rng );

	template<typename JsonMember, bool KnownBounds = false, typename Range>
	constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unknown>, Range &rng );

#if( defined( _MSC_VER ) and not defined( __clang__ ) )
	// Lying to MSVC about being a random iterator causes issues I have not found
	// yet
	template<bool>
	inline constexpr bool can_random_v = false;
#else
	template<bool IsKnown>
	inline constexpr bool can_random_v = IsKnown;
#endif

	template<typename WrappedItem, bool>
	struct wrapped_item_ctor_t_impl {
		constexpr WrappedItem operator( )( ) const {
			return daw::construct_a<WrappedItem>( );
		}

		template<typename... Args, std::enable_if_t<( sizeof...( Args ) > 0 ),
		                                            std::nullptr_t> = nullptr>
		constexpr auto operator( )( Args &&... args ) const {
			using wrapped_type =
			  daw::remove_cvref_t<decltype( *std::declval<WrappedItem>( ) )>;
			static_assert( std::is_constructible_v<WrappedItem, wrapped_type> );
			static_assert( std::is_constructible_v<wrapped_type, Args...> );
			return daw::construct_a<WrappedItem>(
			  daw::construct_a<wrapped_type>( std::forward<Args>( args )... ) );
		}
	};
} // namespace daw::json::json_details
