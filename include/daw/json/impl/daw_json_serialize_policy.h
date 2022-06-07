// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "version.h"

#include "../../daw_writable_output.h"
#include "daw_json_option_bits.h"
#include "daw_json_parse_options_impl.h"
#include "daw_json_serialize_options_impl.h"
#include "daw_json_serialize_policy_details.h"

#include <daw/daw_move.h>

#include <cstddef>
#include <iterator>
#include <memory>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/***
		 * Create the parser options flag for BasicParsePolicy
		 * @tparam Policies Policy types that satisfy the `is_policy_flag` trait.
		 * @param policies A list of parser options to change from the defaults.
		 * @return A json_options_t that encodes the options for the parser
		 */
		template<typename... Policies>
		constexpr json_options_t serialize_options( Policies... policies ) {
			static_assert( ( json_details::is_option_flag<Policies> and ... ),
			               "Only registered policy types are allowed" );
			auto result = json_details::serialization::default_policy_flag;
			if constexpr( sizeof...( Policies ) > 0 ) {
				result |=
				  ( json_details::serialization::set_bits_for( policies ) | ... );
			}
			return result;
		}

		template<typename WritableType,
		         json_options_t PolicyFlags =
		           json_details::serialization::default_policy_flag>
		struct serialization_policy {
			static_assert(
			  is_writable_output_type_v<WritableType>,
			  "Output type does not have a writeable_output_trait specialization" );
			using i_am_a_serialization_policy = void;
			WritableType *m_writable;

			static constexpr json_options_t policy_flags( ) {
				return PolicyFlags;
			}

			std::size_t indentation_level = 0;

			constexpr WritableType const &get( ) const {
				return *m_writable;
			}

			constexpr WritableType &get( ) {
				return *m_writable;
			}
			constexpr serialization_policy( WritableType &writable )
			  : m_writable( std::addressof( writable ) ) {}

			static constexpr options::SerializationFormat serialization_format =
			  json_details::serialization::get_bits_for<options::SerializationFormat>(
			    PolicyFlags );

			static constexpr options::IndentationType indentation_type =
			  json_details::serialization::get_bits_for<options::IndentationType>(
			    PolicyFlags );

			static constexpr options::RestrictedStringOutput
			  restricted_string_output = json_details::serialization::get_bits_for<
			    options::RestrictedStringOutput>( PolicyFlags );

			static constexpr options::NewLineDelimiter newline_delimiter =
			  json_details::serialization::get_bits_for<options::NewLineDelimiter>(
			    PolicyFlags );

			static constexpr options::OutputTrailingComma output_trailing_comma =
			  json_details::serialization::get_bits_for<options::OutputTrailingComma>(
			    PolicyFlags );

			inline constexpr void add_indent( ) {
				++indentation_level;
			}

			inline constexpr void del_indent( ) {
				--indentation_level;
			}

			inline constexpr void output_indent( ) {
				constexpr std::string_view indent =
				  json_details::serialization::generate_indent<serialization_format,
				                                               indentation_type>;
				if( not indent.empty( ) ) {
					for( std::size_t n = 0; n < indentation_level; ++n ) {
						write( indent );
					}
				}
			}

			inline constexpr void output_newline( ) {
				if constexpr( serialization_format !=
				              options::SerializationFormat::Minified ) {
					if constexpr( newline_delimiter == options::NewLineDelimiter::n ) {
						put( '\n' );
					} else {
						write( "\r\n" );
					}
				}
			}

			static constexpr daw::string_view space =
			  serialization_format != options::SerializationFormat::Minified
			    ? " "
			    : nullptr;

			inline constexpr void next_member( ) {
				output_newline( );
				output_indent( );
			}

			template<typename... ContiguousCharRanges>
			constexpr void write( ContiguousCharRanges &&...chrs ) {
				static_assert( sizeof...( ContiguousCharRanges ) > 0 );
				writable_output_trait<WritableType>::write(
				  *m_writable, daw::string_view( chrs )... );
			}

			constexpr void copy_buffer( char const *first, char const *last ) {
				write(
				  daw::string_view( first, static_cast<std::size_t>( last - first ) ) );
			}

			constexpr void put( char c ) {
				writable_output_trait<WritableType>::put( *m_writable, c );
			}

			constexpr serialization_policy &operator=( char c ) {
				put( c );
				return *this;
			}

			constexpr serialization_policy &operator*( ) {
				return *this;
			}

			constexpr serialization_policy &operator++( ) {
				return *this;
			}

			constexpr serialization_policy operator++( int ) & {
				return *this;
			}
		};

		template<typename, typename...>
		inline constexpr bool is_serialization_policy_v = false;

		template<typename OutputIterator, json_options_t PolicyFlags>
		inline constexpr bool is_serialization_policy_v<
		  serialization_policy<OutputIterator, PolicyFlags>> = true;
	} // namespace DAW_JSON_VER
} // namespace daw::json
