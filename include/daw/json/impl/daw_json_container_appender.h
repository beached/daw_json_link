// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link.h
//

#pragma once

#include "version.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_move.h>

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename, typename, typename = void>
			inline constexpr bool has_push_back_v = false;

			template<typename Container, typename Value>
			inline constexpr bool has_push_back_v<
			  Container, Value,
			  std::void_t<decltype( std::declval<Container &>( ).push_back(
			    std::declval<Value>( ) ) )>> = true;

			template<typename, typename, typename = void>
			inline constexpr bool has_insert_end_v = false;

			template<typename Container, typename Value>
			inline constexpr bool has_insert_end_v<
			  Container, Value,
			  std::void_t<decltype( std::declval<Container &>( ).insert(
			    std::end( std::declval<Container &>( ) ),
			    std::declval<Value>( ) ) )>> = true;
		} // namespace json_details
		/***
		 * @brief A generic output iterator that can push_back or insert depending
		 * on what the type supports. This is like std::back_inserter
		 * @tparam Container Container object to append to
		 */
		template<typename Container>
		struct basic_appender {
			using value_type = typename Container::value_type;
			using reference = value_type;
			using pointer = value_type const *;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::output_iterator_tag;

		private:
			Container *m_container;

		public:
			explicit inline constexpr basic_appender( Container &container )
			  : m_container( &container ) {}

			template<typename Value>
			DAW_ATTRIB_FLATINLINE inline constexpr void operator( )( Value &&value ) {
				if constexpr( json_details::has_push_back_v<
				                Container, daw::remove_cvref_t<Value>> ) {
					m_container->push_back( DAW_FWD( value ) );
				} else if constexpr( json_details::has_insert_end_v<
				                       Container, daw::remove_cvref_t<Value>> ) {
					m_container->insert( std::end( *m_container ), DAW_FWD( value ) );
				} else {
					static_assert(
					  json_details::has_push_back_v<Container,
					                                daw::remove_cvref_t<Value>> or
					    json_details::has_insert_end_v<Container,
					                                   daw::remove_cvref_t<Value>>,
					  "basic_appender requires a Container that either has push_back "
					  "or "
					  "insert with the end iterator as first argument" );
				}
			}

			template<typename Value,
			         std::enable_if_t<
			           not std::is_same_v<basic_appender, daw::remove_cvref_t<Value>>,
			           std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr basic_appender &operator=( Value &&v ) {
				operator( )( DAW_FWD( v ) );
				return *this;
			}

			DAW_ATTRIB_INLINE constexpr basic_appender &operator++( ) {
				return *this;
			}

			DAW_ATTRIB_INLINE constexpr basic_appender operator++( int ) & {
				return *this;
			}

			DAW_ATTRIB_INLINE constexpr basic_appender &operator*( ) {
				return *this;
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
