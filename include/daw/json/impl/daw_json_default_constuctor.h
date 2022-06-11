// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../concepts/daw_readable_value.h"
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

/***
 * Customization point traits
 *
 */
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
			  not use_daw_json_exceptions_v ) {
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
			DAW_ATTRIB_INLINE constexpr std::array<T, Sz> operator( )( ) const
			  noexcept( noexcept( std::array<T, Sz>{ } ) ) {
				return { };
			}

			DAW_ATTRIB_INLINE constexpr std::array<T, Sz> &&
			operator( )( std::array<T, Sz> &&v ) const noexcept {
				return DAW_MOVE( v );
			}

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

			template<typename Iterator>
			DAW_ATTRIB_INLINE constexpr std::array<T, Sz>
			operator( )( Iterator first, Iterator last ) const {
				return construct_array( first, last, std::make_index_sequence<Sz>{ } );
			}
		};

		/// @brief Default constructor type for std::vector.  It will reserve up
		/// front for non-random iterators
		template<typename T, typename Alloc>
		struct default_constructor<std::vector<T, Alloc>> {
			// DAW
			DAW_ATTRIB_INLINE std::vector<T, Alloc> operator( )( ) const
			  noexcept( noexcept( std::vector<T, Alloc>( ) ) ) {
				return { };
			}

			DAW_ATTRIB_INLINE std::vector<T, Alloc> &&
			operator( )( std::vector<T, Alloc> &&v ) const
			  noexcept( noexcept( std::vector<T, Alloc>( v ) ) ) {
				return DAW_MOVE( v );
			}

			template<typename Iterator>
			DAW_ATTRIB_INLINE std::vector<T, Alloc>
			operator( )( Iterator first, Iterator last,
			             Alloc const &alloc = Alloc{ } ) const {
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

		/// @brief default constructor for std::unordered_map.  Allows construction
		/// via (Iterator, Iterator, Allocator) \tparam Key \tparam T \tparam Hash
		template<typename Key, typename T, typename Hash, typename CompareEqual,
		         typename Alloc>
		struct default_constructor<
		  std::unordered_map<Key, T, Hash, CompareEqual, Alloc>> {
			DAW_ATTRIB_INLINE std::unordered_map<Key, T, Hash, CompareEqual, Alloc>
			operator( )( ) const noexcept(
			  noexcept( std::unordered_map<Key, T, Hash, CompareEqual, Alloc>( ) ) ) {
				return { };
			}

			DAW_ATTRIB_INLINE std::unordered_map<Key, T, Hash, CompareEqual, Alloc>
			operator( )( std::unordered_map<Key, T, Hash, CompareEqual, Alloc> &&v )
			  const noexcept( noexcept(
			    std::unordered_map<Key, T, Hash, CompareEqual, Alloc>( v ) ) ) {
				return DAW_MOVE( v );
			}

			static constexpr std::size_t count = 1;
			template<typename Iterator>
			DAW_ATTRIB_INLINE std::unordered_map<Key, T, Hash, CompareEqual, Alloc>
			operator( )( Iterator first, Iterator last,
			             Alloc const &alloc = Alloc{ } ) const {
				return std::unordered_map<Key, T, Hash, CompareEqual, Alloc>(
				  first, last, count, Hash{ }, CompareEqual{ }, alloc );
			}
		};

		/***
		 * Default constructor for readable nullable types.
		 */
		template<typename T>
		struct nullable_constructor<
		  T, std::enable_if_t<concepts::is_readable_value_v<T>>> {
			using value_type = concepts::readable_value_type_t<T>;
			using rtraits_t = concepts::readable_value_traits<T>;

			[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
			operator( )( concepts::construct_readable_empty_t ) const
			  noexcept( concepts::is_readable_empty_nothrow_constructible_v<T> ) {
				static_assert( concepts::is_readable_empty_constructible_v<T> );
				return rtraits_t{ }( concepts::construct_readable_empty );
			}

			template<typename... Args,
			         std::enable_if_t<
			           concepts::is_readable_value_constructible_v<T, Args...>,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
			operator( )( Args &&...args ) const noexcept(
			  concepts::is_readable_value_nothrow_constructible_v<T, Args...> ) {
				return rtraits_t{ }( concepts::construct_readable_value,
				                     DAW_FWD( args )... );
			}
		};
	} // namespace DAW_JSON_VER
} // namespace daw::json
