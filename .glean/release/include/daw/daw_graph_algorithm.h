
// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cpp_17.h"
#include "daw_graph.h"
#include "daw_move.h"

namespace daw {
	struct UnorderedWalk {
		// Defaul that specifies that a walk imposes no ordering on the children
	};
	namespace graph_alg_impl {
		struct NoSort {};

		template<typename Graph, typename Node>
		auto get_child_nodes( Graph &&graph, Node &&node ) {
			using node_t =
			  std::remove_reference_t<decltype( graph.get_node( node.id( ) ) )>;

			std::vector<node_t> result{ };
			for( auto m_id : node.outgoing_edges( ) ) {
				result.push_back( graph.get_node( m_id ) );
			}
			return result;
		}

		template<typename Node, typename T, typename Graph, typename Function,
		         typename Compare>
		void topological_sorted_walk( Graph &&graph, Function &&func,
		                              Compare comp = Compare{ } ) {

			// Find all nodes that do not have incoming entries
			std::vector<Node> root_nodes = [&] {
				auto root_node_ids = graph.find_roots( );
				auto result = std::vector<Node>( );
				result.reserve( root_node_ids.size( ) );
				std::transform( std::begin( root_node_ids ), std::end( root_node_ids ),
				                std::back_inserter( result ),
				                [&]( node_id_t id ) { return graph.get_node( id ); } );
				return result;
			}( );

			constexpr bool perform_sort_v =
			  not std::is_same_v<Compare, daw::graph_alg_impl::NoSort>;

			if constexpr( perform_sort_v ) {
				std::sort( std::begin( root_nodes ), std::end( root_nodes ), comp );
			}

			auto excluded_edges =
			  std::unordered_map<node_id_t, std::vector<node_id_t>>( );

			auto const exclude_edge = [&]( node_id_t from, node_id_t to ) {
				auto &child = excluded_edges[to];
				auto pos = std::find( std::begin( child ), std::end( child ), from );
				if( pos == std::end( child ) ) {
					child.push_back( from );
				}
			};

			auto const has_parent_nodes = [&]( node_id_t n_id ) {
				auto incoming = graph.get_raw_node( n_id ).incoming_edges( );
				if( incoming.empty( ) ) {
					return false;
				}
				if( excluded_edges.count( n_id ) == 0 ) {
					return true;
				}
				for( auto ex_id : excluded_edges[n_id] ) {
					incoming.erase( ex_id );
				}
				return not incoming.empty( );
			};

			while( not root_nodes.empty( ) ) {
				auto node = root_nodes.back( );
				root_nodes.pop_back( );
				func( node );
				{
					auto child_nodes = graph_alg_impl::get_child_nodes( graph, node );
					if constexpr( perform_sort_v ) {
						std::sort( std::begin( child_nodes ), std::end( child_nodes ),
						           [&]( auto &&... args ) {
							           return not comp(
							             std::forward<decltype( args )>( args )... );
						           } );
					}
					for( auto child : child_nodes ) {
						exclude_edge( node.id( ), child.id( ) );
						if( not has_parent_nodes( child.id( ) ) ) {
							root_nodes.push_back( child );
						}
					}
				}
				if constexpr( perform_sort_v ) {
					std::sort( std::begin( root_nodes ), std::end( root_nodes ), comp );
				}
			}
		}

		template<typename T, typename ChildOrder, typename Graph, typename Function>
		void bfs_walk( Graph &&graph, daw::node_id_t start_node_id, Function &&func,
		               ChildOrder ord ) {
			std::unordered_set<daw::node_id_t> visited{ };
			std::deque<daw::node_id_t> path{ };
			path.push_back( start_node_id );

			while( not path.empty( ) ) {
				auto current_node = graph.get_node( path.front( ) );
				path.pop_front( );
				func( current_node );
				visited.insert( current_node.id( ) );
				if constexpr( std::is_same_v<ChildOrder, UnorderedWalk> ) {
					std::copy_if( std::begin( current_node.outgoing_edges( ) ),
					              std::end( current_node.outgoing_edges( ) ),
					              std::back_inserter( path ), [&]( auto const &n_id ) {
						              return visited.count( n_id ) == 0;
					              } );
				} else {
					auto children = std::vector<daw::node_id_t>( );

					std::copy_if(
					  std::begin( current_node.outgoing_edges( ) ),
					  std::end( current_node.outgoing_edges( ) ),
					  std::back_inserter( children ),
					  [&]( auto const &n_id ) { return visited.count( n_id ) == 0; } );

					std::sort( children.begin( ), children.end( ),
					           [&]( daw::node_id_t left_id, daw::node_id_t right_id ) {
						           return ord( graph.get_node( left_id ).value( ),
						                       graph.get_node( right_id ).value( ) );
					           } );
					std::copy( children.begin( ), children.end( ),
					           std::back_inserter( path ) );
				}
			}
		}

