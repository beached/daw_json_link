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

#if defined( DAW_JSON_HAS_REFLECTION )
#include <daw/daw_concepts.h>
#endif

#include <daw/daw_move.h>
#include <daw/traits/daw_traits_remove_cvref.h>

#include <type_traits>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_writer_details {
			template<typename JsonClass, typename Value>
			using json_write_value_class_t = typename daw::conditional_t<
			  std::is_same_v<use_default, JsonClass>,
			  json_details::ident_trait<json_details::json_deduced_type, Value>,
			  json_details::ident_trait<json_details::json_deduced_type,
			                            JsonClass>>::type;

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
				json_writer_array
			};
		} // namespace json_writer_details

		template<typename WritableType, typename StackType, auto... PolicyFlags>
		class json_writer_t {
			using iterator_t =
			  decltype( json_details::make_output_iterator<PolicyFlags...>(
			    std::declval<WritableType>( ) ) );

			StackType m_stack{ };
			json_writer_details::json_writer_states m_current_state =
			  json_writer_details::json_writer_states::json_writer_nothing;
			bool m_is_first = true;
			bool m_is_key_written = false;
			iterator_t m_writer;

			constexpr void
			push_state( json_writer_details::json_writer_states new_state ) {
				m_stack.push_back( m_current_state );
				m_current_state = new_state;
			}

			constexpr void pop_state( ) {
				daw_json_ensure( not m_stack.empty( ), ErrorReason::OutputError );
				m_current_state = m_stack.back( );
				m_stack.pop_back( );
			}

			constexpr void write_item_prefix( ) {
				daw_json_ensure( not m_is_key_written, ErrorReason::OutputError );
				if( not m_is_first ) {
					m_writer.put( ',' );
				} else {
					m_is_first = false;
				}
				m_writer.next_member( );
			}

			constexpr void prepare_value( ) {
				if( m_current_state ==
				    json_writer_details::json_writer_states::json_writer_nothing ) {
					daw_json_ensure( m_is_first, ErrorReason::OutputError );
					return;
				}
				if( m_current_state ==
				    json_writer_details::json_writer_states::json_writer_object ) {
					daw_json_ensure( m_is_key_written, ErrorReason::OutputError );
					m_is_key_written = false;
					return;
				}
				if( m_current_state !=
				    json_writer_details::json_writer_states::json_writer_nothing ) {
					write_item_prefix( );
				}
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void do_write_value( T const &value ) {
				prepare_value( );
				to_json<JsonClass>( value, m_writer.get( ) );
				m_is_first = false;
			}

		public:
			template<typename W>
			explicit constexpr json_writer_t( W &&writer )
			  : m_writer( json_details::make_output_iterator<PolicyFlags...>(
			      DAW_FWD( writer ) ) ) {}
			json_writer_t( json_writer_t const & ) = delete;
			json_writer_t &operator=( json_writer_t const & ) = delete;
			json_writer_t( json_writer_t &&other ) = delete;
			json_writer_t &operator=( json_writer_t && ) = delete;

			constexpr void finalize( ) {
				if( m_is_key_written ) {
					write_value( nullptr );
				}
				while( m_current_state !=
				       json_writer_details::json_writer_states::json_writer_nothing ) {
					switch( m_current_state ) {
					case json_writer_details::json_writer_states::json_writer_object:
						close_object( );
						break;
					case json_writer_details::json_writer_states::json_writer_array:
						close_array( );
						break;
					case json_writer_details::json_writer_states::json_writer_nothing:
					default:
						break;
					}
				}
			}

			constexpr void reset( ) {
				finalize( );
				m_current_state =
				  json_writer_details::json_writer_states::json_writer_nothing;
				m_is_first = true;
				m_is_key_written = false;
			}

			DAW_CPP20_CX_DTOR ~json_writer_t( ) noexcept(
			  noexcept( m_writer.put( ' ' ) ) ) {
				finalize( );
			}

			constexpr void open_object( ) {
				prepare_value( );
				push_state(
				  json_writer_details::json_writer_states::json_writer_object );
				m_writer.put( '{' );
				m_writer.add_indent( );
				m_is_first = true;
			}

			constexpr void close_object( ) {
				daw_json_ensure(
				  m_current_state ==
				    json_writer_details::json_writer_states::json_writer_object,
				  ErrorReason::OutputError );
				if( m_is_key_written ) {
					write_value( nullptr );
				}
				m_writer.del_indent( );
				if( not m_is_first ) {
					m_writer.next_member( );
				}
				m_writer.put( '}' );
				pop_state( );
				m_is_first = false;
			}

			constexpr void open_array( ) {
				prepare_value( );
				push_state(
				  json_writer_details::json_writer_states::json_writer_array );
				m_is_first = true;
				m_writer.add_indent( );
				m_writer.put( '[' );
			}

			constexpr void close_array( ) {
				daw_json_ensure(
				  m_current_state ==
				    json_writer_details::json_writer_states::json_writer_array,
				  ErrorReason::OutputError );
				m_writer.del_indent( );
				if( not m_is_first ) {
					m_writer.next_member( );
				}
				m_writer.put( ']' );
				pop_state( );
				m_is_first = false;
			}

			constexpr void write_value( std::nullptr_t ) {
				write_null( );
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void write_value( T const &value ) {
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_object or
				    m_is_key_written,
				  ErrorReason::OutputError );
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_nothing or
				    m_is_first,
				  ErrorReason::OutputError );
				do_write_value<JsonClass>( value );
			}

			template<typename T, typename T2, typename... Ts>
			constexpr void write_value( T const &value, T2 const &value2,
			                            Ts const &...values ) {
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_object or
				    m_is_key_written,
				  ErrorReason::OutputError );
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_nothing or
				    m_is_first,
				  ErrorReason::OutputError );
				open_array( );
				write_array_values( value, value2, values... );
				close_array( );
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void write_value( std::initializer_list<T> const &values ) {
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_object or
				    m_is_key_written,
				  ErrorReason::OutputError );
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_nothing or
				    m_is_first,
				  ErrorReason::OutputError );
				open_array( );
				write_array_values<JsonClass>( values );
				close_array( );
			}

			template<typename JsonClass = use_default, std::size_t N>
			constexpr void write_value( char const ( &str )[N] ) {
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_object or
				    m_is_key_written,
				  ErrorReason::OutputError );
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_nothing or
				    m_is_first,
				  ErrorReason::OutputError );
				do_write_value<JsonClass>( daw::string_view( str, N - 1 ) );
			}

			constexpr void add_key( daw::string_view name ) {
				if( m_is_key_written ) {
					write_value( nullptr );
				}
				daw_json_ensure(
				  m_current_state ==
				    json_writer_details::json_writer_states::json_writer_object,
				  ErrorReason::OutputError );

				write_item_prefix( );
				m_writer.write( "\"", name, "\":", m_writer.space );
				m_is_key_written = true;
			}

			constexpr void write_key_value( daw::string_view name, std::nullptr_t ) {
				add_key( name );
				write_null( );
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void write_key_value( daw::string_view name,
			                                std::initializer_list<T> const &values ) {
				add_key( name );
				open_array( );
				write_array_values<JsonClass>( values );
				close_array( );
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void write_key_value( daw::string_view name, T const &value ) {
				add_key( name );
				write_value<JsonClass>( value );
			}

			template<typename T, typename T2, typename... Ts>
			constexpr void write_key_value( daw::string_view name, T const &value,
			                                T2 const &value2, Ts const &...values ) {
				add_key( name );
				open_array( );
				write_array_values( value, value2, values... );
				close_array( );
			}

			template<typename JsonClass = use_default, std::size_t N>
			constexpr void write_key_value( daw::string_view name,
			                                char const ( &str )[N] ) {
				add_key( name );
				write_value<JsonClass>( daw::string_view( str, N - 1 ) );
			}

			// Must be in array state
			template<
			  typename JsonClass = use_default,
			  typename Range DAW_ENABLEIF( daw::traits::is_container_like_v<Range> )>
			DAW_REQUIRES( daw::traits::is_container_like_v<Range> )
			constexpr void write_array_values( Range const &values ) {
				daw_json_ensure(
				  m_current_state ==
				    json_writer_details::json_writer_states::json_writer_array,
				  ErrorReason::OutputError );
				for( auto const &value : values ) {
					do_write_value<JsonClass>( value );
				}
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void
			write_array_values( std::initializer_list<T> const &values ) {
				daw_json_ensure(
				  m_current_state ==
				    json_writer_details::json_writer_states::json_writer_array,
				  ErrorReason::OutputError );
				for( auto const &value : values ) {
					write_value<JsonClass>( value );
				}
			}

			template<typename T, typename... Ts>
			constexpr void write_array_values( T const &value, Ts const &...values ) {
				daw_json_ensure(
				  m_current_state ==
				    json_writer_details::json_writer_states::json_writer_array,
				  ErrorReason::OutputError );
				write_value( value );
				(void)( ( write_value( values ), true ) and ... );
			}

			constexpr void write_boolean( bool b ) {
				write_value( b );
			}

			constexpr void write_null( ) {
				daw_json_ensure(
				  m_current_state !=
				      json_writer_details::json_writer_states::json_writer_nothing or
				    m_is_first,
				  ErrorReason::OutputError );
				prepare_value( );
				m_writer.write( "null" );
				m_is_first = false;
			}

			template<typename JsonClass = use_default, typename T>
			constexpr void write_number( T const &value ) {
				using JsonMember =
				  json_writer_details::json_write_value_class_t<JsonClass, T>;
				constexpr JsonBaseParseTypes json_base_type =
				  JsonMember::underlying_json_type;
				static_assert(
				  json_base_type == JsonBaseParseTypes::Number or
				    json_base_type == JsonBaseParseTypes::Bool,
				  "The underlying mapping must be a number or boolean type" );
				if constexpr( json_base_type == JsonBaseParseTypes::Bool ) {
					write_value<JsonClass>( static_cast<bool>( value ) ? 1 : 0 );
					return;
				} else {
					write_value<JsonClass>( value );
				}
			}

			/// When outputting a class, uses default to_json
			template<typename JsonClass = use_default, typename T>
			constexpr void write_string( T const &value ) {
				using JsonMember =
				  json_writer_details::json_write_value_class_t<JsonClass, T>;
				constexpr JsonBaseParseTypes json_base_type =
				  JsonMember::underlying_json_type;
				if constexpr( json_base_type == JsonBaseParseTypes::String ) {
					write_value<JsonClass>( value );
				} else if constexpr( json_base_type == JsonBaseParseTypes::Bool or
				                     json_base_type == JsonBaseParseTypes::Number ) {
					prepare_value( );
					m_writer.put( '"' );
					to_json<JsonClass>( value, m_writer.get( ) );
					m_writer.put( '"' );
					m_is_first = false;
				} else {
					auto const tmp = to_json( value );
					do_write_value<json_string_no_name<>>( tmp );
				}
			}

			template<typename JsonClass = use_default, std::size_t N>
			constexpr void write_string( char const ( &str )[N] ) {
				write_value<JsonClass>( daw::string_view( str, N - 1 ) );
			}

#if defined( DAW_JSON_HAS_REFLECTION )
			constexpr void write_enum_string( daw::EnumType auto e ) {
				write_string( refl_details::enum_to_string( e ) );
			}
#endif
		};

		template<auto... PolicyFlags, typename WriterType>
		constexpr auto json_writer( WriterType &&writer ) {
			return json_writer_t<daw::remove_cvref_t<WriterType>,
			                     std::vector<json_writer_details::json_writer_states>,
			                     PolicyFlags...>( writer );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
