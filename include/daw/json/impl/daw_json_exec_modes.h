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

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_is_constant_evaluated.h>

#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		struct default_exec_tag {
			static constexpr std::string_view name = "default_mode";
		};
		struct constexpr_exec_tag : default_exec_tag {
			static constexpr std::string_view name = "constexpr";
		};
		struct runtime_exec_tag : constexpr_exec_tag {
			static constexpr std::string_view name = "runtime";
		};
#if defined( DAW_ALLOW_SSE42 )
		struct sse42_exec_tag : runtime_exec_tag {
			static constexpr std::string_view name = "sse4.2";
		};
		using simd_exec_tag = sse42_exec_tag;
#else
		struct simd_exec_tag : runtime_exec_tag {
			static constexpr std::string_view name = "simd";
		};
#endif

		namespace json_details {
			/// The exec tags exist for compilers that cannot detect constant
			/// evaluation.  When it can be detected the tag does not matter: the
			/// constexpr code paths are used during constant evaluation and the
			/// fastest ones at runtime.
			template<typename ExecTag>
			DAW_ATTRIB_INLINE constexpr bool use_constexpr_exec_mode( ) {
#if defined( DAW_HAS_IF_CONSTEVAL )
				if consteval {
					return true;
				} else {
					return false;
				}
#elif defined( DAW_HAS_IS_CONSTANT_EVALUATED )
				return DAW_IS_CONSTANT_EVALUATED( );
#else
				// Cannot detect constant evaluation, only leave the constexpr code
				// paths when the runtime or simd exec mode was asked for
				return not std::is_base_of_v<runtime_exec_tag, ExecTag>;
#endif
			}

			/// Whether a runtime only search may use SIMD.  Call only after
			/// use_constexpr_exec_mode returned false.  With detection of constant
			/// evaluation every exec tag uses SIMD at runtime, without it only the
			/// simd exec tag does.
			template<typename ExecTag>
			DAW_ATTRIB_INLINE constexpr bool use_simd_exec_mode( ) {
#if defined( DAW_HAS_IF_CONSTEVAL ) or defined( DAW_HAS_IS_CONSTANT_EVALUATED )
				return true;
#else
				return std::is_base_of_v<simd_exec_tag, ExecTag>;
#endif
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json
