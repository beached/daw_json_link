// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../impl/version.h"

#include "daw_nullable_value_fwd.h"

#include <daw/daw_cpp_feature_check.h>

#include <memory>
#include <optional>
#include <type_traits>

#if defined( __cpp_aggregate_paren_init )
#if __cpp_aggregate_paren_init >= 201902L
#define DAW_HAS_AGG_PAREN_INIT
#endif
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace concepts {
			template<typename T>
			struct nullable_value_traits<std::optional<T>> {
				using value_type = T;
				using nullable_type = std::optional<T>;
				static constexpr bool is_nullable = true;

				static constexpr value_type const &read( nullable_type const &val ) {
					assert( has_value( val ) );
					return *val;
				}

				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     nullable_type const &opt ) const
				  noexcept( std::is_nothrow_copy_constructible_v<nullable_type> ) {
					return opt;
				}
				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     nullable_type &&opt ) const
				  noexcept( std::is_nothrow_move_constructible_v<nullable_type> ) {
					return opt;
				}

				template<typename... Args>
				requires( nullable_impl::is_nullable_value_type_constructible_v<
				          value_type, Args...> )
				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     Args &&...args ) const
				  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					if constexpr( std::is_aggregate_v<value_type> and
					              nullable_impl::is_list_constructible_v<value_type,
					                                                     Args...> ) {
						return std::optional<value_type>(
						  value_type{ DAW_FWD( args )... } );
					} else {
#endif
						return std::optional<value_type>( std::in_place,
						                                  DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					}
#endif
				}

				constexpr nullable_type
				operator( )( construct_nullable_with_empty_t ) const noexcept {
					return nullable_type( );
				}

				static constexpr bool has_value( nullable_type const &val ) {
					return val.has_value( );
				}
			};

			template<typename T>
			struct nullable_value_traits<std::unique_ptr<T>> {
				using value_type = T;
				using nullable_type = std::unique_ptr<T>;
				static constexpr bool is_nullable = true;

				static constexpr value_type const &read( nullable_type const &val ) {
					assert( has_value( val ) );
					return *val;
				}

				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     nullable_type &&opt ) const
				  noexcept( std::is_nothrow_move_constructible_v<nullable_type> ) {
					return opt;
				}

				template<typename... Args>
				requires( nullable_impl::is_nullable_value_type_constructible_v<
				          value_type, Args...> )
				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     Args &&...args ) const
				  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					if constexpr( std::is_aggregate_v<value_type> and
					              nullable_impl::is_list_constructible_v<value_type,
					                                                     Args...> ) {
						return std::make_unique<value_type>(
						  value_type{ DAW_FWD( args )... } );
					} else {
#endif
						return std::make_unique<value_type>( DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					}
#endif
				}

				constexpr nullable_type
				operator( )( construct_nullable_with_empty_t ) const noexcept {
					return nullable_type( );
				}

				static constexpr bool has_value( nullable_type const &val ) {
					return static_cast<bool>( val );
				}
			};

			template<typename T>
			struct nullable_value_traits<std::shared_ptr<T>> {
				using value_type = T;
				using nullable_type = std::shared_ptr<T>;
				static constexpr bool is_nullable = true;

				static constexpr value_type const &read( nullable_type const &val ) {
					assert( has_value( val ) );
					return *val;
				}

				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     nullable_type const &opt ) const
				  noexcept( std::is_nothrow_copy_constructible_v<nullable_type> ) {
					return opt;
				}
				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     nullable_type &&opt ) const
				  noexcept( std::is_nothrow_move_constructible_v<nullable_type> ) {
					return opt;
				}

				template<typename... Args>
				requires( nullable_impl::is_nullable_value_type_constructible_v<
				          value_type, Args...> )
				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     Args &&...args ) const
				  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					if constexpr( std::is_aggregate_v<value_type> and
					              nullable_impl::is_list_constructible_v<value_type,
					                                                     Args...> ) {
						return std::make_shared<value_type>(
						  value_type{ DAW_FWD( args )... } );
					} else {
#endif
						return std::make_shared<value_type>( DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					}
#endif
				}

				constexpr nullable_type
				operator( )( construct_nullable_with_empty_t ) const noexcept {
					return nullable_type( );
				}

				static constexpr bool has_value( nullable_type const &val ) {
					return static_cast<bool>( val );
				}
			};

			template<typename T>
			struct nullable_value_traits<T *> {
				using value_type = T;
				using nullable_type = T *;
				static constexpr bool is_nullable = true;

				static constexpr value_type const &read( nullable_type const &val ) {
					assert( has_value( val ) );
					return *val;
				}

				constexpr nullable_type
				operator( )( construct_nullable_with_value_t,
				             nullable_type ptr ) const noexcept {
					return ptr;
				}

				template<typename... Args>
				requires( nullable_impl::is_nullable_value_type_constructible_v<
				          value_type, Args...> )
				constexpr nullable_type operator( )( construct_nullable_with_value_t,
				                                     Args &&...args ) const
				  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					if constexpr( std::is_aggregate_v<T> and
					              nullable_impl::is_list_constructible_v<T, Args...> ) {
						return new value_type{ DAW_FWD( args )... };
					} else {
#endif
						return new value_type( DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
					}
#endif
				}

				constexpr nullable_type
				operator( )( construct_nullable_with_empty_t ) const noexcept {
					return nullptr;
				}

				static constexpr bool has_value( nullable_type const &val ) {
					return static_cast<bool>( val );
				}
			};
		} // namespace concepts
	}   // namespace DAW_JSON_VER
} // namespace daw::json
#if defined( DAW_HAS_AGG_PAREN_INIT )
#undef DAW_HAS_AGG_PAREN_INIT
#endif
