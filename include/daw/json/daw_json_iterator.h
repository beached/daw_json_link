// Copyright (c) Darrell Wright
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
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		inline namespace {
			template<typename CharT, typename ParseState>
			struct op_star_cleanup {
				CharT *&m_can_skip;
				ParseState &tmp;

				DAW_ATTRIB_INLINE
				DAW_SG_CXDTOR inline ~op_star_cleanup( ) noexcept( false ) {
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
					if( DAW_IS_CONSTANT_EVALUATED( ) ) {
						m_can_skip = tmp.first;
					} else {
#endif
						if( std::uncaught_exceptions( ) == 0 ) {
							m_can_skip = tmp.first;
						}
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
					}
#endif
				}
			};
		} // namespace
		/***
		 * Iterator for iterating over JSON array's
		 * @tparam JsonElement type under underlying element in array. If
		 * heterogeneous, a basic_json_value_iterator may be more appropriate
		 * @tparam ParsePolicy Parsing policy type
		 */
		template<typename JsonElement,
		         typename ParsePolicy = NoCommentSkippingPolicyChecked>
		class json_array_iterator {
			using CharT = typename ParsePolicy::CharT;

			template<typename String>
			static inline constexpr ParsePolicy
			get_range( String &&data, std::string_view member_path ) {
				static_assert(
				  std::is_convertible_v<decltype( std::data( data ) ), CharT *>,
				  "Attempt to assign a const char * to a char *" );

				auto [is_found, result] = json_details::find_range<ParsePolicy>(
				  DAW_FWD2( String, data ),
				  { std::data( member_path ), std::size( member_path ) } );
				daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
				daw_json_assert( result.front( ) == '[', ErrorReason::InvalidArrayStart,
				                 result );
				return result;
			}

		public:
			using element_type =
			  typename json_details::json_deduced_type<JsonElement>::without_name;
			static_assert( traits::not_same<element_type, void>::value,
			               "Unknown JsonElement type." );
			using value_type = typename element_type::parse_to_t;
			using reference = value_type;
			using pointer = json_details::arrow_proxy<value_type>;
			using difference_type = std::ptrdiff_t;
			// Can do forward iteration and be stored
			using iterator_category = std::input_iterator_tag;

		private:
			using ParseState = ParsePolicy;
			ParseState m_state = ParseState( );
			/***
			 * This lets us fastpath and just skip n characters as we have already
			 * parsed them
			 */
			mutable CharT *m_can_skip = nullptr;

		public:
			inline constexpr json_array_iterator( ) = default;

			template<
			  typename String,
			  std::enable_if_t<traits::not_same<json_array_iterator,
			                                    daw::remove_cvref_t<String>>::value,
			                   std::nullptr_t> = nullptr>
			inline constexpr explicit json_array_iterator( String &&jd )
			  : m_state( ParsePolicy( std::data( jd ), daw::data_end( jd ) ) ) {

				static_assert(
				  traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
				  "String requires being able to call std::data/std::size.  char const "
				  "* are not able to do this, pass a string_view for char const * to "
				  "ensure you are aware of the strlen cost" );

				static_assert(
				  std::is_convertible_v<decltype( std::data( jd ) ), CharT *> );
				m_state.trim_left( );
				daw_json_assert_weak( m_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, m_state );

				m_state.remove_prefix( );
				m_state.trim_left( );
			}

			template<
			  typename String,
			  std::enable_if_t<traits::not_same<json_array_iterator,
			                                    daw::remove_cvref_t<String>>::value,
			                   std::nullptr_t> = nullptr>
			inline constexpr explicit json_array_iterator(
			  String &&jd, std::string_view start_path )
			  : m_state( get_range( DAW_FWD2( String, jd ), start_path ) ) {

				static_assert(
				  traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
				  "String requires being able to call std::data/std::size.  char const "
				  "* are not able to do this, pass a string_view for char const * to "
				  "ensure you are aware of the strlen cost" );

				static_assert(
				  std::is_convertible_v<decltype( std::data( jd ) ), CharT *>,
				  "Attempt to assign a const char * to a char *" );

				m_state.trim_left( );
				daw_json_assert_weak( m_state.is_opening_bracket_checked( ),
				                      ErrorReason::InvalidArrayStart, m_state );

				m_state.remove_prefix( );
				m_state.trim_left( );
			}

			/***
			 * Parse the current element
			 * @pre good( ) returns true
			 * @return The parsed result of ParseElement
			 */
			[[nodiscard]] inline constexpr value_type operator*( ) const {
				daw_json_assert_weak( m_state.has_more( ) and m_state.front( ) != ']',
				                      ErrorReason::UnexpectedEndOfData, m_state );

				auto tmp = m_state;

				if constexpr( json_details::is_guaranteed_rvo_v<ParsePolicy> ) {
					auto const run_after_parse =
					  op_star_cleanup<CharT, ParseState>{ m_can_skip, tmp };
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

			/***
			 * A dereferenceable value proxy holding the result of operator*
			 * This is for compatibility with the Iterator concepts and should be
			 * avoided
			 * @pre good( ) returns true
			 * @return an arrow_proxy of the operator* result
			 */
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

			/***
			 * Move the parse state to the next element
			 * @return iterator prior to moving
			 */
			inline constexpr json_array_iterator operator++( int ) & {
				auto tmp = *this;
				(void)operator++( );
				return tmp;
			}

			/***
			 * Is it ok to dereference iterator
			 * @return true when there is parse data available
			 */
			[[nodiscard]] inline constexpr bool good( ) const {
				return not m_state.is_null( ) and m_state.has_more( ) and
				       m_state.front( ) != ']';
			}

			/***
			 * Are we good( )
			 * @return result of good( )
			 */
			[[nodiscard]] explicit inline constexpr operator bool( ) const {
				return good( );
			}

			/***
			 * Compare rhs for equivalence
			 * @param rhs Another json_array_iterator
			 * @return true when equivalent to rhs
			 */
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

			/***
			 * Check if the other iterator is not equivalent
			 * @param rhs another json_array_iterator
			 * @return true when rhs is not equivalent
			 */
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

		/***
		 * A range of json_array_iterators
		 * @tparam JsonElement Type of each element in array
		 * @tparam ParsePolicy parsing policy type
		 */
		template<typename JsonElement,
		         typename ParsePolicy = NoCommentSkippingPolicyChecked>
		struct json_array_range {
			using iterator = json_array_iterator<JsonElement, ParsePolicy>;
			using CharT = typename ParsePolicy::CharT;

		private:
			iterator m_first{ };
			iterator m_last{ };

		public:
			constexpr json_array_range( ) = default;

			template<
			  typename String,
			  std::enable_if_t<traits::not_same<json_array_range,
			                                    daw::remove_cvref_t<String>>::value,
			                   std::nullptr_t> = nullptr>
			constexpr explicit json_array_range( String &&jd )
			  : m_first( DAW_FWD2( String, jd ) ) {
				static_assert(
				  std::is_convertible_v<decltype( std::data( jd ) ), CharT *> );
			}

			template<
			  typename String,
			  std::enable_if_t<traits::not_same<json_array_range,
			                                    daw::remove_cvref_t<String>>::value,
			                   std::nullptr_t> = nullptr>
			constexpr explicit json_array_range( String &&jd,
			                                     std::string_view start_path )
			  : m_first( DAW_FWD2( String, jd ), start_path ) {
				static_assert(
				  std::is_convertible_v<decltype( std::data( jd ) ), CharT *>,
				  "Attempt to assign a const char * to a char *" );
			}

			/***
			 * @return first item in range
			 */
			[[nodiscard]] inline constexpr iterator begin( ) {
				return m_first;
			}

			/***
			 * @return one past last item in range
			 */
			[[nodiscard]] inline constexpr iterator end( ) {
				return m_last;
			}

			/***
			 * Are there any elements in range
			 * @return true when begin( ) == end( )
			 */
			[[nodiscard]] inline constexpr bool empty( ) const {
				return m_first == m_last;
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
