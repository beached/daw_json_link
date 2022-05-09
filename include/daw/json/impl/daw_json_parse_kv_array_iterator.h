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

#include <daw/daw_move.h>

#include <ciso646>

namespace daw::json::json_details {
	template<typename Range, bool>
	struct json_parse_kv_array_iterator_base {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		static constexpr bool has_counter = false;
		Range *rng = nullptr;
	};

	template<typename Range>
	struct json_parse_kv_array_iterator_base<Range, true> {
		// We have to lie so that std::distance uses O(1) instead of O(N)
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		static constexpr bool has_counter = true;
		Range *rng = nullptr;

		constexpr difference_type
		operator-( json_parse_kv_array_iterator_base const &rhs ) const {
			if( rhs.rng ) {
				return static_cast<difference_type>( rhs.rng->counter );
			}
			return 0;
		}
	};

	template<typename JsonMember, typename Range, bool KnownBounds>
	struct json_parse_kv_array_iterator
	  : json_parse_kv_array_iterator_base<Range, can_random_v<KnownBounds>> {

		using base =
		  json_parse_kv_array_iterator_base<Range, can_random_v<KnownBounds>>;
		using iterator_category = typename base::iterator_category;
		using json_key_t = typename JsonMember::json_key_t;
		using json_element_t = typename JsonMember::json_value_t;
		using value_type = std::pair<typename json_key_t::parse_to_t const,
		                             typename json_element_t::parse_to_t>;
		using reference = value_type;
		using pointer = arrow_proxy<value_type>;
		using iterator_range_t = Range;
		using difference_type = typename base::difference_type;

		using json_class_type = typename JsonMember::json_class_t;
		inline constexpr json_parse_kv_array_iterator( ) = default;

		inline constexpr explicit json_parse_kv_array_iterator(
		  iterator_range_t &r )
		  : base{ &r } {
			if( base::rng->front( ) == ']' ) {
				if constexpr( not KnownBounds ) {
					// Cleanup at end of value
					base::rng->remove_prefix( );
					base::rng->trim_left_checked( );
					// Ensure we are equal to default
				}
				base::rng = nullptr;
			}
		}

	private:
		static constexpr value_type
		get_pair( typename json_class_type::parse_to_t &&v ) {
			return value_type( std::get<0>( daw::move( v.members ) ),
			                   std::get<1>( daw::move( v.members ) ) );
		}

	public:
		constexpr value_type operator*( ) {
			daw_json_assert_weak( base::rng and base::rng->has_more( ),
			                      ErrorReason::UnexpectedEndOfData, *base::rng );

			if constexpr( KnownBounds ) {
				if constexpr( is_guaranteed_rvo_v<Range> ) {
					struct cleanup_t {
						Range *ptr;
						std::size_t counter;
						CPP20CONSTEXPR inline ~cleanup_t( ) noexcept(
						  not use_daw_json_exceptions_v ) {
#ifdef HAS_CPP20CONSTEXPR
							if( std::is_constant_evaluated( ) ) {
#endif
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
								if( std::uncaught_exceptions( ) == 0 ) {
#endif
									ptr->counter = counter;
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
								}
#endif
#ifdef HAS_CPP20CONSTEXPR
							} else {
								ptr->counter = counter;
							}
#endif
						}
					} const run_after_parse{ base::rng, base::rng->counter };
					(void)run_after_parse;
					return get_pair( parse_value<json_class_type>(
					  ParseTag<JsonParseTypes::Class>{ }, *base::rng ) );
				} else {
					auto const cnt = base::rng->counter;
					auto result = get_pair( parse_value<json_class_type>(
					  ParseTag<JsonParseTypes::Class>{ }, *base::rng ) );
					base::rng->counter = cnt;
					return result;
				}
			} else {
				return get_pair( parse_value<json_class_type>(
				  ParseTag<JsonParseTypes::Class>{ }, *base::rng ) );
			}
		}

		inline constexpr json_parse_kv_array_iterator &operator++( ) {
			daw_json_assert_weak( base::rng, ErrorReason::UnexpectedEndOfData );
			base::rng->clean_tail( );
			daw_json_assert_weak( base::rng->has_more( ),
			                      ErrorReason::UnexpectedEndOfData );
			if( base::rng->front( ) == ']' ) {
#ifndef NDEBUG
				if constexpr( base::has_counter ) {
					daw_json_assert_weak( base::rng->counter == 0,
					                      ErrorReason::UnexpectedEndOfData );
				}
#endif
				if constexpr( not KnownBounds ) {
					// Cleanup at end of value
					base::rng->remove_prefix( );
					base::rng->trim_left_checked( );
					// Ensure we are equal to default
				}
				base::rng = nullptr;
			}
#ifndef NDEBUG
			if constexpr( base::has_counter ) {
				if( base::rng ) {
					daw_json_assert_weak( base::rng->counter > 0,
					                      ErrorReason::UnexpectedEndOfData );
					base::rng->counter--;
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
			return base::rng == rhs.base::rng;
		}

		inline constexpr bool
		operator!=( json_parse_kv_array_iterator const &rhs ) const {
			return base::rng != rhs.base::rng;
		}
	};
} // namespace daw::json::json_details
