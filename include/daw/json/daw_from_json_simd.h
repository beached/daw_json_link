// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.
#pragma once

#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_json_from_json_simd.h"

#if DAW_CPP_VERSION >= 202002L and                                    \
  ( defined( DAW_HAS_AVX512_SIMD ) or defined( DAW_HAS_AVX2_SIMD ) or \
    defined( DAW_HAS_NEON_SIMD ) )
#define DAW_JSON_HAS_FROM_JSON_SIMD 1
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
#if defined( DAW_JSON_HAS_FROM_JSON_SIMD )

		namespace experimental {
			template<typename JsonMember, typename String, auto... PolicyFlags>
			DAW_ATTRIB_NOINLINE [[nodiscard]] auto from_json_simd(
			  String &&document,
			  daw::not_null<std::pmr::memory_resource *> memory_resource,
			  options::parse_flags_t<PolicyFlags...> = { } ) {
				static_assert( json_details::has_json_deduced_type_v<JsonMember>,
				               "The type passed to from_json_simd does not have a "
				               "json_data_contract and one cannot be deduced" );
				using member = json_details::json_deduced_type<JsonMember>;
				auto doc = daw::string_view( DAW_FWD( document ) );
				daw_json_ensure( not doc.empty( ), ErrorReason::EmptyJSONDocument );
				if( doc.is_zero_terminated( ) ) {
					using ParseState = BasicParsePolicy<
					  options::parse_flags_t<PolicyFlags...,
					                         options::ZeroTerminatedString::yes>::value>;
					static_assert(
					  json_details::simd_parse::policy_supports_simd_v<ParseState> );
					return json_details::simd_parse::from_json_simd_impl<member,
					                                                     ParseState>(
					  std::string_view( doc ), memory_resource );
				}
				using ParseState =
				  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;
				static_assert(
				  json_details::simd_parse::policy_supports_simd_v<ParseState> );
				return json_details::simd_parse::from_json_simd_impl<member,
				                                                     ParseState>(
				  std::string_view( doc ), memory_resource );
			}

			/// Experimental shared-index parser. Skipped values receive
			/// structural, not full lexical, validation. See
			/// reflection_json_parser_design.md.
			template<typename JsonMember, typename String, auto... PolicyFlags>
			DAW_ATTRIB_NOINLINE [[nodiscard]] auto from_json_simd(
			  String &&document,
			  options::parse_flags_t<PolicyFlags...> policy_flags = { } ) {

				daw::not_null memory_resource = std::pmr::new_delete_resource( );
				return from_json_simd<JsonMember>(
				  DAW_FWD( document ), memory_resource, policy_flags );
			}
		} // namespace experimental

#endif
	} // namespace DAW_JSON_VER
} // namespace daw::json
