// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../concepts/daw_nullable_value.h"
#include "../daw_json_default_constuctor_fwd.h"
#include "daw_json_assert.h"

#include <daw/cpp_17.h>
#include <daw/daw_attributes.h>
#include <daw/daw_move.h>
#include <daw/daw_scope_guard.h>
#include <daw/daw_traits.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

/// @brief Customization point traits
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename>
			inline constexpr bool is_std_allocator_v = false;

			template<typename T>
			inline constexpr bool is_std_allocator_v<std::allocator<T>> = true;
		} // namespace json_details

		template<typename Iterator>
		struct construct_array_cleanup {
			Iterator &it;

			DAW_ATTRIB_INLINE
			DAW_SG_CXDTOR ~construct_array_cleanup( ) noexcept(
			  not use_daw_json_exceptions_v or noexcept( ++it ) ) {
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
				if( DAW_IS_CONSTANT_EVALUATED( ) ) {
					++it;
				} else {
#endif
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
					if( std::uncaught_exceptions( ) == 0 ) {
#endif
						++it;
#if not defined( DAW_JSON_DONT_USE_EXCEPTIONS )
					}
#endif
#if defined( DAW_HAS_CONSTEXPR_SCOPE_GUARD )
				}
#endif
			}
		};

		/// @brief Default constructor type for std::array and allows (Iterator,
		/// Iterator) construction
		template<typename T, std::size_t Sz>
		struct default_constructor<std::array<T, Sz>> {
			template<typename Iterator, std::size_t... Is>
			DAW_ATTRIB_INLINE static constexpr std::array<T, Sz>
			construct_array( Iterator first, Iterator last,
			                 std::index_sequence<Is...> ) {
				auto const get_result = [&]( std::size_t ) {
					if( first != last ) {
						if constexpr( std::is_move_constructible_v<T> ) {
							auto result = *first;
							++first;
							return result;
						} else {
							auto const run_after_parse = construct_array_cleanup{ first };
							(void)run_after_parse;
							return *first;
						}
					}
					return T{ };
				};
				return std::array<T, Sz>{ get_result( Is )... };
			}

			DAW_ATTRIB_INLINE
			DAW_JSON_CPP23_STATIC_CALL_OP constexpr std::array<T, Sz> operator( )(
			  std::array<T, Sz> &&v ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST noexcept {
				return DAW_MOVE( v );
			}

			template<typename Iterator>
			DAW_ATTRIB_INLINE
			  DAW_JSON_CPP23_STATIC_CALL_OP constexpr std::array<T, Sz>
			  operator( )( Iterator first,
			               Iterator last ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				return construct_array( first, last, std::make_index_sequence<Sz>{ } );
			}
		};

#if defined( DAW_JSON_HAS_CPP23_RANGE_CTOR )
		namespace json_details {
			template<typename F, typename L>
			struct iter_range_t {
				F first;
				L last;

				iter_range_t( ) = default;
				constexpr iter_range_t( F f, L l ) noexcept
				  : first( f )
				  , last( l ) {}

				constexpr F begin( ) const noexcept {
					return first;
				}

				constexpr L end( ) const noexcept {
					return last;
				}
			};
		} // namespace json_details

		/// @brief Default constructor type for std::vector.  It will reserve up
		/// front for non-random iterators
		template<typename T, typename Alloc>
		struct default_constructor<std::vector<T, Alloc>> {

			DAW_ATTRIB_INLINE
			DAW_JSON_CPP23_STATIC_CALL_OP DAW_JSON_CX_VECTOR std::vector<T, Alloc>
			operator( )( std::vector<T, Alloc> &&v )
			  DAW_JSON_CPP23_STATIC_CALL_OP_CONST
			  noexcept( noexcept( std::vector<T, Alloc>( v ) ) ) {
				return DAW_MOVE( v );
			}

			template<typename Iterator>
			DAW_ATTRIB_INLINE
			  DAW_JSON_CPP23_STATIC_CALL_OP DAW_JSON_CX_VECTOR std::vector<T, Alloc>
			  operator( )( Iterator first, Iterator last,
			               Alloc const &alloc = Alloc{ } )
			    DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				if constexpr( requires { last - first; } or
				              not json_details::is_std_allocator_v<Alloc> ) {
					return std::vector<T, Alloc>(
					  std::from_range, json_details::iter_range_t{ first, last }, alloc );
				} else {
					constexpr auto reserve_amount = 4096U / ( sizeof( T ) * 8U );
					auto result = std::vector<T, Alloc>( alloc );
					// Lets use a WAG and go for a 4k page size
					result.reserve( reserve_amount );
					result.assign_range( json_details::iter_range_t{ first, last } );
					return result;
				}
			}
		};

