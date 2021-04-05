// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_skip.h"
#include "namespace.h"

#include "daw_json_arrow_proxy.h"
#include "daw_json_assert.h"
#include "daw_json_parse_name.h"

#include <daw/daw_move.h>

#include <ciso646>
#include <cstddef>
#include <optional>
#include <string_view>
#include <tuple>

namespace DAW_JSON_NS {
	/***
	 * A container for arbitrary JSON values
	 * @tparam Range see IteratorRange
	 */
	template<typename Range>
	class basic_json_value;

	/***
	 * A name/value pair of string_view/json_value
	 * @tparam Range see IteratorRange
	 */
	template<typename Range>
	struct basic_json_pair {
		std::optional<std::string_view> name;
		basic_json_value<Range> value;
	};

	template<std::size_t Idx, typename Range>
	constexpr decltype( auto ) get( basic_json_pair<Range> const &rng ) {
		static_assert( Idx < 2 );
		if constexpr( Idx == 0 ) {
			return rng.name;
		} else {
			return rng.value;
		}
	}

	template<std::size_t Idx, typename Range>
	constexpr decltype( auto ) get( basic_json_pair<Range> &rng ) {
		static_assert( Idx < 2 );
		if constexpr( Idx == 0 ) {
			return rng.name;
		} else {
			return rng.value;
		}
	}

	template<std::size_t Idx, typename Range>
	constexpr decltype( auto ) get( basic_json_pair<Range> &&rng ) {
		static_assert( Idx < 2 );
		if constexpr( Idx == 0 ) {
			return DAW_MOVE( rng.name );
		} else {
			return DAW_MOVE( rng.value );
		}
	}
} // namespace DAW_JSON_NS

namespace std {
	template<typename Range>
	class tuple_element<0, DAW_JSON_NS::basic_json_pair<Range>> {
	public:
		using type = std::optional<std::string_view>;
	};

	template<typename Range>
	class tuple_element<1, DAW_JSON_NS::basic_json_pair<Range>> {
	public:
		using type = DAW_JSON_NS::basic_json_value<Range>;
	};

	template<typename Range>
	class tuple_size<DAW_JSON_NS::basic_json_pair<Range>>
	  : public std::integral_constant<std::size_t, 2> {};
} // namespace std

namespace DAW_JSON_NS {
	/***
	 * Iterator for iterating over arbutrary JSON members and array elements
	 * @tparam Range see IteratorRange
	 */
	template<typename Range>
	struct basic_json_value_iterator {
		using key_type = std::string_view;
		using mapped_type = basic_json_value<Range>;

		using json_pair = basic_json_pair<Range>;
		using value_type = basic_json_pair<Range>;
		using reference = value_type;
		using pointer = json_details::arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

	private:
		Range m_state{ };

		constexpr basic_json_value_iterator( Range rng )
		  : m_state( rng ) {}

		friend class ::DAW_JSON_NS::basic_json_value<Range>;

	public:
		constexpr basic_json_value_iterator( ) = default;

		/***
		 * Name of member
		 * @return The name, if any, of the current member
		 */
		[[nodiscard]] constexpr std::optional<std::string_view> name( ) const {
			if( is_array( ) ) {
				return { };
			}
			auto rng = m_state;
			auto result = json_details::parse_name( rng );
			return std::string_view( std::data( result ), std::size( result ) );
		}

		/***
		 * Get the value currently being referenced
		 * @return A basic_json_value representing the currently referenced element
		 * in the range
		 */
		[[nodiscard]] constexpr basic_json_value<Range> value( ) const {
			if( is_array( ) ) {
				return Range( m_state );
			}
			auto rng = m_state;
			(void)json_details::parse_name( rng );
			return Range( rng.first, rng.last );
		}

		/***
		 * Get the name/value pair of the currently referenced element
		 * @return a json_pair with the name, if any, and json_value
		 */
		[[nodiscard]] constexpr basic_json_pair<Range> operator*( ) {
			if( is_array( ) ) {
				return { { }, Range( m_state.first, m_state.last ) };
			}
			auto rng = m_state;
			auto name = json_details::parse_name( rng );
			return { std::string_view( std::data( name ), std::size( name ) ),
			         Range( rng.first, rng.last ) };
		}

		/***
		 * Return an arrow_proxy object containing the result of operator*
		 * Should not use this method unless you must(std algorithms), it is here
		 * for Iterator compatibility
		 * @return arrow_proxy object containing the result of operator*
		 */
		[[nodiscard]] constexpr pointer operator->( ) {
			return { operator*( ) };
		}