		template<typename T, typename ChildOrder, typename Graph, typename Function>
		void dfs_walk( Graph &&graph, daw::node_id_t start_node_id, Function &&func,
		               ChildOrder ord ) {
			std::unordered_set<daw::node_id_t> visited{ };
			std::vector<daw::node_id_t> path{ };
			path.push_back( start_node_id );

			while( not path.empty( ) ) {
				auto current_node = graph.get_node( path.back( ) );
				path.pop_back( );
				func( current_node );
				visited.insert( current_node.id( ) );
				if constexpr( std::is_same_v<ChildOrder, UnorderedWalk> ) {
					std::copy_if( std::begin( current_node.outgoing_edges( ) ),
					              std::end( current_node.outgoing_edges( ) ),
					              std::back_inserter( path ), [&]( auto const &n_id ) {
						              return visited.count( n_id ) == 0;
					              } );
				} else {
					auto children = std::vector<daw::node_id_t>( );
					std::copy_if(
					  std::begin( current_node.outgoing_edges( ) ),
					  std::end( current_node.outgoing_edges( ) ),
					  std::back_inserter( children ),
					  [&]( auto const &n_id ) { return visited.count( n_id ) == 0; } );

					std::sort( children.begin( ), children.end( ),
					           [&]( daw::node_id_t left_id, daw::node_id_t right_id ) {
						           return not ord( graph.get_node( left_id ).value( ),
						                       graph.get_node( right_id ).value( ) );
					           } );
					path.reserve( path.size( ) + children.size( ) );
					std::copy( children.begin( ), children.end( ),
					           std::back_inserter( path ) );
				}
			}
		}
	} // namespace graph_alg_impl

	// Minimum Spanning Tree
	template<typename T, typename Compare = daw::graph_alg_impl::NoSort>
	void mst( daw::graph_t<T> &graph, Compare /*TODO comp*/ = Compare{ } ) {
		auto root_ids = graph.find_roots( );
		for( auto start_node_id : root_ids ) {
			std::unordered_set<daw::node_id_t> visited{ };
			std::vector<std::pair<std::optional<daw::node_id_t>, daw::node_id_t>>
			  path{ };
			path.push_back( { std::nullopt, start_node_id } );

			while( not path.empty( ) ) {
				auto current_id = path.back( );
				path.pop_back( );
				if( visited.count( current_id.second ) > 0 ) {
					daw::exception::precondition_check(
					  current_id.first.has_value( ),
					  "I assumed that the root would have been first visited" );
					graph.remove_directed_edge( *current_id.first, current_id.second );
					continue;
				}
				visited.insert( current_id.second );
				auto current_node = graph.get_node( current_id.second );
				std::transform(
				  std::begin( current_node.outgoing_edges( ) ),
				  std::end( current_node.outgoing_edges( ) ),
				  std::back_inserter( path ), [parent = current_id.second]( auto id ) {
					  return std::pair<std::optional<daw::node_id_t>, node_id_t>( parent,
					                                                              id );
				  } );
			}
		}
	} // namespace daw

	template<typename T, typename Function,
	         typename Compare = daw::graph_alg_impl::NoSort>
	void topological_sorted_walk( daw::graph_t<T> const &graph, Function &&func,
	                              Compare comp = Compare{ } ) {

		using Node = std::remove_reference_t<decltype(
		  graph.get_node( std::declval<daw::node_id_t>( ) ) )>;

		static_assert( std::is_invocable_v<Function, Node> );

		graph_alg_impl::topological_sorted_walk<Node, T>(
		  graph, std::forward<Function>( func ), daw::move( comp ) );
	}

	template<typename T, typename Function,
	         typename Compare = daw::graph_alg_impl::NoSort>
	void topological_sorted_walk( daw::graph_t<T> &graph, Function &&func,
	                              Compare comp = Compare{ } ) {

		using Node = std::remove_reference_t<decltype(
		  graph.get_node( std::declval<daw::node_id_t>( ) ) )>;

		static_assert( std::is_invocable_v<Function, Node> );

		graph_alg_impl::topological_sorted_walk<Node, T>(
		  graph, std::forward<Function>( func ), daw::move( comp ) );
	}

	template<typename Graph, typename Compare = daw::graph_alg_impl::NoSort>
	class topological_sorted_iterator {
		using Node = std::remove_reference_t<decltype(
		  std::declval<Graph>( ).get_node( std::declval<daw::node_id_t>( ) ) )>;

