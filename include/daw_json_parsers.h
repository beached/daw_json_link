// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/optional.hpp>

#include <daw/daw_exception.h>
#include <daw/daw_parser_addons.h>
#include <daw/daw_parser_helper.h>

#include "daw_json_common.h"

namespace daw {
	namespace json {
		namespace parser {
			namespace {
				namespace impl {
					template<typename CharT, typename Traits>
					constexpr auto skip_ws( daw::basic_string_view<CharT, Traits> view ) noexcept {
						auto result = daw::parser::until_false( view.cbegin( ), view.cend( ),
						                                        daw::parser::is_unicode_whitespace<CharT> );
						return daw::basic_string_view<CharT, Traits>{result.last,
						                                     static_cast<size_t>( view.cend( ) - result.last )};
					}

					template<typename CharT, typename Traits>
					constexpr result_t<double> parse_number( daw::basic_string_view<CharT, Traits> view ) noexcept {
						auto r = 0.0;
						auto neg = false;
						if( '-' == view.front( ) ) {
							neg = true;
							view.remove_prefix( );
						}
						while( view.front( ) >= '0' && view.front( ) <= '9' ) {
							r = ( r * 10.0 ) + ( view.front( ) - '0' );
							view.remove_prefix( );
						}
						if( '.' == view.front( ) ) {
							auto f = 0.0;
							uint_fast8_t n = 0;
							view.remove_prefix( );
							while( view.front( ) >= '0' && view.front( ) <= '9' ) {
								f = ( f * 10.0 ) + ( view.front( ) - '0' );
								view.remove_prefix( );
								++n;
							}
							r += f / std::pow( 10.0, n );
						}
						if( neg ) {
							r = -r;
						}
						return {view, r};
					}

					template<typename CharT, typename Traits>
					constexpr result_t<bool> parse_false( daw::basic_string_view<CharT, Traits> view ) {
						daw::exception::daw_throw_on_false( view.size( ) >= 5,
						                                    "Expected boolean false, something else found" );
						auto const a = 'a' == view[1];
						auto const l = 'l' == view[2];
						auto const s = 's' == view[3];
						auto const e = 'e' == view[4];
						auto const value = ( a * l * s * e ) != 0;
						daw::exception::dbg_throw_on_false( value, "Expected boolean false, something else found" );
						view.remove_prefix( 5 );
						return {view, false};
					}

					template<typename CharT, typename Traits>
					constexpr result_t<bool> parse_true( daw::basic_string_view<CharT, Traits> view ) {
						daw::exception::daw_throw_on_false( view.size( ) >= 4,
						                                    "Expected boolean true, something else found" );
						auto const t = 't' == view[0];
						auto const r = 'r' == view[1];
						auto const u = 'u' == view[2];
						auto const e = 'e' == view[3];
						auto const value = ( t * r * u * e ) != 0;
						daw::exception::dbg_throw_on_false( value, "Expected boolean true, something else found" );
						view.remove_prefix( 4 );
						return {view, true};
					}

					template<typename CharT, typename Traits>
					constexpr auto parse_string_literal( daw::basic_string_view<CharT, Traits> view ) {
						auto const quote_char = view.front( );
						daw::exception::daw_throw_on_false( daw::parser::is_quote( quote_char ),
						                                    "Start of string does not beging with quote character" );

						size_t it = 1;
						size_t last_it = 0;
						bool found = false;
						while( it < view.size( ) ) {
							found = daw::parser::is_a( view[it], quote_char );
							if( found && !daw::parser::is_escape( view[last_it] ) ) {
								break;
							}
							found = false;
							last_it = it++;
						}
						daw::exception::daw_throw_on_false( found, "Could not find end of string before end of input" );
						view.remove_prefix( );
						view.remove_suffix( view.size( ) - it + 1 );
						return view;
					}
				} // namespace impl


			}     // namespace

			template<typename CharT, typename Traits>
			constexpr result_t<bool> is_null( daw::basic_string_view<CharT, Traits> view ) noexcept {
				view = impl::skip_ws( view );
				if( view.size( ) < 4 ) {
					return {view, false};
				}
				auto const n = 'n' == view[0];
				auto const u = 'u' == view[1];
				auto const l = 'l' == view[2];
				auto const l2 = 'l' == view[3];
				auto const value = ( n * u * l * l2 ) != 0;

				if( value ) {
					view.remove_prefix( 4 );
				}
				return {view, value};
			}

			template<typename CharT, typename Traits>
			constexpr result_t<int64_t> parse_json_integer( daw::basic_string_view<CharT, Traits> view ) noexcept {
				view = impl::skip_ws( view );
				auto result = impl::parse_number( view );
				return {result.view, static_cast<int64_t>( result.result )};
			}

			template<typename CharT, typename Traits>
			result_t<boost::optional<int64_t>> parse_json_integer_optional( daw::basic_string_view<CharT, Traits> view ) {
				auto const null_result = is_null( view );
				view = null_result.view;
				if( null_result.result ) {
					return {view, boost::none};
				}
				auto const parse_result = impl::parse_number( view );
				return {parse_result.view, static_cast<int64_t>( parse_result.result )};
			}

			template<typename CharT, typename Traits>
			constexpr result_t<double> parse_json_real( daw::basic_string_view<CharT, Traits> view ) noexcept {
				view = impl::skip_ws( view );
				return impl::parse_number( view );
			}

