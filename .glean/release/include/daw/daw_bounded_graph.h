
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

#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

#include "daw_algorithm.h"
#include "daw_bounded_hash_map.h"
#include "daw_bounded_hash_set.h"
#include "daw_bounded_vector.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_move.h"
#include "daw_utility.h"
#include "iterator/daw_back_inserter.h"

namespace daw {
	template<typename T, size_t MaxVerticesPerNode, typename Hash = std::hash<T>>
	struct bounded_graph_t;

	class node_id_t {
		static inline constexpr size_t const NO_ID =
		  std::numeric_limits<size_t>::max( );

		size_t m_value = NO_ID;

		constexpr size_t value( ) const noexcept {
			daw::exception::dbg_precondition_check( m_value != NO_ID );
			return m_value;
		}

		template<typename T, size_t MaxVerticesPerNode, typename Hash>
		friend struct bounded_graph_t;

	public:
		constexpr node_id_t( ) noexcept = default;
		explicit constexpr node_id_t( size_t id ) noexcept
		  : m_value( id ) {}

		constexpr explicit operator bool( ) const noexcept {
			return m_value != NO_ID;
		}

		constexpr size_t hash( ) const noexcept {
			return m_value; // daw::fnv1a_hash_t{}( m_value );
		}

		constexpr int compare( node_id_t const &rhs ) const noexcept {
			if( m_value == rhs.m_value ) {
				return 0;
			}
			if( m_value < rhs.m_value ) {
				return -1;
			}
			return 1;
		}
	};

	constexpr bool operator==( node_id_t const &lhs,
	                           node_id_t const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	constexpr bool operator!=( node_id_t const &lhs,
	                           node_id_t const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	constexpr bool operator<( node_id_t const &lhs,
	                          node_id_t const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}
} // namespace daw

namespace std {
	template<>
	struct hash<daw::node_id_t> {
		constexpr size_t operator( )( daw::node_id_t id ) const noexcept {
			return id.hash( );
		}
	};
} // namespace std

namespace daw {
	namespace bounded_graph_impl {
		template<typename T, size_t MaxVerticesPerNode>
		class node_impl_t {
			node_id_t m_id{};
			T m_value{};

		public:
			using value_type = T;
			using reference = value_type &;
			using const_reference = value_type const &;
			using edges_t = daw::bounded_hash_set_t<node_id_t, MaxVerticesPerNode,
			                                        std::hash<daw::node_id_t>>;

		private:
			edges_t m_incoming_edges{};
			edges_t m_outgoing_edges{};

		public:
			constexpr node_impl_t( ) noexcept = default;

			constexpr node_impl_t( node_id_t id, T &&value ) noexcept
			  : m_id( id )
			  , m_value( daw::move( value ) ) {

				daw::exception::dbg_precondition_check( id );
			}

			constexpr node_id_t id( ) const {
				return m_id;
			}

			constexpr bool empty( ) const {
				return !static_cast<bool>( m_id );
			}

			explicit constexpr operator bool( ) const noexcept {
				return static_cast<bool>( m_id );
			}

			constexpr reference value( ) {
				return m_value;
			}

			constexpr const_reference value( ) const {
				return m_value;
			}

			constexpr edges_t &incoming_edges( ) {
				return m_incoming_edges;
			}

			constexpr edges_t const &incoming_edges( ) const {
				return m_incoming_edges;
			}

			constexpr edges_t &outgoing_edges( ) {
				return m_outgoing_edges;
			}

			constexpr edges_t const &outgoing_edges( ) const {
				return m_outgoing_edges;
			}
		};
	} // namespace bounded_graph_impl

	class invalid_node_exception {
		std::optional<size_t> m_id = {};

	public:
		constexpr invalid_node_exception( ) noexcept = default;
		explicit constexpr invalid_node_exception( size_t Id ) noexcept
		  : m_id( Id ) {}

		constexpr bool has_id( ) const {
			return static_cast<bool>( m_id );
		}
		constexpr size_t id( ) const {
			return *m_id;
		}
	};

	template<typename>
	struct bounded_graph_node_proxies : std::false_type {};

	template<typename T>
	inline constexpr bool is_bounded_graph_node_v =
	  bounded_graph_node_proxies<T>::value;

	template<typename T, size_t MaxVerticesPerNode, typename Hash>
	class const_graph_node_t {
		bounded_graph_t<T, MaxVerticesPerNode, Hash> const *m_graph = nullptr;
		node_id_t m_node_id{};

	public:
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using edges_t =
		  typename bounded_graph_impl::node_impl_t<T, MaxVerticesPerNode>::edges_t;

		constexpr const_graph_node_t( ) noexcept = default;

		constexpr const_graph_node_t(
		  bounded_graph_t<T, MaxVerticesPerNode, Hash> const *graph_ptr,
		  node_id_t Id ) noexcept
		  : m_graph( graph_ptr )
		  , m_node_id( Id ) {}

		constexpr node_id_t id( ) const noexcept {
			return m_node_id;
		}

		constexpr bounded_graph_t<T, MaxVerticesPerNode, Hash> const *graph( ) const
		  noexcept {
			return m_graph;
		}

		constexpr bool empty( ) const noexcept {
			return m_graph == nullptr or !static_cast<bool>( m_node_id );
		}

		explicit constexpr operator bool( ) const noexcept {
			return m_graph != nullptr and static_cast<bool>( m_node_id );
		}

		constexpr const_reference value( ) const {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).value( );
		}

		constexpr edges_t const &incoming_edges( ) const {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).incoming_edges( );
		}

		constexpr edges_t const &outgoing_edges( ) const {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).outgoing_edges( );
		}

