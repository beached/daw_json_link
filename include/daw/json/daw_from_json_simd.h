// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.
#pragma once

#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_json_from_json_simd.h"

#if DAW_CPP_VERSION >= 202002L and         \
  ( defined( DAW_JSON_HAS_AVX512_SIMD ) or \
    defined( DAW_JSON_HAS_AVX2_SIMD ) or defined( DAW_JSON_HAS_NEON_SIMD ) )
#define DAW_JSON_HAS_FROM_JSON_SIMD 1
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
#if defined( DAW_JSON_HAS_FROM_JSON_SIMD )
		/// Experimental shared-index parser. Skipped values receive
		/// structural, not full lexical, validation. See
		/// reflection_json_parser_design.md.
		template<typename JsonMember, typename String, auto... Flags>
		DAW_ATTRIB_NOINLINE [[nodiscard]] auto
		from_json_simd( String &&document,
		                options::parse_flags_t<Flags...> = { } ) {
			using State = BasicParsePolicy<options::parse_flags_t<Flags...>::value>;
			static_assert( json_details::simd_parse::policy_supports_simd_v<State> );
			static_assert( json_details::is_string_view_like_v<String> );
			daw_json_ensure( std::size( document ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			auto doc =
			  std::string_view( std::data( document ), std::size( document ) );
			if( doc[doc.size( ) - 1] == '\0' ) {
				doc.remove_suffix( 1 );
			}
			using member = json_details::json_deduced_type<JsonMember>;
			daw::not_null memory_resource = std::pmr::new_delete_resource( );
			return json_details::simd_parse::from_json_simd_impl<member, State>(
			  doc, memory_resource );
		}

		template<typename JsonMember, typename String, auto... Flags>
		DAW_ATTRIB_NOINLINE [[nodiscard]] auto
		from_json_simd( String &&document,
		                daw::not_null<std::pmr::memory_resource *> memory_resource,
		                options::parse_flags_t<Flags...> = { } ) {
			using ParseState =
			  BasicParsePolicy<options::parse_flags_t<Flags...>::value>;
			static_assert(
			  json_details::simd_parse::policy_supports_simd_v<ParseState> );
			static_assert( json_details::is_string_view_like_v<String> );
			daw_json_ensure( std::size( document ) != 0,
			                 ErrorReason::EmptyJSONDocument );
			auto doc =
			  std::string_view( std::data( document ), std::size( document ) );
			// account for c-arrays
			if( doc[doc.size( ) - 1] == '\0' ) {
				doc.remove_suffix( 1 );
			}
			static_assert( json_details::has_json_deduced_type_v<JsonMember>,
			               "The type passed to from_json_simd does not have a "
			               "json_data_contract and one cannot be deduced" );
			using member = json_details::json_deduced_type<JsonMember>;

			return json_details::simd_parse::from_json_simd_impl<member, ParseState>(
			  doc, memory_resource );
		}
#endif
	} // namespace DAW_JSON_VER
} // namespace daw::json
