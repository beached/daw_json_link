// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_readable_value_fwd.h"

#include <daw/daw_cpp_feature_check.h>

#include <memory>
#include <optional>
#include <type_traits>

#if defined( __cpp_aggregate_paren_init )
#if __cpp_aggregate_paren_init >= 201902L
#define DAW_HAS_AGG_PAREN_INIT
#endif
#endif

namespace daw {

	template<typename T>
	struct readable_value_traits<std::optional<T>> {
		using value_type = T;
		using readable_type = std::optional<T>;
		static constexpr bool is_readable = true;

		static constexpr value_type const &read( readable_type const &val ) {
			assert( has_value( val ) );
			return *val;
		}

		constexpr readable_type operator( )( construct_readable_value_t,
		                                     readable_type const &opt ) const
		  noexcept( std::is_nothrow_copy_constructible_v<readable_type> ) {
			return opt;
		}
		constexpr readable_type operator( )( construct_readable_value_t,
		                                     readable_type &&opt ) const
		  noexcept( std::is_nothrow_move_constructible_v<readable_type> ) {
			return opt;
		}

		template<
		  typename... Args,
		  std::enable_if_t<readable_impl::is_readable_value_type_constructible_v<
		                     value_type, Args...>,
		                   std::nullptr_t> = nullptr>
		constexpr readable_type operator( )( construct_readable_value_t,
		                                     Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			if constexpr( std::is_aggregate_v<value_type> and
			              readable_impl::is_list_constructible_v<value_type,
			                                                     Args...> ) {
				return std::optional<value_type>( value_type{ DAW_FWD( args )... } );
			} else {
#endif
				return std::optional<value_type>( std::in_place, DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			}
#endif
		}

		constexpr readable_type
		operator( )( construct_readable_empty_t ) const noexcept {
			return readable_type( );
		}

		static constexpr bool has_value( readable_type const &val ) {
			return val.has_value( );
		}
	};

	template<typename T>
	struct readable_value_traits<std::unique_ptr<T>> {
		using value_type = T;
		using readable_type = std::unique_ptr<T>;
		static constexpr bool is_readable = true;

		static constexpr value_type const &read( readable_type const &val ) {
			assert( has_value( val ) );
			return *val;
		}

		constexpr readable_type operator( )( construct_readable_value_t,
		                                     readable_type &&opt ) const
		  noexcept( std::is_nothrow_move_constructible_v<readable_type> ) {
			return opt;
		}

		template<
		  typename... Args,
		  std::enable_if_t<readable_impl::is_readable_value_type_constructible_v<
		                     value_type, Args...>,
		                   std::nullptr_t> = nullptr>
		constexpr readable_type operator( )( construct_readable_value_t,
		                                     Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			if constexpr( std::is_aggregate_v<value_type> and
			              readable_impl::is_list_constructible_v<value_type,
			                                                     Args...> ) {
				return std::make_unique<value_type>( value_type{ DAW_FWD( args )... } );
			} else {
#endif
				return std::make_unique<value_type>( DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			}
#endif
		}

		constexpr readable_type
		operator( )( construct_readable_empty_t ) const noexcept {
			return readable_type( );
		}

		static constexpr bool has_value( readable_type const &val ) {
			return static_cast<bool>( val );
		}
	};

	template<typename T>
	struct readable_value_traits<std::shared_ptr<T>> {
		using value_type = T;
		using readable_type = std::shared_ptr<T>;
		static constexpr bool is_readable = true;

		static constexpr value_type const &read( readable_type const &val ) {
			assert( has_value( val ) );
			return *val;
		}

		constexpr readable_type operator( )( construct_readable_value_t,
		                                     readable_type const &opt ) const
		  noexcept( std::is_nothrow_copy_constructible_v<readable_type> ) {
			return opt;
		}
		constexpr readable_type operator( )( construct_readable_value_t,
		                                     readable_type &&opt ) const
		  noexcept( std::is_nothrow_move_constructible_v<readable_type> ) {
			return opt;
		}

		template<
		  typename... Args,
		  std::enable_if_t<readable_impl::is_readable_value_type_constructible_v<
		                     value_type, Args...>,
		                   std::nullptr_t> = nullptr>
		constexpr readable_type operator( )( construct_readable_value_t,
		                                     Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			if constexpr( std::is_aggregate_v<value_type> and
			              readable_impl::is_list_constructible_v<value_type,
			                                                     Args...> ) {
				return std::make_shared<value_type>( value_type{ DAW_FWD( args )... } );
			} else {
#endif
				return std::make_shared<value_type>( DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			}
#endif
		}

		constexpr readable_type
		operator( )( construct_readable_empty_t ) const noexcept {
			return readable_type( );
		}

		static constexpr bool has_value( readable_type const &val ) {
			return static_cast<bool>( val );
		}
	};

	template<typename T>
	struct readable_value_traits<T *> {
		using value_type = T;
		using readable_type = T *;
		static constexpr bool is_readable = true;

		static constexpr value_type const &read( readable_type const &val ) {
			assert( has_value( val ) );
			return *val;
		}

		constexpr readable_type operator( )( construct_readable_value_t,
		                                     readable_type ptr ) const noexcept {
			return ptr;
		}

		template<
		  typename... Args,
		  std::enable_if_t<readable_impl::is_readable_value_type_constructible_v<
		                     value_type, Args...>,
		                   std::nullptr_t> = nullptr>
		constexpr readable_type operator( )( construct_readable_value_t,
		                                     Args &&...args ) const
		  noexcept( std::is_nothrow_constructible_v<value_type, Args...> ) {
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			if constexpr( std::is_aggregate_v<T> and
			              readable_impl::is_list_constructible_v<T, Args...> ) {
				return new value_type{ DAW_FWD( args )... };
			} else {
#endif
				return new value_type( DAW_FWD( args )... );
#if not defined( DAW_HAS_AGG_PAREN_INIT )
			}
#endif
		}

		constexpr readable_type
		operator( )( construct_readable_empty_t ) const noexcept {
			return nullptr;
		}

		static constexpr bool has_value( readable_type const &val ) {
			return static_cast<bool>( val );
		}
	};

} // namespace daw
#if defined( DAW_HAS_AGG_PAREN_INIT )
#undef DAW_HAS_AGG_PAREN_INIT
#endif
