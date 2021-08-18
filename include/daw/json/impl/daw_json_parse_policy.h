// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_allocator_wrapper.h"
#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_options.h"
#include "daw_json_parse_policy_cpp_comments.h"
#include "daw_json_parse_policy_hash_comments.h"
#include "daw_json_parse_policy_no_comments.h"
#include "daw_json_parse_policy_policy_details.h"

#include "version.h"
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
		/***
		 * Handles the bounds and policy items for parsing execution and comments.
		 * @tparam PolicyFlags set via parse_options method to change compile time
		 * parser options
		 * @tparam Allocator An optional Allocator to allow for passing to objects
		 * created while parsing if they support the Allocator protocol of either
		 * the Allocator argument being last or with a first argument of
		 * std::allocator_arg_t followed by the allocator.`Thing( args..., alloc )`
		 * or `Thing( std::allocator_arg, alloc, args... )`
		 */
		template<json_details::json_options_t PolicyFlags =
		           json_details::default_policy_flag,
		         typename Allocator = json_details::NoAllocator>
		struct BasicParsePolicy : json_details::AllocatorWrapper<Allocator> {
			/***
			 * Allow temporarily setting a sentinel in the buffer to reduce range
			 * checking costs
			 */
			static constexpr bool allow_temporarily_mutating_buffer =
			  json_details::get_bits_for<TemporarilyMutateBuffer>( PolicyFlags ) ==
			  TemporarilyMutateBuffer::yes;

			using CharT =
			  std::conditional_t<allow_temporarily_mutating_buffer, char, char const>;
			using iterator = CharT *;

			/***
			 * see CheckedParseMode
			 */
			static constexpr bool is_unchecked_input =
			  json_details::get_bits_for<CheckedParseMode>( PolicyFlags ) ==
			  CheckedParseMode::no;

			/***
			 * See ExecModeTypes
			 */
			using exec_tag_t =
			  switch_t<json_details::get_bits_for<ExecModeTypes, std::size_t>(
			             PolicyFlags ),
			           constexpr_exec_tag, runtime_exec_tag, simd_exec_tag>;

			static constexpr exec_tag_t exec_tag = exec_tag_t{ };

			/***
			 * see AllowEscapedNames
			 */
			static constexpr bool allow_escaped_names =
			  json_details::get_bits_for<AllowEscapedNames>( PolicyFlags ) ==
			  AllowEscapedNames::yes;

			/***
			 * see ForceFullNameCheck
			 */
			static constexpr bool force_name_equal_check =
			  json_details::get_bits_for<ForceFullNameCheck>( PolicyFlags ) ==
			  ForceFullNameCheck::yes;

			/***
			 * see ZeroTerminatedString
			 */
			static constexpr bool is_zero_terminated_string =
			  json_details::get_bits_for<ZeroTerminatedString>( PolicyFlags ) ==
			  ZeroTerminatedString::yes;

			/***
			 * See IEEE754Precise
			 */
			static constexpr bool precise_ieee754 =
			  json_details::get_bits_for<IEEE754Precise>( PolicyFlags ) ==
			  IEEE754Precise::yes;

			/***
			 * See MinifiedDocument
			 */
			static constexpr bool minified_document =
			  json_details::get_bits_for<MinifiedDocument>( PolicyFlags ) ==
			  MinifiedDocument::yes;

			/***
			 * See ExcludeSpecialEscapes
			 */
			static constexpr bool exclude_special_escapes =
			  json_details::get_bits_for<ExcludeSpecialEscapes>( PolicyFlags ) ==
			  ExcludeSpecialEscapes::yes;

			static constexpr bool allow_leading_zero_plus = true;

			using as_unchecked =
			  BasicParsePolicy<json_details::set_bits<CheckedParseMode>(
			                     PolicyFlags, CheckedParseMode::no ),
			                   Allocator>;
			using as_checked =
			  BasicParsePolicy<json_details::set_bits<CheckedParseMode>(
			                     PolicyFlags, CheckedParseMode::yes ),
			                   Allocator>;

			static constexpr bool use_exact_mappings_by_default =
			  json_details::get_bits_for<UseExactMappingsByDefault>( PolicyFlags ) ==
			  UseExactMappingsByDefault::yes;

			static constexpr bool must_verify_end_of_data_is_valid =
			  json_details::get_bits_for<MustVerifyEndOfDataIsValid>( PolicyFlags ) ==
			  MustVerifyEndOfDataIsValid::yes;

			using CommentPolicy =
			  switch_t<json_details::get_bits_for<PolicyCommentTypes, std::size_t>(
			             PolicyFlags ),
			           NoCommentSkippingPolicy, CppCommentSkippingPolicy,
			           HashCommentSkippingPolicy>;

			iterator first{ };
			iterator last{ };
			iterator class_first{ };
			iterator class_last{ };
			std::size_t counter = 0;
			using ParseState = BasicParsePolicy;

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
			                                   iterator cl, Allocator &alloc )
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

			template<typename Alloc>
			using with_allocator_type = BasicParsePolicy<PolicyFlags, Alloc>;

			template<typename Alloc>
			[[nodiscard]] static inline constexpr with_allocator_type<Alloc>
			with_allocator( iterator f, iterator l, iterator cf, iterator cl,
			                Alloc &alloc ) {
				return with_allocator_type<Alloc>{ f, l, cf, cl, alloc };
			}

			template<typename Alloc>
			[[nodiscard]] constexpr auto
			with_allocator( BasicParsePolicy<PolicyFlags, Alloc> p ) const {
				if constexpr( std::is_same<Alloc, json_details::NoAllocator>::value ) {
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
			with_allocator( Alloc &alloc ) const {
				auto result =
				  with_allocator( first, last, class_first, class_last, alloc );
				result.counter = counter;
				return result;
			}

			using without_allocator_type =
			  BasicParsePolicy<PolicyFlags, json_details::NoAllocator>;

			[[nodiscard]] static inline constexpr without_allocator_type
			without_allocator( BasicParsePolicy p ) {
				auto result = without_allocator_type( p.first, p.last, p.class_first,
				                                      p.class_last );
				result.counter = p.counter;
				return result;
			}

			[[nodiscard]] inline constexpr without_allocator_type
			without_allocator( ) const {
				auto result =
				  without_allocator_type( first, last, class_first, class_last );
				result.counter = counter;
				return result;
			}

			template<typename Alloc>
			[[nodiscard]] static inline constexpr with_allocator_type<Alloc>
			with_allocator( iterator f, iterator l, Alloc &alloc ) {
				return { f, l, f, l, alloc };
			}

			[[nodiscard]] static inline constexpr without_allocator_type
			without_allocator( iterator f, iterator l ) {
				return { f, l };
			}

			[[nodiscard]] static inline constexpr without_allocator_type
			without_allocator( iterator f, iterator l, iterator cf, iterator cl ) {
				return { f, l, cf, cl };
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
				if constexpr( is_zero_terminated_string ) {
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

				if constexpr( traits::not_same_v<ParseState::exec_tag_t,
				                                 constexpr_exec_tag> ) {
					first = reinterpret_cast<CharT *>( std::memchr(
					  first, c, static_cast<std::size_t>( class_last - first ) ) );
				} else {
					while( *first != c ) {
						++first;
					}
				}
			}

			template<char c>
			DAW_ATTRIB_FLATINLINE inline constexpr void move_to_next_of_checked( ) {

				if constexpr( traits::not_same_v<ParseState::exec_tag_t,
				                                 constexpr_exec_tag> ) {
					first = reinterpret_cast<CharT *>( std::memchr(
					  first, c, static_cast<std::size_t>( class_last - first ) ) );
					daw_json_assert( first != nullptr, json_details::missing_token( c ),
					                 *this );
				} else {
					while( DAW_LIKELY( first < last ) and *first != c ) {
						++first;
					}
				}
			}

			template<char c>
			DAW_ATTRIB_FLATINLINE inline constexpr void move_to_next_of( ) {
				if( is_unchecked_input ) {
					move_to_next_of_unchecked<c>( );
				} else {
					move_to_next_of_checked<c>( );
				}
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr char front( ) const {
				return *first;
			}

			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr char front_checked( ) const {
				daw_json_assert( first < last, ErrorReason::UnexpectedEndOfData, *this );
				return *first;
			}

			[[nodiscard]] inline constexpr std::size_t size( ) const {
				return static_cast<std::size_t>( last - first );
			}

			[[nodiscard]] inline constexpr bool is_null( ) const {
				return first == nullptr;
			}

			DAW_ATTRIB_FLATINLINE inline constexpr void remove_prefix( ) {
				++first;
			}

			DAW_ATTRIB_FLATINLINE inline constexpr void
			remove_prefix( std::size_t n ) {
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

			inline constexpr class_pos_t get_class_position( ) const {
				return { class_first, class_last };
			}

			inline constexpr void trim_left_checked( ) {
				return CommentPolicy::trim_left_checked( *this );
			}

			inline constexpr void trim_left_unchecked( ) {
				return CommentPolicy::trim_left_unchecked( *this );
			}

			inline constexpr void move_to_end_of_literal( ) {
				CharT *f = first;
				CharT *const l = last;
				if constexpr( is_unchecked_input ) {
					while( ( static_cast<unsigned char>( *f ) > 0x20U ) &
					       not CommentPolicy::is_literal_end( *f ) ) {
						++f;
					}
				} else {
					while( ( f < l ) and ( ( static_cast<unsigned char>( *f ) > 0x20 ) &
					                       not CommentPolicy::is_literal_end( *f ) ) ) {
						++f;
					}
				}
				first = f;
			}

			[[nodiscard]] inline constexpr bool is_literal_end( ) const {
				return CommentPolicy::is_literal_end( *first );
			}

			DAW_ATTRIB_FLATINLINE [[nodiscard]] inline constexpr bool
			is_space_checked( ) const {
				daw_json_assert_weak( has_more( ), ErrorReason::UnexpectedEndOfData,
				                      *this );
				return ( static_cast<unsigned>( static_cast<unsigned char>( *first ) ) -
				         1U ) <= 0x1FU;
			}

			DAW_ATTRIB_FLATINLINE [[nodiscard]] inline constexpr bool
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

			inline constexpr void trim_left( ) {
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
				if constexpr( is_zero_terminated_string ) {
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
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr ParseState
			skip_bracketed_item_checked( ) {
				return CommentPolicy::template skip_bracketed_item_checked<
				  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr ParseState
			skip_bracketed_item_unchecked( ) {
				return CommentPolicy::template skip_bracketed_item_unchecked<
				  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
			}

			[[nodiscard]] inline constexpr ParseState skip_class( ) {
				if constexpr( is_unchecked_input ) {
					return skip_bracketed_item_unchecked<'{', '}', '[', ']'>( );
				} else {
					return skip_bracketed_item_checked<'{', '}', '[', ']'>( );
				}
			}

			[[nodiscard]] inline constexpr ParseState skip_array( ) {
				if constexpr( is_unchecked_input ) {
					return skip_bracketed_item_unchecked<'[', ']', '{', '}'>( );
				} else {
					return skip_bracketed_item_checked<'[', ']', '{', '}'>( );
				}
			}
		};

		using NoCommentSkippingPolicyChecked = BasicParsePolicy<>;
		using DefaultParsePolicy = BasicParsePolicy<>;

		using NoCommentZeroSkippingPolicyChecked =
		  BasicParsePolicy<parse_options( ZeroTerminatedString::yes )>;

		using NoCommentSkippingPolicyUnchecked =
		  BasicParsePolicy<parse_options( CheckedParseMode::no )>;

		using NoCommentZeroSkippingPolicyUnchecked = BasicParsePolicy<parse_options(
		  CheckedParseMode::no, ZeroTerminatedString::yes )>;

		namespace json_details {
			template<typename>
			struct exec_mode_from_tag_t;

			template<>
			struct exec_mode_from_tag_t<constexpr_exec_tag> {
				static inline constexpr ExecModeTypes value =
				  ExecModeTypes::compile_time;
			};

			template<>
			struct exec_mode_from_tag_t<runtime_exec_tag> {
				static inline constexpr ExecModeTypes value = ExecModeTypes::runtime;
			};

			template<>
			struct exec_mode_from_tag_t<simd_exec_tag> {
				static inline constexpr ExecModeTypes value = ExecModeTypes::simd;
			};

			template<typename ExecMode>
			inline constexpr ExecModeTypes exec_mode_from_tag =
			  exec_mode_from_tag_t<ExecMode>::value;
		} // namespace json_details

		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDNoCommentSkippingPolicyChecked = BasicParsePolicy<
		  parse_options( json_details::exec_mode_from_tag<ExecTag> ), Allocator>;

		static_assert( SIMDNoCommentSkippingPolicyChecked<
		               runtime_exec_tag>::exec_tag_t::always_rvo );

		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDNoCommentSkippingPolicyUnchecked =
		  BasicParsePolicy<parse_options(
		                     CheckedParseMode::no,
		                     json_details::exec_mode_from_tag<ExecTag> ),
		                   Allocator>;

		using HashCommentSkippingPolicyChecked =
		  BasicParsePolicy<parse_options( PolicyCommentTypes::hash )>;

		using HashCommentSkippingPolicyUnchecked = BasicParsePolicy<parse_options(
		  CheckedParseMode::no, PolicyCommentTypes::hash )>;

		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDHashCommentSkippingPolicyChecked =
		  BasicParsePolicy<parse_options(
		                     PolicyCommentTypes::hash,
		                     json_details::exec_mode_from_tag<ExecTag> ),
		                   Allocator>;

		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDHashCommentSkippingPolicyUnchecked =
		  BasicParsePolicy<parse_options(
		                     CheckedParseMode::no, PolicyCommentTypes::hash,
		                     json_details::exec_mode_from_tag<ExecTag> ),
		                   Allocator>;

		using CppCommentSkippingPolicyChecked =
		  BasicParsePolicy<parse_options( PolicyCommentTypes::cpp )>;

		using CppCommentSkippingPolicyUnchecked = BasicParsePolicy<parse_options(
		  CheckedParseMode::no, PolicyCommentTypes::cpp )>;

		/***
		 * Parse using SIMD instructions if available, allow C++ comments and fully
		 * check input
		 */

		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDCppCommentSkippingPolicyChecked =
		  BasicParsePolicy<parse_options(
		                     PolicyCommentTypes::cpp,
		                     json_details::exec_mode_from_tag<ExecTag> ),
		                   Allocator>;

		/***
		 * Parse using SIMD instructions if available, allow C++ comments and do not
		 * do more than minimum checking
		 */
		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDCppCommentSkippingPolicyUnchecked =
		  BasicParsePolicy<parse_options(
		                     CheckedParseMode::no, PolicyCommentTypes::cpp,
		                     json_details::exec_mode_from_tag<ExecTag> ),
		                   Allocator>;

		using ConformancePolicy = BasicParsePolicy<parse_options(
		  AllowEscapedNames::yes, MustVerifyEndOfDataIsValid::yes,
		  IEEE754Precise::yes, ExcludeSpecialEscapes::yes )>;

		namespace json_details {
			/***
			 * We are either not in a constexpr context or we have constexpr dtors.
			 * This is generally used so that we can call cleanup code after parsing
			 * the member
			 */
			template<typename ParsePolicy>
			inline constexpr bool is_guaranteed_rvo_v =
			  ParsePolicy::exec_tag_t::always_rvo;
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