		template<typename Rhs>
		constexpr bool operator==( Rhs const &rhs ) noexcept {
			static_assert( is_bounded_graph_node_v<Rhs>,
			               "Can only do comparison with another graph node proxy" );
			return m_node_id == rhs.id( ) or
			       std::equal_to<>{}( m_graph, rhs.graph( ) );
		}

		template<typename Rhs>
		constexpr bool operator!=( Rhs const &rhs ) noexcept {
			static_assert( is_bounded_graph_node_v<Rhs>,
			               "Can only do comparison with another graph node proxy" );
			return m_node_id != rhs.id( ) or
			       std::not_equal_to<>{}( m_graph, rhs.graph( ) );
		}

		template<typename Rhs>
		constexpr bool operator<( Rhs const &rhs ) noexcept {
			static_assert( is_bounded_graph_node_v<Rhs>,
			               "Can only do comparison with another graph node proxy" );
			daw::exception::dbg_precondition_check(
			  std::equal_to<>{}( m_graph, rhs.graph( ) ) );
			return m_node_id < rhs.id( );
		}
	};

	template<typename T, size_t MaxVerticesPerNode, typename Hash>
	struct bounded_graph_node_proxies<
	  const_graph_node_t<T, MaxVerticesPerNode, Hash>> : std::true_type {};

	template<typename T, size_t MaxVerticesPerNode, typename Hash = std::hash<T>>
	class bounded_graph_node_t {
		bounded_graph_t<T, MaxVerticesPerNode, Hash> *m_graph = nullptr;
		node_id_t m_node_id{};

	public:
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using edges_t =
		  typename bounded_graph_impl::node_impl_t<T, MaxVerticesPerNode>::edges_t;

		constexpr bounded_graph_node_t( ) noexcept = default;

		constexpr bounded_graph_node_t(
		  bounded_graph_t<T, MaxVerticesPerNode, Hash> *graph_ptr,
		  node_id_t Id ) noexcept
		  : m_graph( graph_ptr )
		  , m_node_id( Id ) {}

		constexpr node_id_t id( ) const noexcept {
			return m_node_id;
		}

		constexpr bounded_graph_t<T, MaxVerticesPerNode, Hash> const *graph( ) const
		  noexcept {
			return m_graph;
		}

		constexpr bool empty( ) const noexcept {
			return m_graph == nullptr or !static_cast<bool>( m_node_id );
		}

		explicit constexpr operator bool( ) const noexcept {
			return m_graph != nullptr and static_cast<bool>( m_node_id );
		}

		constexpr reference value( ) {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).value( );
		}

		constexpr const_reference value( ) const {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).value( );
		}

		constexpr edges_t &incoming_edges( ) {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).incoming_edges( );
		}

		constexpr edges_t const &incoming_edges( ) const {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).incoming_edges( );
		}

		constexpr edges_t &outgoing_edges( ) {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).outgoing_edges( );
		}

		constexpr edges_t const &outgoing_edges( ) const {
			daw::exception::dbg_precondition_check<invalid_node_exception>(
			  m_graph != nullptr and m_node_id != node_id_t{} );
			return m_graph->get_raw_node( m_node_id ).outgoing_edges( );
		}

		template<typename Rhs>
		constexpr bool operator==( Rhs const &rhs ) noexcept {
			static_assert( is_bounded_graph_node_v<Rhs>,
			               "Can only do comparison with another graph node proxy" );
			return m_node_id == rhs.id( ) or
			       std::equal_to<>{}( m_graph, rhs.graph( ) );
		}

		template<typename Rhs>
		constexpr bool operator!=( Rhs const &rhs ) noexcept {
			static_assert( is_bounded_graph_node_v<Rhs>,
			               "Can only do comparison with another graph node proxy" );
			return m_node_id != rhs.id( ) or
			       std::not_equal_to<>{}( m_graph, rhs.graph( ) );
		}

		template<typename Rhs>
		constexpr bool operator<( Rhs const &rhs ) noexcept {
			static_assert( is_bounded_graph_node_v<Rhs>,
			               "Can only do comparison with another graph node proxy" );
			daw::exception::dbg_precondition_check(
			  std::equal_to<>{}( m_graph, rhs.graph( ) ) );
			return m_node_id < rhs.id( );
		}

		constexpr operator const_graph_node_t<T, MaxVerticesPerNode, Hash>( ) const
		  noexcept {
			return const_graph_node_t<T, MaxVerticesPerNode, Hash>( m_graph,
			                                                        m_node_id );
		}
	};

	template<typename T, size_t MaxVerticesPerNode, typename Hash>
	struct bounded_graph_node_proxies<
	  bounded_graph_node_t<T, MaxVerticesPerNode, Hash>> : std::true_type {};

	template<typename T, size_t MaxVerticesPerNode, typename Hash>
	struct bounded_graph_t {
		using raw_node_t = bounded_graph_impl::node_impl_t<T, MaxVerticesPerNode>;
		using node_t = bounded_graph_node_t<T, MaxVerticesPerNode, Hash>;
		using const_node_t = const_graph_node_t<T, MaxVerticesPerNode, Hash>;

	private:
		size_t cur_id = 0;
		daw::bounded_hash_map<size_t, raw_node_t, MaxVerticesPerNode, Hash>
		  m_nodes{};

	public:
		constexpr bounded_graph_t( ) noexcept = default;

		template<typename... Args>
		constexpr node_id_t add_node( Args &&... args ) {
			auto const id_value = cur_id++;
			auto id = node_id_t( id_value );

			m_nodes.insert(
			  id_value,
			  bounded_graph_impl::node_impl_t<T, MaxVerticesPerNode>(
			    id, daw::construct_a<T>( std::forward<Args>( args )... ) ) );

			return id;
		}

		constexpr bool has_node( node_id_t id ) const {
			return m_nodes.exists( id.value( ) );
		}

		// Be very careful when storing as the node
		// can be removed
		constexpr raw_node_t &get_raw_node( node_id_t id ) noexcept {
			daw::exception::dbg_precondition_check( has_node( id ) );
			auto pos = m_nodes.find( id.value( ) );
			return pos->value;
		}

		// Be very careful when storing as the node
		// can be removed
		constexpr raw_node_t const &get_raw_node( node_id_t id ) const noexcept {
			daw::exception::dbg_precondition_check( has_node( id ) );
			auto pos = m_nodes.find( id.value( ) );
			return pos->value;
		}

		constexpr const_node_t get_node( node_id_t id ) const noexcept {
			daw::exception::dbg_precondition_check( has_node( id ) );
			return const_node_t( this, id );
		}

		constexpr node_t get_node( node_id_t id ) noexcept {
			daw::exception::dbg_precondition_check( has_node( id ) );
			return node_t( this, id );
		}

		constexpr size_t size( ) const noexcept {
			return m_nodes.size( );
		}

		constexpr void add_directed_edge( node_id_t from, node_id_t to ) {
			daw::exception::dbg_precondition_check( has_node( from ) );
			daw::exception::dbg_precondition_check( has_node( to ) );

			auto &n_from = get_raw_node( from );
			auto &n_to = get_raw_node( to );

			n_from.outgoing_edges( ).insert( to );
			n_to.incoming_edges( ).insert( from );
		}

		constexpr void add_undirected_edge( node_id_t node0, node_id_t node1 ) {
			daw::exception::dbg_precondition_check( has_node( node0 ) );
			daw::exception::dbg_precondition_check( has_node( node1 ) );

			auto &n_0 = get_raw_node( node0 );
			auto &n_1 = get_raw_node( node1 );

			n_0.outgoing_edges( ).insert( node1 );
			n_0.incoming_edges( ).insert( node1 );
			n_1.outgoing_edges( ).insert( node0 );
			n_1.incoming_edges( ).insert( node0 );
		}

		constexpr void remove_node( node_id_t id ) {
			daw::exception::dbg_precondition_check( has_node( id ) );

			auto &node = get_raw_node( id );
			for( auto n_id : node.incoming_edges( ) ) {
				get_node( n_id ).outgoing_edges( ).erase( id );
			}
			for( auto n_id : node.outgoing_edges( ) ) {
				get_node( n_id ).incoming_edges( ).erase( id );
			}
			m_nodes.erase( id.value( ) );
		}

		constexpr void remove_directed_edge( node_id_t from, node_id_t to ) {
			daw::exception::dbg_precondition_check( has_node( from ) );
			daw::exception::dbg_precondition_check( has_node( to ) );

			raw_node_t &from_node = get_raw_node( from );
			raw_node_t &to_node = get_raw_node( to );
			from_node.outgoing_edges( ).erase( to );
			to_node.incoming_edges( ).erase( from );
		}

		constexpr void remove_edges( node_id_t node0_id, node_id_t node1_id ) {
			daw::exception::dbg_precondition_check( has_node( node0_id ) );
			daw::exception::dbg_precondition_check( has_node( node1_id ) );

			raw_node_t &inode0 = get_raw_node( node0_id );
			raw_node_t &node1 = get_raw_node( node1_id );
			inode0.outgoing_edges( ).erase( node1_id );
			inode0.incoming_edges( ).erase( node1_id );
			node1.outgoing_edges( ).erase( node0_id );
			node1.incoming_edges( ).erase( node0_id );
		}

		template<typename Compare = std::equal_to<>>
		constexpr daw::bounded_vector_t<node_id_t, MaxVerticesPerNode>
		find_by_value( T const &value, Compare compare = Compare{} ) const {
			daw::bounded_vector_t<node_id_t, MaxVerticesPerNode> result{};

			for( auto const &node : m_nodes ) {
				if( daw::invoke( compare, node.value.value( ), value ) ) {
					result.push_back( node.value.id( ) );
				}
			}
			return result;
		}

		template<typename Predicate, typename Visitor>
		constexpr void visit( Predicate &&pred, Visitor &&vis ) {
			static_assert( std::is_invocable_v<Predicate, raw_node_t const &>,
			               "Predicate must accept a node as argument" );
			static_assert( std::is_invocable_v<Visitor, raw_node_t &>,
			               "Visitor must accept a node as argument" );
			for( auto &node_ref : m_nodes ) {
				auto node = get_node( node_ref.value.id( ) );
				if( daw::invoke( pred, node ) ) {
					daw::invoke( vis, daw::move( node ) );
				}
			}
		}

		template<typename Predicate, typename Visitor>
		constexpr void visit( Predicate &&pred, Visitor &&vis ) const {
			static_assert( std::is_invocable_v<Predicate, raw_node_t const &>,
			               "Predicate must accept a node as argument" );
			static_assert( std::is_invocable_v<Visitor, raw_node_t const &>,
			               "Visitor must accept a node as argument" );
			for( auto const &node_ref : m_nodes ) {
				auto node = get_node( node_ref.value.id( ) );
				if( daw::invoke( pred, node ) ) {
					daw::invoke( vis, daw::move( node ) );
				}
			}
		}

		template<typename Predicate>
		constexpr daw::bounded_vector_t<node_id_t, MaxVerticesPerNode>
		find( Predicate &&pred ) const {
			daw::bounded_vector_t<node_id_t, MaxVerticesPerNode> result{};
			visit( pred, [&result]( auto const &node ) {
				result.push_back( node.id( ) );
			} );
			return result;
		}

		constexpr daw::bounded_vector_t<node_id_t, MaxVerticesPerNode>
		find_roots( ) const {
			return find(
			  []( auto const &node ) { return node.incoming_edges( ).empty( ); } );
		}

		constexpr daw::bounded_vector_t<node_id_t, MaxVerticesPerNode>
		find_leaves( ) const {
			return find(
			  []( auto const &node ) { return node.outgoing_edges( ).empty( ); } );
		}
	};

} // namespace daw
