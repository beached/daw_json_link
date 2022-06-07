// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "impl/version.h"

#include "../daw_writable_output.h"
#include "daw_to_json_fwd.h"
#include "impl/daw_json_container_appender.h"
#include "impl/daw_json_link_types_fwd.h"
#include "impl/to_daw_json_string.h"

#include <daw/daw_traits.h>

#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {

		template<typename JsonClass, typename Value, typename WritableType,
		         auto... PolicyFlags,
		         std::enable_if_t<
		           is_writable_output_type_v<daw::remove_cvref_t<WritableType>>,
		           std::nullptr_t>>
		[[maybe_unused]] constexpr daw::rvalue_to_value_t<WritableType>
		to_json( Value const &value, WritableType &&it,
		         options::output_flags_t<PolicyFlags...> ) {
			using json_class_t = typename std::conditional_t<
			  std::is_same_v<use_default, JsonClass>,
			  json_details::ident_trait<json_details::json_deduced_type, Value>,
			  json_details::ident_trait<json_details::json_deduced_type,
			                            JsonClass>>::type;

			using output_t = daw::rvalue_to_value_t<WritableType>;
			if constexpr( std::is_pointer_v<daw::remove_cvref_t<WritableType>> ) {
				daw_json_ensure( it != nullptr, ErrorReason::NullOutputIterator );
			}
			auto out_it = [&] {
				if constexpr( is_serialization_policy_v<
				                daw::remove_cvref_t<WritableType>> ) {
					if constexpr( sizeof...( PolicyFlags ) == 0 ) {
						return it;
					} else {
						return serialization_policy<typename output_t::iterator_type,
						                            json_details::serialization::set_bits(
						                              output_t::policy_flags( ),
						                              PolicyFlags... )>( it.get( ) );
					}
				} else {
					return serialization_policy<
					  daw::remove_cvref_t<WritableType>,
					  options::output_flags_t<PolicyFlags...>::value>( it );
				}
			}( );
			return json_details::member_to_string( template_arg<json_class_t>, out_it,
			                                       value )
			  .get( );
		}

		template<typename Result, typename JsonClass, typename Value,
		         auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result
		to_json( Value const &value,
		         options::output_flags_t<PolicyFlags...> flags ) {
			static_assert(
			  is_writable_output_type_v<Result>,
			  "Result type must has a specialization of writable_output_trait" );

			Result r{ };
			if constexpr( std::is_same_v<Result, std::string> ) {
				r.reserve( 4096 );
			}
			(void)to_json<JsonClass>( value, r, flags );
			if constexpr( std::is_same_v<Result, std::string> ) {
				r.shrink_to_fit( );
			}
			return r;
		}

		template<typename JsonElement, typename Container, typename WritableType,
		         auto... PolicyFlags,
		         std::enable_if_t<
		           is_writable_output_type_v<daw::remove_cvref_t<WritableType>>,
		           std::nullptr_t>>
		[[maybe_unused]] constexpr daw::rvalue_to_value_t<WritableType>
		to_json_array( Container const &c, WritableType &&it,
		               options::output_flags_t<PolicyFlags...> ) {
			static_assert(
			  traits::is_container_like_v<daw::remove_cvref_t<Container>>,
			  "Supplied container must support begin( )/end( )" );
			using output_t = daw::rvalue_to_value_t<WritableType>;

			if constexpr( std::is_pointer_v<daw::remove_cvref_t<output_t>> ) {
				daw_json_ensure( it != nullptr, ErrorReason::InvalidNull );
			}
			auto out_it = [&] {
				if constexpr( is_serialization_policy_v<
				                daw::remove_cvref_t<WritableType>> ) {
					if constexpr( sizeof...( PolicyFlags ) == 0 ) {
						return it;
					} else {
						return serialization_policy<typename output_t::iterator_type,
						                            json_details::serialization::set_bits(
						                              output_t::policy_flags( ),
						                              PolicyFlags... )>( it.get( ) );
					}
				} else {
					return serialization_policy<
					  daw::remove_cvref_t<WritableType>,
					  options::output_flags_t<PolicyFlags...>::value>( it );
				}
			}( );
			out_it.put( '[' );
			out_it.add_indent( );
			// Not const & as some types(vector<bool>::const_reference are not ref
			// types
			auto first = std::begin( c );
			auto last = std::end( c );
			bool const has_elements = first != last;
			while( first != last ) {
				(void)[&out_it]( auto &&v ) {
					using v_type = DAW_TYPEOF( v );
					using JsonMember = typename std::conditional_t<
					  std::is_same_v<JsonElement, use_default>,
					  json_details::ident_trait<json_details::json_deduced_type, v_type>,
					  json_details::ident_trait<json_details::json_deduced_type,
					                            JsonElement>>::type;

					static_assert(
					  not std::is_same_v<
					    JsonMember,
					    missing_json_data_contract_for_or_unknown_type<JsonElement>>,
					  "Unable to detect unnamed mapping" );
					// static_assert( not std::is_same_v<JsonElement, JsonMember> );
					out_it.next_member( );

					out_it = json_details::member_to_string( template_arg<JsonMember>,
					                                         out_it, v );
				}
				( *first );
				++first;
				if( first != last ) {
					out_it.put( ',' );
				}
			}
			// The last character will be a ',' prior to this
			out_it.del_indent( );
			if( has_elements ) {
				out_it.output_newline( );
			}
			out_it.put( ']' );
			return out_it.get( );
		}

		template<typename Result, typename JsonElement, typename Container,
		         auto... PolicyFlags>
		[[maybe_unused, nodiscard]] constexpr Result
		to_json_array( Container const &c,
		               options::output_flags_t<PolicyFlags...> flags ) {
			static_assert( traits::is_container_like_v<Container>,
			               "Supplied container must support begin( )/end( )" );
			static_assert(
			  is_writable_output_type_v<Result>,
			  "Result type must has a specialization of writable_output_trait" );

			Result r{ };
			if constexpr( std::is_same_v<Result, std::string> ) {
				r.reserve( 4096 );
			}
			(void)to_json_array<JsonElement>( c, r, flags );
			if constexpr( std::is_same_v<Result, std::string> ) {
				r.shrink_to_fit( );
			}
			return r;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
