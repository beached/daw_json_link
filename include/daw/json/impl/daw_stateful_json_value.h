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

#include "daw_arrow_proxy.h"
#include "daw_json_link_impl.h"
#include "daw_json_parse_name.h"
#include "daw_murmur3.h"
#include "daw_stateful_json_value.h"

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

		using array_locs = std::vector<Range>;
		using class_locs =
		  std::vector<json_details::basic_stateful_json_value_state<Range>>;
		using loc_var = std::variant<array_locs, class_locs>;
		loc_var m_locs;

		constexpr array_locs const &get_array_locs( ) const {
			return std::get<array_locs>( m_locs );
		}

		constexpr array_locs &get_array_locs( ) {
			return std::get<array_locs>( m_locs );
		}

		constexpr class_locs const &get_class_locs( ) const {
			return std::get<class_locs>( m_locs );
		}

		constexpr class_locs &get_class_locs( ) {
			return std::get<class_locs>( m_locs );
		}

		[[nodiscard]] constexpr basic_json_value_iterator<Range>
		search_locs( daw::string_view key ) const {
			daw_json_assert_weak( is_class( ),
			                      "Expected class type for string member lookup" );
			for( auto const &l : get_class_locs( ) ) {
				if( l.is_match( key ) ) {
					return l.location;
				}
			}
			return m_value.end( );
		}

		[[nodiscard]] constexpr basic_json_value_iterator<Range>
		add_member( daw::string_view key ) {
			daw_json_assert_weak( is_class( ),
			                      "Expected class type for string member lookup" );

			auto &loc = get_class_locs( );
			auto it = [&] {
				if( loc.empty( ) ) {
					return m_value.begin( );
				}
				auto res = loc.back( ).location;
				++res;
				return res;
			}( );
			auto last = m_value.end( );
			while( it != last ) {
				auto name = it.name( );
				daw_json_assert_weak( name, "Expected a class member, not array item" );
				auto const &new_loc = loc.emplace_back(
				  daw::string_view( name->data( ), name->size( ) ), it );
				if( new_loc.is_match( key ) ) {
					return it;
				}
				++it;
			}
			return last;
		}

	public:
		constexpr basic_stateful_json_value( basic_json_value<Range> val )
		  : m_value( std::move( val ) )
		  , m_locs( m_value.is_array( ) ? loc_var( array_locs( ) )
		                                : loc_var( class_locs( ) ) ) {

			auto const t = m_value.type( );
			daw_json_assert_weak(
			  t == JsonBaseParseTypes::Array or t == JsonBaseParseTypes::Class,
			  "basic_state_full_json_value is only valid on arrays or classes" );
		}

		constexpr basic_stateful_json_value( )
		  : basic_stateful_json_value( basic_json_value<Range>( "{}" ) ) {}

		constexpr void reset( basic_json_value<Range> val ) {
			bool const was_an_array = is_array( );
			m_value = std::move( val );
			bool const is_an_array = m_value.is_array( );
			if( is_an_array ) {
				if( was_an_array ) {
					get_array_locs( ).clear( );
				} else {
					m_locs = array_locs( );
				}
			} else {
				if( was_an_array ) {
					m_locs = class_locs( );
				} else {
					get_class_locs( ).clear( );
				}
			}
		}

		[[nodiscard]] constexpr basic_json_value<Range>
		operator[]( std::string_view key ) {
			daw_json_assert_weak( is_class( ),
			                      "Attempt to access a member of a non-class type" );

			daw::string_view k = daw::string_view( key.data( ), key.size( ) );
			auto loc = search_locs( k );
			if( loc == m_value.end( ) ) {
				loc = add_member( k );
				daw_json_assert_weak( loc != m_value.end( ),
				                      "Expected member to exist" );
			}
			return loc->value;
		}

		[[nodiscard]] constexpr basic_json_value<Range> get_json_value( ) const {
			return m_value;
		}

		[[nodiscard]] constexpr bool is_array( ) const {
			return std::holds_alternative<array_locs>( m_locs );
		}

		[[nodiscard]] constexpr bool is_class( ) const {
			return std::holds_alternative<class_locs>( m_locs );
		}

		[[nodiscard]] JsonBaseParseTypes type( ) const {
			return m_value.type( );
		}

		constexpr std::string_view get_string_view( ) const {
			return m_value.get_string_view( );
		}

		std::string get_string( ) const {
			return m_value.get_string( );
		}
	};

	using stateful_json_value =
	  basic_stateful_json_value<json_details::IteratorRange<char const *, false>>;
} // namespace daw::json
