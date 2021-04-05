// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_arrow_proxy.h"
#include "daw_json_assert.h"
#include "daw_json_parse_value_fwd.h"
#include "namespace.h"

#include <daw/daw_move.h>

#include <ciso646>

namespace DAW_JSON_NS::json_details {
	template<typename ParseState, bool>
	struct json_parse_kv_array_iterator_base {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		static constexpr bool has_counter = false;
		ParseState *parse_state = nullptr;
	};

	template<typename ParseState>
	struct json_parse_kv_array_iterator_base<ParseState, true> {
		// We have to lie so that std::distance uses O(1) instead of O(N)
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		static constexpr bool has_counter = true;
		ParseState *parse_state = nullptr;

		constexpr difference_type
		operator-( json_parse_kv_array_iterator_base const &rhs ) const {
			if( rhs.parse_state ) {
				return static_cast<difference_type>( rhs.parse_state->counter );
			}
			return 0;
		}
	};

	template<typename JsonMember, typename ParseState, bool KnownBounds>
	struct json_parse_kv_array_iterator
	  : json_parse_kv_array_iterator_base<ParseState, can_random_v<KnownBounds>> {

		using base =
		  json_parse_kv_array_iterator_base<ParseState, can_random_v<KnownBounds>>;
		using iterator_category = typename base::iterator_category;
		using json_key_t = typename JsonMember::json_key_t;
		using json_element_t = typename JsonMember::json_value_t;
		using value_type = std::pair<typename json_key_t::parse_to_t const,
		                             typename json_element_t::parse_to_t>;
		using reference = value_type;
		using pointer = arrow_proxy<value_type>;
		using iterator_range_t = ParseState;
		using difference_type = typename base::difference_type;

		using json_class_type = typename JsonMember::json_class_t;
		inline constexpr json_parse_kv_array_iterator( ) = default;

		inline constexpr explicit json_parse_kv_array_iterator(
		  iterator_range_t &r )
		  : base{ &r } {
			if( base::parse_state->front( ) == ']' ) {
				if constexpr( not KnownBounds ) {
					// Cleanup at end of value
					base::parse_state->remove_prefix( );
					base::parse_state->trim_left_checked( );
					// Ensure we are equal to default
				}
				base::parse_state = nullptr;
			}
		}

	private:
		static constexpr value_type
		get_pair( typename json_class_type::parse_to_t &&v ) {
			return value_type( std::get<0>( DAW_MOVE( v.members ) ),
			                   std::get<1>( DAW_MOVE( v.members ) ) );
		}

	public:
		constexpr value_type operator*( ) {
			daw_json_assert_weak(
			  base::parse_state and base::parse_state->has_more( ),
			  ErrorReason::UnexpectedEndOfData, *base::parse_state );

			if constexpr( KnownBounds ) {
				if constexpr( is_guaranteed_rvo_v<ParseState> ) {
					struct cleanup_t {
						ParseState *ptr;
						std::size_t counter;
						CPP20CONSTEXPR inline ~cleanup_t( ) noexcept( false ) {
#ifdef HAS_CPP20CONSTEXPR
							if( std::is_constant_evaluated( ) ) {
#endif
								if( std::uncaught_exceptions( ) == 0 ) {
									ptr->counter = counter;
								}
#ifdef HAS_CPP20CONSTEXPR
							} else {
								ptr->counter = counter;
							}
#endif
						}
					} const run_after_parse{ base::parse_state,
					                         base::parse_state->counter };
					(void)run_after_parse;
					return get_pair( parse_value<json_class_type>(
					  ParseTag<JsonParseTypes::Class>{ }, *base::parse_state ) );
				} else {
					auto const cnt = base::parse_state->counter;
					auto result = get_pair( parse_value<json_class_type>(
					  ParseTag<JsonParseTypes::Class>{ }, *base::parse_state ) );
					base::parse_state->counter = cnt;
					return result;
				}
			} else {
				return get_pair( parse_value<json_class_type>(
				  ParseTag<JsonParseTypes::Class>{ }, *base::parse_state ) );
			}
		}

		inline constexpr json_parse_kv_array_iterator &operator++( ) {
			daw_json_assert_weak( base::parse_state,
			                      ErrorReason::UnexpectedEndOfData );
			base::parse_state->clean_tail( );
			daw_json_assert_weak( base::parse_state->has_more( ),
			                      ErrorReason::UnexpectedEndOfData );
			if( base::parse_state->front( ) == ']' ) {
#ifndef NDEBUG
				if constexpr( base::has_counter ) {
					daw_json_assert_weak( base::parse_state->counter == 0,
					                      ErrorReason::UnexpectedEndOfData );
				}
#endif
				if constexpr( not KnownBounds ) {
					// Cleanup at end of value
					base::parse_state->remove_prefix( );
					base::parse_state->trim_left_checked( );
					// Ensure we are equal to default
				}
				base::parse_state = nullptr;
			}
#ifndef NDEBUG
			if constexpr( base::has_counter ) {
				if( base::parse_state ) {
					daw_json_assert_weak( base::parse_state->counter > 0,
					                      ErrorReason::UnexpectedEndOfData );
					base::parse_state->counter--;
				}
			}
#endif
			return *this;
		}

		inline constexpr json_parse_kv_array_iterator operator++( int ) {
			auto result = *this;
			(void)this->operator++( );
			return result;
		}

		inline constexpr bool
		operator==( json_parse_kv_array_iterator const &rhs ) const {
			return base::parse_state == rhs.base::parse_state;
		}

		inline constexpr bool
		operator!=( json_parse_kv_array_iterator const &rhs ) const {
			return base::parse_state != rhs.base::parse_state;
		}
	};
} // namespace DAW_JSON_NS::json_details
