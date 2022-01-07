// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/daw_consteval.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_likely.h>
#include <daw/daw_not_null.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <string_view>
#include <utility>

namespace daw::json {
	DAW_JSON_INLINE_NS namespace DAW_JSON_VER {
		// If the compiler supports CNTTP types allow for strings in json data
		// contracts.  Both support passing local char const[], but the type is
		// different.  To keep old behaviour when using C++20, define
		// DAW_USE_CPP17_ABI
#if not defined( DAW_USE_CPP17_ABI )
#if defined( __cpp_nontype_template_parameter_class )
#define DAW_JSON_USE_CNTTP_NAMES
#elif defined( __cpp_nontype_template_args )
#if __cpp_nontype_template_args >= 201911L
#define DAW_JSON_USE_CNTTP_NAMES
#elif defined( __apple_build_version__ ) and __cplusplus >= 202002
#if __apple_build_version__ >= 13000000
#define DAW_JSON_USE_CNTTP_NAMES
#endif
#elif defined( __clang_major__ ) and __cplusplus >= 202002
#if __clang_major__ >= 12
#define DAW_JSON_USE_CNTTP_NAMES
#endif
#endif
#endif
#endif
#if defined( DAW_JSON_USE_CNTTP_NAMES )
		// C++ 20 Non-Type Class Template Arguments

		/**
		 * A fixed string used for member names in json descriptions
		 * @tparam N size of string plus 1.  Do not set explicitly.  Use CTAD
		 */
		template<std::size_t N>
		struct json_name {
			static_assert( N > 0 );
			char const m_data[N]{ };

		private:
			template<std::size_t... Is>
			DAW_CONSTEVAL json_name( char const ( &ptr )[N], std::index_sequence<Is...> )
			  : m_data{ ptr[Is]... } {}

		public:
			DAW_CONSTEVAL json_name( char const ( &ptr )[N] )
			  : json_name( ptr, std::make_index_sequence<N>{ } ) {}

			constexpr operator daw::string_view( ) const {
				return { m_data, N - 1 };
			}

			// Needed for copy_to_iterator
			[[nodiscard]] constexpr daw::not_null<char const *> begin( ) const {
				return { daw::never_null, m_data };
			}

			// Needed for copy_to_iterator
			[[nodiscard]] constexpr daw::not_null<char const *> end( ) const {
				return { daw::never_null, m_data + static_cast<ptrdiff_t>( size( ) ) };
			}

			[[nodiscard]] static constexpr std::size_t size( ) {
				return N - 1;
			}

			template<std::size_t M>
			constexpr bool operator==( json_name<M> const &rhs ) const {
				if( N != M ) {
					return false;
				}
				for( std::size_t n = 0; n < N; ++n ) {
					if( m_data[n] != rhs.m_data[n] ) {
						return false;
					}
				}
				return true;
			}

			constexpr bool operator==( daw::string_view rhs ) const {
				return daw::string_view( m_data, N - 1 ) == rhs;
			}

			constexpr bool operator==( std::string_view rhs ) const {
				return std::string_view( m_data, N - 1 ) == rhs;
			}

			constexpr operator std::string_view( ) const {
				return std::string_view( m_data, N - 1 );
			}
		};
		template<typename... Chars>
		json_name( Chars... ) -> json_name<sizeof...( Chars )>;

		template<std::size_t N>
		json_name( char const( & )[N] ) -> json_name<N>;

#define JSONNAMETYPE ::daw::json::json_name
		namespace json_details {
			inline constexpr JSONNAMETYPE default_key_name{ "key" };
			inline constexpr JSONNAMETYPE default_value_name{ "value" };
		} // namespace json_details

#else
#define JSONNAMETYPE char const *
		namespace json_details {
			inline constexpr char const default_key_name[] = "key";
			inline constexpr char const default_value_name[] = "value";
		} // namespace json_details
#endif
	} // namespace DAW_JSON_VER
} // namespace daw::json