			template<typename CharT, typename Traits>
			result_t<boost::optional<double>> parse_json_real_optional( daw::basic_string_view<CharT, Traits> view ) {
				auto const null_result = is_null( view );
				view = null_result.view;
				if( null_result.result ) {
					return {view, boost::none};
				}
				auto parse_result = impl::parse_number( view );
				return {parse_result.view, parse_result.result};
			}

			template<typename CharT, typename Traits>
			constexpr result_t<bool> skip_json_value( daw::basic_string_view<CharT, Traits> view ) {
				daw::exception::daw_throw( "skip_json_value isn't implemented yet" );
				return {view, false};
			}

			template<typename CharT, typename Traits>
			constexpr result_t<bool> parse_json_boolean( daw::basic_string_view<CharT, Traits> view ) {
				view = impl::skip_ws( view );
				if( 'f' == view.front( ) ) {
					return impl::parse_false( view );
				}
				return impl::parse_true( view );
			}

			template<typename CharT, typename Traits>
			result_t<boost::optional<bool>> parse_json_boolean_optional( daw::basic_string_view<CharT, Traits> view ) {
				auto const null_result = is_null( view );
				view = null_result.view;
				if( null_result.result ) {
					return {view, boost::none};
				}
				if( 'f' == view.front( ) ) {
					auto result = impl::parse_false( view );
					return {result.view, result.result};
				}
				auto result = impl::parse_true( view );
				return {result.view, result.result};
			}

			template<typename CharT, typename Traits>
			constexpr result_t<daw::string_view> parse_json_string( daw::basic_string_view<CharT, Traits> view ) {
				view = impl::skip_ws( view );
				auto const parse_result = impl::parse_string_literal( view );
				view.remove_prefix( parse_result.size( ) + 2 ); // 2 quotes
				return {view, parse_result};
			}

			template<typename CharT, typename Traits>
			result_t<boost::optional<daw::string_view>>
			parse_json_string_optional( daw::basic_string_view<CharT, Traits> view ) {
				auto const null_result = is_null( view );
				view = null_result.view;
				if( null_result.result ) {
					return {view, boost::none};
				}
				auto const parse_result = impl::parse_string_literal( view );
				view.remove_prefix( parse_result.size( ) + 2 ); // 2 quotes
				return {view, parse_result};
			}

			template<typename Derived, typename CharT, typename Traits>
			result_t<boost::optional<Derived>> parse_json_object_optional( daw::basic_string_view<CharT, Traits> view ) {
				auto const null_result = is_null( view );
				view = null_result.view;
				if( null_result.result ) {
					return {view, boost::none};
				}
				auto result = Derived::from_json_string( view );
				return {result.view, std::move( result.result )};
			}

			template<typename CharT, typename Traits, typename ItemSetter, typename Parser>
			constexpr daw::string_view parse_json_array( daw::basic_string_view<CharT, Traits> view, ItemSetter item_setter,
			                                             Parser parser ) {
				view = impl::skip_ws( view );
				daw::exception::daw_throw_on_false( '[' == view.front( ), "Could not find start of array" );
				view.remove_prefix( );
				while( !view.empty( ) && ']' != view.front( ) ) {
					view = impl::skip_ws( view );
					auto result = parser( view );
					item_setter( result.result );
					view = result.view;
					view = impl::skip_ws( view );
					if( ',' != view.front( ) ) {
						break;
					}
					view.remove_prefix( );
				}
				daw::exception::daw_throw_on_true( view.empty( ), "Could not find end of array" );
				daw::exception::daw_throw_on_false( ']' == view.front( ), "Could not find end of array" );
				view.remove_prefix( );
				return view;
			}

			template<typename CharT, typename Traits, typename ItemSetter>
			constexpr daw::string_view parse_json_integer_array( daw::basic_string_view<CharT, Traits> view,
			                                                     ItemSetter item_setter ) {
				return parse_json_array( view, item_setter, &parse_json_integer<CharT, Traits> );
			}

			template<typename CharT, typename Traits, typename ItemSetter>
			constexpr daw::string_view parse_json_real_array( daw::basic_string_view<CharT, Traits> view,
			                                                  ItemSetter item_setter ) {
				return parse_json_array( view, item_setter, &parse_json_real<CharT, Traits> );
			}

			template<typename CharT, typename Traits, typename ItemSetter>
			constexpr daw::string_view parse_json_boolean_array( daw::basic_string_view<CharT, Traits> view,
			                                                     ItemSetter item_setter ) {
				return parse_json_array( view, item_setter, &parse_json_boolean<CharT, Traits> );
			}

			template<typename CharT, typename Traits, typename ItemSetter>
			constexpr daw::string_view parse_json_string_array( daw::basic_string_view<CharT, Traits> view,
			                                                    ItemSetter item_setter ) {
				return parse_json_array( view, item_setter, &parse_json_string<CharT, Traits> );
			}

			template<typename ObjT, typename CharT, typename Traits, typename ItemSetter>
			constexpr daw::string_view parse_json_object_array( daw::basic_string_view<CharT, Traits> view,
			                                                    ItemSetter item_setter ) {
				return parse_json_array( view, item_setter, &ObjT::from_json_string );
			}
		} // namespace parser
	}     // namespace json
} // namespace daw
