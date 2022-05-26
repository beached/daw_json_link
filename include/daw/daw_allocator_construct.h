// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_attributes.h>
#include <daw/daw_move.h>

#include <memory>
#include <type_traits>

namespace daw {
	template<typename ResultType, typename Constructor, typename Allocator,
	         typename... Args>
	DAW_ATTRIB_INLINE constexpr auto try_alloc_construct( Allocator &&allocator,
	                                                      Args &&...args ) {
		using alloc_t = typename std::allocator_traits<
		  Allocator>::template rebind_alloc<ResultType>;
		if constexpr( std::is_invocable_v<Constructor, Args..., alloc_t> ) {
			auto alloc = static_cast<alloc_t>( allocator );
			return Constructor{ }( DAW_FWD( args )..., alloc );
		} else if constexpr( std::is_invocable_v<Constructor, std::allocator_arg_t,
		                                         alloc_t, Args...> ) {
			auto alloc = static_cast<alloc_t>( allocator );
			return Constructor{ }( std::allocator_arg, alloc, DAW_FWD( args )... );
		} else {
			static_assert( std::is_invocable_v<Constructor, Args...>,
			               "Unable to construct value with the supplied arguments" );
			return Constructor{ }( DAW_FWD( args )... );
		}
	}
} // namespace daw
