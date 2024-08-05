// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "daw_json_assert.h"

#include <memory>
#include <optional>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename Alloc, bool /*is_empty*/>
			class AllocatorWrapperBase {
				using allocator_t = std::remove_reference_t<Alloc>;
				std::optional<allocator_t const *> allocator_ptr;

			public:
				explicit AllocatorWrapperBase( ) = default;
				explicit constexpr AllocatorWrapperBase(
				  allocator_t const &alloc ) noexcept
				  : allocator_ptr( &alloc ) {}

				[[nodiscard]] constexpr allocator_t const &get_allocator( ) const {
					daw_json_ensure( allocator_ptr.has_value( ),
					                 ErrorReason::UnexpectedNull );
					return **allocator_ptr;
				}
			};

			template<typename Alloc>
			class AllocatorWrapperBase<Alloc, true /*is_empty*/> {
				using has_stateless_allocator = void;
				using allocator_t = std::remove_reference_t<Alloc>;
				static constexpr allocator_t allocator{ };

			public:
				explicit AllocatorWrapperBase( ) = default;
				explicit constexpr AllocatorWrapperBase(
				  allocator_t const & ) noexcept {}

				[[nodiscard]] constexpr allocator_t const &get_allocator( ) const {
					return allocator;
				}
			};

			template<typename A, typename T, typename = void>
			inline constexpr bool allocator_has_rebind_v = false;

			template<typename A, typename T>
			inline constexpr bool allocator_has_rebind_v<
			  A, T,
			  std::void_t<
			    typename std::allocator_traits<A>::template rebind_traits<T>::type>> =
			  true;

			template<typename Alloc>
			struct AllocatorWrapper
			  : AllocatorWrapperBase<Alloc, std::is_empty_v<Alloc>> {
				using allocator_type = std::remove_reference_t<Alloc>;

				using AllocatorWrapperBase<Alloc,
				                           std::is_empty_v<Alloc>>::get_allocator;

				explicit AllocatorWrapper( ) = default;
				explicit constexpr AllocatorWrapper(
				  allocator_type const &alloc ) noexcept
				  : AllocatorWrapperBase<allocator_type,
				                         std::is_empty_v<allocator_type>>( alloc ) {}

				static constexpr bool has_allocator = true;

				template<typename A, typename T>
				struct allocator_type_as_rebind {
					using type =
					  typename std::allocator_traits<A>::template rebind_alloc<T>;
				};

				template<typename A, typename T>
				static constexpr bool has_rebind_v = allocator_has_rebind_v<A, T>;

				// DAW FIX
				template<typename T>
				using allocator_type_as = typename std::allocator_traits<
				  allocator_type>::template rebind_alloc<T>;

				template<typename T>
				[[nodiscard]] constexpr auto get_allocator_for( ) const {
					return static_cast<allocator_type_as<T>>( this->get_allocator( ) );
				}
			};

			struct NoAllocator {};

			template<>
			class AllocatorWrapper<NoAllocator> {
			public:
				explicit AllocatorWrapper( ) = default;
				explicit constexpr AllocatorWrapper( NoAllocator const & ) noexcept {}

				static constexpr bool has_allocator = false;

				using allocator_type = std::allocator<char>;

				template<typename T>
				using allocator_type_as = std::allocator<T>;

				template<typename T>
				[[nodiscard]] constexpr static std::allocator<T> get_allocator_for( ) {
					return std::allocator<T>( );
				}

				[[nodiscard]] DAW_CONSTEVAL static NoAllocator get_allocator( ) {
					return { };
				}
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
