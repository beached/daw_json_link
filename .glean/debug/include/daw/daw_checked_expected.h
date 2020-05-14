// The MIT License ( MIT )
//
// Copyright ( c ) 2018-2020 Darrell Wright
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

#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_exchange.h"
#include "daw_move.h"
#include "daw_optional.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename ExpectedException>
		void is_expected_exception( std::exception_ptr ptr ) {
			try {
				std::rethrow_exception( ptr );
			} catch( ExpectedException const & ) { return; } catch( ... ) {
				struct unexpected_exception_type {
				}; // Cannot catch what you cannot name
				daw::exception::daw_throw<unexpected_exception_type>( );
			}
		}

		template<typename... ExpectedExceptions, typename T>
		constexpr auto is_expected_exception( T && ) noexcept
		  -> std::enable_if_t<( sizeof...( ExpectedExceptions ) == 0 )> {}

		template<typename ExpectedException, typename... ExpectedExceptions>
		auto is_expected_exception( std::exception_ptr ptr )
		  -> std::enable_if_t<( sizeof...( ExpectedExceptions ) > 0 )> {

			try {
				std::rethrow_exception( ptr );
			} catch( ExpectedException const & ) { return; } catch( ... ) {
				is_expected_exception<ExpectedExceptions...>( ptr );
			}
		}
	} // namespace impl

	template<typename T, typename... ExpectedExceptions>
	struct checked_expected_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		daw::optional<value_type> m_value;
		std::exception_ptr m_exception;

	public:
		struct exception_tag {};

		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		checked_expected_t( ) = default;
		checked_expected_t( checked_expected_t const & ) = default;
		checked_expected_t &operator=( checked_expected_t const & ) = default;
		checked_expected_t( checked_expected_t && ) noexcept = default;
		checked_expected_t &operator=( checked_expected_t && ) noexcept = default;
		~checked_expected_t( ) = default;

		friend bool operator==( checked_expected_t const &lhs,
		                        checked_expected_t const &rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) ==
			       std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////
		checked_expected_t( value_type value ) noexcept
		  : m_value{daw::move( value )}
		  , m_exception{} {}

		checked_expected_t &operator=( value_type value ) noexcept {
			m_value = daw::move( value );
			m_exception = nullptr;
			return *this;
		}

		checked_expected_t( std::exception_ptr ptr )
		  : m_value{}
		  , m_exception{daw::move( ptr )} {

			impl::is_expected_exception<ExpectedExceptions...>( m_exception );
		}

		checked_expected_t &operator=( std::exception_ptr ptr ) {
			if( m_value ) {
				m_value.reset( );
			}
			m_exception = ptr;
			return *this;
		}

		template<typename ExceptionType>
		checked_expected_t( exception_tag, ExceptionType const &ex ) noexcept
		  : checked_expected_t{std::make_exception_ptr( ex )} {

			static_assert(
			  daw::traits::is_one_of_v<ExceptionType, ExpectedExceptions...>,
			  "Unexpected exception type" );
		}

		checked_expected_t( exception_tag )
		  : checked_expected_t{std::current_exception( )} {

			impl::is_expected_exception<ExpectedExceptions...>( m_exception );
		}

		//		template<class Function, typename... Args, typename =
		// std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename... Args,
		         std::enable_if_t<traits::is_callable_v<Function, Args...>,
		                          std::nullptr_t> = nullptr>
		static checked_expected_t from_code( Function func, Args &&... args ) {
			try {
				auto tmp = func( std::forward<Args>( args )... );
				return checked_expected_t{daw::move( tmp )};
			} catch( ... ) { return checked_expected_t{exception_tag{}}; }
		}

		// template<class Function, typename... Args, typename =
		// std::enable_if_t<is_callable_v<Function, Args...>>>
		template<class Function, typename... Args>
		checked_expected_t( Function func, Args &&... args )
		  : checked_expected_t{checked_expected_t::from_code(
		      func, std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return static_cast<bool>( m_value );
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return m_exception;
		}

		bool empty( ) const noexcept {
			return !( has_value( ) or has_exception( ) );
		}

		explicit operator bool( ) const noexcept {
			return !empty( );
		}

		operator value_type( ) const {
			return get( );
		}

		void throw_if_exception( ) const {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
		}

		reference get( ) {
			throw_if_exception( );
			return *m_value;
		}

		const_reference get( ) const {
			throw_if_exception( );
			return *m_value;
		}

		reference operator*( ) {
			return get( );
		}

		const_reference operator*( ) const {
			return get( );
		}

		pointer operator->( ) {
			return &get( );
		}

		const_pointer operator->( ) const {
			return &get( );
		}

		std::string get_exception_message( ) const {
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) {
				return std::string{e.what( )};
			} catch( ... ) { return {}; }
			return {};
		}

	}; // class checked_expected_t

	static_assert( traits::is_regular<checked_expected_t<int>>, "" );

	template<typename... ExpectedExceptions>
	struct checked_expected_t<void, ExpectedExceptions...> {
		using value_type = void;

		struct exception_tag {};

	private:
		std::exception_ptr m_exception;
		bool m_value;

		checked_expected_t( bool b ) noexcept
		  : m_exception{}
		  , m_value{b} {}

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Summary: No value, aka null
		//////////////////////////////////////////////////////////////////////////
		checked_expected_t( checked_expected_t &&other ) noexcept
		  : m_exception{daw::exchange( other.m_exception, nullptr )}
		  , m_value{daw::exchange( other.m_value, false )} {}

		checked_expected_t( checked_expected_t const &other )
		  : m_exception{}
		  , m_value{other.m_value} {
			if( other.m_exception ) {
				m_exception = other.m_exception;
			}
		}
		checked_expected_t &operator=( checked_expected_t const &rhs ) {
			if( this != &rhs ) {
				if( rhs.m_exception ) {
					m_exception = rhs.m_exception;
				}
				m_value = rhs.m_value;
			}
			return *this;
		}

		~checked_expected_t( ) = default;

		friend bool operator==( checked_expected_t const &lhs,
		                        checked_expected_t const &rhs ) {
			return std::tie( lhs.m_value, lhs.m_exception ) ==
			       std::tie( rhs.m_value, rhs.m_exception );
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: With value
		//////////////////////////////////////////////////////////////////////////

		checked_expected_t( ) noexcept
		  : checked_expected_t{false} {}

		template<typename T>
		checked_expected_t( T && ) noexcept
		  : checked_expected_t{true} {}

		checked_expected_t &operator=( bool ) noexcept {
			m_value = true;
			m_exception = nullptr;
			return *this;
		}

		checked_expected_t( std::exception_ptr ptr )
		  : m_exception{ptr}
		  , m_value{false} {

			impl::is_expected_exception<ExpectedExceptions...>( ptr );
		}

		checked_expected_t &operator=( std::exception_ptr ptr ) {
			impl::is_expected_exception<ExpectedExceptions...>( ptr );
			m_value = false;
			m_exception = ptr;
			return *this;
		}

		template<typename ExceptionType>
		checked_expected_t( exception_tag, ExceptionType const &ex ) noexcept
		  : checked_expected_t{std::make_exception_ptr( ex )} {

			static_assert(
			  daw::traits::is_one_of_v<ExceptionType, ExpectedExceptions...>,
			  "Unexpected exception type" );
		}

		checked_expected_t( exception_tag )
		  : checked_expected_t{std::current_exception( )} {

			impl::is_expected_exception<ExpectedExceptions...>( m_exception );
		}

		//		template<class Function, typename... Args, typename =
		// std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename Arg, typename... Args>
		static checked_expected_t from_code( Function func, Arg &&arg,
		                                     Args &&... args ) {
			try {
				func( std::forward<Arg>( arg ), std::forward<Args>( args )... );
				return checked_expected_t{true};
			} catch( ... ) { return checked_expected_t{exception_tag{}}; }
		}

		template<class Function>
		static checked_expected_t from_code( Function func ) {
			try {
				func( );
				return checked_expected_t{true};
			} catch( ... ) { return checked_expected_t{exception_tag{}}; }
		}

		//		template<class Function, typename... Args, typename =
		// std::enable_if_t<is_callable_v<Function,
		// Args...>>>
		template<class Function, typename... Args,
		         typename result = decltype(
		           std::declval<Function>( )( std::declval<Args>( )... ) )>
		checked_expected_t( Function func, Args &&... args )
		  : checked_expected_t{checked_expected_t::from_code(
		      func, std::forward<Args>( args )... )} {}

		bool has_value( ) const noexcept {
			return m_value;
		}

		bool has_exception( ) const noexcept {
			return static_cast<bool>( m_exception );
		}

		std::exception_ptr get_exception_ptr( ) noexcept {
			return m_exception;
		}

		bool empty( ) const noexcept {
			return !( has_value( ) or has_exception( ) );
		}

		explicit operator bool( ) const noexcept {
			return !empty( );
		}

		void throw_if_exception( ) const {
			if( has_exception( ) ) {
				std::rethrow_exception( m_exception );
			}
		}

		value_type get( ) const {
			throw_if_exception( );
		}

		std::string get_exception_message( ) const {
			try {
				throw_if_exception( );
			} catch( std::exception const &e ) {
				return std::string{e.what( )};
			} catch( ... ) { return {}; }
			return {};
		}
	}; // class checked_expected_t<void>

	namespace impl {
		template<typename T, typename...>
		using first_t = T;

		template<typename T, typename U, typename...>
		using second_t = U;
	} // namespace impl

	template<typename... ExpectedExceptions, typename Function, typename... Args>
	auto checked_from_code( Function func, Args &&... args ) {
		using result_t =
		  std::decay_t<decltype( func( std::forward<Args>( args )... ) )>;
		return checked_expected_t<result_t, ExpectedExceptions...>::from_code(
		  func, std::forward<Args>( args )... );
	}

	template<typename Function, typename Result, typename... Exceptions>
	class checked_function_t {
		Function func;

	public:
		constexpr checked_function_t( Function f )
		  : func{daw::move( f )} {}

		template<typename... Args>
		constexpr decltype( auto ) operator( )( Args &&... args ) const {
			return checked_expected_t<Result, Exceptions...>::from_code(
			  func, std::forward<Args>( args )... );
		}
	};

	template<typename Result, typename... Exceptions, typename Function>
	constexpr checked_function_t<Function, Result, Exceptions...>
	make_checked_function( Function &&func ) noexcept {
		return checked_function_t<Function, Result, Exceptions...>{
		  std::forward<Function>( func )};
	}
} // namespace daw
