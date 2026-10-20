// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/impl/version.h>

#include "daw/json/concepts/daw_writable_output_fwd.h"
#include "daw/json/concepts/impl/daw_writable_output_details.h"
#include "daw/json/daw_json_switches.h"

#include <daw/cpp_20.h>
#include <daw/daw_attributes.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_is_constant_evaluated.h>
#include <daw/daw_likely.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>

/// string_writer grows its string with resize_and_overwrite, leaving the new
/// bytes uninitialized, when the standard library has it. That skips zero
/// filling memory that is overwritten right away, but the bytes past what was
/// written are never initialized, which resize_and_overwrite's precondition
/// does not allow. Define DAW_JSON_NO_LESS_SAFE_STRING_WRITER to grow with
/// resize( ) instead
#if defined( DAW_HAS_CPP23_STR_RESIZE_OVERWRITE ) and \
  not defined( DAW_JSON_NO_LESS_SAFE_STRING_WRITER )
#define DAW_JSON_STRING_WRITER_SKIPS_ZERO_FILL
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		/// @brief A writable output that serializes into a std::string. The
		/// string's own output trait resizes it on every write; this keeps spare
		/// room and only grows the string when it runs out, so a write is a size
		/// check and a copy. finish( ) cuts the string to what was written.
		class string_writer {
			std::string m_str{ };
			std::size_t m_size = 0;

			/// Out of line so the room check in reserve( ) stays small enough to
			/// inline into every put and write.  Starting from the string's own
			/// capacity means small outputs stay in the SSO buffer
			DAW_ATTRIB_NOINLINE DAW_CPP20_CX_ALLOC2 void grow( std::size_t needed ) {
				auto const n = ( std::max )(
				  { m_str.size( ) * 2, m_str.capacity( ), needed } );
#if defined( DAW_JSON_STRING_WRITER_SKIPS_ZERO_FILL )
				if( not DAW_IS_CONSTANT_EVALUATED_COMPAT( ) ) {
					m_str.resize_and_overwrite(
					  n, []( char *, std::size_t sz ) DAW_JSON_CPP23_STATIC_CALL_OP {
						  return sz;
					  } );
					return;
				}
#endif
				m_str.resize( n );
			}

		public:
			explicit string_writer( ) = default;

			/// @param initial bytes of room to start with
			explicit DAW_CPP20_CX_ALLOC string_writer( std::size_t initial ) {
				grow( initial );
			}

			/// @brief Room for n more bytes; commit( ) them once written
			[[nodiscard]] DAW_ATTRIB_INLINE DAW_CPP20_CX_ALLOC2 char *
			reserve( std::size_t n ) {
				if( DAW_UNLIKELY( m_str.size( ) - m_size < n ) ) {
					grow( m_size + n );
				}
				return m_str.data( ) + m_size;
			}

			DAW_ATTRIB_INLINE DAW_CPP20_CX_ALLOC2 void commit( std::size_t n ) {
				m_size += n;
			}

			DAW_ATTRIB_INLINE DAW_CPP20_CX_ALLOC2 void write( daw::string_view text ) {
				(void)concepts::writeable_output_details::copy_to_buffer(
				  reserve( text.size( ) ), text );
				commit( text.size( ) );
			}

			DAW_ATTRIB_INLINE DAW_CPP20_CX_ALLOC2 void put( char c ) {
				*reserve( 1 ) = c;
				commit( 1 );
			}

			/// @brief Bytes written so far
			[[nodiscard]] constexpr std::size_t size( ) const {
				return m_size;
			}

			/// @brief The text written. Shrinking the string neither fills nor
			/// reallocates it
			[[nodiscard]] DAW_CPP20_CX_ALLOC std::string finish( ) && {
				m_str.resize( m_size );
				m_size = 0;
				return std::move( m_str );
			}
		};

		namespace concepts {
			/// @brief Specialization for string_writer
			template<>
			struct writable_output_trait<string_writer> : std::true_type {
				using CharT = char;

				template<typename... StringViews>
				static DAW_CPP20_CX_ALLOC void write( string_writer &out,
				                                      StringViews const &...svs ) {
					static_assert( sizeof...( StringViews ) > 0 );
					auto const total_size = ( std::size( svs ) + ... );
					DAW_CPP23_STATIC_LOCAL constexpr auto writer =
					  []( char *&p, auto sv ) DAW_JSON_CPP23_STATIC_CALL_OP {
						  if( sv.empty( ) ) {
							  return 0;
						  }
						  p = writeable_output_details::copy_to_buffer( p, sv );
						  return 1;
					  };
					auto *ptr = out.reserve( total_size );
					( (void)writer( ptr, svs ), ... );
					out.commit( total_size );
				}

				static DAW_CPP20_CX_ALLOC void put( string_writer &out, char c ) {
					out.put( c );
				}

				/// Room for at least n more bytes
				static DAW_CPP20_CX_ALLOC void reserve( string_writer &out,
				                                        std::size_t n ) {
					(void)out.reserve( n );
				}
			};
		} // namespace concepts
	} // namespace DAW_JSON_VER
} // namespace daw::json
