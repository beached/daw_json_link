// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_json_arrow_proxy.h"
#include "impl/daw_json_parse_class.h"
#include "impl/daw_json_parse_name.h"
#include "impl/daw_json_value.h"
#include "impl/daw_murmur3.h"
#include "impl/version.h"

#include <daw/daw_move.h>
#include <daw/daw_uint_buffer.h>

#include <ciso646>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename ParseState>
			struct basic_stateful_json_value_state {
				daw::string_view name;
				daw::UInt32 hash_value;
				basic_json_value_iterator<ParseState> location;

				explicit constexpr basic_stateful_json_value_state(
				  daw::string_view Name, basic_json_value_iterator<ParseState> val )
				  : name( Name )
				  , hash_value( daw::name_hash( Name ) )
				  , location( DAW_MOVE( val ) ) {}

				[[nodiscard]] constexpr bool is_match( daw::string_view Name ) const {
					return name == Name;
				}

				[[nodiscard]] constexpr bool is_match( daw::string_view Name,
				                                       daw::UInt32 hash ) const {
					if( hash != hash_value ) {
						return false;
					}
					return name == Name;
				}
			};
		} // namespace json_details

		struct json_member_name {
			daw::string_view name;
			daw::UInt32 hash_value;

			constexpr json_member_name( std::string_view Name )
			  : name( std::data( Name ), std::size( Name ) )
			  , hash_value( daw::name_hash( name ) ) {}
		};

		/**
		 * Maintains the parse positions of a json_value so that you pay the lookup
		 * costs once
		 * @tparam ParseState see IteratorRange
		 */
		template<typename ParseState>
		class basic_stateful_json_value {
			basic_json_value<ParseState> m_value;
			std::vector<json_details::basic_stateful_json_value_state<ParseState>>
			  m_locs{ };

			/***
			 * Move parser until member name matches key if needed
			 * @param member to move_to
			 * @return position of member or size
			 */
			[[nodiscard]] constexpr std::size_t move_to( json_member_name member ) {
				std::size_t pos = 0;
				std::size_t const Sz = std::size( m_locs );
				for( ; pos < Sz; ++pos ) {
					if( m_locs[pos].is_match( member.name, member.hash_value ) ) {
						return pos;
					}
				}

				auto it = [&] {
					if( m_locs.empty( ) ) {
						return m_value.begin( );
					}
					auto res = m_locs.back( ).location;
					++res;
					return res;
				}( );
				auto const last = m_value.end( );
				while( it != last ) {
					auto name = it.name( );
					daw_json_assert_weak( name, ErrorReason::MissingMemberName );
					auto const &new_loc = m_locs.emplace_back(
					  daw::string_view( std::data( *name ), std::size( *name ) ), it );
					if( new_loc.is_match( member.name ) ) {
						return pos;
					}
					++pos;
					++it;
				}
				return std::size( m_locs );
			}

			/***
			 * Move parser until member index matches, if needed
			 * @param index position of member to move to
			 * @return position in members or size
			 */
			[[nodiscard]] constexpr std::size_t move_to( std::size_t index ) {
				if( index < std::size( m_locs ) ) {
					return index;
				}
				auto it = [&] {
					if( m_locs.empty( ) ) {
						return m_value.begin( );
					}
					auto res = m_locs.back( ).location;
					++res;
					return res;
				}( );
				auto last = m_value.end( );
				std::size_t pos = std::size( m_locs );
				while( it != last ) {
					auto name = it.name( );
					if( name ) {
						m_locs.emplace_back(
						  daw::string_view( std::data( *name ), std::size( *name ) ), it );
					} else {
						m_locs.emplace_back( daw::string_view( ), it );
					}
					if( pos == index ) {
						return pos;
					}
					++pos;
					++it;
				}
				return std::size( m_locs );
			}

		public:
			constexpr basic_stateful_json_value( basic_json_value<ParseState> val )
			  : m_value( DAW_MOVE( val ) ) {

				daw_json_assert_weak( ( [&] {
					                      auto t = m_value.type( );
					                      return ( t == JsonBaseParseTypes::Class ) |
					                             ( t == JsonBaseParseTypes::Array );
				                      }( ) ),
				                      ErrorReason::ExpectedArrayOrClassStart,
				                      val.get_raw_state( ) );
			}

			constexpr basic_stateful_json_value( )
			  : basic_stateful_json_value( basic_json_value<ParseState>( "{}" ) ) {}

			constexpr basic_stateful_json_value( std::string_view json_data )
			  : basic_stateful_json_value(
			      basic_json_value<ParseState>( json_data ) ) {}
			/**
			 * Reuse state storage for another basic_json_value
			 * @param val Value to contain state for
			 */
			constexpr void reset( basic_json_value<ParseState> val ) {
				m_value = DAW_MOVE( val );
				m_locs.clear( );
			}

			/***
			 * Create a basic_json_member for the named member
			 * @pre name must be valid
			 * @param key name of member
			 * @return a new basic_json_member
			 */
			[[nodiscard]] constexpr basic_json_value<ParseState>
			operator[]( std::string_view key ) {
				std::size_t pos = move_to( json_member_name( key ) );
				daw_json_assert_weak( pos < std::size( m_locs ),
				                      ErrorReason::UnknownMember );
				return m_locs[pos].location->value;
			}

			/***
			 * Create a basic_json_member for the named member
			 * @pre name must be valid
			 * @param member name of member
			 * @return a new basic_json_member
			 */
			[[nodiscard]] constexpr basic_json_value<ParseState>
			operator[]( json_member_name member ) {
				std::size_t pos = move_to( member );
				daw_json_assert_weak( pos < std::size( m_locs ),
				                      ErrorReason::UnknownMember );
				return m_locs[pos].location->value;
			}

			/***
			 * Create a basic_json_member for the named member
			 * @pre name must be valid
			 * @param key name of member
			 * @return a new basic_json_member for the JSON data or an empty one if
			 * the member does not exist
			 */
			[[nodiscard]] constexpr basic_json_value<ParseState>
			at( std::string_view key ) {
				auto const k = std::string_view( std::data( key ), std::size( key ) );
				std::size_t pos = move_to( k );
				if( pos < std::size( m_locs ) ) {
					return m_locs[pos].location->value;
				}
				return { };
			}

			/***
			 * Count the number of elements/members in the JSON class or array
			 * This method is O(N) worst case and O(1) if the locations have already
			 * been determined
			 * @return number of members/elements
			 */
			[[nodiscard]] std::size_t size( ) {
				JsonBaseParseTypes const current_type = m_value.type( );
				switch( current_type ) {
				case JsonBaseParseTypes::Array:
				case JsonBaseParseTypes::Class:
					return move_to( daw::numeric_limits<std::size_t>::max( ) );
				default:
					return 0;
				}
			}

			/***
			 * Return the index of named member
			 * This method is O(N) worst case and O(1) if the locations have already
			 * @param key name of member
			 * @return the position of the member or the count of members if not
			 * present
			 */
			[[nodiscard]] std::size_t index_of( std::string_view key ) {
				auto const k = std::string_view( std::data( key ), std::size( key ) );
				return move_to( k );
			}

			/***
			 * Is the named member present
			 * This method is O(N) worst case and O(1) if the locations have already
			 * @param key name of member
			 * @return true if the member is present
			 */
			[[nodiscard]] constexpr bool contains( std::string_view key ) {
				auto const k =
				  std::string_view( std::data( key ), std::size( key ) );
				return move_to( k ) < std::size( m_locs );
			}

			/***
			 * Is the indexed member/element present
			 * This method is O(N) worst case and O(1) if the locations have already
			 * @param index position of member/element
			 * @return true if the member/element is present
			 */
			[[nodiscard]] constexpr bool contains( std::size_t index ) {
				return move_to( index ) < size( m_locs );
			}

			/***
			 * Get the position of the named member
			 * @tparam Integer An integer type
			 * @param index position of member.  If negative it returns the position
			 * from one past last, e.g. -1 is last item
			 * @return The name of the member or an empty optional
			 */
			template<typename Integer,
			         std::enable_if_t<std::is_integral<Integer>::value,
			                          std::nullptr_t> = nullptr>
			[[nodiscard]] std::optional<std::string_view> name_of( Integer index ) {
				if constexpr( std::is_signed<Integer>::value ) {
					if( index < 0 ) {
						index = -index;
						auto sz = size( );
						if( static_cast<std::size_t>( index ) >= sz ) {
							return { };
						}
						sz -= static_cast<std::size_t>( index );
						return std::string_view( std::data( m_locs[sz].name( ) ),
						                         std::size( m_locs[sz].name( ) ) );
					}
				}
				std::size_t pos = move_to( static_cast<std::size_t>( index ) );
				if( pos < std::size( m_locs ) ) {
					return std::string_view( std::data( m_locs[pos].name( ) ),
					                         std::size( m_locs[pos].name( ) ) );
				}
				return { };
			}

			/***
			 * basic_json_value for the indexed member
			 * @tparam Integer An integer type
			 * @param index position of member.  If negative it returns the position
			 * from one past last, e.g. -1 is last item
			 * @pre index must exist
			 * @return A new basic_json_value for the indexed member
			 */
			template<typename Integer,
			         std::enable_if_t<std::is_integral<Integer>::value,
			                          std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr basic_json_value<ParseState>
			operator[]( Integer index ) {
				if constexpr( std::is_signed<Integer>::value ) {
					if( index < 0 ) {
						index = -index;
						auto sz = size( );
						daw_json_assert_weak( ( static_cast<std::size_t>( index ) < sz ),
						                      ErrorReason::UnknownMember );
						sz -= static_cast<std::size_t>( index );
						return m_locs[sz].location->value;
					}
				}
				std::size_t pos = move_to( static_cast<std::size_t>( index ) );
				daw_json_assert_weak( pos < std::size( m_locs ),
				                      ErrorReason::UnknownMember );
				return m_locs[pos].location->value;
			}

			/***
			 * basic_json_value for the indexed member
			 * @tparam Integer An integer type
			 * @param index position of member.  If negative it returns the position
			 * from one past last, e.g. -1 is last item
			 * @return A new basic_json_value for the indexed member
			 */
			template<typename Integer,
			         std::enable_if_t<std::is_integral<Integer>::value,
			                          std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr std::optional<basic_json_value<ParseState>>
			at( Integer index ) {
				if constexpr( std::is_signed<Integer>::value ) {
					if( index < 0 ) {
						index = -index;
						auto sz = size( );
						if( static_cast<std::size_t>( index ) >= sz ) {
							return { };
						}
						sz -= static_cast<std::size_t>( index );
						return m_locs[sz].location->value( );
					}
				}
				std::size_t pos = move_to( static_cast<std::size_t>( index ) );
				if( pos < std::size( m_locs ) ) {
					return m_locs[pos].location->value( );
				}
				return { };
			}

			/***
			 * @return A copy of the underlying basic_json_value
			 */
			[[nodiscard]] constexpr basic_json_value<ParseState>
			get_json_value( ) const {
				return m_value;
			}
		};

		using json_value_state =
		  basic_stateful_json_value<NoCommentSkippingPolicyChecked>;
	} // namespace DAW_JSON_VER
} // namespace daw::json
