// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/daw_arrow_proxy.h"
#include "impl/daw_json_link_impl.h"
#include "impl/daw_json_parse_name.h"
#include "impl/daw_json_value.h"
#include "impl/daw_murmur3.h"

namespace daw::json {
	namespace json_details {
		template<typename Range>
		struct basic_stateful_json_value_state {
			daw::string_view name;
#ifndef _MSC_VER
			std::uint32_t hash_value;
#endif
			basic_json_value_iterator<Range> location;

#ifndef _MSC_VER
			explicit constexpr basic_stateful_json_value_state(
			  daw::string_view Name, basic_json_value_iterator<Range> val ) noexcept
			  : name( Name )
			  , hash_value( daw::murmur3_32( Name ) )
			  , location( std::move( val ) ) {}
#else
			explicit constexpr basic_stateful_json_value_state(
			  daw::string_view Name, basic_json_value_iterator<Range> val ) noexcept
			  : name( Name )
			  , location( std::move( val ) ) {}
#endif

			[[nodiscard]] constexpr bool is_match( daw::string_view Name ) const
			  noexcept {
#ifndef _MSC_VER
				uint32_t const h = daw::murmur3_32( Name );
				if( hash_value != h ) {
					return false;
				}
#endif
				return name == Name;
			}
		};
	} // namespace json_details
	template<typename Range>
	class basic_stateful_json_value {
		basic_json_value<Range> m_value;
		std::vector<json_details::basic_stateful_json_value_state<Range>> m_locs{};

		[[nodiscard]] constexpr std::size_t move_to( daw::string_view key ) {
			std::size_t pos = 0;
			for( ; pos < m_locs.size( ); ++pos ) {
				if( m_locs[pos].is_match( key ) ) {
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
			auto last = m_value.end( );
			while( it != last ) {
				auto name = it.name( );
				daw_json_assert_weak( name, "Expected a class member, not array item" );
				auto const &new_loc = m_locs.emplace_back(
				  daw::string_view( name->data( ), name->size( ) ), it );
				if( new_loc.is_match( key ) ) {
					return pos;
				}
				++pos;
				++it;
			}
			return m_locs.size( );
		}

		[[nodiscard]] constexpr std::size_t move_to( std::size_t index ) {
			if( index < m_locs.size( ) ) {
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
			std::size_t pos = m_locs.size( );
			while( it != last ) {
				auto name = it.name( );
				if( name ) {
					m_locs.emplace_back( daw::string_view( name->data( ), name->size( ) ),
					                     it );
				} else {
					m_locs.emplace_back( daw::string_view( ), it );
				}
				if( pos == index ) {
					return pos;
				}
				++pos;
				++it;
			}
			return m_locs.size( );
		}

	public:
		constexpr basic_stateful_json_value( basic_json_value<Range> val )
		  : m_value( std::move( val ) ) {

			auto const t = m_value.type( );
			daw_json_assert_weak(
			  t == JsonBaseParseTypes::Array or t == JsonBaseParseTypes::Class,
			  "basic_state_full_json_value is only valid on arrays or classes" );
		}

		constexpr basic_stateful_json_value( )
		  : basic_stateful_json_value( basic_json_value<Range>( "{}" ) ) {}

		constexpr void reset( basic_json_value<Range> val ) {
			m_value = std::move( val );
			m_locs.clear( );
		}

		[[nodiscard]] constexpr basic_json_value<Range>
		operator[]( std::string_view key ) {
			daw::string_view const k = daw::string_view( key.data( ), key.size( ) );
			std::size_t pos = move_to( k );
			daw_json_assert_weak( pos < m_locs.size( ), "Unknown member" );
			return m_locs[pos].location->value;
		}

		[[nodiscard]] constexpr basic_json_value<Range> at( std::string_view key ) {
			daw::string_view const k = daw::string_view( key.data( ), key.size( ) );
			std::size_t pos = move_to( k );
			if( pos < m_locs.size( ) ) {
				return m_locs[pos].location->value;
			}
			return {};
		}

		[[nodiscard]] std::size_t size( ) {
			return move_to( std::numeric_limits<std::size_t>::max( ) );
		}

		[[nodiscard]] std::size_t index_of( std::string_view key ) {
			daw::string_view const k = daw::string_view( key.data( ), key.size( ) );
			return move_to( k );
		}

		[[nodiscard]] constexpr bool contains( std::string_view key ) {
			daw::string_view const k = daw::string_view( key.data( ), key.size( ) );
			return move_to( k ) < m_locs.size( );
		}

		[[nodiscard]] constexpr bool contains( std::size_t index ) {
			return move_to( index ) < m_locs.size( );
		}

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		[[nodiscard]] std::optional<std::string_view> name_of( Integer index ) {
			if constexpr( std::is_signed_v<Integer> ) {
				if( index < 0 ) {
					index = -index;
					auto sz = size( );
					if( static_cast<std::size_t>( index ) >= sz ) {
						return {};
					}
					sz -= static_cast<std::size_t>( index );
					return std::string_view( m_locs[sz].name( ).data( ),
					                         m_locs[sz].name( ).size( ) );
				}
			}
			std::size_t pos = move_to( static_cast<std::size_t>( index ) );
			if( pos < m_locs.size( ) ) {
				return std::string_view( m_locs[pos].name( ).data( ),
				                         m_locs[pos].name( ).size( ) );
			}
			return {};
		}

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr basic_json_value<Range>
		operator[]( Integer index ) {
			if constexpr( std::is_signed_v<Integer> ) {
				if( index < 0 ) {
					index = -index;
					auto sz = size( );
					daw_json_assert_weak( static_cast<std::size_t>( index ) < sz,
					                      "Unknown member" );
					sz -= static_cast<std::size_t>( index );
					return m_locs[sz].location->value;
				}
			}
			std::size_t pos = move_to( static_cast<std::size_t>( index ) );
			daw_json_assert_weak( pos < m_locs.size( ), "Unknown member" );
			return m_locs[pos].location->value;
		}

		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr std::optional<basic_json_value<Range>>
		at( Integer index ) {
			if constexpr( std::is_signed_v<Integer> ) {
				if( index < 0 ) {
					index = -index;
					auto sz = size( );
					if( static_cast<std::size_t>( index ) >= sz ) {
						return {};
					}
					sz -= static_cast<std::size_t>( index );
					return m_locs[sz].location->value;
				}
			}
			std::size_t pos = move_to( static_cast<std::size_t>( index ) );
			if( pos < m_locs.size( ) ) {
				return m_locs[pos].location->value;
			}
			return {};
		}

		[[nodiscard]] constexpr basic_json_value<Range> get_json_value( ) const {
			return m_value;
		}
	};

	using json_value_state =
	  basic_stateful_json_value<json_details::IteratorRange<char const *, false>>;
} // namespace daw::json
