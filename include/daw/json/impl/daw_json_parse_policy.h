// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_allocator_wrapper.h"
#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_options_impl.h"
#include "daw_json_parse_policy_cpp_comments.h"
#include "daw_json_parse_policy_hash_comments.h"
#include "daw_json_parse_policy_no_comments.h"
#include "daw_json_parse_policy_policy_details.h"
#include "daw_json_string_util.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_likely.h>
#include <daw/daw_traits.h>

#include <cassert>
#include <ciso646>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// @brief Handles the bounds and policy items for parsing execution and
		/// comments.
		/// @tparam PolicyFlags set via parse_options method to change compile time
		/// parser options
		/// @tparam Allocator An optional Allocator to allow for passing to objects
		/// created while parsing if they support the Allocator protocol of either
		/// the Allocator argument being last or with a first argument of
		/// std::allocator_arg_t followed by the allocator.`Thing( args..., alloc )`
		/// or `Thing( std::allocator_arg, alloc, args... )`
		///
		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		struct BasicParsePolicy : json_details::AllocatorWrapper<Allocator> {

			static constexpr bool is_default_parse_policy =
			  PolicyFlags == json_details::default_policy_flag and
			  std::is_same_v<Allocator, json_details::NoAllocator>;

			static DAW_CONSTEVAL json_options_t policy_flags( ) {
				return PolicyFlags;
			}

			/***
			 * Allow temporarily setting a sentinel in the buffer to reduce range
			 * checking costs
			 */
			static DAW_CONSTEVAL bool allow_temporarily_mutating_buffer( ) {
				return json_details::get_bits_for<options::TemporarilyMutateBuffer>(
				         PolicyFlags ) == options::TemporarilyMutateBuffer::yes;
			}

			using CharT = std::conditional_t<allow_temporarily_mutating_buffer( ),
			                                 char, char const>;
			using iterator = CharT *;

			/***
			 * see options::CheckedParseMode
			 */
			static constexpr bool is_unchecked_input =
			  json_details::get_bits_for<options::CheckedParseMode>( PolicyFlags ) ==
			  options::CheckedParseMode::no;

			/***
			 * See options::ExecModeTypes
			 */
			using exec_tag_t =
			  switch_t<json_details::get_bits_for<options::ExecModeTypes,
			                                      std::size_t>( PolicyFlags ),
			           constexpr_exec_tag, runtime_exec_tag, simd_exec_tag>;

			static constexpr exec_tag_t exec_tag = exec_tag_t{ };

			/***
			 * see options::AllowEscapedNames
			 */
			static DAW_CONSTEVAL bool allow_escaped_names( ) {
				return json_details::get_bits_for<options::AllowEscapedNames>(
				         PolicyFlags ) == options::AllowEscapedNames::yes;
			}

			/***
			 * see options::ForceFullNameCheck
			 */
			static DAW_CONSTEVAL bool force_name_equal_check( ) {
				return json_details::get_bits_for<options::ForceFullNameCheck>(
				         PolicyFlags ) == options::ForceFullNameCheck::yes;
			}

			/***
			 * see options::ZeroTerminatedString
			 */
			static DAW_CONSTEVAL bool is_zero_terminated_string( ) {
				return json_details::get_bits_for<options::ZeroTerminatedString>(
				         PolicyFlags ) == options::ZeroTerminatedString::yes;
			}

			/***
			 * See options::IEEE754Precise
			 */
			static DAW_CONSTEVAL bool precise_ieee754( ) {
				return json_details::get_bits_for<options::IEEE754Precise>(
				         PolicyFlags ) == options::IEEE754Precise::yes;
			}

			/***
			 * See options::MinifiedDocument
			 */
			static DAW_CONSTEVAL bool minified_document( ) {
				return json_details::get_bits_for<options::MinifiedDocument>(
				         PolicyFlags ) == options::MinifiedDocument::yes;
			}

			/***
			 * See options::ExcludeSpecialEscapes
			 */
			static DAW_CONSTEVAL bool exclude_special_escapes( ) {
				return json_details::get_bits_for<options::ExcludeSpecialEscapes>(
				         PolicyFlags ) == options::ExcludeSpecialEscapes::yes;
			}

			/// @brief Allow numbers with leading zeros and pluses when parsing
			static DAW_CONSTEVAL bool allow_leading_zero_plus( ) {
				return true;
			}

			static constexpr bool use_exact_mappings_by_default =
			  json_details::get_bits_for<options::UseExactMappingsByDefault>(
			    PolicyFlags ) == options::UseExactMappingsByDefault::yes;

			static constexpr bool must_verify_end_of_data_is_valid =
			  json_details::get_bits_for<options::MustVerifyEndOfDataIsValid>(
			    PolicyFlags ) == options::MustVerifyEndOfDataIsValid::yes;

			static constexpr bool expect_long_strings =
			  json_details::get_bits_for<options::ExpectLongNames>( PolicyFlags ) ==
			  options::ExpectLongNames::yes;

			using CommentPolicy =
			  switch_t<json_details::get_bits_for<options::PolicyCommentTypes,
			                                      std::size_t>( PolicyFlags ),
			           NoCommentSkippingPolicy, CppCommentSkippingPolicy,
			           HashCommentSkippingPolicy>;

			iterator first{ };
			iterator last{ };
			iterator class_first{ };
			iterator class_last{ };
			std::size_t counter = 0;

			template<auto... PolicyOptions>
			using SetPolicyOptions = BasicParsePolicy<
			  json_details::set_bits( PolicyFlags, PolicyOptions... ), Allocator>;

			inline constexpr BasicParsePolicy( ) = default;

			inline constexpr BasicParsePolicy( iterator f, iterator l )
			  : first( f )
			  , last( l )
			  , class_first( f )
			  , class_last( l ) {}

			inline constexpr BasicParsePolicy( iterator f, iterator l,
			                                   Allocator &alloc )
			  : json_details::AllocatorWrapper<Allocator>( alloc )
			  , first( f )
			  , last( l )
			  , class_first( f )
			  , class_last( l ) {}

			inline constexpr BasicParsePolicy( iterator f, iterator l, iterator cf,
			                                   iterator cl )
			  : first( f )
			  , last( l )
			  , class_first( cf )
			  , class_last( cl ) {}

			inline constexpr BasicParsePolicy( iterator f, iterator l, iterator cf,
			                                   iterator cl, Allocator const &alloc )
			  : json_details::AllocatorWrapper<Allocator>( alloc )
			  , first( f )
			  , last( l )
			  , class_first( cf )
			  , class_last( cl ) {}

			[[nodiscard]] inline constexpr BasicParsePolicy
			copy( iterator f = iterator{ }, iterator l = iterator{ },
			      iterator cf = iterator{ }, iterator cl = iterator{ } ) const {
				BasicParsePolicy result = *this;
				if( f ) {
					result.first = f;
				}
				if( l ) {
					result.last = l;
				}
				if( cf ) {
					result.class_first = cf;
				}
				if( cl ) {
					result.class_last = cl;
				}
				return result;
			}

			constexpr decltype( auto ) get_allocator( ) const {
				return json_details::AllocatorWrapper<Allocator>::get_allocator( );
			}

			template<typename Alloc>
			using with_allocator_type = BasicParsePolicy<PolicyFlags, Alloc>;

			template<typename Alloc>
			[[nodiscard]] static inline constexpr with_allocator_type<Alloc>
			with_allocator( iterator f, iterator l, iterator cf, iterator cl,
			                Alloc const &alloc ) {
				return with_allocator_type<Alloc>{ f, l, cf, cl, alloc };
			}

			[[nodiscard]] static inline constexpr BasicParsePolicy
			with_allocator( iterator f, iterator l, iterator cf, iterator cl,
			                json_details::NoAllocator const & ) {
				return BasicParsePolicy( f, l, cf, cl );
			}

			template<typename Alloc>
			[[nodiscard]] constexpr auto
			with_allocator( BasicParsePolicy<PolicyFlags, Alloc> p ) const {
				if constexpr( std::is_same_v<Alloc, json_details::NoAllocator> ) {
					return *this;
				} else {
					auto result = with_allocator( first, last, class_first, class_last,
					                              p.get_allocator( ) );
					result.counter = p.counter;
					return result;
				}
			}

			template<typename Alloc>
			[[nodiscard]] inline constexpr with_allocator_type<Alloc>
			with_allocator( Alloc const &alloc ) const {
				auto result =
				  with_allocator( first, last, class_first, class_last, alloc );
				result.counter = counter;
				return result;
			}

			[[nodiscard]] inline constexpr auto
			with_allocator( json_details::NoAllocator const & ) const {
				return *this;
			}

			using without_allocator_type =
			  BasicParsePolicy<PolicyFlags, json_details::NoAllocator>;

			template<typename Alloc>
			[[nodiscard]] static inline constexpr with_allocator_type<Alloc>
			with_allocator( iterator f, iterator l, Alloc const &alloc ) {
				return { f, l, f, l, alloc };
			}

			template<typename Alloc>
			[[nodiscard]] static inline constexpr BasicParsePolicy
			with_allocator( iterator f, iterator l,
			                json_details::NoAllocator const & ) {
				return { f, l, f, l };
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr iterator
			data( ) const {
				return first;
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr iterator
			data_end( ) const {
				return last;
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr iterator
			begin( ) const {
				return first;
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr iterator
			end( ) const {
				return last;
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool empty( ) const {
				if( not first ) {
					return true;
				}
				if constexpr( is_zero_terminated_string( ) ) {
					return first >= last or *first == '\0';
				} else {
					return first >= last;
				}
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr bool
			has_more( ) const {
				return first < last;
			}

			template<std::size_t N>
			inline constexpr bool starts_with( char const ( &rhs )[N] ) const {
				static_assert( N > 0 );
				if( size( ) < ( N - 1 ) ) {
					return false;
				}
				bool result = true;
				for( std::size_t n = 0; n < ( N - 1 ); ++n ) {
					result = result & ( first[n] == rhs[n] );
				}
				return result;
			}

			template<char c>
			DAW_ATTRIB_FLATINLINE inline constexpr void move_to_next_of_unchecked( ) {
				first =
				  json_details::memchr_unchecked<c, exec_tag_t, expect_long_strings>(
				    first, last );
			}

			template<char c>
			DAW_ATTRIB_FLATINLINE inline constexpr void move_to_next_of_checked( ) {
				first =
				  json_details::memchr_checked<c, exec_tag_t, expect_long_strings>(
				    first, last );
			}

			template<char c>
			DAW_ATTRIB_FLATINLINE inline constexpr void move_to_next_of( ) {
				if( is_unchecked_input ) {
					move_to_next_of_unchecked<c>( );
				} else {
					move_to_next_of_checked<c>( );
				}
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr char front( ) const {
				return *first;
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr char front_checked( ) const {
				daw_json_ensure( first < last, ErrorReason::UnexpectedEndOfData,
				                 *this );
				return *first;
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::size_t size( ) const {
				assert( last >= first );
				return static_cast<std::size_t>( last - first );
			}

			[[nodiscard]] inline constexpr bool is_null( ) const {
				return first == nullptr;
			}

			DAW_ATTRIB_INLINE constexpr void remove_prefix( ) {
				++first;
			}

			DAW_ATTRIB_INLINE constexpr void remove_prefix( std::size_t n ) {
				first += static_cast<std::ptrdiff_t>( n );
			}

			inline constexpr void set_class_position( ) {
				class_first = first;
				class_last = last;
			}

			struct class_pos_t {
				CharT *f;
				CharT *l;
			};

			inline constexpr void set_class_position( class_pos_t new_pos ) {
				class_first = new_pos.f;
				class_last = new_pos.l;
			}

			[[nodiscard]] inline constexpr class_pos_t get_class_position( ) const {
				return { class_first, class_last };
			}

			inline constexpr void trim_left_checked( ) {
				return CommentPolicy::trim_left_checked( *this );
			}

			inline constexpr void trim_left_unchecked( ) {
				return CommentPolicy::trim_left_unchecked( *this );
			}

			[[nodiscard]] inline constexpr bool is_literal_end( ) const {
				return CommentPolicy::is_literal_end( *first );
			}

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
			is_space_unchecked( ) const {
				return ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
				         1U ) <= 0x1FU;
			}

			[[nodiscard]] inline constexpr bool is_opening_bracket_checked( ) const {
				return DAW_LIKELY( first < last ) and *first == '[';
			}

			[[nodiscard]] inline constexpr bool is_opening_brace_checked( ) const {
				return DAW_LIKELY( first < last ) and *first == '{';
			}

			[[nodiscard]] inline constexpr bool is_closing_brace_checked( ) const {
				return DAW_LIKELY( first < last ) and *first == '}';
			}

			[[nodiscard]] inline constexpr bool is_quotes_checked( ) const {
				return DAW_LIKELY( first < last ) and *first == '"';
			}

			DAW_ATTRIB_INLINE constexpr void trim_left( ) {
				if constexpr( is_unchecked_input ) {
					trim_left_unchecked( );
				} else {
					trim_left_checked( );
				}
			}

			inline constexpr void move_next_member_or_end_unchecked( ) {
				trim_left_unchecked( );
				if( *first == ',' ) {
					++first;
					trim_left_unchecked( );
				}
			}

			DAW_ATTRIB_FLATINLINE inline constexpr void
			move_next_member_or_end_checked( ) {
				trim_left_checked( );
				if constexpr( is_zero_terminated_string( ) ) {
					if( *first == ',' ) {
						++first;
						trim_left( );
					}
				} else {
					if( DAW_LIKELY( first < last ) and *first == ',' ) {
						++first;
						trim_left( );
					}
				}
			}

			DAW_ATTRIB_FLATINLINE inline constexpr void move_next_member_or_end( ) {
				if constexpr( is_unchecked_input ) {
					move_next_member_or_end_unchecked( );
				} else {
					move_next_member_or_end_checked( );
				}
			}

			DAW_ATTRIB_FLATINLINE inline constexpr void move_next_member( ) {
				if constexpr( is_unchecked_input ) {
					CommentPolicy::move_next_member_unchecked( *this );
				} else {
					// We have no guarantee that all members are available
					move_next_member_or_end_checked( );
				}
			}

			inline constexpr void move_to_next_class_member( ) {
				CommentPolicy::template move_to_next_of<'"', '}'>( *this );
			}

			[[nodiscard]] inline constexpr bool is_at_next_class_member( ) const {
				return parse_policy_details::in<'"', '}'>( *first );
			}

			[[nodiscard]] inline constexpr bool is_at_next_array_element( ) const {
				return parse_policy_details::in<',', ']'>( *first );
			}

			[[nodiscard]] inline constexpr bool is_at_token_after_value( ) const {
				return parse_policy_details::in<',', '}', ']'>( *first );
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr BasicParsePolicy
			skip_bracketed_item_checked( ) {
				return CommentPolicy::template skip_bracketed_item_checked<
				  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr BasicParsePolicy
			skip_bracketed_item_unchecked( ) {
				return CommentPolicy::template skip_bracketed_item_unchecked<
				  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
			}

			[[nodiscard]] inline constexpr BasicParsePolicy skip_class( ) {
				if constexpr( is_unchecked_input ) {
					return skip_bracketed_item_unchecked<'{', '}', '[', ']'>( );
				} else {
					return skip_bracketed_item_checked<'{', '}', '[', ']'>( );
				}
			}

			[[nodiscard]] inline constexpr BasicParsePolicy skip_array( ) {
				if constexpr( is_unchecked_input ) {
					return skip_bracketed_item_unchecked<'[', ']', '{', '}'>( );
				} else {
					return skip_bracketed_item_checked<'[', ']', '{', '}'>( );
				}
			}
		};

		BasicParsePolicy( ) -> BasicParsePolicy<>;

		BasicParsePolicy( char const *, char const * ) -> BasicParsePolicy<>;

		template<typename Allocator>
		BasicParsePolicy( char const *, char const *, Allocator const & )
		  -> BasicParsePolicy<json_details::default_policy_flag, Allocator>;

		BasicParsePolicy( char const *, char const *, char const *, char const * )
		  -> BasicParsePolicy<>;

		template<typename Allocator>
		BasicParsePolicy( char const *, char const *, char const *, char const *,
		                  Allocator const & )
		  -> BasicParsePolicy<json_details::default_policy_flag, Allocator>;

		struct DefaultParsePolicy
		  : BasicParsePolicy<json_details::default_policy_flag,
		                     json_details::NoAllocator> {
			using BasicParsePolicy::BasicParsePolicy;

			constexpr DefaultParsePolicy( BasicParsePolicy const &other ) noexcept
			  : BasicParsePolicy( other ) {}
			constexpr DefaultParsePolicy( BasicParsePolicy &&other ) noexcept
			  : BasicParsePolicy( DAW_MOVE( other ) ) {}
		};

		template<json_options_t PolicyFlags = json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		using GetParsePolicy =
		  std::conditional_t<(PolicyFlags == json_details::default_policy_flag and
		                      std::is_same_v<Allocator, json_details::NoAllocator>),
		                     DefaultParsePolicy,
		                     BasicParsePolicy<PolicyFlags, Allocator>>;
		template<typename ParsePolicy>
		using TryDefaultParsePolicy =
		  std::conditional_t<ParsePolicy::is_default_parse_policy,
		                     DefaultParsePolicy, ParsePolicy>;

		namespace options {
			/***
			 * @brief Specify parse policy flags in to_json calls.  See cookbook item
			 * parse_options.md
			 */
			template<auto... PolicyFlags>
			struct parse_flags_t {
				static_assert(
				  ( json_details::is_option_flag<decltype( PolicyFlags )> and ... ),
				  "Only registered policy types are allowed" );
				static constexpr json_options_t value = parse_options( PolicyFlags... );
			};
			template<>
			struct parse_flags_t<> {
				static constexpr json_options_t value =
				  json_details::default_policy_flag;
			};

			namespace details {
				template<typename... Ts>
				std::false_type is_policy_flag( Ts... );

				template<auto... PolicyFlags>
				std::true_type is_policy_flag( parse_flags_t<PolicyFlags...> );

				template<auto... PolicyFlags>
				DAW_CONSTEVAL auto make_parse_flags( ) {
					if constexpr( decltype( details::is_policy_flag(
					                PolicyFlags... ) )::value ) {
						static_assert( sizeof...( PolicyFlags ) == 1 );
						// We know there is only one but need to unpack
						return ( PolicyFlags, ... );
					} else {
						return parse_flags_t<PolicyFlags...>{ };
					}
				}
			} // namespace details
			///
			/// @brief Specify parse policy flags in to_json calls.  See cookbook item
			/// parse_options.md
			///
			template<auto... PolicyFlags>
			inline constexpr auto parse_flags =
			  details::make_parse_flags<PolicyFlags...>( );
		} // namespace options

#define DAW_JSON_CONFORMANCE_FLAGS                       \
	daw::json::options::AllowEscapedNames::yes,            \
	  daw::json::options::MustVerifyEndOfDataIsValid::yes, \
	  daw::json::options::IEEE754Precise::yes,             \
	  daw::json::options::ExcludeSpecialEscapes::yes

		inline constexpr auto ConformancePolicy =
		  options::parse_flags<DAW_JSON_CONFORMANCE_FLAGS>;

	} // namespace DAW_JSON_VER
} // namespace daw::json
