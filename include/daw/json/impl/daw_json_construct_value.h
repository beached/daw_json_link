// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/daw_allocator_construct.h"
#include "daw/json/daw_json_switches.h"

#include <daw/daw_attributes.h>
#include <daw/daw_fwd_pack_apply.h>

#include <cstddef>
#include <utility>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename Value, typename Constructor, typename ParseState,
			         typename... Args>
			DAW_ATTRIB_INLINE static constexpr auto
			construct_value( ParseState &parse_state, Args &&...args ) {
				// Silence MSVC warning, used in other if constexpr case
				(void)parse_state;
				if constexpr( ParseState::has_allocator ) {
					auto alloc = parse_state.template get_allocator_for<Value>( );
					return daw::try_alloc_construct<Value, Constructor>(
					  std::move( alloc ), DAW_FWD( args )... );
				} else {
					static_assert(
					  daw::is_callable_v<Constructor, Args...>,
					  "Unable to construct value with the supplied arguments" );
					return Constructor{ }( DAW_FWD( args )... );
				}
			}

#if not defined( DAW_JSON_USE_GENERIC_LAMBDAS )
			template<typename Constructor>
			struct construct_value_tp_invoke_t {
				template<typename... TArgs, std::size_t... Is>
				DAW_ATTRIB_INLINE constexpr auto
				operator( )( fwd_pack<TArgs...> &&tp,
				             std::index_sequence<Is...> ) const {
					return Constructor{ }( get<Is>( std::move( tp ) )... );
				}

				template<typename... TArgs, typename Allocator, std::size_t... Is>
				DAW_ATTRIB_INLINE constexpr auto
				operator( )( fwd_pack<TArgs...> &&tp, Allocator &alloc,
				             std::index_sequence<Is...> ) const {
					return Constructor{ }( get<Is>( std::move( tp ) )...,
					                       DAW_FWD( alloc ) );
				}

				template<typename Alloc, typename... TArgs, std::size_t... Is>
				DAW_ATTRIB_INLINE constexpr auto
				operator( )( std::allocator_arg_t, Alloc &&alloc,
				             fwd_pack<TArgs...> &&tp,
				             std::index_sequence<Is...> ) const {

					return Constructor{ }( std::allocator_arg,
					                       DAW_FWD( alloc ),
					                       get<Is>( std::move( tp ) )... );
				}
			};
			template<typename Constructor>
			inline constexpr auto construct_value_tp_invoke =
			  construct_value_tp_invoke_t<Constructor>{ };
#endif

			template<typename Value, typename Constructor, typename ParseState,
			         typename... Args>
			DAW_ATTRIB_FLATINLINE static constexpr auto
			construct_value_tp( ParseState &parse_state,
			                    fwd_pack<Args...> &&tp_args ) {

#if defined( DAW_JSON_USE_GENERIC_LAMBDAS )
				if constexpr( ParseState::has_allocator ) {
					// ParseState has a user allocator, pass that if we can to the
					// constructed value
					using alloc_t =
					  typename ParseState::template allocator_type_as<Value>;
					auto alloc = parse_state.template get_allocator_for<Value>( );
					// There are several ways that the allocator is passed on
					if constexpr( daw::is_callable_v<Constructor, Args..., alloc_t> ) {
						return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
							// Type( args..., alloc )
							return Constructor{ }( get<Is>( std::move( tp_args ) )...,
							                       std::move( alloc ) );
						}( std::make_index_sequence<sizeof...( Args )>{ } );
					} else if constexpr( daw::is_callable_v<Constructor,
					                                        std::allocator_arg_t,
					                                        alloc_t,
					                                        Args...> ) {
						// Type( std::allocator_arg, alloc, args... )
						return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
							return Constructor{ }( std::allocator_arg,
							                       std::move( alloc ),
							                       get<Is>( std::move( tp_args ) )... );
						}( std::make_index_sequence<sizeof...( Args )>{ } );
					} else {
						// This type does not take a known allocator in the constructor,
						// fallback to normal construction
						// Type( args... )
						static_assert(
						  daw::is_callable_v<Constructor, Args...>,
						  "Unable to construct value with the supplied arguments" );
						return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
							return Constructor{ }( get<Is>( std::move( tp_args ) )... );
						}( std::make_index_sequence<sizeof...( Args )>{ } );
					}
				} else {
					// Type( args... )
					// No ParseState user allocator
					// Silence MSVC warning, used in other if constexpr case
					(void)parse_state;
					static_assert(
					  daw::is_callable_v<Constructor, Args...>,
					  "Unable to construct value with the supplied arguments" );
					return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
						return Constructor{ }( get<Is>( std::move( tp_args ) )... );
					}( std::make_index_sequence<sizeof...( Args )>{ } );
				}
#else
				if constexpr( ParseState::has_allocator ) {
					// ParseState has a user allocator, pass that if we can to the
					// constructed value
					using alloc_t =
					  typename ParseState::template allocator_type_as<Value>;
					auto alloc = parse_state.template get_allocator_for<Value>( );
					if constexpr( daw::is_callable_v<Constructor, Args..., alloc_t> ) {
						// Type( args..., alloc )
						return construct_value_tp_invoke<Constructor>(
						  std::move( tp_args ),
						  std::move( alloc ),
						  std::index_sequence_for<Args...>{ } );
					} else if constexpr( daw::is_callable_v<Constructor,
					                                        std::allocator_arg_t,
					                                        alloc_t,
					                                        Args...> ) {
						// Type( std::allocator_arg, alloc, args... )
						return construct_value_tp_invoke<Constructor>(
						  std::allocator_arg,
						  std::move( alloc ),
						  std::move( tp_args ),
						  std::index_sequence_for<Args...>{ } );
					} else {
						// This type does not take a known allocator in the constructor,
						// fallback to normal construction
						// Type( args... )
						static_assert(
						  daw::is_callable_v<Constructor, Args...>,
						  "Unable to construct value with the supplied arguments" );
						return construct_value_tp_invoke<Constructor>(
						  std::move( tp_args ), std::index_sequence_for<Args...>{ } );
					}
				} else {
					// No ParseState user allocator
					// Silence MSVC warning, used in other if constexpr case
					// Type( args... )
					(void)parse_state;
					static_assert(
					  daw::is_callable_v<Constructor, Args...>,
					  "Unable to construct value with the supplied arguments" );
					return construct_value_tp_invoke<Constructor>(
					  std::move( tp_args ), std::index_sequence_for<Args...>{ } );
				}
#endif
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