		std::shared_ptr<std::vector<Node>> m_nodes;
		using iterator_t = typename std::vector<Node>::iterator;
		iterator_t m_iterator;

		template<typename G, typename C>
		static std::vector<Node> get_nodes( G &&g, C &&c ) {
			auto result = std::vector<Node>( );
			topological_sorted_walk(
			  std::forward<G>( g ),
			  [&result]( auto &&n ) {
				  result.push_back( std::forward<decltype( n )>( n ) );
			  },
			  std::forward<C>( c ) );
			return result;
		}

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = Node;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;
		using const_reference = value_type const &;

		topological_sorted_iterator( daw::remove_cvref_t<Graph> const &graph,
		                             Compare comp = Compare{ } )
		  : m_nodes( std::make_shared<std::vector<Node>>(
		      get_nodes( graph, std::move( comp ) ) ) )
		  , m_iterator( m_nodes->begin( ) ) {}

		topological_sorted_iterator( daw::remove_cvref_t<Graph> &graph,
		                             Compare comp = Compare{ } )
		  : m_nodes( std::make_shared<std::vector<Node>>(
		      get_nodes( graph, std::move( comp ) ) ) )
		  , m_iterator( m_nodes->begin( ) ) {}

		topological_sorted_iterator end( ) {
			auto result = *this;
			result.m_iterator = m_nodes->end( );
			return result;
		}

		topological_sorted_iterator end( ) const {
			auto result = *this;
			result.m_iterator = result.end( );
			return result;
		}

		topological_sorted_iterator &operator+=( difference_type n ) noexcept {
			m_iterator += n;
			return *this;
		}

		topological_sorted_iterator &operator-=( difference_type n ) noexcept {
			m_iterator -= n;
			return *this;
		}

		reference operator*( ) noexcept {
			return *m_iterator;
		}

		const_reference operator*( ) const noexcept {
			return *m_iterator;
		}

		pointer operator->( ) noexcept {
			return &( *m_iterator );
		}

		const_pointer operator->( ) const noexcept {
			return &( *m_iterator );
		}

		topological_sorted_iterator &operator++( ) noexcept {
			++m_iterator;
			return *this;
		}

		topological_sorted_iterator operator++( int ) noexcept {
			auto result = *this;
			++m_iterator;
			return result;
		}

		topological_sorted_iterator &operator--( ) noexcept {
			--m_iterator;
			return *this;
		}

		topological_sorted_iterator operator--( int ) noexcept {
			auto result = *this;
			--m_iterator;
			return result;
		}

		topological_sorted_iterator operator+( difference_type n ) const noexcept {
			auto result = *this;
			result.m_iterator += n;
			return result;
		}

		topological_sorted_iterator operator-( difference_type n ) const noexcept {
			auto result = *this;
			result.m_iterator -= n;
			return result;
		}

		size_t size( ) const noexcept {
			return static_cast<size_t>(
			  std::distance( m_iterator, m_nodes->end( ) ) );
		}

		bool empty( ) const noexcept {
			return m_iterator == m_nodes->end( );
		}

		friend bool operator==( topological_sorted_iterator const &lhs,
		                        topological_sorted_iterator const &rhs ) noexcept {
			return lhs.m_iterator == rhs.m_iterator;
		}

		friend bool operator!=( topological_sorted_iterator const &lhs,
		                        topological_sorted_iterator const &rhs ) noexcept {
			return lhs.m_iterator != rhs.m_iterator;
		}

		friend bool operator<( topological_sorted_iterator const &lhs,
		                       topological_sorted_iterator const &rhs ) noexcept {
			return lhs.m_iterator < rhs.m_iterator;
		}

		friend bool operator<=( topological_sorted_iterator const &lhs,
		                        topological_sorted_iterator const &rhs ) noexcept {
			return lhs.m_iterator <= rhs.m_iterator;
		}

		friend bool operator>( topological_sorted_iterator const &lhs,
		                       topological_sorted_iterator const &rhs ) noexcept {
			return lhs.m_iterator > rhs.m_iterator;
		}

		friend bool operator>=( topological_sorted_iterator const &lhs,
		                        topological_sorted_iterator const &rhs ) noexcept {
			return lhs.m_iterator >= rhs.m_iterator;
		}
	};

