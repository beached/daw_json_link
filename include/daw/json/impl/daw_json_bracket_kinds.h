// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/version.h"

#include <daw/daw_attributes.h>

#include <cstddef>
#include <cstdint>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// The kinds of the open brackets of a skip that checks its brackets,
			/// a bit for each nesting level that is set when it is a class.  Levels
			/// start at 0, and at most DAW_JSON_STRICT_SKIP_MAX_DEPTH are open.
			/// Used by the checked scalar skips and the SIMD skips with
			/// options::StrictSkipChecks::yes.  Up to 64 levels it is one word
			class bracket_kinds {
			public:
				static constexpr std::size_t max_depth = DAW_JSON_STRICT_SKIP_MAX_DEPTH;
				static_assert( max_depth > 0U,
				               "DAW_JSON_STRICT_SKIP_MAX_DEPTH must be at least 1" );

			private:
				static constexpr std::size_t word_count = ( max_depth + 63U ) / 64U;

				std::uint64_t m_bits[word_count] = { };

				[[nodiscard]] DAW_ATTRIB_INLINE static constexpr std::size_t
				word_of( std::size_t level ) noexcept {
					if constexpr( word_count == 1U ) {
						(void)level;
						return 0U;
					} else {
						return level / 64U;
					}
				}

			public:
				/// Record whether the bracket at level is a class.  level must be
				/// less than max_depth
				DAW_ATTRIB_INLINE constexpr void set( std::size_t level,
				                                      bool is_class ) noexcept {
					auto &word = m_bits[word_of( level )];
					auto const bit = std::uint64_t{ 1 } << ( level % 64U );
					word = is_class ? ( word | bit ) : ( word & ~bit );
				}

				/// Whether the bracket at level is a class
				[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
				is_class( std::size_t level ) const noexcept {
					return ( ( m_bits[word_of( level )] >> ( level % 64U ) ) & 1U ) !=
					       0U;
				}
			};
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
