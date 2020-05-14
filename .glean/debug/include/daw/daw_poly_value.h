// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <functional>
#include <memory>
#include <utility>

#include "daw_enable_if.h"
#include "daw_traits.h"

namespace daw {
	namespace poly_value_impl {
		template<typename BaseClass, typename Child = BaseClass>
		std::function<
		  std::unique_ptr<BaseClass>( std::unique_ptr<BaseClass> const &rhs )>
		make_copier( ) {
			return []( std::unique_ptr<BaseClass> const &rhs )
			         -> std::unique_ptr<BaseClass> {
				return std::make_unique<Child>(
				  *static_cast<Child const *>( rhs.get( ) ) );
			};
		}
	} // namespace poly_value_impl

	template<typename T = void>
	struct construct_emplace_t {};

	template<typename T = void>
	inline constexpr construct_emplace_t<T> construct_emplace{};

	template<typename BaseClass,
	         typename Deleter = std::default_delete<BaseClass>,
	         bool AllowDefaultConstruction = true, bool AllowCopy = true>
	class poly_value
	  : public daw::enable_default_constructor<BaseClass,
	                                           AllowDefaultConstruction>,
	    public daw::enable_copy_constructor<BaseClass, AllowCopy>,
	    public daw::enable_copy_assignment<BaseClass, AllowCopy> {
		std::unique_ptr<BaseClass, Deleter> m_ptr;
		std::function<std::unique_ptr<BaseClass>(
		  std::unique_ptr<BaseClass> const &rhs )>
		  m_copier;

	public:
		template<typename T, daw::enable_when_t<std::is_base_of_v<
		                       BaseClass, daw::remove_cvref_t<T>>> = nullptr>
		poly_value( T &&value )
		  : m_ptr(
		      std::make_unique<daw::remove_cvref_t<T>>( std::forward<T>( value ) ) )
		  , m_copier(
		      poly_value_impl::make_copier<BaseClass, daw::remove_cvref_t<T>>( ) ) {
		}

		template<typename T, typename... Args>
		poly_value( construct_emplace_t<T>, Args &&... args )
		  : m_ptr( std::make_unique<T>( std::forward<Args>( args )... ) )
		  , m_copier( poly_value_impl::make_copier<BaseClass, T>( ) ) {}

		poly_value( )
		  : m_ptr( std::make_unique<BaseClass>( ) )
		  , m_copier( poly_value_impl::make_copier<BaseClass>( ) ) {}

		poly_value( poly_value && ) noexcept = default;
		poly_value &operator=( poly_value && ) noexcept = default;
		~poly_value( ) noexcept = default;

		poly_value( poly_value const &other )
		  : daw::enable_default_constructor<BaseClass>( other )
		  , daw::enable_copy_constructor<BaseClass>( other )
		  , daw::enable_copy_assignment<BaseClass>( other )
		  , m_ptr( other.m_copier( other.m_ptr ) )
		  , m_copier( other.m_copier ) {}

		poly_value &operator=( poly_value const &rhs ) {
			if( this != &rhs ) {
				m_ptr = rhs.m_copier( rhs.m_ptr );
				m_copier = rhs.m_copier;
			}
			return *this;
		}

		template<typename Child,
		         daw::enable_when_t<std::is_base_of_v<BaseClass, Child>,
		                            !std::is_same_v<BaseClass, Child>,
		                            std::is_copy_constructible_v<Child>> = nullptr>
		poly_value( poly_value<Child> const &other )
		  : m_ptr( std::make_unique<Child>( *other.m_ptr ) )
		  , m_copier( poly_value_impl::make_copier<BaseClass, Child>( ) ) {}

		template<typename Child,
		         daw::enable_when_t<std::is_base_of_v<BaseClass, Child> and
		                            !std::is_same_v<BaseClass, Child>> = nullptr>
		poly_value &operator=( poly_value<Child> const &rhs ) {
			m_ptr = std::make_unique<Child>( *rhs.m_ptr );
			m_copier = poly_value_impl::make_copier<BaseClass, Child>( );
			return *this;
		}

		template<typename T, daw::enable_when_t<std::is_base_of_v<
		                       BaseClass, daw::remove_cvref_t<T>>> = nullptr>
		poly_value &operator=( T &&rhs ) {
			m_ptr =
			  std::make_unique<daw::remove_cvref_t<T>>( std::forward<T>( rhs ) );
			m_copier =
			  poly_value_impl::make_copier<BaseClass, daw::remove_cvref_t<T>>( );
			return *this;
		}

		template<typename T,
		         daw::enable_when_t<std::is_base_of_v<BaseClass, T>> = nullptr>
		poly_value( T *other )
		  : m_ptr( std::unique_ptr<T>( other ) )
		  , m_copier( poly_value_impl::make_copier<BaseClass, T>( ) ) {}

		template<typename T, typename D,
		         daw::enable_when_t<std::is_base_of_v<BaseClass, T>> = nullptr>
		poly_value( T *other, D &&deleter )
		  : m_ptr( std::unique_ptr<T>( other ), std::forward<D>( deleter ) )
		  , m_copier( poly_value_impl::make_copier<BaseClass, T>( ) ) {}

		template<typename T,
		         daw::enable_when_t<std::is_base_of_v<BaseClass, T>> = nullptr>
		poly_value &operator=( T *rhs ) {
			m_ptr = std::unique_ptr<T>( rhs );
			m_copier = poly_value_impl::make_copier<BaseClass, T>( );
			return *this;
		}

		BaseClass const &operator*( ) const {
			return *m_ptr;
		}

		BaseClass &operator*( ) {
			return *m_ptr;
		}

		BaseClass const *operator->( ) const {
			return m_ptr.get( );
		}

		BaseClass *operator->( ) {
			return m_ptr.get( );
		}

		operator BaseClass &( ) & noexcept {
			return *m_ptr;
		}

		operator BaseClass const &( ) const &noexcept {
			return *m_ptr;
		}

		operator BaseClass &&( ) && noexcept {
			return *m_ptr;
		}

		operator BaseClass const *( ) const noexcept {
			return static_cast<BaseClass const *>( m_ptr.get( ) );
		}

		operator BaseClass *( ) noexcept {
			return static_cast<BaseClass *>( m_ptr.get( ) );
		}
	};

	template<typename BaseClass, typename ChildClass = BaseClass,
	         typename... Args>
	poly_value<BaseClass> make_poly_value( Args &&... args ) {
		return {construct_emplace<ChildClass>, std::forward<Args>( args )...};
	}
} // namespace daw
