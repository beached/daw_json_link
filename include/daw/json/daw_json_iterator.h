// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link.h"
#include "impl/daw_json_link_impl.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_move.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iterator/daw_back_inserter.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>

namespace daw::json {

	/***
	 * Iterator for iterating over JSON array's
	 * @tparam JsonElement type under underlying element in array. If
	 * heterogenous, a basic_json_value_iterator may be more appropriate
	 * @tparam ParsePolicy Parsing policy type
	 */
	template<typename JsonElement,
	         typename ParsePolicy = NoCommentSkippingPolicyChecked>
	class json_array_iterator {

		template<typename String>
		static inline constexpr ParsePolicy
		get_range( String &&data, std::string_view member_path ) {
			auto [is_found, result] = json_details::find_range<ParsePolicy>(
			  DAW_FWD( data ),
			  { std::data( member_path ), std::size( member_path ) } );
			daw_json_assert( is_found, ErrorReason::JSONPathNotFound );
			daw_json_assert( result.front( ) == '[', ErrorReason::InvalidArrayStart,
			                 result );
			return result;
		}

	public:
		using element_type =
		  json_details::unnamed_default_type_mapping<JsonElement>;
		static_assert( not std::is_same_v<element_type, void>,
		               "Unknown JsonElement type." );
		using value_type = typename element_type::parse_to_t;
		using reference = value_type;
		using pointer = json_details::arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		// Can do forward iteration and be stored
		using iterator_category = std::input_iterator_tag;

	private:
		using Range = ParsePolicy;
		Range m_state = Range( );
		/***
		 * This lets us fastpath and just skip n characters as we have already
		 * parsed them
		 */
		mutable char const *m_can_skip = nullptr;

	public:
		inline constexpr json_array_iterator( ) = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_iterator, daw::remove_cvref_t<String>>> = nullptr>
		inline constexpr explicit json_array_iterator( String &&jd )
		  : m_state( ParsePolicy( std::data( jd ), daw::data_end( jd ) ) ) {

			static_assert(
			  daw::traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
			  "StringRaw must be like a string_view" );
			m_state.trim_left( );
			daw_json_assert_weak( m_state.is_opening_bracket_checked( ),
			                      ErrorReason::InvalidArrayStart, m_state );

			m_state.remove_prefix( );
			m_state.trim_left( );
		}

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_iterator, daw::remove_cvref_t<String>>> = nullptr>
		inline constexpr explicit json_array_iterator( String &&jd,
		                                               std::string_view start_path )
		  : m_state( get_range( DAW_FWD( jd ), start_path ) ) {

			static_assert(
			  daw::traits::is_string_view_like_v<daw::remove_cvref_t<String>>,
			  "StringRaw must be like a string_view" );
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
				auto const ae = daw::on_exit_success( [&] { m_can_skip = tmp.first; } );
				return json_details::parse_value<element_type>(
				  ParseTag<element_type::expected_type>{ }, tmp );
			} else {
				auto result = json_details::parse_value<element_type>(
				  ParseTag<element_type::expected_type>{ }, tmp );

				m_can_skip = tmp.first;
				return result;
			}
		}

		/***
		 * A dereferenable value proxy holding the result of operator*
		 * This is for compatability with the Iterator concepts and should be
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
			m_state.clean_tail( );
			return *this;
		}

		/***
		 * Move the parse state to the next element
		 * @return iterator prior to moving
		 */
		inline constexpr json_array_iterator operator++( int ) {
			auto tmp = *this;
			(void)operator++( );
			return tmp;
		}

		/***
		 * Is it ok to dereference itereator
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
		 * Compare rhs for equivilence
		 * @param rhs Another json_array_iterator
		 * @return true when equivilent to rhs
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
		 * Check if the other iterator is not equivilent
		 * @param rhs another json_array_iterator
		 * @return true when rhs is not equivilent
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

	private:
		iterator m_first{ };
		iterator m_last{ };

	public:
		constexpr json_array_range( ) = default;

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_range, daw::remove_cvref_t<String>>> = nullptr>
		constexpr explicit json_array_range( String &&jd )
		  : m_first( DAW_FWD( jd ) ) {}

		template<typename String,
		         daw::enable_when_t<not std::is_same_v<
		           json_array_range, daw::remove_cvref_t<String>>> = nullptr>
		constexpr explicit json_array_range( String &&jd,
		                                     std::string_view start_path )
		  : m_first( DAW_FWD( jd ), start_path ) {}

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
} // namespace daw::json
