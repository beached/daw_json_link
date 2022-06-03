// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include <daw/json/daw_json_exception.h>
#include <daw/json/impl/daw_json_assert.h>

namespace daw {
	struct DefaultValueDeleter {
		template<typename T>
		void operator( )( T *ptr ) const {
			delete ptr;
		}
	};
	struct DefaultArrayDeleter {
		template<typename T>
		void operator( )( T *ptr ) const {
			delete[] ptr;
		}
	};
	template<typename T>
	using DefaultDeleter =
	  std::conditional_t<std::is_array_v<T>, DefaultArrayDeleter,
	                     DefaultValueDeleter>;

	template<typename T, typename Deleter = DefaultDeleter<T>>
	struct ref_counted_ptr {
		T *ptr = nullptr;
		std::size_t *count = &init_count;

		static inline std::size_t init_count = 0xBEEF'FEEDULL;
		static inline std::size_t deleted_count = 0xDEAD'BEEFULL;

		std::size_t *inc_count( ) const {
			assert( count );
			++( *count );
			return count;
		}

		std::size_t *dec_count( ) const {
			assert( count );
			--( *count );
			return count;
		}

		ref_counted_ptr( ) = default;

		~ref_counted_ptr( ) {
			release( );
		}

		explicit constexpr ref_counted_ptr( T *p ) noexcept
		  : ptr( p )
		  , count( new std::size_t( 1 ) ) {}

		constexpr ref_counted_ptr( ref_counted_ptr const &other )
		  : ptr{ other.ptr }
		  , count{ other.inc_count( ) } {}

		constexpr ref_counted_ptr &operator=( ref_counted_ptr const &rhs ) {
			if( this != &rhs ) {
				release( );
				ptr = rhs.ptr;
				count = rhs.count;
				if( count ) {
					++( *count );
				}
			}
			return *this;
		}

		constexpr void release( ) {
			if( ptr ) {
				if( *dec_count( ) == 0 ) {
					Deleter{ }( ptr );
					delete count;
					ptr = nullptr;
					count = &deleted_count;
				}
			}
		}

		constexpr T *operator->( ) const noexcept {
			return ptr;
		}
		constexpr T &operator*( ) const noexcept {
			return *ptr;
		}

		constexpr bool operator==( ref_counted_ptr const &rhs ) const noexcept {
			return ptr == rhs.ptr;
		}

		constexpr bool operator!=( ref_counted_ptr const &rhs ) const noexcept {
			return ptr != rhs.ptr;
		}
	};

	struct fixed_allocator_impl {
		unsigned char *buffer_start;
		unsigned char *ptr;
		std::size_t capacity;

		fixed_allocator_impl( std::size_t Size )
		  : buffer_start( new unsigned char[Size] )
		  , ptr( buffer_start )
		  , capacity( Size ) {}

		fixed_allocator_impl( fixed_allocator_impl const & ) = delete;
		fixed_allocator_impl( fixed_allocator_impl && ) = delete;
		fixed_allocator_impl &operator=( fixed_allocator_impl const & ) = delete;
		fixed_allocator_impl &operator=( fixed_allocator_impl && ) = delete;

		~fixed_allocator_impl( ) {
			delete[] buffer_start;
		}
	};

	template<typename T>
	class [[nodiscard]] fixed_allocator {
		ref_counted_ptr<fixed_allocator_impl> m_data;

		constexpr fixed_allocator( ref_counted_ptr<fixed_allocator_impl> const &d )
		  : m_data( d ) {
			assert( m_data->buffer_start and m_data->ptr );
		}

	public:
		template<class U>
		struct rebind {
			using other = fixed_allocator<U>;
		};
		using value_type = T;

		fixed_allocator( ) = default;

		fixed_allocator( std::size_t Size )
		  : m_data( new fixed_allocator_impl( Size ) ) {
			assert( m_data->buffer_start and m_data->ptr );
		}

		[[nodiscard]] T *allocate( std::size_t n ) noexcept {
			assert( m_data->buffer_start and m_data->ptr );
			// Ensure aligned.  Underlying ptr could be shared so must be done
#ifndef NDEBUG
			daw_json_ensure( ( ( used( ) + n ) < m_data->capacity ),
			                 daw::json::ErrorReason::Unknown );
#endif
			m_data->ptr += static_cast<std::ptrdiff_t>(
			  alignof( T ) -
			  ( reinterpret_cast<std::uintptr_t>( m_data->ptr ) % alignof( T ) ) );

			T *r = reinterpret_cast<T *>( m_data->ptr );
			m_data->ptr += n * sizeof( T );
			return r;
		}

		static constexpr void deallocate( void *const, std::size_t ) noexcept {}

		void constexpr release( ) noexcept {
			assert( m_data->buffer_start and m_data->ptr );
			m_data->ptr = m_data->buffer_start;
		}

		[[nodiscard]] constexpr std::size_t used( ) const {
			assert( m_data->ptr and m_data->buffer_start and
			        m_data->buffer_start <= m_data->ptr );
			return static_cast<std::size_t>( m_data->ptr - m_data->buffer_start );
		}

		template<typename>
		friend class daw::fixed_allocator;

		template<typename U>
		[[nodiscard]] constexpr operator fixed_allocator<U>( ) const {
			return fixed_allocator<U>( m_data );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator==( fixed_allocator<U> const &rhs ) const noexcept {
			return m_data == rhs.m_data;
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator!=( fixed_allocator<U> const &rhs ) const noexcept {
			return m_data != rhs.m_data;
		}
	};
	template<typename T>
	fixed_allocator( T ) -> fixed_allocator<char>;
} // namespace daw
