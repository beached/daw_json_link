// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_enums.h"
#include "daw_json_parse_digit.h"
#include "daw_json_parse_iso8601_utils.h"
#include "daw_json_serialize_impl.h"
#include "version.h"

#include <daw/daw_string_view.h>
#include <daw/daw_utility.h>

#include <chrono>
#include <ciso646>
#include <optional>
#include <string_view>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		template<JsonNullable>
		struct construct_from_iso8601_timestamp;

		template<>
		struct construct_from_iso8601_timestamp<JsonNullable::Never> {
			using result_type = std::chrono::time_point<std::chrono::system_clock,
			                                            std::chrono::milliseconds>;

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( char const *ptr, std::size_t sz ) const {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};

		template<>
		struct construct_from_iso8601_timestamp<JsonNullable::Nullable> {
			using result_type =
			  std::optional<std::chrono::time_point<std::chrono::system_clock,
			                                        std::chrono::milliseconds>>;

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( ) const {
				return { };
			}

			[[maybe_unused, nodiscard]] inline constexpr result_type
			operator( )( char const *ptr, std::size_t sz ) const {
				return datetime::parse_iso8601_timestamp( daw::string_view( ptr, sz ) );
			}
		};

		template<typename T>
		struct custom_from_converter_t {
			[[nodiscard]] inline constexpr decltype( auto ) operator( )( ) {
				if constexpr( std::disjunction<
				                std::is_same<T, std::string_view>,
				                std::is_same<T, std::optional<std::string_view>>>::
				                value ) {
					return std::string_view{ };
				} else {
					// Use ADL customization point
					return from_string( daw::tag<T> );
				}
			}

			[[nodiscard]] inline constexpr decltype( auto )
			operator( )( std::string_view sv ) {
				if constexpr( std::disjunction<
				                std::is_same<T, std::string_view>,
				                std::is_same<T, std::optional<std::string_view>>>::
				                value ) {
					return sv;
				} else {
					// Use ADL customization point
					return from_string( daw::tag<T>, sv );
				}
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
