// The MIT License (MIT)
//
// Copyright (c) 2013-2020 Darrell Wright
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

#include <algorithm>
#include <exception>
#include <iterator>
#include <type_traits>
#include <utility>

#if !defined( NODEBUGTHROW ) and                                               \
  ( defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
    defined( _CPPUNWIND ) )

#define MAY_THROW_EXCEPTIONS true
#include <stdexcept>

#else

#define MAY_THROW_EXCEPTIONS false

#endif

namespace daw::exception {
	struct basic_exception {
		template<typename Arg>
		constexpr basic_exception( Arg ) noexcept {}

		constexpr basic_exception( ) noexcept = default;
	};
	struct arithmetic_exception : public basic_exception {};
	struct not_implemented_exception : public basic_exception {};

	[[maybe_unused]] constexpr bool may_throw_v = MAY_THROW_EXCEPTIONS;

#if MAY_THROW_EXCEPTIONS
	using NotImplemented = std::runtime_error;
	using FatalError = std::runtime_error;
	using NullPtrAccessException = std::runtime_error;
	using AssertException = std::runtime_error;
	using FileException = std::runtime_error;
	using MethodNotImplemented = std::runtime_error;
	using UnexpectedEnumValue = std::runtime_error;
	using DefaultException = std::runtime_error;
#else
	using NotImplemented = basic_exception;
	using FatalError = basic_exception;
	using NullPtrAccessException = basic_exception;
	using AssertException = basic_exception;
	using FileException = basic_exception;
	using MethodNotImplemented = basic_exception;
	using UnexpectedEnumValue = basic_exception;
	using DefaultException = basic_exception;
#endif

	template<typename T, T error_number>
	struct errno_exception : public basic_exception,
	                         public std::integral_constant<T, error_number> {};

	template<
	  typename ExceptionType = DefaultException,
	  typename std::enable_if<std::is_default_constructible<ExceptionType>::value,
	                          std::nullptr_t>::type = nullptr>
	[[noreturn]] void daw_throw( ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
		throw ExceptionType{};
#else
		std::abort( );
#endif
	}

	template<typename ExceptionType = DefaultException,
	         typename std::enable_if<
	           !std::is_default_constructible<ExceptionType>::value,
	           std::nullptr_t>::type = nullptr>
	[[noreturn]] void daw_throw( ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
		throw ExceptionType( "" );
#else
		std::abort( );
#endif
	}

