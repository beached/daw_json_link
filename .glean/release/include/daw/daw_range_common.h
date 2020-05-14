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

#include "daw_traits.h"

namespace daw {
	namespace range {
		namespace impl {
			template<typename Collection>
			auto to_vector( Collection const &collection ) {
				using value_type =
				  daw::traits::root_type_t<typename Collection::value_type>;
				std::vector<value_type> result;
				std::copy( std::begin( collection ), std::end( collection ),
				           std::back_inserter( result ) );
				return result;
			}

			template<typename IteratorF, typename IteratorL>
			auto to_vector( IteratorF first, IteratorL last ) {
				using value_type = typename std::iterator_traits<IteratorF>::value_type;
				std::vector<value_type> result;
				std::copy( first, last, std::back_inserter( result ) );
				return result;
			}

			template<typename T>
			using cleanup_t = daw::traits::root_type_t<T>;

		} // namespace impl

		template<typename Arg, typename... Args>
		auto make_range_reference( Arg &&arg, Args &&... args );
		template<typename Arg, typename... Args>
		auto make_range_collection( Arg &&arg, Args &&... args );
	} // namespace range
} // namespace daw