		/***
		 * Move the parser to the next value
		 * @return A reference to the iterator
		 */
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

		/***
		 * Moved parser to next value
		 * @return copy of iterator upon entering method
		 */
		constexpr basic_json_value_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		/***
		 * Is the value this iterator iterates over an array
		 * @return true if the value is an array
		 */
		[[nodiscard]] constexpr bool is_array( ) const {
			return *m_state.class_first == '[';
		}

		/***
		 * Is the value this iterator iterates over an class
		 * @return true if the value is an class
		 */
		[[nodiscard]] constexpr bool is_class( ) const {
			return *m_state.class_first == '{';
		}

		/***
		 * Can we increment more
		 * @return True if safe to increment more
		 */
		[[nodiscard]] constexpr bool good( ) const {
			if( not m_state.has_more( ) or m_state.is_null( ) ) {
				return false;
			}
			switch( m_state.front( ) ) {
			case '[':
			case '{':
			case '"':
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
			case 't':
			case 'f':
			case 'n':
				return true;
			case '}':
			case ']':
				return false;
			}
			daw_json_error( ErrorReason::ExpectedTokenNotFound, m_state );
		}

		/***
		 * Can we increment more
		 * @return True if safe to increment more
		 */
		[[nodiscard]] constexpr explicit operator bool( ) const {
			return good( );
		}

		/***
		 * Check for equivilence with rhs iterator
		 * @param rhs iterator to compare for equivilence with
		 * @return true if both are equivilent
		 */
		[[nodiscard]] constexpr bool
		operator==( basic_json_value_iterator<Range> const &rhs ) const {
			if( good( ) ) {
				if( rhs.good( ) ) {
					return m_state.first == rhs.m_state.first;
				}
				return false;
			}
			return not rhs.good( );
		}

		/***
		 * Check if rhs is not equivilent to self
		 * @param rhs iterator to compare for equivilence with
		 * @return true if the rhs is not equivilent
		 */
		[[nodiscard]] constexpr bool
		operator!=( basic_json_value_iterator<Range> const &rhs ) const {
			return not operator==( rhs );
		}
	};

	template<typename Range>
	struct basic_json_value_iterator_range {
		using iterator = basic_json_value_iterator<Range>;
		iterator first;
		iterator last;

		[[nodiscard]] constexpr iterator begin( ) {
			return first;
		}
		[[nodiscard]] constexpr iterator end( ) {
			return last;
		}
	};
	template<typename Range>
	basic_json_value_iterator_range( basic_json_value_iterator<Range>,
	                                 basic_json_value_iterator<Range> )
	  -> basic_json_value_iterator_range<Range>;

	/***
	 * A container for arbitrary JSON values
	 * @tparam Range see IteratorRange
	 */
	template<typename Range>
	class basic_json_value {
		Range m_rng{ };

	public:
		using iterator = basic_json_value_iterator<Range>;

		/***
		 * Construct from IteratorRange
		 * @param rng string data where start is the start of our value
		 */
		inline constexpr basic_json_value( Range rng )
		  : m_rng( DAW_MOVE( rng ) ) {
			// Ensure we are at the actual value
			m_rng.trim_left( );
		}

		/***
		 * Construct from std::string_view
		 */
		explicit inline constexpr basic_json_value( std::string_view sv )
		  : m_rng( std::data( sv ), daw::data_end( sv ) ) {}

		/***
		 * Construct from char const *, std::size_t
		 */
		explicit inline constexpr basic_json_value( char const *first,
		                                            std::size_t sz )
		  : m_rng( first, first + static_cast<std::ptrdiff_t>( sz ) ) {}

		/***
		 * Construct from char const *, char const *
		 */
		explicit inline constexpr basic_json_value( char const *first,
		                                            char const *last )
		  : m_rng( first, last ) {}

		/***
		 * Get a copy of the underlying range
		 * @return IteratorRange containing values JSON data
		 */
		[[nodiscard]] inline constexpr Range get_range( ) const {
			return m_rng;
		}

		/***
		 * Get the first member/item
		 * @pre type of value is class or array
		 * @return basic_json_value_iterator to the first item/member
		 */
		[[nodiscard]] inline constexpr iterator begin( ) const {
			Range rng = Range( m_rng.first, m_rng.last );
			rng.remove_prefix( );
			rng.trim_left( );
			return basic_json_value_iterator<Range>( rng );
		}

