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
#include "daw_json_parse_policy_cpp_comments.h"
#include "daw_json_parse_policy_hash_comments.h"
#include "daw_json_parse_policy_no_comments.h"
#include "daw_json_parse_policy_policy_details.h"

#include "version.h"
#include <daw/cpp_17.h>
#include <daw/daw_hide.h>
#include <daw/daw_traits.h>

#include <cassert>
#include <ciso646>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename>
			inline constexpr unsigned policy_bits_width = 0;

			template<typename>
			struct is_policy_flag : std::false_type {};

			template<typename>
			inline constexpr int default_policy_value = 0;
		} // namespace json_details

		enum class ExecModeTypes : unsigned {
			compile_time,
			runtime,
			simd
		}; // 2bits
		namespace json_details {
			template<>
			struct is_policy_flag<ExecModeTypes> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<ExecModeTypes> = 2;

			template<>
			inline constexpr auto default_policy_value<ExecModeTypes> =
			  ExecModeTypes::compile_time;
		} // namespace json_details

		enum class ZeroTerminatedString : unsigned { no, yes }; // 1bit
		namespace json_details {
			template<>
			struct is_policy_flag<ZeroTerminatedString> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<ZeroTerminatedString> = 1;

			template<>
			inline constexpr auto default_policy_value<ZeroTerminatedString> =
			  ZeroTerminatedString::no;
		} // namespace json_details

		enum class PolicyCommentTypes : unsigned { none, cpp, hash }; // 2bits
		namespace json_details {

			template<>
			struct is_policy_flag<PolicyCommentTypes> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<PolicyCommentTypes> = 2;

			template<>
			inline constexpr auto default_policy_value<PolicyCommentTypes> =
			  PolicyCommentTypes::none;
		} // namespace json_details

		enum class CheckedParseMode : unsigned { yes, no }; // 1bit
		namespace json_details {
			template<>
			struct is_policy_flag<CheckedParseMode> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<CheckedParseMode> = 1;

			template<>
			inline constexpr auto default_policy_value<CheckedParseMode> =
			  CheckedParseMode::yes;
		} // namespace json_details

		enum class AllowEscapedNames : unsigned { no, yes };
		namespace json_details {
			template<>
			struct is_policy_flag<AllowEscapedNames> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<AllowEscapedNames> = 1;

			template<>
			inline constexpr auto default_policy_value<AllowEscapedNames> =
			  AllowEscapedNames::no;
		} // namespace json_details

		enum class IEEE754Precise : unsigned { no, yes };
		namespace json_details {
			template<>
			struct is_policy_flag<IEEE754Precise> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<IEEE754Precise> = 1;

			template<>
			inline constexpr auto default_policy_value<IEEE754Precise> =
			  IEEE754Precise::no;
		} // namespace json_details

		enum class ForceFullNameCheck : unsigned { no, yes };

		namespace json_details {
			template<>
			struct is_policy_flag<ForceFullNameCheck> : std::true_type {};

			template<>
			inline constexpr unsigned policy_bits_width<ForceFullNameCheck> = 1;

			template<>
			inline constexpr auto default_policy_value<ForceFullNameCheck> =
			  ForceFullNameCheck::no;

			template<typename Policy, typename... Policies>
			struct policy_bits_start_impl {
				static constexpr auto idx =
				  traits::pack_index_of_v<Policy, Policies...>;
				static_assert( idx >= 0, "Policy is not registered" );
				using tp_policies = std::tuple<Policies...>;

				template<std::size_t Pos, int End>
				static constexpr unsigned do_step( ) {
					if constexpr( Pos >= static_cast<std::size_t>( End ) ) {
						return 0U;
					}
					return policy_bits_width<std::tuple_element_t<Pos, tp_policies>>;
				}

				template<std::size_t... Is>
				static constexpr unsigned calc( std::index_sequence<Is...> ) {
					return ( do_step<Is, idx>( ) + ... );
				}
			};

			template<typename Policy, typename... Policies>
			inline constexpr unsigned basic_policy_bits_start =
			  policy_bits_start_impl<Policy, Policies...>::template calc(
			    std::index_sequence_for<Policies...>{ } );

			template<typename Policy>
			inline constexpr unsigned policy_bits_start =
			  basic_policy_bits_start<Policy, ExecModeTypes, ZeroTerminatedString,
			                          PolicyCommentTypes, CheckedParseMode,
			                          AllowEscapedNames, IEEE754Precise,
			                          ForceFullNameCheck>;
			// struct is_policy_flag<PolicyCommentTypes> : std::true_type {};

		} // namespace json_details

		namespace json_details {
			template<typename Policy, typename... Policies>
			constexpr Policy get_policy_or( std::tuple<Policies...> const &pols ) {
				constexpr int const pack_idx =
				  daw::traits::pack_index_of_v<Policy, Policies...>;

				if constexpr( pack_idx != -1 ) {
					return std::get<static_cast<std::size_t>( pack_idx )>( pols );
				} else {
					return json_details::default_policy_value<Policy>;
				}
			}

			template<typename Policy>
			constexpr void set_bits( std::uint32_t &value, Policy e ) {
				static_assert( policy_bits_width<Policy> > 0,
				               "Only registered policy types are allowed" );
				unsigned new_bits = static_cast<unsigned>( e );
				constexpr unsigned mask = (1U << policy_bits_width<Policy>)-1U;
				new_bits &= mask;
				new_bits <<= policy_bits_start<Policy>;
				value &= ~mask;
				value |= new_bits;
			}

			template<typename Policy>
			constexpr std::uint32_t set_bits( std::uint32_t const &v, Policy e ) {
				static_assert( policy_bits_width<Policy> > 0,
				               "Only registered policy types are allowed" );
				auto value = v;
				unsigned new_bits = static_cast<unsigned>( e );
				constexpr unsigned mask = (1U << policy_bits_width<Policy>)-1U;
				new_bits &= mask;
				new_bits <<= policy_bits_start<Policy>;
				value &= ~mask;
				value |= new_bits;
				return value;
			}

			template<typename Policy, typename Result = Policy>
			constexpr Result get_bits( std::uint32_t value ) {
				static_assert( policy_bits_width<Policy> > 0,
				               "Only registered policy types are allowed" );
				constexpr unsigned mask =
				  ( 1U << (policy_bits_start<Policy> + policy_bits_width<Policy>)) - 1U;
				value &= mask;
				value >>= policy_bits_start<Policy>;
				return static_cast<Result>( Policy{ value } );
			}

			template<std::size_t Idx, typename... Ts>
			using switch_t = std::tuple_element_t<Idx, std::tuple<Ts...>>;
		} // namespace json_details

		template<typename... Policies>
		constexpr std::uint32_t parse_options( Policies... policies ) {
			static_assert( sizeof...( Policies ) <= 3,
			               "Only three supported policy flags" );
			static_assert(
			  std::conjunction_v<json_details::is_policy_flag<Policies>...>,
			  "Invalid policy flag types" );
			std::uint32_t result = 0;
			if constexpr( sizeof...( Policies ) > 0 ) {
				auto const pols = std::tuple{ policies... };
				auto const exec_mode =
				  json_details::get_policy_or<ExecModeTypes>( pols );
				auto const zero_mode =
				  json_details::get_policy_or<ZeroTerminatedString>( pols );
				auto const comment_type =
				  json_details::get_policy_or<PolicyCommentTypes>( pols );
				auto const checked_mode =
				  json_details::get_policy_or<CheckedParseMode>( pols );

				json_details::set_bits( result, exec_mode );
				json_details::set_bits( result, zero_mode );
				json_details::set_bits( result, comment_type );
				json_details::set_bits( result, checked_mode );
			}
			return result;
		}

		/***
		 * Handles the bounds and policy items for parsing execution and comments.
		 * @tparam IsUncheckedInput If true, do not perform all validity checks on
		 * input.  This implies that we can trust the source to be perfect
		 * @tparam CommentPolicy The policy that handles skipping whitespace where
		 * comments may or may not be allowed:w
		 * @tparam ExecMode A Policy type for selecting if we must be constexpr, can
		 * use C/C++ runtime only methods, or if SIMD intrinsics are allowed
		 * @tparam AllowEscapedNames Are escapes allowed in member names.  When
		 * true, the slower string parser is used
		 */
		template<std::uint32_t PolicyFlags = 0,
		         typename Allocator = json_details::NoAllocator>
		struct BasicParsePolicy : json_details::AllocatorWrapper<Allocator> {
			using iterator = char const *;
			static constexpr bool is_unchecked_input =
			  json_details::get_bits<CheckedParseMode>( PolicyFlags ) ==
			  CheckedParseMode::no;

			using exec_tag_t = json_details::switch_t<
			  json_details::get_bits<ExecModeTypes, std::size_t>( PolicyFlags ),
			  constexpr_exec_tag, runtime_exec_tag, simd_exec_tag>;

			static constexpr exec_tag_t exec_tag = exec_tag_t{ };

			static constexpr bool allow_escaped_names =
			  json_details::get_bits<AllowEscapedNames>( PolicyFlags ) ==
			  AllowEscapedNames::yes;

			static constexpr bool force_name_equal_check =
			  json_details::get_bits<ForceFullNameCheck>( PolicyFlags ) ==
			  ForceFullNameCheck::yes;

			static constexpr bool is_zero_terminated_string =
			  json_details::get_bits<ZeroTerminatedString>( PolicyFlags ) ==
			  ZeroTerminatedString::yes;

			static constexpr bool precise_ieee754 =
			  json_details::get_bits<IEEE754Precise>( PolicyFlags ) ==
			  IEEE754Precise::yes;

			using CharT = char;

			using as_unchecked =
			  BasicParsePolicy<json_details::set_bits<CheckedParseMode>(
			                     PolicyFlags, CheckedParseMode::no ),
			                   Allocator>;
			using as_checked =
			  BasicParsePolicy<json_details::set_bits<CheckedParseMode>(
			                     PolicyFlags, CheckedParseMode::yes ),
			                   Allocator>;

			using CommentPolicy = json_details::switch_t<
			  json_details::get_bits<PolicyCommentTypes, std::size_t>( PolicyFlags ),
			  NoCommentSkippingPolicy, CppCommentSkippingPolicy,
			  HashCommentSkippingPolicy>;

			iterator first{ };
			iterator last{ };
			iterator class_first{ };
			iterator class_last{ };
			std::size_t counter = 0;
			using ParseState = BasicParsePolicy;

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

			inline constexpr BasicParsePolicy
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
			static inline constexpr with_allocator_type<Alloc>
			with_allocator( iterator f, iterator l, iterator cf, iterator cl,
			                Alloc &alloc ) {
				return { f, l, cf, cl, alloc };
			}

			template<typename Alloc>
			constexpr auto
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
			inline constexpr with_allocator_type<Alloc>
			with_allocator( Alloc &alloc ) const {
				auto result =
				  with_allocator( first, last, class_first, class_last, alloc );
				result.counter = counter;
				return result;
			}

			using without_allocator_type =
			  BasicParsePolicy<PolicyFlags, json_details::NoAllocator>;

			static inline constexpr without_allocator_type
			without_allocator( BasicParsePolicy p ) {
				auto result = without_allocator_type( p.first, p.last, p.class_first,
				                                      p.class_last );
				result.counter = p.counter;
				return result;
			}

			inline constexpr without_allocator_type without_allocator( ) const {
				auto result =
				  without_allocator_type( first, last, class_first, class_last );
				result.counter = counter;
				return result;
			}

			template<typename Alloc>
			static inline constexpr with_allocator_type<Alloc>
			with_allocator( iterator f, iterator l, Alloc &alloc ) {
				return { f, l, f, l, alloc };
			}

			static inline constexpr without_allocator_type
			without_allocator( iterator f, iterator l ) {
				return { f, l };
			}

			static inline constexpr without_allocator_type
			without_allocator( iterator f, iterator l, iterator cf, iterator cl ) {
				return { f, l, cf, cl };
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator
			data( ) const {
				return first;
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator
			data_end( ) const {
				return last;
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator
			begin( ) const {
				return first;
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator
			end( ) const {
				return last;
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool empty( ) const {
				if constexpr( is_zero_terminated_string ) {
					return first >= last or *first == '\0';
				} else {
					return first >= last;
				}
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool
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
			DAW_ATTRIBUTE_FLATTEN inline constexpr void move_to_next_of_unchecked( ) {

				if constexpr( traits::not_same_v<ParseState::exec_tag_t,
				                                 constexpr_exec_tag> ) {
					first = reinterpret_cast<char const *>( std::memchr(
					  first, c, static_cast<std::size_t>( class_last - first ) ) );
				} else {
					while( *first != c ) {
						++first;
					}
				}
			}

			template<char c>
			DAW_ATTRIBUTE_FLATTEN inline constexpr void move_to_next_of_checked( ) {

				if constexpr( traits::not_same_v<ParseState::exec_tag_t,
				                                 constexpr_exec_tag> ) {
					first = reinterpret_cast<char const *>( std::memchr(
					  first, c, static_cast<std::size_t>( class_last - first ) ) );
					daw_json_assert( first != nullptr, json_details::missing_token( c ),
					                 *this );
				} else {
					while( DAW_JSON_LIKELY( first < last ) and *first != c ) {
						++first;
					}
				}
			}

			template<char c>
			DAW_ATTRIBUTE_FLATTEN inline constexpr void move_to_next_of( ) {
				if( is_unchecked_input ) {
					move_to_next_of_unchecked<c>( );
				} else {
					move_to_next_of_checked<c>( );
				}
			}

			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr char front( ) const {
				return *first;
			}

			[[nodiscard]] inline constexpr std::size_t size( ) const {
				return static_cast<std::size_t>( last - first );
			}

			[[nodiscard]] inline constexpr bool is_null( ) const {
				return first == nullptr;
			}

			DAW_ATTRIBUTE_FLATTEN inline constexpr void remove_prefix( ) {
				++first;
			}

			DAW_ATTRIBUTE_FLATTEN inline constexpr void
			remove_prefix( std::size_t n ) {
				first += static_cast<std::ptrdiff_t>( n );
			}

			inline constexpr void set_class_position( ) {
				class_first = first;
				class_last = last;
			}

			inline constexpr void trim_left_checked( ) {
				return CommentPolicy::trim_left_checked( *this );
			}

			inline constexpr void trim_left_unchecked( ) {
				return CommentPolicy::trim_left_unchecked( *this );
			}

			inline constexpr void move_to_end_of_literal( ) {
				char const *f = first;
				char const *const l = last;
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

			DAW_ATTRIBUTE_FLATTEN [[nodiscard]] inline constexpr bool
			is_space_checked( ) const {
				daw_json_assert_weak( has_more( ), ErrorReason::UnexpectedEndOfData,
				                      *this );
				return static_cast<unsigned char>( *first ) <= 0x20U;
			}

			DAW_ATTRIBUTE_FLATTEN [[nodiscard]] inline constexpr bool
			is_space_unchecked( ) const {
				return static_cast<unsigned char>( *first ) <= 0x20U;
			}

			[[nodiscard]] inline constexpr bool is_opening_bracket_checked( ) const {
				return DAW_JSON_LIKELY( first < last ) and *first == '[';
			}

			[[nodiscard]] inline constexpr bool is_opening_brace_checked( ) const {
				return DAW_JSON_LIKELY( first < last ) and *first == '{';
			}

			[[nodiscard]] inline constexpr bool is_closing_brace_checked( ) const {
				return DAW_JSON_LIKELY( first < last ) and *first == '}';
			}

			[[nodiscard]] inline constexpr bool is_quotes_checked( ) const {
				return DAW_JSON_LIKELY( first < last ) and *first == '"';
			}

			inline constexpr void trim_left( ) {
				if constexpr( is_unchecked_input ) {
					trim_left_unchecked( );
				} else {
					trim_left_checked( );
				}
			}

			inline constexpr void clean_tail_unchecked( ) {
				trim_left_unchecked( );
				if( *first == ',' ) {
					++first;
					trim_left_unchecked( );
				}
			}

			template<char EndChar = '\0'>
			inline constexpr void clean_end_of_value( bool at_first ) {
				trim_left( );
				if constexpr( is_unchecked_input ) {
					if( *first == ',' ) {
						++first;
						trim_left( );
					}
				} else {
					if( ( not at_first ) & ( first < last ) ) {
						if( *first == ',' ) {
							++first;
							trim_left( );
						} else {
							if constexpr( EndChar != '\0' ) {
								daw_json_assert( *first == EndChar,
								                 ErrorReason::ExpectedTokenNotFound, *this );
							}
						}
					}
				}
			}

			inline constexpr void clean_tail_checked( ) {
				trim_left_checked( );
				if( DAW_JSON_LIKELY( first < last ) and *first == ',' ) {
					++first;
					trim_left_checked( );
				}
			}

			DAW_ATTRIBUTE_FLATTEN inline constexpr void clean_tail( ) {
				if constexpr( is_unchecked_input ) {
					clean_tail_unchecked( );
				} else {
					clean_tail_checked( );
				}
			}

			inline constexpr void move_to_next_class_member( ) {
				CommentPolicy::template move_to_next_of<'"', '}'>( *this );
			}

			[[nodiscard]] inline constexpr bool is_at_next_class_member( ) const {
				return parse_policy_details::in<'"', '}'>( *first );
			}

			[[nodiscard]] inline constexpr bool is_at_token_after_value( ) const {
				return parse_policy_details::in<',', '}', ']'>( *first );
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr ParseState
			skip_bracketed_item_checked( ) {
				return CommentPolicy::template skip_bracketed_item_checked<
				  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
			}

			template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
			[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr ParseState
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

		using NoCommentSkippingPolicyChecked = BasicParsePolicy<parse_options( )>;

		using NoCommentZeroSkippingPolicyChecked =
		  BasicParsePolicy<parse_options( ZeroTerminatedString::yes )>;

		using NoCommentSkippingPolicyUnchecked =
		  BasicParsePolicy<parse_options( CheckedParseMode::no )>;

		using NoCommentZeroSkippingPolicyUnchecked = BasicParsePolicy<parse_options(
		  CheckedParseMode::no, ZeroTerminatedString::yes )>;

		namespace json_details {
			template<typename ExecTag>
			inline constexpr ExecModeTypes exec_mode_from_tag =
			  ExecModeTypes::compile_time;

			template<>
			inline constexpr ExecModeTypes exec_mode_from_tag<runtime_exec_tag> =
			  ExecModeTypes::runtime;

			template<>
			inline constexpr ExecModeTypes exec_mode_from_tag<simd_exec_tag> =
			  ExecModeTypes::simd;
		} // namespace json_details

		template<typename ExecTag, typename Allocator = json_details::NoAllocator>
		using SIMDNoCommentSkippingPolicyChecked = BasicParsePolicy<
		  parse_options( json_details::exec_mode_from_tag<ExecTag> ), Allocator>;

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
