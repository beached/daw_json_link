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
#include <daw/daw_string_view.h>

#include <cstddef>
#include <string_view>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER DAW_ATTRIB_HIDDEN {
		// If the compiler supports CNTTP types allow for strings in json data
		// contracts.  Both support passing local char const[], but the type is
		// different.  To keep old behaviour when using C++20, define
		// DAW_USE_CPP17_ABI
#if defined( __cpp_nontype_template_parameter_class ) and \
  not defined( DAW_USE_CPP17_ABI )
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
			DAW_CONSTEVAL json_name( char const ( &ptr )[N],
			                         std::index_sequence<Is...> )
			  : m_data{ ptr[Is]... } {}

		public:
			DAW_CONSTEVAL json_name( char const ( &ptr )[N] )
			  : json_name( ptr, std::make_index_sequence<N>{ } ) {}

			constexpr operator daw::string_view( ) const {
				return { m_data, N - 1 };
			}

			// Needed for copy_to_iterator
			[[nodiscard]] constexpr char const *begin( ) const {
				return m_data;
			}

			// Needed for copy_to_iterator
			[[nodiscard]] constexpr char const *end( ) const {
				return m_data + static_cast<ptrdiff_t>( size( ) );
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
		json_name( char const ( & )[N] ) -> json_name<N>;

#define JSONNAMETYPE ::daw::json::json_name
		// Convenience for array members that are required to be unnamed
		inline constexpr JSONNAMETYPE no_name{ "\a" };
		inline constexpr daw::string_view no_name_sv = daw::string_view( no_name );

		namespace json_details {
			inline constexpr JSONNAMETYPE default_key_name{ "key" };
			inline constexpr JSONNAMETYPE default_value_name{ "value" };
		} // namespace json_details

#else
#define JSONNAMETYPE char const *
		// Convenience for array members that are required to be unnamed
		inline constexpr char const no_name[] = "\a";
		inline constexpr daw::string_view no_name_sv = daw::string_view( no_name );
		namespace json_details {

			inline constexpr char const default_key_name[] = "key";
			inline constexpr char const default_value_name[] = "value";

		} // namespace json_details
#endif
		namespace json_details {
			template<typename JsonMember>
			using is_no_name = std::bool_constant<( JsonMember::name == no_name_sv )>;

			template<typename JsonMember>
			inline constexpr bool is_no_name_v = is_no_name<JsonMember>::value;
		} // namespace json_details
	}   // namespace DAW_ATTRIB_HIDDEN
} // namespace daw::json
