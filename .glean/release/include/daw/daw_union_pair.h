// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
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
#include <cstdint>
#include <cstdlib>
#include <utility>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_move.h"

namespace daw {
	struct visit_null_union_pair_exception {};
	template<typename Type0, typename Type1>
	class union_pair_t {
		enum class data_types : uint_least8_t { type_0, type_1, type_nothing };

		union {
			std::nullptr_t nothing;
			Type0 value0;
			Type1 value1;
		};

		data_types type = data_types::type_nothing;

		constexpr Type0 *get_type0_ptr( ) noexcept {
			return &value0;
		}

		constexpr Type0 const *get_type0_ptr( ) const noexcept {
			return &value0;
		}

		constexpr Type1 *get_type1_ptr( ) noexcept {
			return &value1;
		}

		constexpr Type1 const *get_type1_ptr( ) const noexcept {
			return &value1;
		}

		constexpr void destroy_type0( ) {
			type = data_types::type_nothing;
			get_type0_ptr( )->~Type0( );
			nothing = nullptr;
		}

		constexpr void destroy_type1( ) {
			type = data_types::type_nothing;
			get_type1_ptr( )->~Type1( );
			nothing = nullptr;
		}

		constexpr void clear( ) noexcept {
			switch( type ) {
			case data_types::type_nothing:
				break;
			case data_types::type_0:
				destroy_type0( );
				break;
			case data_types::type_1:
				destroy_type1( );
				break;
			}
		}

		template<typename T>
		constexpr void store_type0( T &&value ) {
			static_assert( std::is_convertible_v<T, Type0>,
			               "Incompatible type T cannot convert to Type0" );

			if( type != data_types::type_0 ) {
				clear( );
			}
			type = data_types::type_nothing;
			value0 = std::forward<T>( value );
			type = data_types::type_0;
		}

		template<typename T>
		constexpr void store_type1( T &&value ) {
			static_assert( std::is_convertible_v<T, Type1>,
			               "Incompatible type T cannot convert to Type1" );

			if( type != data_types::type_1 ) {
				clear( );
			}
			type = data_types::type_nothing;
			value1 = std::forward<T>( value );
			type = data_types::type_1;
		}

	public:
		constexpr union_pair_t( ) noexcept
		  : type( data_types::type_nothing ) {}

		~union_pair_t( ) noexcept {
			clear( );
		}

		constexpr uint_least8_t which( ) const noexcept {
			return static_cast<uint_least8_t>( type );
		}

		constexpr union_pair_t( Type0 &&ptr ) noexcept
		  : type( data_types::type_nothing ) {

			store_type0( daw::move( ptr ) );
		}

		constexpr union_pair_t( Type0 const &ptr )
		  : type( data_types::type_nothing ) {

			store_type0( ptr );
		}

		constexpr union_pair_t &operator=( Type0 const &val ) {
			store_type0( val );
			return *this;
		}

		constexpr union_pair_t &operator=( Type0 &&val ) noexcept {
			store_type0( daw::move( val ) );
			return *this;
		}

		constexpr union_pair_t &operator=( Type1 const &val ) {
			store_type1( val );
			return *this;
		}

		constexpr union_pair_t &operator=( Type1 &&val ) noexcept {
			store_type1( daw::move( val ) );
			return *this;
		}

		constexpr union_pair_t( Type1 &&ptr ) noexcept
		  : type( data_types::type_nothing ) {

			store_type1( daw::move( ptr ) );
		}

		constexpr union_pair_t( Type1 const &ptr )
		  : type( data_types::type_nothing ) {

			store_type1( ptr );
		}

		constexpr union_pair_t( union_pair_t const &other )
		  : type( data_types::type_nothing ) {

			switch( other.type ) {
			case data_types::type_0:
				store_type0( *other.get_type0_ptr( ) );
				break;
			case data_types::type_1:
				store_type1( *other.get_type1_ptr( ) );
				break;
			case data_types::type_nothing:
				nothing = nullptr;
				break;
			}
		}

		constexpr union_pair_t( union_pair_t &&other ) noexcept
		  : type( data_types::type_nothing ) {

			switch( other.type ) {
			case data_types::type_0:
				store_type0( daw::move( *other.get_type0_ptr( ) ) );
				break;
			case data_types::type_1:
				store_type1( daw::move( *other.get_type1_ptr( ) ) );
				break;
			case data_types::type_nothing:
				nothing = nullptr;
				break;
			}
		}

		constexpr union_pair_t &operator=( union_pair_t const &rhs ) {
			if( this != &rhs ) {
				switch( rhs.type ) {
				case data_types::type_0:
					store_type0( *rhs.get_type0_ptr( ) );
					break;
				case data_types::type_1:
					store_type1( *rhs.get_type1_ptr( ) );
					break;
				case data_types::type_nothing:
					clear( );
					break;
				}
			}
			return *this;
		}

		constexpr union_pair_t &operator=( union_pair_t &&rhs ) noexcept {
			if( this != &rhs ) {
				switch( rhs.type ) {
				case data_types::type_0:
					store_type0( daw::move( *rhs.get_type0_ptr( ) ) );
					break;
				case data_types::type_1:
					store_type1( daw::move( *rhs.get_type1_ptr( ) ) );
					break;
				case data_types::type_nothing:
					clear( );
					break;
				}
			}
			return *this;
		}

		template<typename Visitor>
		constexpr decltype( auto ) visit( Visitor &&vis ) {
			switch( type ) {
			case data_types::type_0:
				return vis( *get_type0_ptr( ) );
			case data_types::type_1:
				return vis( *get_type1_ptr( ) );
			case data_types::type_nothing:
				break;
			}
			daw::exception::daw_throw<visit_null_union_pair_exception>( );
		}

		template<typename Visitor>
		constexpr decltype( auto ) visit( Visitor &&vis ) const {
			switch( type ) {
			case data_types::type_0:
				return vis( *get_type0_ptr( ) );
			case data_types::type_1:
				return vis( *get_type1_ptr( ) );
			case data_types::type_nothing:
				break;
			}
			daw::exception::daw_throw<visit_null_union_pair_exception>( );
		}
	};
} // namespace daw
