// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_arrow_proxy.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_parse_value_fwd.h"

#include <daw/daw_attributes.h>

#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename ParseState, bool>
			struct json_parse_array_iterator_base {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = false;

				ParseState *parse_state = nullptr;
			};

			template<typename ParseState>
			struct json_parse_array_iterator_base<ParseState, true> {
#if defined( DAW_JSON_HAS_CPP23_RANGE_CTOR )
				using iterator_category = std::input_iterator_tag;
#else
				using iterator_category = std::random_access_iterator_tag;
#endif
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = true;

				ParseState *parse_state = nullptr;
				difference_type counter = 0;

				explicit json_parse_array_iterator_base( ) = default;

				explicit constexpr json_parse_array_iterator_base(
				  daw::not_null<ParseState *> pd ) noexcept
				  : parse_state( pd )
				  , counter( static_cast<difference_type>( pd->counter ) ) {}

				constexpr difference_type
				operator-( json_parse_array_iterator_base const &rhs ) const {
					// rhs is the iterator with the parser in it.  We should know how many
					// items are in play because we already counted them in the skip_array
					// call.

					return rhs.counter;
				}
			};

			template<typename JsonMember, typename ParseState, bool KnownBounds>
			struct json_parse_array_iterator
			  : json_parse_array_iterator_base<
			      ParseState, can_be_random_iterator_v<KnownBounds>> {

				using base =
				  json_parse_array_iterator_base<ParseState,
				                                 can_be_random_iterator_v<KnownBounds>>;

				using iterator_category = typename base::iterator_category;
				using element_t = typename JsonMember::json_element_t;
				using value_type = json_result_t<element_t>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using parse_state_t = ParseState;
				using difference_type = typename base::difference_type;
				using size_type = std::size_t;

				json_parse_array_iterator( ) = default;
#if defined( DAW_JSON_USE_FULL_DEBUG_ITERATORS )
				// This code requires C++ 20 to be useful in a constant expression as it
				// requires a non-trivial destructor
				json_parse_array_iterator( json_parse_array_iterator const & ) =
				  default;
				json_parse_array_iterator &
				operator=( json_parse_array_iterator const & ) = default;
				json_parse_array_iterator( json_parse_array_iterator && ) = default;
				json_parse_array_iterator &
				operator=( json_parse_array_iterator && ) = default;
				DAW_JSON_CPP20_CX_DTOR ~json_parse_array_iterator( ) {
					if constexpr( base::has_counter ) {
						daw_json_assert_weak( base::counter == 0,
						                      ErrorReason::AttemptToAccessPastEndOfValue );
					}
				}
#endif
				/// @param size_hint expected element count, 0 when unknown.  Only used
				/// to reserve, the elements are still parsed until the closing bracket
				constexpr explicit json_parse_array_iterator( parse_state_t &r,
				                                              std::size_t size_hint = 0 )
				  : base{ &r }
				  , m_size_hint( size_hint ) {
					if( DAW_UNLIKELY( base::parse_state->front( ) == ']' ) ) {
						if constexpr( not KnownBounds ) {
							// Cleanup at end of value
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					}
				}

				[[noreturn]] DAW_ATTRIB_NOINLINE value_type operator*( ) const {
					DAW_UNLIKELY_BRANCH
					// This is hear to satisfy indirectly_readable
					daw_json_error( true, ErrorReason::UnexpectedEndOfData );
				}

				DAW_ATTRIB_INLINE constexpr value_type operator*( ) {
					daw_json_assert_weak( base::parse_state and
					                        base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );

					return parse_value<element_t, false, element_t::expected_type>(
					  *base::parse_state );
				}

				DAW_ATTRIB_INLINE constexpr json_parse_array_iterator &operator++( ) {
					daw_json_assert_weak( base::parse_state,
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					base::parse_state->trim_left( );

					daw_json_assert_weak(
					  base::parse_state->has_more( ) and
					    base::parse_state->is_at_next_array_element( ),
					  ErrorReason::UnexpectedEndOfData,
					  *base::parse_state );

					base::parse_state->move_next_member_or_end( );
					daw_json_assert_weak( base::parse_state->has_more( ),
					                      ErrorReason::UnexpectedEndOfData,
					                      *base::parse_state );
					if( base::parse_state->front( ) == ']' ) {
#if not defined( NDEBUG )
						if constexpr( base::has_counter ) {
							daw_json_assert_weak( base::counter == 0,
							                      ErrorReason::AttemptToAccessPastEndOfValue,
							                      *base::parse_state );
						}
#endif
						if constexpr( not KnownBounds ) {
							// Cleanup at end of value
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					} else {
#if not defined( NDEBUG )
						if constexpr( base::has_counter ) {
							daw_json_assert_weak( base::counter > 0,
							                      ErrorReason::AttemptToAccessPastEndOfValue,
							                      *base::parse_state );
							--base::counter;
						}
#endif
					}
					return *this;
				}

				DAW_ATTRIB_INLINE constexpr void operator++( int ) {
					(void)operator++( );
				}

				friend inline constexpr bool
				operator==( json_parse_array_iterator const &lhs,
				            json_parse_array_iterator const &rhs ) {
					return lhs.parse_state == rhs.parse_state;
				}

				friend inline constexpr bool
				operator!=( json_parse_array_iterator const &lhs,
				            json_parse_array_iterator const &rhs ) {
					return not( lhs == rhs );
				}

				/// @brief Elements to reserve: the count when known, otherwise the
				/// policy's ArrayReserve amount
				[[nodiscard]] constexpr std::size_t size_hint( ) const {
					if( m_size_hint != 0 ) {
						return m_size_hint;
					}
					return ParseState::array_reserve_amount;
				}

				constexpr json_parse_array_iterator &begin( ) {
					return *this;
				}

				constexpr json_parse_array_iterator end( ) const {
					return { };
				}

			private:
				std::size_t m_size_hint = 0;
			};

#if defined( DAW_JSON_HAS_CPP23_RANGE_CTOR )
			/// @brief End of an array whose element count is known from skipping
			/// it earlier.  With json_parse_array_counted_iterator it is a sized
			/// sentinel, so std::from_range allocates once
			struct json_parse_array_sized_end {};

			/// @brief Array iterator that also tracks how many elements remain.
			/// The end is still found by the parser, the count only sizes the
			/// allocation
			template<typename Iterator>
			struct json_parse_array_counted_iterator {
				using iterator_concept = std::input_iterator_tag;
				using iterator_category = std::input_iterator_tag;
				using value_type = typename Iterator::value_type;
				using reference = typename Iterator::reference;
				using difference_type = std::ptrdiff_t;

				Iterator m_iterator{ };
				difference_type m_remaining = 0;

				json_parse_array_counted_iterator( ) = default;

				explicit constexpr json_parse_array_counted_iterator(
				  Iterator it, difference_type count )
				  : m_iterator( std::move( it ) )
				  , m_remaining( count ) {}

				DAW_ATTRIB_INLINE constexpr reference operator*( ) {
					return *m_iterator;
				}

				DAW_ATTRIB_INLINE constexpr reference operator*( ) const {
					return *m_iterator;
				}

				DAW_ATTRIB_INLINE constexpr json_parse_array_counted_iterator &
				operator++( ) {
					++m_iterator;
					--m_remaining;
					return *this;
				}

				DAW_ATTRIB_INLINE constexpr void operator++( int ) {
					(void)operator++( );
				}

				friend constexpr bool
				operator==( json_parse_array_counted_iterator const &it,
				            json_parse_array_sized_end ) {
					return it.m_iterator == Iterator( );
				}

				friend constexpr difference_type
				operator-( json_parse_array_sized_end,
				           json_parse_array_counted_iterator const &it ) {
					return it.m_remaining;
				}

				friend constexpr difference_type
				operator-( json_parse_array_counted_iterator const &it,
				           json_parse_array_sized_end ) {
					return -it.m_remaining;
				}
			};
#endif
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
