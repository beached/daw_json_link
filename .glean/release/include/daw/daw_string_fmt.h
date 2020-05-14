// The MIT License (MIT)
//
// Copyright (c) 2017-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_algorithm.h"
#include "daw_bounded_vector.h"
#include "daw_exception.h"
#include "daw_move.h"
#include "daw_parser_helper_sv.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "daw_visit.h"
#include "impl/daw_int_to_iterator.h"

#include <cstddef>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace daw {
	namespace string_fmt {
		namespace v1 {
			struct invalid_string_fmt_index {};
			namespace string_fmt_details {
				template<size_t N>
				std::string to_string( char const ( &str )[N] ) {
					return std::string{str};
				}

				template<typename T>
				using has_to_string = decltype( std::declval<T>( ).to_string( ) );

				template<typename T>
				constexpr bool has_to_string_v = daw::is_detected_v<has_to_string, T>;

				template<typename T>
				using can_cast_to_string =
				  decltype( static_cast<std::string>( std::declval<T>( ) ) );

				template<typename T>
				constexpr bool can_cast_to_string_v =
				  daw::is_detected_v<can_cast_to_string, T>;

				template<typename T,
				         std::enable_if_t<
				           all_true_v<has_to_string_v<T>, !can_cast_to_string_v<T>>,
				           std::nullptr_t> = nullptr>
				inline std::string to_string( T const &value ) {
					return value.to_string( );
				}

				template<typename T, std::enable_if_t<can_cast_to_string_v<T>,
				                                      std::nullptr_t> = nullptr>
				inline std::string to_string( T const &value ) {
					return static_cast<std::string>( value );
				}

				template<uint8_t Cnt, uint8_t Sz, typename... Args,
				         std::enable_if_t<( Cnt >= Sz ), std::nullptr_t> = nullptr>
				inline std::string get_arg_impl( uint8_t const, Args &&... ) {
					daw::exception::daw_throw<invalid_string_fmt_index>( );
				}

				template<uint8_t Cnt, uint8_t Sz, typename Arg, typename... Args,
				         std::enable_if_t<( Cnt < Sz ), std::nullptr_t> = nullptr>
				std::string get_arg_impl( uint8_t const idx, Arg &&arg,
				                          Args &&... args ) {
					if( Cnt == idx ) {
						using daw::string_fmt::v1::string_fmt_details::to_string;
						using std::to_string;
						return to_string( std::forward<Arg>( arg ) );
					}
					return get_arg_impl<Cnt + 1, Sz>( idx,
					                                  std::forward<Args>( args )... );
				}

				template<typename... Args>
				std::string get_arg( uint8_t const idx, Args &&... args ) {
					return get_arg_impl<0, sizeof...( Args )>(
					  idx, std::forward<Args>( args )... );
				}
			} // namespace string_fmt_details

			class fmt_t {
				std::string m_format_str;

			public:
				template<typename String,
				         std::enable_if_t<std::is_convertible_v<String, std::string>,
				                          std::nullptr_t> = nullptr>
				fmt_t( String &&format_str )
				  : m_format_str{std::forward<String>( format_str )} {}

				template<size_t N>
				fmt_t( char const ( &format_str )[N] )
				  : m_format_str{std::string{format_str}} {}

				template<typename... Args>
				std::string operator( )( Args &&... args ) const {
					std::string result{};
					result.reserve( m_format_str.size( ) +
					                ( sizeof...( args ) * 4 ) ); // WAG for size
					daw::string_view sv{m_format_str};
					result += sv.pop_front( "{" );
					while( !sv.empty( ) ) {
						auto const idx_str = sv.pop_front( "}" );
						auto const idx =
						  daw::parser::parse_unsigned_int<uint8_t>( idx_str );
						result +=
						  string_fmt_details::get_arg( idx, std::forward<Args>( args )... );
						result += sv.pop_front( "{" ).to_string( );
					}
					return result;
				}
			};

			template<typename... Args>
			std::string fmt( std::string format_str, Args &&... args ) {
				return fmt_t{daw::move( format_str )}( std::forward<Args>( args )... );
			}
		} // namespace v1
		namespace v2 {
			namespace string_fmt_details {
				using daw::string_fmt::v1::string_fmt_details::to_string;
				using std::to_string;
				template<typename T>
				constexpr auto to_string_test( )
				  -> decltype( to_string( std::declval<T>( ) ) );

				template<typename T>
				using has_to_string_test = decltype( to_string_test<T>( ) );

				template<typename T>
				inline constexpr bool has_to_string_v =
				  daw::is_detected_v<has_to_string_test, T>;

				template<typename CharT>
				struct parse_token {
					std::variant<size_t, CharT, daw::basic_string_view<CharT>> m_data;

					constexpr parse_token( ) noexcept = default;

					explicit constexpr parse_token(
					  daw::basic_string_view<CharT> sv ) noexcept
					  : m_data( sv ) {}

					explicit constexpr parse_token( CharT c ) noexcept
					  : m_data( c ) {}

					explicit constexpr parse_token( size_t idx ) noexcept
					  : m_data( idx ) {}

					template<typename OutputIterator, typename... Args>
					constexpr OutputIterator operator( )( OutputIterator out,
					                                      Args &&... args ) const {
						return daw::visit_nt(
						  m_data,
						  [&out]( daw::basic_string_view<CharT> sv ) {
							  return daw::algorithm::copy( sv.begin( ), sv.end( ), out );
						  },
						  [&]( CharT c ) {
							  *out++ = c;
							  return out;
						  },
						  [&]( size_t pos ) {
							  daw::pack_apply(
							    pos,
							    [&]( auto &&val ) {
								    using val_t = decltype( val );
								    if constexpr( std::is_convertible_v<
								                    val_t, daw::basic_string_view<CharT>> ) {
									    auto sv = daw::basic_string_view<CharT>( val );
									    out = daw::algorithm::copy( sv.begin( ), sv.end( ), out );
								    } else if constexpr( std::is_convertible_v<
								                           val_t,
								                           std::basic_string_view<CharT>> ) {
									    auto sv = std::basic_string_view<CharT>( val );
									    out = daw::algorithm::copy( sv.begin( ), sv.end( ), out );
								    } else if constexpr( std::is_same_v<
								                           std::basic_string<CharT>,
								                           daw::remove_cvref_t<val_t>> ) {
									    out =
									      daw::algorithm::copy( val.begin( ), val.end( ), out );
								    } else if constexpr( daw::can_to_os_string_int_v<
								                           daw::remove_cvref_t<val_t>> ) {
									    out = daw::to_os_string_int<CharT>( out, val );
								    } else if constexpr( has_to_string_v<val_t> ) {
									    using daw::string_fmt::v1::string_fmt_details::to_string;
									    using std::to_string;
									    auto const str = to_string( std::forward<val_t>( val ) );
									    out =
									      daw::algorithm::copy( str.begin( ), str.end( ), out );
								    } else if constexpr( traits::is_streamable_v<std::istream &,
								                                                 val_t> ) {
									    std::basic_stringstream<CharT> ss{};
									    ss << val;
									    auto const str = ss.str( );
									    out =
									      daw::algorithm::copy( str.begin( ), str.end( ), out );
								    }
							    },
							    std::forward<Args>( args )... );
							  return out;
						  } );
					}
				};

				template<typename CharT>
				constexpr size_t cxstrlen( CharT const *ptr ) noexcept {
					size_t result = 0;
					while( ptr[result] != '\0' ) {
						++result;
					}
					return result;
				}

				struct private_ctor {};

				template<typename T>
				auto has_reserve_test( )
				  -> decltype( std::declval<T>( ).reserve( std::declval<size_t>( ) ) );

				template<typename T>
				using has_reserve_detector = decltype( has_reserve_test<T>( ) );

				template<typename T>
				inline constexpr bool has_reserve_v =
				  daw::is_detected_v<has_reserve_detector, T>;

			} // namespace string_fmt_details
			template<typename CharT, size_t N>
			class fmt_t {
				daw::bounded_vector_t<string_fmt_details::parse_token<CharT>, N / 2>
				  m_tokens;

				constexpr static daw::bounded_vector_t<
				  string_fmt_details::parse_token<CharT>, N / 2>
				parse_tokens( daw::basic_string_view<CharT> msg ) {
					daw::bounded_vector_t<string_fmt_details::parse_token<CharT>, N / 2>
					  result{};
					size_t sz = 0;
					while( !msg.empty( ) and sz < msg.size( ) ) {
						assert( result.size( ) < result.capacity( ) );
						switch( msg[sz] ) {
						case static_cast<CharT>( '\\' ):
							if( sz > 0 ) {
								result.emplace_back( string_fmt_details::parse_token<CharT>(
								  msg.pop_front( sz ) ) );
								sz = 0;
							}
							assert( !msg.empty( ) );
							result.emplace_back(
							  string_fmt_details::parse_token<CharT>( msg.pop_front( ) ) );
							continue;
						case static_cast<CharT>( '{' ): {
							if( sz > 0 ) {
								result.emplace_back( string_fmt_details::parse_token<CharT>(
								  msg.pop_front( sz ) ) );
								sz = 0;
							}
							msg.remove_prefix( );
							assert( !msg.empty( ) );
							auto const digit =
							  daw::parser::parse_unsigned_int<size_t>( msg.pop_front( "}" ) );
							result.emplace_back(
							  string_fmt_details::parse_token<CharT>( digit ) );
							continue;
						}
						}
						++sz;
					}
					if( sz > 0 ) {
						assert( result.size( ) < result.capacity( ) );
						result.emplace_back(
						  string_fmt_details::parse_token<CharT>( msg ) );
					}
					return result;
				}

			public:
				explicit constexpr fmt_t( CharT const ( &fmt_string )[N] )
				  : m_tokens( parse_tokens( fmt_string ) ) {}

				constexpr fmt_t( string_fmt_details::private_ctor,
				                 CharT const *fmt_string )
				  : m_tokens(
				      parse_tokens( daw::basic_string_view<CharT>( fmt_string, N ) ) ) {
				}

				template<typename Result = std::basic_string<CharT>, typename... Args>
				constexpr Result operator( )( Args &&... args ) const {
					Result result{};
					if constexpr( string_fmt_details::has_reserve_v<Result> ) {
						result.reserve( N * 2U );
					}
					auto it = std::back_inserter( result );
					for( auto const &token : m_tokens ) {
						it = ( token )( it, std::forward<Args>( args )... );
					}
					return result;
				}
			};
			template<typename CharT, size_t N>
			fmt_t( CharT const ( & )[N] )->fmt_t<CharT, N>;

			template<typename CharT, size_t N, typename... Args>
			constexpr auto fmt( CharT const ( &format_str )[N], Args &&... args ) {
				auto const formatter = fmt_t( format_str );
				return formatter( std::forward<Args>( args )... );
			}

			template<
			  char const *fmt_string, typename Result = std::basic_string<char>,
			  size_t N = string_fmt_details::cxstrlen( fmt_string ), typename... Args>
			constexpr Result fmt( Args &&... args ) {
				constexpr auto const formatter =
				  fmt_t<char, N>( string_fmt_details::private_ctor{}, fmt_string );
				return formatter.template operator( )<Result>(
				  std::forward<Args>( args )... );
			}
		} // namespace v2
	}   // namespace string_fmt
	using string_fmt::v1::invalid_string_fmt_index;
	using string_fmt::v2::fmt;
	using string_fmt::v2::fmt_t;
} // namespace daw
