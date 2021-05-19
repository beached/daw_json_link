// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include <daw/cpp_17.h>
#include <daw/daw_traits.h>

#include <cstddef>
#include <memory>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
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
			  : AllocatorWrapperBase<Alloc, std::is_empty<Alloc>::value> {
				using allocator_type = std::remove_reference_t<Alloc>;

				AllocatorWrapper( allocator_type &alloc ) noexcept
				  : AllocatorWrapperBase<allocator_type,
				                         std::is_empty<allocator_type>::value>(
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
				  daw::is_detected<has_allocator_type_as_rebind, A, T>::value;

				// DAW FIX
				template<typename T>
				using allocator_type_as =
				  std::conditional_t<has_rebind_v<allocator_type, T>,
				                     allocator_type_as_rebind<allocator_type, T>,
				                     allocator_type>;

				template<typename T>
				auto get_allocator_for( template_param<T> ) const {
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
				[[maybe_unused]] inline std::allocator<T>
				get_allocator_for( template_param<T> ) const {
					return std::allocator<T>( );
				}
			};
		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json