	template<typename ExceptionType = DefaultException, typename Arg,
	         typename... Args>
	[[noreturn]] void daw_throw( Arg &&arg, Args &&... args ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
		throw ExceptionType( std::forward<Arg>( arg ),
		                     std::forward<Args>( args )... );
#else
		std::abort( );
#endif
	}

#ifndef NODEBUGTHROW
	template<typename ExceptionType = DefaultException, typename... Args>
	[[noreturn]] constexpr void debug_throw( Args &&... args ) {
		daw_throw<ExceptionType>( std::forward<Args>( args )... );
	}
#else
	template<typename ExceptionType = DefaultException, typename... Args>
	constexpr void debug_throw( Args &&... ) {}
#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr void dbg_throw_on_null( ValueType const *const value,
	                                  Args &&... args ) {
		if( nullptr == value ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}
#else
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr void dbg_throw_on_null( ValueType &&, Args &&... ) {}

#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr ValueType *dbg_throw_on_null_or_return( ValueType *value,
	                                                  Args &&... args ) {
		if( nullptr == value ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return value;
	}
#else
	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr ValueType *dbg_throw_on_null_or_return( ValueType *value,
	                                                  Args &&... args ) {
		return value;
	}
#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr auto dbg_throw_on_false_or_return( ValueType &&value, bool test,
	                                             Args &&... args ) {
		if( !static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return std::forward<ValueType>( value );
	}

	template<typename ExceptionType = AssertException, typename... Args>
	constexpr bool dbg_throw_on_false_or_return( bool test, Args &&... args ) {
		if( !static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return true;
	}
#else
#define dbg_throw_on_false_or_return( test, ... ) ( test )
#endif

#ifndef NODEBUGTHROW
	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void dbg_throw_on_false( Bool &&test, Args &&... args ) {
		if( !static_cast<bool>( std::forward<Bool>( test ) ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void DebugAssert( Bool &&test, Args &&... args ) {
		if( !static_cast<bool>( std::forward<Bool>( test ) ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = AssertException>
	constexpr void dbg_throw_on_false( bool test ) {
		if( !static_cast<bool>( test ) ) {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
			throw ExceptionType{};
#else
			std::abort( );
#endif
		}
	}
#else
	template<typename... T>
	constexpr void dbg_throw_on_false( T... ) noexcept {}

	template<typename... T>
	constexpr void DebugAssert( T... ) noexcept {}
#endif

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr ValueType &dbg_throw_on_true_or_return( ValueType &value, bool test,
	                                                  Args &&... args ) {
		if( static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return value;
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr bool dbg_throw_on_true_or_return( const Bool &test,
	                                            Args &&... args ) {
		if( static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return test;
	}

	template<typename ExceptionType = AssertException, typename... Args>
	constexpr void dbg_throw_on_true( bool test, Args &&... args ) {
		if( static_cast<bool>( test ) ) {
			debug_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = NullPtrAccessException, typename ValueType,
	         typename... Args>
	constexpr void daw_throw_on_null( ValueType const *const value,
	                                  Args &&... args ) {
		if( nullptr == value ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr void daw_throw_on_null( ValueType *value, Args &&... args ) {
		if( nullptr == value ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr ValueType *daw_throw_on_null_or_return( ValueType *value,
	                                                  Args &&... args ) {
		if( nullptr == value ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return value;
	}

	template<typename ExceptionType = AssertException, typename... Args,
	         typename Bool>
	constexpr bool daw_throw_on_false_or_return( Bool &&test, Args &&... args ) {
		if( !static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return true;
	}

	struct Terminator {};

	template<typename ExceptionType = Terminator, typename Bool, typename... Args>
	constexpr void precondition_check( Bool &&condition, Args &&... args ) {
		if( !static_cast<bool>( condition ) ) {
			if constexpr( std::is_same<Terminator, ExceptionType>::value ) {
				std::abort( );
			} else {
				daw_throw<ExceptionType>( std::forward<Args>( args )... );
			}
		}
	}

	template<typename ExceptionType = Terminator, typename Bool, typename... Args>
	constexpr void postcondition_check( Bool &&condition, Args &&... args ) {
		if( !static_cast<bool>( condition ) ) {
			if constexpr( std::is_same<Terminator, ExceptionType>::value ) {
				std::abort( );
			} else {
				daw_throw<ExceptionType>( std::forward<Args>( args )... );
			}
		}
	}
#ifndef NODEBUGTHROW
	template<typename Exception = AssertException, typename Bool,
	         typename... Args>
	constexpr void dbg_precondition_check( Bool &&condition, Args &&... ) {

		//			static_assert( std::is_convertible<Bool, bool>::value );
		if( not static_cast<bool>( condition ) ) {
			std::abort( );
		}
	}

	template<typename Bool, typename... Args>
	constexpr void dbg_postcondition_check( Bool &&condition, Args &&... ) {
		static_assert( std::is_convertible<Bool, bool>::value );
		if( not static_cast<bool>( condition ) ) {
			std::abort( );
		}
	}
#else
	template<typename Exception = AssertException, typename Bool,
	         typename... Args>
	constexpr void dbg_precondition_check( Bool &&, Args &&... ) {}

	template<typename Bool, typename... Args>
	constexpr void dbg_postcondition_check( Bool &&, Args &&... ) {}
#endif
	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args, typename Bool>
	constexpr ValueType daw_throw_on_false_or_return( ValueType &&value,
	                                                  Bool &&test,
	                                                  Args &&... args ) {
		if( !static_cast<bool>( std::forward<Bool>( test ) ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return std::forward<ValueType>( value );
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void daw_throw_on_false( Bool &&test, Args &&... args ) {
		if( !static_cast<bool>( std::forward<Bool>( test ) ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr void Assert( Bool const &test, Args &&... args ) {
		if( !static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = DefaultException, typename Bool>
	constexpr void daw_throw_on_false( Bool &&test ) {
		if( !static_cast<bool>( std::forward<Bool>( test ) ) ) {
			daw_throw<ExceptionType>( );
		}
	}

	template<typename ExceptionType = DefaultException, typename Bool>
	constexpr void daw_throw_on_true( Bool const &test ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( );
		}
	}

	template<typename ExceptionType = DefaultException>
	constexpr void daw_throw_value_on_true( ExceptionType &&test ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<ExceptionType>( test ) );
		}
	}

	template<typename ExceptionType>
	constexpr void daw_throw_value_on_false( ExceptionType const &test ) {
		if( !static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<ExceptionType>( test ) );
		}
	}

	template<typename ExceptionType = MethodNotImplemented>
	[[noreturn]] constexpr void daw_throw_not_implemented( ) {
		daw_throw<ExceptionType>( "Method not implemented" );
	}

	template<typename ExceptionType = UnexpectedEnumValue>
	[[noreturn]] constexpr void daw_throw_unexpected_enum( ) {
		daw_throw<ExceptionType>( "Unknown/Unexpected enum" );
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args, typename Bool>
	constexpr ValueType daw_throw_on_true_or_return( ValueType &&value,
	                                                 Bool &&test,
	                                                 Args &&... args ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return std::forward<ValueType>( value );
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args, typename Bool>
	constexpr ValueType &&daw_throw_on_true_or_return( ValueType &&value,
	                                                   Bool &&test,
	                                                   Args &&... args ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return std::forward<ValueType>( value );
	}

	template<typename ExceptionType = AssertException, typename ValueType,
	         typename... Args>
	constexpr ValueType daw_throw_on_true_or_return( ValueType &&value, bool test,
	                                                 Args &&... args ) {
		if( static_cast<bool>( test ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return std::forward<ValueType>( value );
	}

	template<typename ExceptionType = AssertException, typename Bool,
	         typename... Args>
	constexpr bool daw_throw_on_true_or_return( Bool &&test, Args &&... args ) {
		if( static_cast<bool>( std::forward<Bool>( test ) ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
		return false;
	}

	template<typename ExceptionType = DefaultException, typename Bool>
	constexpr void daw_throw_on_true( Bool &&test ) {
		if( static_cast<bool>( std::forward<Bool>( test ) ) ) {
			daw_throw<ExceptionType>( );
		}
	}

	template<
	  typename ExceptionType = AssertException, typename Bool, typename... Args,
	  typename std::enable_if<( sizeof...( Args ) > 0 ), std::nullptr_t>::type =
	    nullptr>
	constexpr void daw_throw_on_true( Bool &&test, Args &&... args ) {
		if( static_cast<bool>( std::forward<Bool>( test ) ) ) {
			daw_throw<ExceptionType>( std::forward<Args>( args )... );
		}
	}

	template<typename ExceptionType = AssertException, typename Predicate,
	         typename Message, typename Container>
	constexpr void assert_all_false( Container &&container,
	                                 Message &&assert_message,
	                                 Predicate &&predicate ) {
		if( std::find_if( std::begin( container ), std::end( container ),
		                  std::forward<Predicate>( predicate ) ) !=
		    std::end( container ) ) {

			daw_throw<ExceptionType>( std::forward<Message>( assert_message ) );
		}
	}

	template<typename ExceptionType = AssertException, typename Predicate,
	         typename Message, typename Container>
	constexpr void assert_all_true( Container const &container,
	                                Message &&assert_message,
	                                Predicate &&predicate ) {
		assert_all_false<ExceptionType>(
		  container, std::forward<Message>( assert_message ),
		  [predicate = std::forward<Predicate>( predicate )]( auto const &v ) {
			  return !predicate( v );
		  } );
	}

	template<typename Function, typename... Args>
	void no_exception( Function func, Args &&... args ) noexcept {
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
		try {
#endif
			func( std::forward<Args>( args )... );
#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
		} catch( ... ) {}
#endif
	}

#if defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or                  \
  defined( _CPPUNWIND )
	template<typename Exception, typename... Args>
	std::exception_ptr make_exception_ptr( Args &&... args ) noexcept {
		try {
			throw Exception{std::forward<Args>( args )...};
		} catch( ... ) { return std::current_exception( ); }
	}
#endif
} // namespace daw::exception
