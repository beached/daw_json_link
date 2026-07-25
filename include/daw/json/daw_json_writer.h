// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

// Allow writing to a Writer incrementally

#include "daw/json/impl/version.h"

#include "daw/json/concepts/daw_writable_output.h"
#include "daw/json/daw_json_link.h"

#include <daw/daw_move.h>
#include <daw/daw_pipelines.h>

#include <optional>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		enum class json_writer_states : std::uint8_t {
			/**
			 * Not writing a member or value
			 */
			json_writer_nothing,
			/**
			 * Indicates we are inside a class
			 */
			json_writer_object,
			/**
			 * Indicates we are inside an array
			 */
			json_writer_array,
			/**
			 * Indicates that the root value is complete
			 */
			json_writer_complete
		};

		template<typename WriterType, typename StackType, auto... PolicyFlags>
		class json_writer_t {
			using iterator_t =
			  decltype( json_details::make_output_iterator<PolicyFlags...>(
			    std::declval<WriterType>( ) ) );

			/*std::vector<json_writer_states>*/ StackType m_stack{ };
			json_writer_states m_current_state =
			  json_writer_states::json_writer_nothing;
			bool m_is_first = true;
			iterator_t m_writer;

			constexpr void push_state( json_writer_states new_state ) {
				daw_json_ensure( m_current_state !=
				                   json_writer_states::json_writer_complete,
				                 ErrorReason::OutputError );
				daw_json_ensure( new_state !=
				                     json_writer_states::json_writer_complete and
				                   new_state != json_writer_states::json_writer_nothing,
				                 ErrorReason::OutputError );
				m_stack.push_back( m_current_state );
				m_current_state = new_state;
			}

			constexpr void pop_state( ) {
				daw_json_ensure( not m_stack.empty( ), ErrorReason::OutputError );
				m_current_state = m_stack.back( );
				m_stack.pop_back( );
			}

			constexpr void do_next_member( bool writer_next_member = true ) {
				if( not m_is_first ) {
					m_writer.put( ',' );
				} else {
					m_is_first = false;
				}
				if( writer_next_member ) {
					m_writer.next_member( );
				}
			}

			template<typename T>
			constexpr void do_write_value( T const &value ) {
				do_next_member( );
				to_json( value, m_writer.get( ) );
			}

		public:
			explicit constexpr json_writer_t( WriterType &writer )
			  : m_writer(
			      json_details::make_output_iterator<PolicyFlags...>( writer ) ) {}
			json_writer_t( json_writer_t const & ) = delete;
			json_writer_t &operator=( json_writer_t const & ) = delete;
			json_writer_t( json_writer_t &&other ) = delete;
			json_writer_t &operator=( json_writer_t && ) = delete;

			constexpr ~json_writer_t( ) noexcept( noexcept( m_writer.put( ' ' ) ) ) {
				while( m_current_state != json_writer_states::json_writer_nothing ) {
					switch( m_current_state ) {
					case json_writer_states::json_writer_object:
						close_object( );
						break;
					case json_writer_states::json_writer_array:
						close_array( );
						break;
					default:
						break;
					}
				}
			}

			constexpr void open_object( ) {
				push_state( json_writer_states::json_writer_object );
				// do_next_member( );
				m_is_first = true;
				m_writer.put( '{' );
				m_writer.add_indent( );
				m_is_first = true;
			}

			constexpr void close_object( ) {
				daw_json_ensure( m_current_state ==
				                   json_writer_states::json_writer_object,
				                 ErrorReason::OutputError );
				m_writer.del_indent( );
				m_writer.next_member( );
				m_writer.put( '}' );
				pop_state( );
				m_is_first = false;
			}

			constexpr void open_array( ) {
				push_state( json_writer_states::json_writer_array );
				do_next_member( );
				m_is_first = true;
				m_writer.add_indent( );
				m_writer.put( '[' );
			}

			constexpr void close_array( ) {
				daw_json_ensure( m_current_state ==
				                   json_writer_states::json_writer_array,
				                 ErrorReason::OutputError );
				pop_state( );
				m_writer.del_indent( );
				m_writer.put( ']' );
				m_is_first = false;
			}

			// Write a value similar to to_json.
			template<typename T>
			constexpr void write_value( T const &value ) {
				if( m_current_state == json_writer_states::json_writer_object ) {
					std::abort( );
				}
				if( m_current_state == json_writer_states::json_writer_nothing and
				    not m_is_first ) {
					std::abort( );
				}
				do_write_value( value );
			}

			// Must be in array state
			template<typename Range>
			constexpr void write_values( Range const &values ) {
				if( m_current_state != json_writer_states::json_writer_array ) {
					// error
					std::abort( );
				}
				for( auto const &value : values ) {
					do_write_value( value );
				}
			}

			template<typename T>
			constexpr void write_values( std::initializer_list<T> const &values ) {
				if( m_current_state != json_writer_states::json_writer_array ) {
					// error
					std::abort( );
				}
				for( auto const &value : values ) {
					do_write_value( value );
				}
			}

			template<typename T>
			constexpr void write_key_value( daw::string_view name, T const &value ) {
				do_next_member( );
				m_writer.write( "\"", name, "\":", m_writer.space );
				to_json( value, m_writer.get( ) );
			}

			constexpr void write_key_value( daw::string_view name, std::nullptr_t ) {
				do_next_member( );
				m_writer.write( "\"", name, "\":", m_writer.space, "null" );
			}

			template<typename T>
			constexpr void write_key_value( daw::string_view name,
			                                std::initializer_list<T> const &values ) {
				do_next_member( );
				m_writer.write( "\"", name, "\":", m_writer.space );
				push_state( json_writer_states::json_writer_array );
				m_is_first = true;
				m_writer.put( '[' );
				m_writer.add_indent( );
				for( auto const &value : values ) {
					if( not m_is_first ) {
						m_writer.write( "," );
					} else {
						m_is_first = false;
					}
					m_writer.next_member( );
					to_json( value, m_writer.get( ) );
				}
				m_writer.del_indent( );
				m_writer.next_member( );
				m_writer.put( ']' );
				pop_state( );
				m_is_first = false;
			}

			template<std::size_t N>
			constexpr void write_key_value( daw::string_view name,
			                                char const ( &str )[N] ) {
				static_assert( N > 0 );
				write_key_value( name, daw::string_view( str, N - 1 ) );
			}
		};

		template<auto... PolicyFlags, typename WriterType>
		constexpr auto json_writer( WriterType &writer ) {
			return json_writer_t<WriterType,
			                     std::vector<json_writer_states>,
			                     PolicyFlags...>( writer );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
