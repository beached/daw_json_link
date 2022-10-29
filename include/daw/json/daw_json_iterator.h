﻿// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "impl/daw_json_link_types_fwd.h"
#include "impl/daw_json_parse_class.h"

#include <daw/daw_cxmath.h>
#include <daw/daw_move.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_array_iterator_details {
			template<typename CharT, ParseState ParseState>
			struct op_star_cleanup {
				CharT *&m_can_skip;
				ParseState &tmp;

				DAW_ATTRIB_INLINE
				DAW_SG_CXDTOR ~op_star_cleanup( ) noexcept(
				  not use_daw_json_exceptions_v ) {
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
					if( DAW_IS_CONSTANT_EVALUATED( ) ) {
						m_can_skip = tmp.first;
					} else {
#endif
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
						if( std::uncaught_exceptions( ) == 0 ) {
#endif
							m_can_skip = tmp.first;
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
						}
#endif
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
					}
#endif
				}
			};
		} // namespace json_array_iterator_details
		/***
		 * Iterator for iterating over JSON array's
		 * @tparam JsonElement type under underlying element in array. If
		 * heterogeneous, a basic_json_value_iterator may be more appropriate
		 * @tparam ParsePolicy Parsing policy type
		 */
		template<typename JsonElement, auto... PolicyFlags>
		class json_array_iterator {
			using ParseState = BasicParsePolicy<
			  options::details::make_parse_flags<PolicyFlags...>( ).value>;
			using CharT = typename ParseState::CharT;

			static inline constexpr ParseState
			get_range( daw::string_view data, daw::string_view member_path ) {
				auto [is_found, result] = json_details::find_range<ParseState>(
				  DAW_FWD( data ),
				  { std::data( member_path ), std::size( member_path ) } );
				daw_json_ensure( is_found, ErrorReason::JSONPathNotFound );
				daw_json_ensure( result.front( ) == '[', ErrorReason::InvalidArrayStart,
				                 result );
				return result;
			}

		public:
			using element_type = json_details::json_deduced_type<JsonElement>;
			static_assert( not std::is_same_v<element_type, void>,
			               "Unknown JsonElement type." );
			using value_type = typename element_type::parse_to_t;
			using reference = value_type;
			using pointer = json_details::arrow_proxy<value_type>;
			using difference_type = std::ptrdiff_t;
			// Can do forward iteration and be stored
			using iterator_category = std::input_iterator_tag;

		private:
			ParseState m_state = ParseState( );
			/***
			 * This lets us fastpath and just skip n characters as we have already
			 * parsed them
			 */
			mutable CharT *m_can_skip = nullptr;

		public:
			json_array_iterator( ) = default;

			inline constexpr explicit json_array_iterator( daw::string_view jd )
			  : m_state( ParseState( std::data( jd ), daw::data_end( jd ) ) ) {

				m_state.trim_left( );
				daw_json_assert_weak( m_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, m_state );

				m_state.remove_prefix( );
				m_state.trim_left( );
			}

			inline constexpr explicit json_array_iterator(
			  daw::string_view jd, daw::string_view start_path )
			  : m_state( get_range( jd, start_path ) ) {

				m_state.trim_left( );
				daw_json_assert_weak( m_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, m_state );

				m_state.remove_prefix( );
				m_state.trim_left( );
			}

			/// @brief Parse the current element
			/// @pre good( ) returns true
			/// @return The parsed result of ParseElement
			[[nodiscard]] inline constexpr value_type operator*( ) const {
				daw_json_assert_weak( m_state.has_more( ) and m_state.front( ) != ']',
				                      ErrorReason::UnexpectedEndOfData, m_state );

				auto tmp = m_state;

				if constexpr( json_details::use_direct_construction_v<ParseState,
				                                                      JsonElement> ) {
					auto const run_after_parse =
					  json_array_iterator_details::op_star_cleanup<CharT, ParseState>{
					    m_can_skip, tmp };
					(void)run_after_parse;
					return json_details::parse_value<element_type>(
					  tmp, ParseTag<element_type::expected_type>{ } );
				} else {
					auto result = json_details::parse_value<element_type>(
					  tmp, ParseTag<element_type::expected_type>{ } );

					m_can_skip = tmp.first;
					return result;
				}
			}

			/// @brief A dereferencable value proxy holding the result of operator*
			/// This is for compatibility with the Iterator concepts and should be
			/// avoided
			/// @pre good( ) returns true
			/// @return an arrow_proxy of the operator* result
			[[nodiscard]] inline pointer operator->( ) const {
				return pointer{ operator*( ) };
			}

			/***
			 * Move the parse state to the next element
			 * @return iterator after moving
			 */
			inline constexpr json_array_iterator &operator++( ) {
				daw_json_assert_weak( m_state.has_more( ) and m_state.front( ) != ']',
				                      ErrorReason::UnexpectedEndOfData, m_state );
				if( m_can_skip ) {
					m_state.first = m_can_skip;
					m_can_skip = nullptr;
				} else {
					(void)json_details::skip_known_value<element_type>( m_state );
				}
				m_state.move_next_member_or_end( );
				return *this;
			}

			///
			/// @brief Move the parse state to the next element
			///
			inline constexpr void operator++( int ) & {
				(void)operator++( );
			}

			/***
			 * Is it ok to dereference iterator
			 * @return true when there is parse data available
			 */
			[[nodiscard]] inline constexpr bool good( ) const {
				return not m_state.is_null( ) and m_state.has_more( ) and
				       m_state.front( ) != ']';
			}

			/// @brief Are we good( )
			/// @return result of good( )
			[[nodiscard]] explicit inline constexpr operator bool( ) const {
				return good( );
			}

			/// @brief Compare rhs for equivalence
			/// @param rhs Another json_array_iterator
			/// @return true when equivalent to rhs
			[[nodiscard]] inline constexpr bool
			operator==( json_array_iterator const &rhs ) const {
				if( not( *this ) ) {
					return not rhs;
				}
				if( not rhs ) {
					return false;
				}
				return ( m_state.first == rhs.m_state.first );
			}

			/// @brief Check if the other iterator is not equivalent
			/// @param rhs another json_array_iterator
			/// @return true when rhs is not equivalent
			[[nodiscard]] inline constexpr bool
			operator!=( json_array_iterator const &rhs ) const {
				if( not( *this ) ) {
					return static_cast<bool>( rhs );
				}
				if( not rhs ) {
					return true;
				}
				return m_state.first != rhs.m_state.first;
			}
		};

		/// @brief A range of json_array_iterators
		/// @tparam JsonElement Type of each element in array
		/// @tparam ParsePolicy parsing policy type
		template<typename JsonElement, auto... PolicyFlags>
		struct json_array_range {
			using ParsePolicy = BasicParsePolicy<
			  options::details::make_parse_flags<PolicyFlags...>( ).value>;
			using iterator = json_array_iterator<JsonElement, PolicyFlags...>;
			using CharT = typename ParsePolicy::CharT;

		private:
			iterator m_first{ };
			iterator m_last{ };

		public:
			json_array_range( ) = default;

			constexpr explicit json_array_range( daw::string_view jd )
			  : m_first( jd ) {}

			constexpr explicit json_array_range( daw::string_view jd,
			                                     daw::string_view start_path )
			  : m_first( jd, start_path ) {}

			/// @return first item in range
			[[nodiscard]] inline constexpr iterator begin( ) const {
				return m_first;
			}

			/// @return one past last item in range
			[[nodiscard]] inline constexpr iterator end( ) const {
				return m_last;
			}

			/// @brief Are there any elements in range
			/// @return true when begin( ) == end( )
			[[nodiscard]] inline constexpr bool empty( ) const {
				return m_first == m_last;
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
