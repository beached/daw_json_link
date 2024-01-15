// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_req_helper.h"

#include <daw/daw_consteval.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_likely.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <string_view>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {

#if defined( DAW_JSON_CNTTP_JSON_NAME )
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
			DAW_ATTRIB_INLINE DAW_CONSTEVAL
			json_name( char const ( &ptr )[N], std::index_sequence<Is...> ) noexcept
			  : m_data{ ptr[Is]... } {}

		public:
			DAW_ATTRIB_INLINE DAW_CONSTEVAL
			json_name( char const ( &ptr )[N] ) noexcept
			  : json_name( ptr, std::make_index_sequence<N>{ } ) {}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr
			operator daw::string_view( ) const noexcept {
				return { m_data, N - 1 };
			}

			// Needed for copy_to_iterator
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr char const *
			begin( ) const noexcept {
				return m_data;
			}

			// Needed for copy_to_iterator
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr char const *
			end( ) const noexcept {
				return m_data + static_cast<ptrdiff_t>( size( ) );
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::size_t size( ) noexcept {
				return N - 1;
			}

			template<std::size_t M>
			[[nodiscard]] constexpr bool
			operator==( json_name<M> const &rhs ) const noexcept {
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

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
			operator==( daw::string_view rhs ) const noexcept {
				return daw::string_view( m_data, N - 1 ) == rhs;
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
			operator==( std::string_view rhs ) const noexcept {
				return std::string_view( m_data, N - 1 ) == rhs;
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr
			operator std::string_view( ) const noexcept {
				return std::string_view( m_data, N - 1 );
			}
		};
		template<typename... Chars>
		json_name( Chars... ) -> json_name<sizeof...( Chars )>;

		template<std::size_t N>
		json_name( char const ( & )[N] ) -> json_name<N>;

#define JSONNAMETYPE ::daw::json::json_name
		inline constexpr JSONNAMETYPE default_key_name{ "key" };
		inline constexpr JSONNAMETYPE default_value_name{ "value" };

#else
#define JSONNAMETYPE char const *
		inline constexpr char const default_key_name[] = "key";
		inline constexpr char const default_value_name[] = "value";
#endif
		namespace json_details {
			DAW_JSON_MAKE_REQ_TRAIT( has_name_v, T::name );

			template<typename... Ts>
			inline constexpr bool all_have_name_v = ( has_name_v<Ts> and ... );

			template<typename T>
			inline constexpr bool is_no_name_v = not has_name_v<T>;

			template<typename... Ts>
			inline constexpr bool are_no_name_v = ( is_no_name_v<Ts> and ... );
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
