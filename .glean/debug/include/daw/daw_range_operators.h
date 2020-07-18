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

#include <algorithm>
#include <iterator>
#include <numeric>
#include <typeinfo>
#include <utility>
#include <vector>

#include "daw_algorithm.h"
#include "daw_move.h"
#include "daw_range_collection.h"
#include "daw_range_reference.h"
#include "daw_traits.h"

#if not defined( _MSC_VER ) or defined( __clang__ ) // internal compiler error on at least <= MSVC 2015.3

namespace daw {
	namespace range {
		namespace operators {
			namespace details {
				template<int...>
				struct seq {};

				template<int N, int... S>
				struct gens : gens<N - 1, N - 1, S...> {};

				template<int... S>
				struct gens<0, S...> {
					typedef seq<S...> type;
				};
			} // namespace details
		}   // namespace operators
	}     // namespace range
} // namespace daw

#define DAW_RANGE_GENERATE_VCLAUSE( clause_name )                              \
	namespace daw {                                                              \
		namespace range {                                                          \
			namespace operators {                                                    \
				namespace details {                                                    \
					template<typename... Args>                                           \
					class clause_name##_t {                                              \
						std::tuple<Args...> clause_name##_args;                            \
                                                                               \
						template<                                                          \
						  typename Container, typename... ClauseArgs,                      \
						  typename std::enable_if_t<daw::all_true_v<                       \
						    !daw::range::is_range_reference_v<Container>,                  \
						    !daw::range::is_range_collection_v<Container>>> * = nullptr,   \
						  typename = void>                                                 \
						static auto clause_name##_helper( Container const &container,      \
						                                  ClauseArgs &&... clause_args ) { \
							return from( container )                                         \
							  .clause_name( std::forward<ClauseArgs>( clause_args )... );    \
						}                                                                  \
                                                                               \
						template<                                                          \
						  typename Container, typename... ClauseArgs,                      \
						  typename std::enable_if_t<                                       \
						    daw::range::is_range_reference_v<Container>> * = nullptr>      \
						static auto clause_name##_helper( Container container,             \
						                                  ClauseArgs &&... clause_args ) { \
							return container.clause_name(                                    \
							  std::forward<ClauseArgs>( clause_args )... );                  \
						}                                                                  \
                                                                               \
						template<                                                          \
						  typename Container, typename... ClauseArgs,                      \
						  typename std::enable_if_t<                                       \
						    daw::range::is_range_collection_v<Container>> * = nullptr>     \
						static auto clause_name##_helper( Container const &container,      \
						                                  ClauseArgs &&... clause_args ) { \
							return container.clause_name(                                    \
							  std::forward<ClauseArgs>( clause_args )... );                  \
						}                                                                  \
                                                                               \
						template<typename Container>                                       \
						auto delayed_dispatch( Container &&container ) const {             \
							return call_##clause_name(                                       \
							  std::forward<Container>( container ),                          \
							  typename gens<sizeof...( Args )>::type( ) );                   \
						}                                                                  \
                                                                               \
						template<typename Container, int... S>                             \
						auto call_##clause_name( Container &&container,                    \
						                         seq<S...> ) const {                       \
							return clause_name##_helper(                                     \
							  std::forward<Container>( container ),                          \
							  std::get<S>( clause_name##_args )... );                        \
						}                                                                  \
                                                                               \
						template<typename Iterator, int... S>                              \
						auto call_##clause_name(                                           \
						  daw::range::ReferenceRange<Iterator> container,                  \
						  seq<S...> ) const {                                              \
							return clause_name##_helper(                                     \
							  container, std::get<S>( clause_name##_args )... );             \
						}                                                                  \
                                                                               \
					public:                                                              \
						clause_name##_t( std::tuple<Args...> &&args )                      \
						  : clause_name                                                    \
						  ##_args( std::forward<std::tuple<Args>>( args )... ) {}          \
                                                                               \
						template<typename Container>                                       \
						auto operator( )( Container &&container ) const {                  \
							return delayed_dispatch( std::forward<Container>( container ) ); \
						}                                                                  \
					};                                                                   \
				}                                                                      \
                                                                               \
				template<typename... Args>                                             \
				auto clause_name( Args &&... clause_name##_args ) {                    \
					std::tuple<Args...> param =                                          \
					  std::make_tuple( std::forward<Args...>( clause_name##_args )... ); \
					return daw::range::operators::details::clause_name##_t<Args...>(     \
					  daw::move( param ) );                                              \
				}                                                                      \
			}                                                                        \
		}                                                                          \
	}                                                                            \
	template<typename Container, typename... Args,                               \
	         typename std::enable_if_t<daw::all_true_v<                          \
	           !daw::range::is_range_reference_v<Container>,                     \
	           !daw::range::is_range_collection_v<Container>>> * = nullptr,      \
	         typename = void>                                                    \
	auto operator<<(                                                             \
	  Container &&container,                                                     \
	  daw::range::operators::details::clause_name##_t<Args...> const             \
	    &predicate ) {                                                           \
		return predicate( std::forward<Container>( container ) );                  \
	}                                                                            \
	template<typename Container, typename... Args,                               \
	         typename std::enable_if_t<                                          \
	           daw::range::is_range_collection_v<Container>> * = nullptr>        \
	auto operator<<(                                                             \
	  Container &&container,                                                     \
	  daw::range::operators::details::clause_name##_t<Args...> const             \
	    &predicate ) {                                                           \
		return predicate( std::forward<Container>( container ) );                  \
	}                                                                            \
	template<typename Container, typename... Args,                               \
	         typename std::enable_if_t<                                          \
	           daw::range::is_range_reference_v<Container>> * = nullptr>         \
	auto operator<<(                                                             \
	  Container &&container,                                                     \
	  daw::range::operators::details::clause_name##_t<Args...> const             \
	    &predicate ) {                                                           \
		return predicate( std::forward<Container>( container ) );                  \
	}

DAW_RANGE_GENERATE_VCLAUSE( accumulate )
DAW_RANGE_GENERATE_VCLAUSE( as_vector )
DAW_RANGE_GENERATE_VCLAUSE( erase )
DAW_RANGE_GENERATE_VCLAUSE( erase_where_equal_to )
DAW_RANGE_GENERATE_VCLAUSE( find )
DAW_RANGE_GENERATE_VCLAUSE( find_if )
DAW_RANGE_GENERATE_VCLAUSE( partition )
DAW_RANGE_GENERATE_VCLAUSE( shuffle )
DAW_RANGE_GENERATE_VCLAUSE( sort )
DAW_RANGE_GENERATE_VCLAUSE( stable_partition )
DAW_RANGE_GENERATE_VCLAUSE( stable_sort )
DAW_RANGE_GENERATE_VCLAUSE( transform )
DAW_RANGE_GENERATE_VCLAUSE( unique )
DAW_RANGE_GENERATE_VCLAUSE( where )
DAW_RANGE_GENERATE_VCLAUSE( for_each )

#undef DAW_RANGE_GENERATE_VCLAUSE
#endif //	_MSC_VER
