// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_assert.h"
#include "daw_json_parse_common.h"
#include "daw_json_parse_policy_cpp_comments.h"
#include "daw_json_parse_policy_hash_comments.h"
#include "daw_json_parse_policy_no_comments.h"
#include "daw_json_parse_policy_policy_details.h"

#include <daw/cpp_17.h>
#include <daw/daw_hide.h>

#include <cassert>
#include <ciso646>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::json {
	namespace json_details {
		template<typename Alloc, bool /*is_empty*/>
		class AllocatorWrapperBase {
			using allocator_t = std::remove_reference_t<Alloc>;
			allocator_t *allocator_ptr;

		public:
			AllocatorWrapperBase( allocator_t &alloc ) noexcept
			  : allocator_ptr( &alloc ) {}

			allocator_t &get_allocator( ) const {
				return *allocator_ptr;
			}
		};

		template<typename Alloc>
		class AllocatorWrapperBase<Alloc, true /*is_empty*/> {
			using allocator_t = std::remove_reference_t<Alloc>;
			static constexpr allocator_t allocator{ };

		public:
			constexpr AllocatorWrapperBase( ) = default;
			constexpr AllocatorWrapperBase( allocator_t & ) noexcept {}

			allocator_t &get_allocator( ) const {
				return allocator;
			}
		};

		template<typename Alloc>
		struct AllocatorWrapper
		  : AllocatorWrapperBase<Alloc, std::is_empty_v<Alloc>> {
			using allocator_type = std::remove_reference_t<Alloc>;

			AllocatorWrapper( allocator_type &alloc ) noexcept
			  : AllocatorWrapperBase<allocator_type, std::is_empty_v<allocator_type>>(
			      alloc ) {}

			static constexpr bool has_allocator = true;

			template<typename A, typename T>
			struct allocator_type_as_rebind {
				using type =
				  typename std::allocator_traits<A>::template rebind_alloc<T>;
			};

			template<typename A, typename T>
			using has_allocator_type_as_rebind =
			  typename std::allocator_traits<A>::template rebind_traits<T>::type;

			template<typename A, typename T>
			static inline constexpr bool has_rebind_v =
			  daw::is_detected_v<has_allocator_type_as_rebind, A, T>;

			// DAW FIX
			template<typename T>
			using allocator_type_as =
			  std::conditional_t<has_rebind_v<allocator_type, T>,
			                     allocator_type_as_rebind<allocator_type, T>,
			                     allocator_type>;

			template<typename T>
			auto get_allocator_for( ) const {
				return static_cast<allocator_type_as<T>>( this->get_allocator( ) );
			}
		};

		struct NoAllocator {};
		template<>
		class AllocatorWrapper<NoAllocator> {
		public:
			constexpr AllocatorWrapper( ) noexcept = default;
			static constexpr bool has_allocator = false;

			using allocator_type = std::allocator<char>;

			template<typename T>
			using allocator_type_as = std::allocator<T>;

			template<typename T>
			auto get_allocator_for( ) const {
				return std::allocator<T>( );
			}
		};
	} // namespace json_details
	/***
	 * Handles the bounds and policy items for parsing execution and comments.
	 * @tparam IsUncheckedInput If true, do not perform all validity checks on
	 * input.  This implies that we can trust the source to be perfect
	 * @tparam CommentPolicy The policy that handles skipping whitespace where
	 * comments may or may not be allowed:w
	 * @tparam ExecMode A Policy type for selecting if we must be constexpr, can
	 * use C/C++ runtime only methods, or if SIMD intrinsics are allowed
	 * @tparam AllowEscapedNames Are escapes allowed in member names.  When true,
	 * the slower string parser is used
	 */
	template<bool IsUncheckedInput, typename CommentPolicy, typename ExecMode,
	         bool AllowEscapedNames,
	         typename Allocator = json_details::NoAllocator>
	struct BasicParsePolicy : json_details::AllocatorWrapper<Allocator> {
		using iterator = char const *;
		static constexpr bool is_unchecked_input = IsUncheckedInput;
		using exec_tag_t = ExecMode;
		static constexpr exec_tag_t exec_tag = exec_tag_t{ };
		static constexpr bool allow_escaped_names = AllowEscapedNames;
		static constexpr bool force_name_equal_check = false;
		using CharT = char;

		using as_unchecked = BasicParsePolicy<true, CommentPolicy, exec_tag_t,
		                                      allow_escaped_names, Allocator>;
		using as_checked = BasicParsePolicy<false, CommentPolicy, exec_tag_t,
		                                    allow_escaped_names, Allocator>;

		iterator first{ };
		iterator last{ };
		iterator class_first{ };
		iterator class_last{ };
		std::size_t counter = 0;
		using Range = BasicParsePolicy;

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

		inline constexpr BasicParsePolicy copy( iterator f = iterator{ },
		                                        iterator l = iterator{ },
		                                        iterator cf = iterator{ },
		                                        iterator cl = iterator{ } ) const {
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
		using with_allocator_type =
		  BasicParsePolicy<IsUncheckedInput, CommentPolicy, ExecMode,
		                   AllowEscapedNames, Alloc>;

		template<typename Alloc>
		static inline constexpr with_allocator_type<Alloc>
		with_allocator( iterator f, iterator l, iterator cf, iterator cl,
		                Alloc &alloc ) {
			return { f, l, cf, cl, alloc };
		}

		template<typename Alloc>
		constexpr auto
		with_allocator( BasicParsePolicy<IsUncheckedInput, CommentPolicy, ExecMode,
		                                 AllowEscapedNames, Alloc>
		                  p ) const {

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
		inline constexpr with_allocator_type<Alloc>
		with_allocator( Alloc &alloc ) const {
			auto result =
			  with_allocator( first, last, class_first, class_last, alloc );
			result.counter = counter;
			return result;
		}

		using without_allocator_type =
		  BasicParsePolicy<IsUncheckedInput, CommentPolicy, ExecMode,
		                   AllowEscapedNames, json_details::NoAllocator>;

		static inline constexpr without_allocator_type
		without_allocator( BasicParsePolicy p ) {
			auto result =
			  without_allocator_type( p.first, p.last, p.class_first, p.class_last );
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
		begin( ) const {
			return first;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr iterator end( ) const {
			return last;
		}

		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr bool empty( ) const {
			return first >= last;
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

			if constexpr( not std::is_same_v<Range::exec_tag_t,
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

			if constexpr( not std::is_same_v<Range::exec_tag_t,
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

		DAW_ATTRIBUTE_FLATTEN inline constexpr void remove_prefix( std::size_t n ) {
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
			if constexpr( IsUncheckedInput ) {
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

		[[nodiscard]] inline constexpr bool is_exponent_checked( ) const {
			return DAW_JSON_LIKELY( first < last ) and
			       ( ( *first == 'e' ) bitor ( *first == 'E' ) );
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
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Range
		skip_bracketed_item_checked( ) {
			return CommentPolicy::template skip_bracketed_item_checked<
			  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
		}

		template<char PrimLeft, char PrimRight, char SecLeft, char SecRight>
		[[nodiscard]] DAW_ATTRIBUTE_FLATTEN inline constexpr Range
		skip_bracketed_item_unchecked( ) {
			return CommentPolicy::template skip_bracketed_item_unchecked<
			  PrimLeft, PrimRight, SecLeft, SecRight>( *this );
		}

		[[nodiscard]] inline constexpr Range skip_class( ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'{', '}', '[', ']'>( );
			} else {
				return skip_bracketed_item_checked<'{', '}', '[', ']'>( );
			}
		}

		[[nodiscard]] inline constexpr Range skip_array( ) {
			if constexpr( is_unchecked_input ) {
				return skip_bracketed_item_unchecked<'[', ']', '{', '}'>( );
			} else {
				return skip_bracketed_item_checked<'[', ']', '{', '}'>( );
			}
		}
	}; // namespace daw::json

	using NoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, default_exec_tag, false>;

	using NoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, default_exec_tag, false>;

	template<typename ExecTag, typename Allocator = json_details::NoAllocator>
	using SIMDNoCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, NoCommentSkippingPolicy, ExecTag, false, Allocator>;

	template<typename ExecTag, typename Allocator = json_details::NoAllocator>
	using SIMDNoCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, NoCommentSkippingPolicy, ExecTag, false, Allocator>;

	using HashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, default_exec_tag, false>;

	using HashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, default_exec_tag, false>;

	template<typename ExecTag, typename Allocator = json_details::NoAllocator>
	using SIMDHashCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, HashCommentSkippingPolicy, ExecTag, false,
	                   Allocator>;

	template<typename ExecTag, typename Allocator = json_details::NoAllocator>
	using SIMDHashCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, HashCommentSkippingPolicy, ExecTag, false,
	                   Allocator>;

	using CppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, default_exec_tag, false>;

	using CppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, default_exec_tag, false>;

	/***
	 * Parse using SIMD instructions if available, allow C++ comments and fully
	 * check input
	 */
	template<typename ExecTag, typename Allocator = json_details::NoAllocator>
	using SIMDCppCommentSkippingPolicyChecked =
	  BasicParsePolicy<false, CppCommentSkippingPolicy, ExecTag, false,
	                   Allocator>;

	/***
	 * Parse using SIMD instructions if available, allow C++ comments and do not
	 * do more than minimum checking
	 */
	template<typename ExecTag, typename Allocator = json_details::NoAllocator>
	using SIMDCppCommentSkippingPolicyUnchecked =
	  BasicParsePolicy<true, CppCommentSkippingPolicy, ExecTag, false, Allocator>;

	namespace json_details {
		/***
		 * We are either not in a constexpr context or we have constexpr dtors. This
		 * is generally used so that we can call cleanup code after parsing the
		 * member
		 */
		template<typename ParsePolicy>
		inline constexpr bool is_guaranteed_rvo_v =
		  ParsePolicy::exec_tag_t::always_rvo;
	} // namespace json_details
} // namespace daw::json