		/***
		 * End of range over class/arrays members/items
		 * @return default constructed basic_json_value_iterator
		 */
		[[nodiscard]] inline constexpr iterator end( ) const {
			return basic_json_value_iterator<Range>( );
		}

		/***
		 * Get the type of JSON value
		 * @return a JSONBaseParseTypes enum value with the type of this JSON value
		 */
		[[nodiscard]] JsonBaseParseTypes type( ) const {
			if( not m_rng.has_more( ) ) {
				return JsonBaseParseTypes::None;
			}
			switch( m_rng.front( ) ) {
			case '"':
				return JsonBaseParseTypes::String;
			case '{':
				return JsonBaseParseTypes::Class;
			case '[':
				return JsonBaseParseTypes::Array;
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
					if( m_rng.starts_with( "true" ) ) {
						return JsonBaseParseTypes::Bool;
					}
					return JsonBaseParseTypes::None;
				} else {
					return JsonBaseParseTypes::Bool;
				}
			case 'f':
				if constexpr( not Range::is_unchecked_input ) {
					if( m_rng.starts_with( "false" ) ) {
						return JsonBaseParseTypes::Bool;
					}
					return JsonBaseParseTypes::None;
				} else {
					return JsonBaseParseTypes::Bool;
				}
			case 'n':
				daw_json_assert_weak( m_rng.starts_with( "null" ),
				                      ErrorReason::InvalidNull, m_rng );
				return JsonBaseParseTypes::Null;
			}
			return JsonBaseParseTypes::None;
		}

		/***
		 * Get the JSON data
		 * @return the JSON data as a std::string_view
		 */
		[[nodiscard]] constexpr std::string_view get_string_view( ) const {
			auto rng = m_rng;
			auto result = json_details::skip_value( rng );
			if( is_string( ) ) {
				--result.first;
				++result.last;
			}
			return { std::data( result ), std::size( result ) };
		}

		/***
		 * Get a copy of the JSON data
		 * @return the JSON data as a std::string
		 */
		template<typename Allocator = std::allocator<char>,
		         typename Traits = std::char_traits<char>>
		[[nodiscard]] std::basic_string<char, Traits, Allocator>
		get_string( Allocator const &alloc = std::allocator<char>( ) ) const {
			auto rng = m_rng;
			auto result = json_details::skip_value( rng );
			if( is_string( ) ) {
				--result.first;
				++result.last;
			}
			return { std::data( result ), std::size( result ), alloc };
		}

		/***
		 * Is the JSON value a null literal
		 * @return true if the value is a null literal
		 */
		[[nodiscard]] constexpr bool is_null( ) const {
			return ( m_rng.starts_with( "null" ) );
		}

		/***
		 * Is the JSON value a class
		 * @return true if the value is a class
		 */
		[[nodiscard]] constexpr bool is_class( ) const {
			return m_rng.is_opening_brace_checked( );
		}

		/***
		 * Is the JSON value a array
		 * @return true if the value is a array
		 */
		[[nodiscard]] constexpr bool is_array( ) const {
			return m_rng.is_opening_bracket_checked( );
		}

		/***
		 * Is the JSON value a number literal
		 * @return true if the value is a number literal
		 */
		[[nodiscard]] constexpr bool is_number( ) const {
			if( not m_rng.has_more( ) ) {
				return false;
			}
			switch( m_rng.front( ) ) {
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
			case '+':
			case '-':
				return true;
			}
			return false;
		}

		/***
		 * Is the JSON value a string
		 * @return true if the value is a string
		 */
		[[nodiscard]] inline constexpr bool is_string( ) const {
			return m_rng.is_quotes_checked( );
		}

		/***
		 * Is the JSON value a boolean
		 * @return true if the value is a boolean
		 */
		[[nodiscard]] constexpr bool is_bool( ) const {
			if( not m_rng.has_more( ) ) {
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

		/***
		 * Is the JSON data unrecognizable.  JSON members will start with one of
		 * ",[,{,0,1,2,3,4,5,6,7,8,9,-,t,f, or n and this does not
		 * @return true if the parser is unsure what the data is
		 */
		[[nodiscard]] inline constexpr bool is_unknown( ) const {
			return type( ) == JsonBaseParseTypes::None;
		}

		template<typename NewRange>
		explicit inline constexpr operator basic_json_value<NewRange>( ) const {
			auto new_range = NewRange( m_rng.first, m_rng.last );
			new_range.class_first = m_rng.class_first;
			new_range.class_last = m_rng.class_last;
			return basic_json_value<NewRange>( DAW_MOVE( new_range ) );
		}
	};
} // namespace DAW_JSON_NS
