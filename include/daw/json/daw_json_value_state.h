// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

			[[nodiscard]] constexpr bool
			is_match( daw::string_view Name ) const noexcept {
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
		std::vector<json_details::basic_stateful_json_value_state<Range>> m_locs{ };

		[[nodiscard]] constexpr std::size_t move_to( daw::string_view key ) {
			std::size_t pos = 0;
			for( ; pos < m_locs.size( ); ++pos ) {
				if( m_locs[pos].is_match( key ) ) {
					return pos;
				}
			}
			auto it = [&] {
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
				auto res = m_locs.back( ).location;
				++res;
				return res;
			}( );
			auto last = m_value.end( );
			std::size_t pos = m_locs.size( );
			while( it != last ) {
				auto name = it.name( );
				auto const &new_loc = [&] {
					if( name ) {
						return m_locs.emplace_back(
						  daw::string_view( name->data( ), name->size( ) ), it );
					}
					return m_locs.emplace_back( daw::string_view( ), it );
				};
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
			return { };
		}

		[[nodiscard]] std::size_t size( ) {
			return move_to( std::numeric_limits<std::size_t>::max( ) );
		}

		[[nodiscard]] std::size_t index_of( std::string_view key ) {
			daw::string_view const k = daw::string_view( key.data( ), key.size( ) );
			return move_to( k );
		}

		[[nodiscard]] std::optional<std::string_view> name_of( std::size_t index ) {
			auto pos = move_to( index );
			if( pos < m_locs.size( ) ) {
				return std::string_view( m_locs[pos].name( ).data( ),
				                         m_locs[pos].name( ).size( ) );
			}
			return { };
		}

		[[nodiscard]] constexpr basic_json_value<Range>
		operator[]( std::size_t index ) {
			std::size_t pos = move_to( index );
			daw_json_assert_weak( pos < m_locs.size( ), "Unknown member" );
			return m_locs[pos].location->value;
		}

		[[nodiscard]] constexpr std::optional<basic_json_value<Range>>
		at( std::size_t index ) {
			auto pos = move_to( index );
			if( pos < m_locs.size( ) ) {
				return m_locs[pos].location->value;
			}
			return { };
		}

		[[nodiscard]] constexpr basic_json_value<Range> get_json_value( ) const {
			return m_value;
		}
	};

	using json_value_state =
	  basic_stateful_json_value<json_details::IteratorRange<char const *, false>>;
} // namespace daw::json
