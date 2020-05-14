// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_arrow_proxy.h"
#include "daw_json_parse_name.h"

namespace daw::json {
	template<typename Range>
	class basic_json_value;

	template<typename Range>
	struct basic_json_pair {
		std::optional<std::string_view> name;
		basic_json_value<Range> value;
	};

	template<typename Range>
	struct basic_json_value_iterator {
		using key_type = std::string_view;
		using mapped_type = basic_json_value<Range>;

		using json_pair = basic_json_pair<Range>;
		using value_type = json_pair;
		using reference = value_type;
		using pointer = json_details::arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

	private:
		Range m_state{};
		Range m_new_state{};

		constexpr basic_json_value_iterator( Range rng )
		  : m_state( rng ) {}

		friend class ::daw::json::basic_json_value<Range>;

	public:
		constexpr basic_json_value_iterator( ) = default;

		constexpr std::optional<std::string_view> name( ) const {
			if( is_array( ) ) {
				return {};
			}
			auto rng = m_state;
			auto result = parse_name( rng );
			return std::string_view( result.data( ), result.size( ) );
		}

		[[nodiscard]] constexpr mapped_type value( ) const {
			if( is_array( ) ) {
				return Range( m_state );
			}
			auto rng = m_state;
			(void)parse_name( rng );
			return Range( rng.first, rng.last );
		}

		[[nodiscard]] constexpr value_type operator*( ) const {
			if( is_array( ) ) {
				return {{}, Range( m_state )};
			}
			auto rng = m_state;
			auto name = parse_name( rng );
			return {std::string_view( name.data( ), name.size( ) ),
			        Range( rng.first, rng.last )};
		}

		[[nodiscard]] constexpr pointer operator->( ) const {
			return {operator*( )};
		}

		constexpr basic_json_value_iterator &operator++( ) {
			if( good( ) ) {
				if( is_class( ) ) {
					(void)json_details::parse_name( m_state );
				}
				(void)json_details::skip_value( m_state );
				m_state.clean_tail( );
			}
			return *this;
		}

		constexpr basic_json_value_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr bool is_array( ) const {
			return *m_state.class_first == '[';
		}

		[[nodiscard]] constexpr bool is_class( ) const {
			return *m_state.class_first == '{';
		}

		constexpr bool good( ) const {
			return m_state.can_parse_more( ) and not m_state.in( "]}" );
		}

		constexpr explicit operator bool( ) const {
			return good( );
		}

		constexpr bool
		operator==( basic_json_value_iterator<Range> const &rhs ) const {
			if( good( ) ) {
				if( rhs.good( ) ) {
					return m_state.begin( ) == rhs.m_state.begin( );
				}
				return false;
			}
			return not rhs.good( );
		}

		constexpr bool
		operator!=( basic_json_value_iterator<Range> const &rhs ) const {
			return not operator==( rhs );
		}
	};

	template<typename Range>
	class basic_json_value {
		Range m_rng{};

	public:
		using iterator = basic_json_value_iterator<Range>;

		constexpr basic_json_value( Range rng )
		  : m_rng( std::move( rng ) ) {
			m_rng.trim_left( );
		}

		constexpr basic_json_value( std::string_view sv )
		  : m_rng( sv.data( ), sv.data( ) + sv.size( ) ) {}

		[[nodiscard]] constexpr Range get_range( ) const {
			return m_rng;
		}

		[[nodiscard]] constexpr iterator begin( ) const {
			Range rng = Range( m_rng.first, m_rng.last );
			rng.remove_prefix( );
			rng.trim_left( );
			return basic_json_value_iterator<Range>( rng );
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return basic_json_value_iterator<Range>( );
		}

		[[nodiscard]] JsonBaseParseTypes type( ) const {
			if( not m_rng.can_parse_more( ) ) {
				return JsonBaseParseTypes::None;
			}
			switch( m_rng.front( ) ) {
			case '"':
				return JsonBaseParseTypes::String;
			case '{':
				return JsonBaseParseTypes::Class;
			case '[':
				return JsonBaseParseTypes::Array;
			case '+':
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return JsonBaseParseTypes::Number;
			case 't':
				if constexpr( not Range::is_unchecked_input ) {
					if( m_rng == "true" ) {
						return JsonBaseParseTypes::Bool;
					}
					return JsonBaseParseTypes::None;
				} else {
					return JsonBaseParseTypes::Bool;
				}
			case 'f':
				if constexpr( not Range::is_unchecked_input ) {
					if( m_rng == "false" ) {
						return JsonBaseParseTypes::Bool;
					}
					return JsonBaseParseTypes::None;
				} else {
					return JsonBaseParseTypes::Bool;
				}
			case 'n':
				daw_json_assert_weak( m_rng == "null", "Expected a null" );
				return JsonBaseParseTypes::Null;
			}
			return JsonBaseParseTypes::None;
		}

		constexpr std::string_view get_string_view( ) const {
			auto rng = m_rng;
			auto result = json_details::skip_value( rng );
			if( is_string( ) ) {
				--result.first;
				++result.last;
			}
			return {result.first, result.size( )};
		}

		std::string get_string( ) const {
			auto rng = m_rng;
			auto result = json_details::skip_value( rng );
			if( is_string( ) ) {
				--result.first;
				++result.last;
			}
			return {result.first, result.size( )};
		}

		constexpr bool is_null( ) const {
			return ( not m_rng.can_parse_more( ) ) or m_rng == "null";
		}

		constexpr bool is_class( ) const {
			return m_rng.can_parse_more( ) and m_rng.front( ) == '{';
		}

		constexpr bool is_array( ) const {
			return m_rng.can_parse_more( ) and m_rng.front( ) == '[';
		}

		constexpr bool is_number( ) const {
			return m_rng.can_parse_more( ) and m_rng.in( "0123456789+-" );
		}

		constexpr bool is_string( ) const {
			return m_rng.can_parse_more( ) and m_rng.front( ) == '"';
		}

		constexpr bool is_bool( ) const {
			if( not m_rng.can_parse_more( ) ) {
				return false;
			}
			switch( m_rng.front( ) ) {
			case 't':
				if constexpr( not Range::is_unchecked_input ) {
					return m_rng == "true";
				} else {
					return true;
				}
			case 'f':
				if constexpr( not Range::is_unchecked_input ) {
					return m_rng == "false";
				} else {
					return true;
				}
			}
			return false;
		}

		constexpr bool is_unknown( ) const {
			return type( ) == JsonBaseParseTypes::None;
		}
	};
} // namespace daw::json