	template<typename Graph, typename Compare = daw::graph_alg_impl::NoSort>
	auto make_topological_sorted_range( Graph &&g, Compare c = Compare{ } ) {
		static_assert( not std::is_rvalue_reference_v<Graph>,
		               "Temporaries are not supported" );
		auto frst = topological_sorted_iterator<Graph, Compare>(
		  std::forward<Graph>( g ), std::forward<Compare>( c ) );
		using iterator_t = decltype( frst );
		struct result_t {
			iterator_t first;
			iterator_t last;

			iterator_t begin( ) const {
				return first;
			}

			std::reverse_iterator<iterator_t> rbegin( ) const {
				return std::reverse_iterator<iterator_t>( first );
			}

			iterator_t end( ) const {
				return last;
			}

			std::reverse_iterator<iterator_t> rend( ) const {
				return std::reverse_iterator<iterator_t>( last );
			}

			size_t size( ) const noexcept {
				return first.size( );
			}

			bool empty( ) const noexcept {
				return first.empty( );
			}
		};
		auto l = frst.end( );
		return result_t{ std::move( frst ), std::move( l ) };
	}

	template<typename Graph, typename Compare = daw::graph_alg_impl::NoSort>
	auto make_reverse_topological_sorted_range( Graph &&g,
	                                            Compare c = Compare{ } ) {
		static_assert( not std::is_rvalue_reference_v<Graph>,
		               "Temporaries are not supported" );
		auto frst = topological_sorted_iterator<Graph, Compare>(
		  std::forward<Graph>( g ), std::forward<Compare>( c ) );
		using iterator_t = decltype( frst );
		struct result_t {
			iterator_t first;
			iterator_t last;

			iterator_t rbegin( ) const {
				return first;
			}

			std::reverse_iterator<iterator_t> begin( ) const {
				return std::reverse_iterator<iterator_t>( first );
			}

			iterator_t rend( ) const {
				return last;
			}

			std::reverse_iterator<iterator_t> end( ) const {
				return std::reverse_iterator<iterator_t>( last );
			}

			size_t size( ) const noexcept {
				return first.size( );
			}

			bool empty( ) const noexcept {
				return first.empty( );
			}
		};
		auto l = frst.end( );
		return result_t{ std::move( frst ), std::move( l ) };
	}

	template<typename T, typename Func,
	         typename Compare = daw::graph_alg_impl::NoSort>
	void reverse_topological_sorted_walk( daw::graph_t<T> const &known_deps,
	                                      Func visitor,
	                                      Compare &&comp = Compare{ } ) {
		auto nodes = std::vector<daw::node_id_t>( );
		topological_sorted_walk(
		  known_deps, [&]( auto const &n ) { nodes.push_back( n.id( ) ); },
		  std::forward<Compare>( comp ) );

		std::reverse( nodes.begin( ), nodes.end( ) );
		for( auto const &id : nodes ) {
			auto cur_node = known_deps.get_node( id );
			(void)visitor( cur_node );
		}
	}

	template<typename T, typename Func,
	         typename Compare = daw::graph_alg_impl::NoSort>
	void reverse_topological_sorted_walk( daw::graph_t<T> &known_deps,
	                                      Func visitor,
	                                      Compare &&comp = Compare{ } ) {
		auto nodes = std::vector<daw::node_id_t>( );
		topological_sorted_walk(
		  known_deps, [&]( auto const &n ) { nodes.push_back( n.id( ) ); },
		  std::forward<Compare>( comp ) );

		std::reverse( nodes.begin( ), nodes.end( ) );
		for( auto const &id : nodes ) {
			auto cur_node = known_deps.get_node( id );
			(void)visitor( cur_node );
		}
	}

	template<typename ChildOrder = UnorderedWalk, typename T, typename Function>
	void bfs_walk( daw::graph_t<T> const &graph, daw::node_id_t start_node_id,
	               Function &&func, ChildOrder ord = ChildOrder{ } ) {

		graph_alg_impl::bfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ), ord );
	}

	template<typename ChildOrder = UnorderedWalk, typename T, typename Function>
	void bfs_walk( daw::graph_t<T> &graph, daw::node_id_t start_node_id,
	               Function &&func, ChildOrder ord = ChildOrder{ } ) {

		graph_alg_impl::bfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ), ord );
	}

	template<typename T, typename Function, typename ChildOrder = UnorderedWalk>
	void dfs_walk( daw::graph_t<T> const &graph, daw::node_id_t start_node_id,
	               Function &&func, ChildOrder ord = ChildOrder{ } ) {

		graph_alg_impl::dfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ), ord );
	}

	template<typename T, typename Function, typename ChildOrder = UnorderedWalk>
	void dfs_walk( daw::graph_t<T> &graph, daw::node_id_t start_node_id,
	               Function &&func, ChildOrder ord = ChildOrder{ } ) {

		graph_alg_impl::dfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ), ord );
	}

} // namespace daw
