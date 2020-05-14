// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

#include "daw_move.h"

namespace daw {
	namespace mixins {
		template<typename Derived, typename ContainerType>
		struct ContainerProxy {
			using container_type = ContainerType;
			using iterator = typename container_type::iterator;
			using const_iterator = typename container_type::const_iterator;
			using value_type = typename container_type::value_type;
			using reference = typename container_type::reference;
			using const_reference = typename container_type::const_reference;
			using size_type = typename container_type::size_type;

		private:
			Derived &derived( ) {
				return *static_cast<Derived *>( this );
			}

			Derived const &derived( ) const {
				return *static_cast<Derived const *>( this );
			}

		public:
			iterator begin( ) {
				return derived( ).container( ).begin( );
			}

			iterator end( ) {
				return derived( ).container( ).end( );
			}

			const_iterator begin( ) const {
				return derived( ).container( ).begin( );
			}

			const_iterator end( ) const {
				return derived( ).container( ).end( );
			}

			const_iterator cbegin( ) {
				return derived( ).container( ).cbegin( );
			}

			const_iterator cend( ) {
				return derived( ).container( ).cend( );
			}

			iterator insert( iterator where, value_type item ) {
				return derived( ).container( ).insert( where, daw::move( item ) );
			}

			template<typename... Args>
			void emplace( iterator where, Args &&... args ) {
				return derived( ).container( ).emplace( where,
				                                        std::forward<Args>( args )... );
			}

			size_type size( ) const {
				return std::distance( cbegin( ), cend( ) );
			}
		};

		//////////////////////////////////////////////////////////////////////////
		/// Summary:	Provides a minimal interface for a vector like container
		///				with a member that is a container.
		///				Requires a member in Derived called container( ) that
		///				returns the container
		template<typename Derived, typename container_type>
		class VectorLikeProxy : public ContainerProxy<Derived, container_type> {
			using base_t = ContainerProxy<Derived, container_type>;

		public:
			void push_back( typename base_t::value_type &&value ) {
				this->insert( this->end( ), daw::move( value ) );
			}

			void push_back( typename base_t::const_reference value ) {
				this->insert( this->end( ), value );
			}

			template<typename... Args>
			void emplace_back( Args &&... args ) {
				this->emplace( this->end( ), std::forward<Args>( args )... );
			}

			typename base_t::reference operator[]( typename base_t::size_type pos ) {
				return *( this->begin( ) + pos );
			}

			typename base_t::const_reference
			operator[]( typename base_t::size_type pos ) const {
				return *( this->cbegin( ) + pos );
			}
		};

		/////////////////////////////////////11/////////////////////////////////////
		/// Summary:	Provides a minimal interface for a map like container
		///				with a member that is a container.
		///				Requires a member in Derived called container( ) that
		///				returns the container, find( ) that searches
		///				for a key and key_type and mapped_type
		template<typename Derived, typename MapType>
		struct MapLikeProxy : public ContainerProxy<Derived, MapType> {
			using key_type = typename MapType::key_type;
			using mapped_type = typename MapType::mapped_type;

		private:
			using base_t = ContainerProxy<Derived, MapType>;

		public:
			typename base_t::reference operator[]( key_type key ) {
				return this->derived( ).find( key );
			}

			typename base_t::const_reference operator[]( key_type key ) const {
				return this->derived( ).find( key );
			}
		};
	} // namespace mixins
} // namespace daw
