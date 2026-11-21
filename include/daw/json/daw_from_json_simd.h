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

		namespace json_details::simd_parse {
			/// Pick the policy for whether the document is zero terminated and
			/// parse it.  The document is writable when string mutation is allowed
			template<typename JsonMember, typename CharT, auto... PolicyFlags>
			[[nodiscard]] auto from_json_simd_dispatch(
			  daw::span<CharT> doc, bool is_zero_terminated,
			  daw::not_null<std::pmr::memory_resource *> memory_resource ) {
				static_assert( json_details::has_json_deduced_type_v<JsonMember>,
				               "The type passed to from_json_simd does not have a "
				               "json_data_contract and one cannot be deduced" );
				using member = json_details::json_deduced_type<JsonMember>;
				daw_json_ensure( not doc.empty( ), ErrorReason::EmptyJSONDocument );
				if( is_zero_terminated ) {
					using ParseState = BasicParsePolicy<
					  options::parse_flags_t<PolicyFlags...,
					                         options::ZeroTerminatedString::yes>::value>;
					static_assert( policy_supports_simd_v<ParseState> );
					return from_json_simd_impl<member, ParseState>( doc,
					                                                memory_resource );
				}
				using ParseState =
				  BasicParsePolicy<options::parse_flags_t<PolicyFlags...>::value>;
				static_assert( policy_supports_simd_v<ParseState> );
				return from_json_simd_impl<member, ParseState>( doc, memory_resource );
			}
		} // namespace json_details::simd_parse

		namespace experimental {
			template<typename JsonMember, typename String, auto... PolicyFlags>
			DAW_ATTRIB_NOINLINE [[nodiscard]] auto from_json_simd(
			  String &&document,
			  daw::not_null<std::pmr::memory_resource *> memory_resource,
			  options::parse_flags_t<PolicyFlags...> = { } ) {
				auto doc = daw::string_view( DAW_FWD( document ) );
				return json_details::simd_parse::
				  from_json_simd_dispatch<JsonMember, char const, PolicyFlags...>(
				    daw::span<char const>( doc.data( ), doc.size( ) ),
				    static_cast<bool>( doc.is_zero_terminated( ) ),
				    memory_resource );
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

			/// As from_json_insitu, parse a writable buffer with string mutation
			/// enabled.  The buffer must remain alive and at a stable address while
			/// borrowed results are used.  Parsing may modify the buffer, including
			/// on failure
			template<typename JsonMember, typename String, auto... PolicyFlags>
			DAW_ATTRIB_NOINLINE [[nodiscard]] auto from_json_simd_insitu(
			  String &&document,
			  daw::not_null<std::pmr::memory_resource *> memory_resource,
			  options::parse_flags_t<PolicyFlags...> = { } ) {
				static_assert(
				  std::is_same_v<decltype( std::data( document ) ), char *>,
				  "from_json_simd_insitu requires a mutable character "
				  "buffer" );
				auto doc =
				  daw::span<char>( std::data( document ), std::size( document ) );
				// Only a string view knows whether its source is zero terminated
				bool is_zero_terminated = false;
				if constexpr( std::is_constructible_v<daw::string_view, String &> ) {
					is_zero_terminated = static_cast<bool>(
					  daw::string_view( document ).is_zero_terminated( ) );
				}
				return json_details::simd_parse::from_json_simd_dispatch<
				  JsonMember,
				  char,
				  PolicyFlags...,
				  options::AllowStringMutation::yes>(
				  doc, is_zero_terminated, memory_resource );
			}

			template<typename JsonMember, typename String, auto... PolicyFlags>
			DAW_ATTRIB_NOINLINE [[nodiscard]] auto from_json_simd_insitu(
			  String &&document,
			  options::parse_flags_t<PolicyFlags...> policy_flags = { } ) {

				daw::not_null memory_resource = std::pmr::new_delete_resource( );
				return from_json_simd_insitu<JsonMember>(
				  DAW_FWD( document ), memory_resource, policy_flags );
			}
		} // namespace experimental

#endif
	} // namespace DAW_JSON_VER
} // namespace daw::json