#else
		/// @brief Default constructor type for std::vector.  It will reserve up
		/// front for non-random iterators
		template<typename T, typename Alloc>
		struct default_constructor<std::vector<T, Alloc>> {

			DAW_ATTRIB_INLINE
			DAW_JSON_CPP23_STATIC_CALL_OP DAW_JSON_CX_VECTOR std::vector<T, Alloc>
			operator( )( std::vector<T, Alloc> &&v )
			  DAW_JSON_CPP23_STATIC_CALL_OP_CONST
			  noexcept( noexcept( std::vector<T, Alloc>( v ) ) ) {
				return DAW_MOVE( v );
			}

			template<typename Iterator>
			DAW_ATTRIB_INLINE
			  DAW_JSON_CPP23_STATIC_CALL_OP DAW_JSON_CX_VECTOR std::vector<T, Alloc>
			  operator( )( Iterator first, Iterator last,
			               Alloc const &alloc = Alloc{ } )
			    DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				if constexpr( std::is_same_v<std::random_access_iterator_tag,
				                             typename std::iterator_traits<
				                               Iterator>::iterator_category> or
				              not json_details::is_std_allocator_v<Alloc> ) {
					return std::vector<T, Alloc>( first, last, alloc );
				} else {
					constexpr auto reserve_amount = 4096U / ( sizeof( T ) * 8U );
					auto result = std::vector<T, Alloc>( alloc );
					// Lets use a WAG and go for a 4k page size
					result.reserve( reserve_amount );
					result.assign( first, last );
					return result;
				}
			}
		};
#endif

		/// @brief default constructor for std::unordered_map.  Allows construction
		/// via (Iterator, Iterator, Allocator)
		/// @tparam Key Key type in unordered map
		/// @tparam T Value type in unordered map
		/// @tparam Hash Hash type in unordered map
		template<typename Key, typename T, typename Hash, typename CompareEqual,
		         typename Alloc>
		struct default_constructor<
		  std::unordered_map<Key, T, Hash, CompareEqual, Alloc>> {

			DAW_ATTRIB_INLINE DAW_JSON_CPP23_STATIC_CALL_OP
			  std::unordered_map<Key, T, Hash, CompareEqual, Alloc>
			  operator( )( std::unordered_map<Key, T, Hash, CompareEqual, Alloc> &&v )
			    DAW_JSON_CPP23_STATIC_CALL_OP_CONST noexcept( noexcept(
			      std::unordered_map<Key, T, Hash, CompareEqual, Alloc>( v ) ) ) {
				return DAW_MOVE( v );
			}

			static constexpr std::size_t count = 1;
			template<typename Iterator>
			DAW_ATTRIB_INLINE DAW_JSON_CPP23_STATIC_CALL_OP
			  std::unordered_map<Key, T, Hash, CompareEqual, Alloc>
			  operator( )( Iterator first, Iterator last,
			               Alloc const &alloc = Alloc{ } )
			    DAW_JSON_CPP23_STATIC_CALL_OP_CONST {
				return std::unordered_map<Key, T, Hash, CompareEqual, Alloc>(
				  first, last, count, Hash{ }, CompareEqual{ }, alloc );
			}
		};

		/// @brief Default constructor for readable nullable types.
		template<typename T>
		struct nullable_constructor<
		  T, std::enable_if_t<concepts::is_nullable_value_v<T>>> {
			using value_type = concepts::nullable_value_type_t<T>;
			using rtraits_t = concepts::nullable_value_traits<T>;

			[[nodiscard]] DAW_ATTRIB_INLINE
			  DAW_JSON_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( concepts::construct_nullable_with_empty_t )
			    DAW_JSON_CPP23_STATIC_CALL_OP_CONST
			  noexcept( concepts::is_nullable_empty_nothrow_constructible_v<T> ) {
				static_assert( concepts::is_nullable_empty_constructible_v<T> );
				return rtraits_t{ }( concepts::construct_nullable_with_empty );
			}

			template<typename... Args,
			         std::enable_if_t<
			           concepts::is_nullable_value_constructible_v<T, Args...>,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE
			  DAW_JSON_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( Args &&...args ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST
			  noexcept(
			    concepts::is_nullable_value_nothrow_constructible_v<T, Args...> ) {
				return rtraits_t{ }( concepts::construct_nullable_with_value,
				                     DAW_FWD( args )... );
			}

			template<typename Pointer,
			         std::enable_if_t<
			           concepts::is_nullable_pointer_constructible_v<T, Pointer *>,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE
			  DAW_JSON_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( concepts::construct_nullable_with_pointer_t,
			               Pointer *ptr ) DAW_JSON_CPP23_STATIC_CALL_OP_CONST
			  noexcept(
			    concepts::is_nullable_value_nothrow_constructible_v<T, Pointer> ) {
				return rtraits_t{ }( concepts::construct_nullable_with_pointer, ptr );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
