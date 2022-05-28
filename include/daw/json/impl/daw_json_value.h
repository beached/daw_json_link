// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../daw_json_data_contract.h"
#include "daw_json_arrow_proxy.h"
#include "daw_json_assert.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_policy.h"
#include "daw_json_skip.h"
#include "daw_json_traits.h"
#include "daw_json_value_fwd.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_move.h>
#include <daw/daw_utility.h>

#include <cassert>
#include <ciso646>
#include <cstddef>
#include <optional>
#include <string_view>
#include <tuple>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// @brief A name/value pair of string_view/json_value
		/// @tparam ParseState see IteratorRange
		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		struct basic_json_pair {
			using ParseState = BasicParsePolicy<PolicyFlags, Allocator>;

			std::optional<std::string_view> name;
			basic_json_value<PolicyFlags, Allocator> value;
		};

		template<std::size_t Idx, json_options_t PolicyFlags, typename Allocator>
		constexpr decltype( auto )
		get( basic_json_pair<PolicyFlags, Allocator> const &parse_state ) {
			static_assert(
			  Idx < 2,
			  "Invalid index. Valid values are 0 for name, and 1 for value" );
			if constexpr( Idx == 0 ) {
				return parse_state.name;
			} else {
				return parse_state.value;
			}
		}

		template<std::size_t Idx, json_options_t PolicyFlags, typename Allocator>
		constexpr decltype( auto )
		get( basic_json_pair<PolicyFlags, Allocator> &parse_state ) {
			static_assert(
			  Idx < 2,
			  "Invalid index. Valid values are 0 for name, and 1 for value" );
			if constexpr( Idx == 0 ) {
				return parse_state.name;
			} else {
				return parse_state.value;
			}
		}

		template<std::size_t Idx, json_options_t PolicyFlags, typename Allocator>
		constexpr decltype( auto )
		get( basic_json_pair<PolicyFlags, Allocator> &&parse_state ) {
			static_assert(
			  Idx < 2,
			  "Invalid index. Valid values are 0 for name, and 1 for value" );
			if constexpr( Idx == 0 ) {
				return DAW_MOVE( parse_state.name );
			} else {
				return DAW_MOVE( parse_state.value );
			}
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json

namespace std {
	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	class tuple_element<0, daw::json::basic_json_pair<PolicyFlags, Allocator>> {
	public:
		using type = std::optional<std::string_view>;
	};

	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	class tuple_element<1, daw::json::basic_json_pair<PolicyFlags, Allocator>> {
	public:
		using type = daw::json::basic_json_value<PolicyFlags, Allocator>;
	};

	template<daw::json::json_options_t PolicyFlags, typename Allocator>
	class tuple_size<daw::json::basic_json_pair<PolicyFlags, Allocator>>
	  : public std::integral_constant<std::size_t, 2> {};
} // namespace std

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/***
		 * Iterator for iterating over arbitrary JSON members and array elements
		 * @tparam ParseState see IteratorRange
		 */
		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		struct basic_json_value_iterator {
			using key_type = std::string_view;
			using mapped_type = basic_json_value<PolicyFlags, Allocator>;

			using json_pair = basic_json_pair<PolicyFlags, Allocator>;
			using value_type = basic_json_pair<PolicyFlags, Allocator>;
			using reference = value_type;
			using pointer = json_details::arrow_proxy<value_type>;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::forward_iterator_tag;

		private:
			using ParseState = BasicParsePolicy<PolicyFlags, Allocator>;
			ParseState m_state{ };

			explicit constexpr basic_json_value_iterator(
			  BasicParsePolicy<PolicyFlags, Allocator> const &parse_state )
			  : m_state( parse_state ) {}

			friend class basic_json_value<PolicyFlags, Allocator>;

		public:
			basic_json_value_iterator( ) = default;

			/***
			 * Name of member
			 * @return The name, if any, of the current member
			 */
			[[nodiscard]] constexpr std::optional<std::string_view> name( ) const {
				if( is_array( ) ) {
					return { };
				}
				auto parse_state = m_state;
				auto result = json_details::parse_name( parse_state );
				return std::string_view( std::data( result ), std::size( result ) );
			}

			/***
			 * Get the value currently being referenced
			 * @return A basic_json_value representing the currently referenced
			 * element in the range
			 */
			[[nodiscard]] constexpr basic_json_value<PolicyFlags, Allocator>
			value( ) const {
				if( is_array( ) ) {
					return ParseState( m_state );
				}
				auto parse_state = m_state;
				(void)json_details::parse_name( parse_state );
				return ParseState( parse_state.first, parse_state.last,
				                   parse_state.first, parse_state.last,
				                   parse_state.get_allocator( ) );
			}

			/***
			 * Get the name/value pair of the currently referenced element
			 * @return a json_pair with the name, if any, and json_value
			 */
			[[nodiscard]] constexpr basic_json_pair<PolicyFlags, Allocator>
			operator*( ) {
				if( is_array( ) ) {
					return { { },
					         basic_json_value( ParseState( m_state.first, m_state.last,
					                                       m_state.first, m_state.last,
					                                       m_state.get_allocator( ) ) ) };
				}
				auto parse_state = m_state;
				auto name = json_details::parse_name( parse_state );
				return { std::string_view( std::data( name ), std::size( name ) ),
				         basic_json_value( ParseState(
				           parse_state.first, parse_state.last, parse_state.first,
				           parse_state.last, parse_state.get_allocator( ) ) ) };
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
					m_state.move_next_member_or_end( );
				}
				return *this;
			}

			///
			/// @brief Move parser to next value
			///
			inline constexpr void operator++( int ) & {
				// auto result = *this;
				operator++( );
				// return result;
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

			/// @brief Can we increment more
			/// @return True if safe to increment more
			[[nodiscard]] constexpr explicit operator bool( ) const {
				return good( );
			}

			[[nodiscard]] constexpr BasicParsePolicy<PolicyFlags, Allocator> const &
			get_raw_state( ) const {
				return m_state;
			}

			/***
			 * Check for equivalence with rhs iterator
			 * @param rhs iterator to compare for equivalence with
			 * @return true if both are equivalent
			 */
			template<json_options_t P, typename A>
			[[nodiscard]] constexpr bool
			operator==( basic_json_value_iterator<P, A> const &rhs ) const {
				if( good( ) ) {
					if( rhs.good( ) ) {
						return m_state.first == rhs.m_state.first;
					}
					return false;
				}
				return not rhs.good( );
			}

			/***
			 * Check if rhs is not equivalent to self
			 * @param rhs iterator to compare for equivalence with
			 * @return true if the rhs is not equivalent
			 */
			template<json_options_t P, typename A>
			[[nodiscard]] constexpr bool
			operator!=( basic_json_value_iterator<P, A> const &rhs ) const {
				return not operator==( rhs );
			}
		};

		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		struct basic_json_value_iterator_range {
			using iterator = basic_json_value_iterator<PolicyFlags, Allocator>;
			using CharT = typename BasicParsePolicy<PolicyFlags, Allocator>::CharT;
			iterator first;
			iterator last;

			[[nodiscard]] constexpr iterator begin( ) {
				return first;
			}
			[[nodiscard]] constexpr iterator end( ) {
				return last;
			}
		};

		template<json_options_t PolicyFlags, typename Allocator>
		basic_json_value_iterator_range(
		  basic_json_value_iterator<PolicyFlags, Allocator>,
		  basic_json_value_iterator<PolicyFlags, Allocator> )
		  -> basic_json_value_iterator_range<PolicyFlags, Allocator>;

		/// @brief A container for arbitrary JSON values
		/// @tparam ParseState see IteratorRange
		template<json_options_t PolicyFlags, typename Allocator>
		class basic_json_value {
			using ParseState = BasicParsePolicy<PolicyFlags, Allocator>;
			BasicParsePolicy<PolicyFlags, Allocator> m_parse_state{ };

		public:
			using CharT = typename ParseState::CharT;
			using iterator = basic_json_value_iterator<PolicyFlags, Allocator>;
			using value_type = basic_json_pair<PolicyFlags, Allocator>;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			basic_json_value( ) = default;

			/// @brief Construct from IteratorRange
			/// @param parse_state string data where start is the start of our value
			template<json_options_t P, typename A>
			explicit inline constexpr basic_json_value(
			  BasicParsePolicy<P, A> parse_state )
			  : m_parse_state( DAW_MOVE( parse_state ) ) {
				// Ensure we are at the actual value.
				m_parse_state.trim_left( );
			}

			/// @brief Construct from string_view
			explicit inline constexpr basic_json_value( daw::string_view sv )
			  : m_parse_state( std::data( sv ), daw::data_end( sv ) ) {}

			/// @brief Construct from CharT *, std::size_t
			explicit inline constexpr basic_json_value( CharT *first, std::size_t sz )
			  : m_parse_state( first, first + static_cast<std::ptrdiff_t>( sz ) ) {}

			/// @brief Construct from CharT *, CharT *
			explicit inline constexpr basic_json_value( CharT *first, CharT *last )
			  : m_parse_state( first, last ) {}

			/// @brief Get a copy of the underlying parse state
			/// @return IteratorRange containing values JSON data
			[[nodiscard]] inline constexpr ParseState get_raw_state( ) const {
				return m_parse_state;
			}

			/// @brief Get the first member/item
			/// @pre type of value is class or array
			/// @return basic_json_value_iterator to the first item/member
			[[nodiscard]] inline constexpr iterator begin( ) const {
				auto parse_state = ParseState( m_parse_state.first, m_parse_state.last,
				                               m_parse_state.first, m_parse_state.last,
				                               m_parse_state.get_allocator( ) );
				parse_state.remove_prefix( );
				parse_state.trim_left( );
				return iterator( parse_state );
			}

			/// @brief End of range over class/arrays members/items
			/// @return default constructed basic_json_value_iterator
			[[nodiscard]] inline constexpr iterator end( ) const {
				return iterator( );
			}

			[[nodiscard]] constexpr basic_json_value
			find_class_member( daw::string_view name ) const {
				if( type( ) != JsonBaseParseTypes::Class ) {
					return basic_json_value{ };
				}
				auto pos =
				  daw::algorithm::find_if( begin( ), end( ), [name]( auto const &jp ) {
					  assert( jp.name );
					  return *jp.name == name;
				  } );
				if( pos == end( ) ) {
					return basic_json_value( );
				}
				return ( *pos ).value;
			}

			[[nodiscard]] constexpr basic_json_value
			find_member( daw::string_view name ) const {
				auto jv = *this;
				while( not name.empty( ) and jv ) {
					char last_char = 0;
					auto member = [&] {
						if( name.front( ) == '[' ) {
							return name.pop_front_until( ']' );
						}
						return name.pop_front_until(
						  [&]( char c ) {
							  if( last_char == '\\' ) {
								  last_char = 0;
								  return false;
							  }
							  last_char = c;
							  if( c == '.' or c == '[' ) {
								  return true;
							  }
							  return false;
						  },
						  nodiscard );
					}( );
					if( not name.empty( ) and name.front( ) == '.' ) {
						name.remove_prefix( );
					}
					if( member.front( ) == '[' ) {
						member.remove_prefix( );
						auto index_ps = BasicParsePolicy<PolicyFlags, Allocator>(
						  std::data( member ), daw::data_end( member ), nullptr, nullptr,
						  m_parse_state.get_allocator( ) );
						auto const index = json_details::parse_value<
						  json_details::json_deduced_type<std::size_t>, true>(
						  index_ps, ParseTag<JsonParseTypes::Unsigned>{ } );
						jv = jv.find_element( index );
						if( not name.empty( ) and name.front( ) == '.' ) {
							name.remove_prefix( );
						}
						continue;
					}
					jv = jv.find_class_member( member );
				}
				return jv;
			}

			template<typename Result>
			[[nodiscard]] constexpr auto
			as( template_param<Result> = template_arg<Result> ) const {
				using result_t = json_details::json_deduced_type<Result>;
				auto state = m_parse_state;
				return json_details::parse_value<result_t>(
				  state, ParseTag<result_t::expected_type>{ } );
			}

			[[nodiscard]] constexpr basic_json_value
			operator[]( daw::string_view json_path ) const {
				return find_member( json_path );
			}

			[[nodiscard]] constexpr basic_json_value
			find_element( std::size_t index ) const {
				auto first = begin( );
				auto const last = end( );
				while( nsc_and( index > 0, first != last ) ) {
					--index;
					++first;
				}
				if( index == 0 ) {
					return ( *first ).value;
				}
				return basic_json_value( );
			}

			[[nodiscard]] constexpr basic_json_value
			find_array_element( std::size_t index ) const {
				assert( type( ) == JsonBaseParseTypes::Array );
				return find_element( index );
			}

			[[nodiscard]] constexpr basic_json_value
			operator[]( std::size_t index ) const {
				return find_element( index );
			}

			/// @brief Get the type of JSON value
			/// @return a JSONBaseParseTypes enum value with the type of this JSON
			/// value
			[[nodiscard]] constexpr JsonBaseParseTypes type( ) const noexcept {
				if( not m_parse_state.has_more( ) ) {
					return JsonBaseParseTypes::None;
				}
				switch( m_parse_state.front( ) ) {
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
					if constexpr( not ParseState::is_unchecked_input ) {
						if( m_parse_state.starts_with( "true" ) ) {
							return JsonBaseParseTypes::Bool;
						}
						return JsonBaseParseTypes::None;
					} else {
						return JsonBaseParseTypes::Bool;
					}
				case 'f':
					if constexpr( not ParseState::is_unchecked_input ) {
						if( m_parse_state.starts_with( "false" ) ) {
							return JsonBaseParseTypes::Bool;
						}
						return JsonBaseParseTypes::None;
					} else {
						return JsonBaseParseTypes::Bool;
					}
				case 'n':
					daw_json_assert_weak( m_parse_state.starts_with( "null" ),
					                      ErrorReason::InvalidNull, m_parse_state );
					return JsonBaseParseTypes::Null;
				}
				return JsonBaseParseTypes::None;
			}

			/// @brief Construct a string range of the current value.
			/// @return the JSON data as a ParseState
			[[nodiscard]] constexpr ParseState get_state( ) const {
				auto parse_state = m_parse_state;
				auto result = json_details::skip_value( parse_state );
				if( is_string( ) ) {
					--result.first;
					++result.last;
				}
				return result;
			}

			/// @brief Construct a string range of the current value.  Strings start
			/// inside the quotes
			/// @return the JSON data as a std::string_view
			[[nodiscard]] constexpr std::string_view get_string_view( ) const {
				auto parse_state = m_parse_state;
				auto result = json_details::skip_value( parse_state );
				return { std::data( result ), std::size( result ) };
			}

			/// @brief Construct a string range of the current value.  Strings start
			/// inside the quotes
			/// @return the JSON data as a std::string
			template<typename Alloc = std::allocator<char>,
			         typename Traits = std::char_traits<char>>
			[[nodiscard]] std::basic_string<char, Traits, Alloc>
			get_string( Alloc const &alloc = Alloc( ) ) const {
				auto parse_state = m_parse_state;
				auto result = json_details::skip_value( parse_state );
				return { std::data( result ), std::size( result ), alloc };
			}

			/// @brief Is the JSON value a null literal
			/// @return true if the value is a null literal
			[[nodiscard]] constexpr bool is_null( ) const noexcept {
				return type( ) == JsonBaseParseTypes::Null;
			}

			/// @brief Is the JSON value a class
			/// @return true if the value is a class
			[[nodiscard]] constexpr bool is_class( ) const noexcept {
				return type( ) == JsonBaseParseTypes::Class;
			}

			/// @brief Is the JSON value a array
			/// @return true if the value is a array
			[[nodiscard]] constexpr bool is_array( ) const noexcept {
				return type( ) == JsonBaseParseTypes::Array;
			}

			/// @brief Is the JSON value a number literal
			/// @return true if the value is a number literal
			[[nodiscard]] constexpr bool is_number( ) const noexcept {
				return type( ) == JsonBaseParseTypes::Number;
			}

			/// @brief Is the JSON value a string
			/// @return true if the value is a string
			[[nodiscard]] inline constexpr bool is_string( ) const noexcept {
				return type( ) == JsonBaseParseTypes::String;
			}

			/// @brief Is the JSON value a boolean
			/// @return true if the value is a boolean
			[[nodiscard]] constexpr bool is_bool( ) const noexcept {
				return type( ) == JsonBaseParseTypes::Bool;
			}

			/// @brief Is the JSON data unrecognizable. JSON members will start with
			/// one of ",[,{,0,1,2,3,4,5,6,7,8,9,-,t,f, or n
			/// @return true if the parser is unsure what the data is
			[[nodiscard]] inline constexpr bool is_unknown( ) const noexcept {
				return type( ) == JsonBaseParseTypes::None;
			}

			template<json_options_t P, typename A>
			[[nodiscard]] constexpr
			operator basic_json_value<P, A>( ) const noexcept {
				auto new_range =
				  BasicParsePolicy<P, A>( m_parse_state.first, m_parse_state.last );
				new_range.class_first = m_parse_state.class_first;
				new_range.class_last = m_parse_state.class_last;
				return basic_json_value<P, A>( DAW_MOVE( new_range ) );
			}

			[[nodiscard]] explicit constexpr operator bool( ) const noexcept {
				return type( ) != JsonBaseParseTypes::None;
			}
		};

		template<json_options_t PolicyFlags, typename Allocator>
		basic_json_value( BasicParsePolicy<PolicyFlags, Allocator> )
		  -> basic_json_value<PolicyFlags, Allocator>;

		basic_json_value( daw::string_view ) -> basic_json_value<>;

		basic_json_value( char const *first, std::size_t sz ) -> basic_json_value<>;

		basic_json_value( char const *first, char const *last )
		  -> basic_json_value<>;

		namespace json_details {
			template<typename>
			inline constexpr bool is_json_value = false;

			template<json_options_t PolicyFlags, typename Allocator>
			inline constexpr bool
			  is_json_value<basic_json_value<PolicyFlags, Allocator>> = true;

			template<json_options_t PolicyFlags, typename Allocator>
			inline constexpr bool
			  is_string_view_like_v<basic_json_value<PolicyFlags, Allocator>> = false;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
