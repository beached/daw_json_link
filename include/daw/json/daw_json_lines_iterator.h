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
		namespace json_lines_details {
			template<typename CharT, typename ParseState>
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
		} // namespace json_lines_details

		/// @brief Iterator for iterating over JSON array's
		/// @tparam JsonElement type under underlying element in array. If
		/// heterogeneous, a basic_json_value_iterator may be more appropriate
		template<typename JsonElement = json_value, auto... PolicyFlags>
		class json_lines_iterator {
			using ParsePolicy = BasicParsePolicy<
			  options::details::make_parse_flags<PolicyFlags...>( ).value>;
			using CharT = typename ParsePolicy::CharT;

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
			using ParseState = ParsePolicy;
			ParseState m_state = ParseState( );
			/***
			 * This lets us fastpath and just skip n characters as we have already
			 * parsed them
			 */
			mutable CharT *m_can_skip = nullptr;

		public:
			json_lines_iterator( ) = default;

			inline constexpr explicit json_lines_iterator(
			  daw::string_view json_lines_doc )
			  : m_state( ParsePolicy( std::data( json_lines_doc ),
			                          daw::data_end( json_lines_doc ) ) ) {

				m_state.trim_left( );
			}

			/// @brief Parse the current element
			/// @pre good( ) returns true
			/// @return The parsed result of ParseElement
			[[nodiscard]] inline constexpr value_type operator*( ) const {
				daw_json_assert_weak( m_state.has_more( ),
				                      ErrorReason::UnexpectedEndOfData, m_state );

				auto tmp = m_state;

				if constexpr( json_details::use_direct_construction_v<ParsePolicy,
				                                                      JsonElement> ) {
					auto const run_after_parse =
					  json_lines_details::op_star_cleanup<CharT, ParseState>{ m_can_skip,
					                                                          tmp };
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

			/// @brief A dereferencable value proxy holding the result of operator* .
			/// This is for compatibility with the Iterator concepts and should be
			/// avoided
			/// @pre good( ) returns true
			/// @return an arrow_proxy of the operator* result
			[[nodiscard]] inline pointer operator->( ) const {
				return pointer{ operator*( ) };
			}

			/// @brief Move the parse state to the next element
			/// @return iterator after moving
			inline constexpr json_lines_iterator &operator++( ) {
				daw_json_assert_weak( m_state.has_more( ),
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

			/// @brief Move the parse state to the next element
			inline constexpr void operator++( int ) & {
				(void)operator++( );
			}

			/// @brief Is it ok to dereference iterator
			/// @return true when there is parse data available
			[[nodiscard]] inline constexpr bool good( ) const {
				return not m_state.is_null( ) and m_state.has_more( );
			}

			/// @brief Are we good( )
			/// @return result of good( )
			[[nodiscard]] explicit inline constexpr operator bool( ) const {
				return good( );
			}

			/// @brief Compare rhs for equivalence
			/// @param rhs Another json_lines_iterator
			/// @return true when equivalent to rhs
			[[nodiscard]] inline constexpr bool
			operator==( json_lines_iterator const &rhs ) const {
				if( not( *this ) ) {
					return not rhs;
				}
				if( not rhs ) {
					return false;
				}
				return ( m_state.first == rhs.m_state.first );
			}

			/// @brief Check if the other iterator is not equivalent
			/// @param rhs another json_lines_iterator
			/// @return true when rhs is not equivalent
			[[nodiscard]] inline constexpr bool
			operator!=( json_lines_iterator const &rhs ) const {
				if( not( *this ) ) {
					return static_cast<bool>( rhs );
				}
				if( not rhs ) {
					return true;
				}
				return m_state.first != rhs.m_state.first;
			}
		};

		/// @brief A range of json_lines_iterators
		/// @tparam JsonElement Type of each element in array
		/// @tparam ParsePolicy parsing policy type
		template<typename JsonElement = json_value, auto... PolicyFlags>
		struct json_lines_range {
			using ParsePolicy = BasicParsePolicy<
			  options::details::make_parse_flags<PolicyFlags...>( ).value>;
			using iterator = json_lines_iterator<JsonElement, PolicyFlags...>;
			using CharT = typename ParsePolicy::CharT;

		private:
			iterator m_first{ };
			iterator m_last{ };

		public:
			json_lines_range( ) = default;

			constexpr explicit json_lines_range( daw::string_view json_lines_doc )
			  : m_first( json_lines_doc ) {}

			/// @return first item in range
			[[nodiscard]] inline constexpr iterator begin( ) {
				return m_first;
			}

			/// @return one past last item in range
			[[nodiscard]] inline constexpr iterator end( ) {
				return m_last;
			}

			/// @brief Are there any elements in range
			/// @return true when begin( ) == end( )
			[[nodiscard]] inline constexpr bool empty( ) const {
				return m_first == m_last;
			}
		};

		/// @brief parition the jsonl/nbjson document into num_partition non
		/// overlapping sub-ranges. This can be used to parallelize json lines
		/// parsing
		template<typename JsonElement = json_value, auto... ParsePolicies>
		std::vector<json_lines_range<JsonElement, ParsePolicies...>>
		partition_jsonl_document( std::size_t num_partitions,
		                          daw::string_view jsonl_doc ) {
			using result_t =
			  std::vector<json_lines_range<JsonElement, ParsePolicies...>>;
			if( num_partitions <= 1 ) {
				return result_t{
				  json_lines_range<JsonElement, ParsePolicies...>( jsonl_doc ) };
			}
			auto approx_segsize = jsonl_doc.size( ) / num_partitions;
			auto result = result_t{ };
			char const *const last = daw::data_end( jsonl_doc );
			while( not jsonl_doc.empty( ) ) {
				char const *tmp = std::data( jsonl_doc ) + approx_segsize;
				if( tmp >= last ) {
					result.emplace_back( jsonl_doc );
					break;
				}
				while( tmp < last and * tmp != '\n' ) {
					++tmp;
				}
				if( tmp < last ) {
					++tmp;
				}
				auto sz = static_cast<std::size_t>( tmp - std::data( jsonl_doc ) );
				auto doc = jsonl_doc.pop_front( sz );
				doc.trim_suffix( );
				if( not doc.empty( ) ) {
					result.emplace_back( doc );
				}
			}
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
