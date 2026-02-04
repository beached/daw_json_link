// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#if not defined( DAW_ALLOW_SSE42 )
#error "This should not be included when SSE4.2 is not enabled"
#endif

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_arrow_proxy.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_parse_value_fwd.h"
#include "daw/json/impl/daw_not_const_ex_functions.h"

#include <daw/daw_attributes.h>
#include <daw/daw_not_null.h>

#include <emmintrin.h>
#include <nmmintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <wmmintrin.h>
#include <xmmintrin.h>
#if defined( DAW_HAS_MSVC_LIKE )
#include <intrin.h>
#endif

#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename ParseState, bool /*KnownBounds*/>
			struct json_parse_array_sse42_iterator_base {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = false;

				ParseState *parse_state = nullptr;
			};

			template<typename ParseState>
			struct json_parse_array_sse42_iterator_base<ParseState, true> {
				using iterator_category = std::input_iterator_tag;
				using difference_type = std::ptrdiff_t;
				static constexpr bool has_counter = true;

				daw::not_null<ParseState *> parse_state;
				difference_type counter = 0;

				explicit constexpr json_parse_array_sse42_iterator_base(
				  daw::not_null<ParseState *> pd ) noexcept
				  : parse_state( pd )
				  , counter( static_cast<difference_type>( pd->counter ) ) {}

				constexpr difference_type
				operator-( json_parse_array_sse42_iterator_base const &rhs ) const {
					// rhs is the iterator with the parser in it.  We should know how many
					// items are in play because we already counted them in the skip_array
					// call.

					return rhs.counter;
				}
			};

			template<typename JsonMember, typename ParseState, bool KnownBounds>
			struct json_parse_array_sse42_iterator
			  : json_parse_array_sse42_iterator_base<
			      ParseState, can_be_random_iterator_v<KnownBounds>> {

				using base = json_parse_array_sse42_iterator_base<
				  ParseState, can_be_random_iterator_v<KnownBounds>>;

				using iterator_category = typename base::iterator_category;
				using element_t = typename JsonMember::json_element_t;
				using value_type = json_result_t<element_t>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using parse_state_t = ParseState;
				using difference_type = typename base::difference_type;
				using size_type = std::size_t;

				json_parse_array_sse42_iterator( ) = default;
#if defined( DAW_JSON_USE_FULL_DEBUG_ITERATORS )
				// This code requires C++ 20 to be useful in a constant expression as it
				// requires a non-trivial destructor
				json_parse_array_sse42_iterator(
				  json_parse_array_sse42_iterator const & ) = default;

				json_parse_array_sse42_iterator &
				operator=( json_parse_array_sse42_iterator const & ) = default;

				json_parse_array_sse42_iterator( json_parse_array_sse42_iterator && ) =
				  default;

				json_parse_array_sse42_iterator &
				operator=( json_parse_array_sse42_iterator && ) = default;

				DAW_JSON_CPP20_CX_DTOR ~json_parse_array_sse42_iterator( ) {
					if constexpr( base::has_counter ) {
						daw_json_assert_weak( base::counter == 0,
						                      ErrorReason::AttemptToAccessPastEndOfValue );
					}
				}
#endif

			private:
				struct masks_t {
					std::uint32_t array_boundaries_mask = 0;
					std::uint32_t whitespace_mask = 0;
				};

				static auto get_masks( daw::not_null<char const *> ptr ) {
					static constexpr std::size_t sse_array_boundaries_sz = 2;
					static auto const sse_array_boundaries = uload16_char_data_simd(
					  ( std::array<char, 16>{ ',', ']' } ).data( ) );
					static constexpr std::size_t sse_whitespace_sz = 4;
					static auto const sse_whitespace = uload16_char_data_simd(
					  ( std::array<char, 16>{ ' ', '\t', '\n', '\r' } ).data( ) );

					auto const text_chunk = uload16_char_data_simd( ptr );
					auto const array_boundaries_mask_reg = _mm_cmpestrm(
					  sse_array_boundaries,
					  sse_array_boundaries_sz,
					  text_chunk,
					  16,
					  _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_BIT_MASK );
					auto const whitespace_mask_reg = _mm_cmpestrm(
					  sse_whitespace,
					  sse_whitespace_sz,
					  text_chunk,
					  16,
					  _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_BIT_MASK );

					return masks_t{ static_cast<std::uint32_t>(
					                  _mm_extract_epi16( array_boundaries_mask_reg, 0 ) ),
					                static_cast<std::uint32_t>(
					                  _mm_extract_epi16( whitespace_mask_reg, 0 ) ) };
				}

				masks_t m_masks{ };
				std::size_t m_first_offset = 0;

			public:
				explicit json_parse_array_sse42_iterator( parse_state_t &r )
				  : base{ &r } {
					if( DAW_UNLIKELY( base::parse_state->front( ) == ']' ) ) {
						if constexpr( not KnownBounds ) {
							// Cleanup at end of value
							base::parse_state->remove_prefix( );
							base::parse_state->trim_left_checked( );
							// Ensure we are equal to default
						}
						base::parse_state = nullptr;
					}
					// Do initial scan to build SIMD state
					if( base::parse_state->size( ) < 16 ) {
						return;
					}
					m_masks = get_mask( base::parse_state->first );
					if( m_masks.array_boundaries_mask != 0 ) {}
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

				DAW_ATTRIB_INLINE constexpr json_parse_array_sse42_iterator &
				operator++( ) {
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
				operator==( json_parse_array_sse42_iterator const &lhs,
				            json_parse_array_sse42_iterator const &rhs ) {
					return lhs.parse_state == rhs.parse_state;
				}

				friend inline constexpr bool
				operator!=( json_parse_array_sse42_iterator const &lhs,
				            json_parse_array_sse42_iterator const &rhs ) {
					return not( lhs == rhs );
				}

				constexpr json_parse_array_sse42_iterator &begin( ) {
					return *this;
				}

				constexpr json_parse_array_sse42_iterator end( ) const {
					return { };
				}
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
