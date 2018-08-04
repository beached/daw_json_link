// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <tuple>

#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

namespace daw {
	namespace json {
		struct json_link_exception {};

		template<typename... Args>
		struct decoder_t;

		template<typename T>
		constexpr decltype( auto ) parse_class( daw::string_view str );

		namespace impl {
			constexpr void skip_whitespace( daw::string_view &str ) noexcept {
				struct is_whitespace {
					constexpr bool operator( )( char c ) const noexcept {
						return daw::parser::is_unicode_whitespace( c );
					}
				};

				str.remove_prefix( str.find_first_not_of_if( is_whitespace( ) ) );
			}

			template<typename T>
			struct json_number_t {
				template<typename... DecoderArgs>
				constexpr decltype( auto )
				operator( )( daw::string_view str,
				             decoder_t<DecoderArgs...> const &decoder,
				             daw::string_view JsonLabel ) {
					return parse_json_number( tag<T>{}, str );
				}

				static constexpr bool
				is_expected_type( daw::string_view &str ) noexcept {
					skip_whitespace( str );
					if( str.empty( ) ) {
						return false;
					}
					switch( str.front( ) ) {
					case '-':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						return true;
					case '0':
					default:
						return false;
					}
				}
			};

			template<typename T>
			struct json_bool_t {
				constexpr decltype( auto ) operator( )( daw::string_view str ) {
					return parse_json_bool( tag<T>{}, str );
				}

				static constexpr bool
				is_expected_type( daw::string_view &str ) noexcept {
					skip_whitespace( str );
					if( str.empty( ) ) {
						return false;
					}
					switch( str.front( ) ) {
					case 't':
					case 'f':
						return true;
					default:
						return false;
					}
				}
			};

			template<typename T>
			struct json_string_t {
				constexpr decltype( auto ) operator( )( daw::string_view str ) {
					return parse_json_string( tag<T>{}, str );
				}

				static constexpr bool
				is_expected_type( daw::string_view &str ) noexcept {
					skip_whitespace( str );
					if( str.empty( ) ) {
						return false;
					}
					return str.front( ) == '"';
				}
			};

			template<typename T>
			struct json_class_t {
				constexpr decltype( auto ) operator( )( daw::string_view str ) {
					return parse_class<T>( str );
				}

				static constexpr bool
				is_expected_type( daw::string_view &str ) noexcept {
					skip_whitespace( str );
					if( str.empty( ) ) {
						return false;
					}
					return str.front( ) == '{';
				}
			};

			template<typename T>
			struct json_array_t {

				static constexpr bool
				is_expected_type( daw::string_view &str ) noexcept {
					skip_whitespace( str );
					if( str.empty( ) ) {
						return false;
					}
					return str.front( ) == '[';
				}
			};

			template<typename T>
			struct json_class_root_t {};

			template<typename T>
			constexpr void
			is_first_json_class_root_test( json_class_root_t<T> * ) noexcept {}

			template<typename T>
			using is_first_json_class_root_detect = decltype(
			  is_first_json_class_root_test( static_cast<T *>( nullptr ) ) );

			template<typename T, typename...>
			constexpr bool is_first_json_class_root_v =
			  daw::is_detected_v<is_first_json_class_root_detect, T>;

			template<typename... Args>
			struct decoder_t {
				std::array<daw::string_view, sizeof...( Args )> labels = {};
				using types = std::tuple<Args...>;
				using size = std::integral_constant<size_t, sizeof...( Args )>;

				template<typename T>
				using json_number = decoder_t<Args..., json_number_t<T>>;

				template<typename T>
				using json_bool = decoder_t<Args..., json_bool_t<T>>;

				template<typename T>
				using json_string = decoder_t<Args..., json_string_t<T>>;

				template<typename T>
				using json_class = decoder_t<Args..., json_class_t<T>>;

				template<typename T>
				using json_array = decoder_t<Args..., json_array_t<T>>;

				template<typename... JsonLabels>
				constexpr decoder_t( JsonLabels &&... lbls ) noexcept
				  : labels{daw::string_view( lbls )...} {

					static_assert(
					  ( sizeof...( JsonLabels ) + 1 ) == sizeof...( Args ),
					  "There is a mismatch between arg count and the label count" );

					static_assert(
					  impl::is_first_json_class_root_v<Args...>,
					  "Unexpected class, first must be json_class_root_t<T>" );
				}
			};
		} // namespace impl

		template<typename ClassType>
		struct link {
			template<typename T>
			using json_number = impl::decoder_t<impl::json_class_root_t<ClassType>,
			                                    impl::json_number_t<T>>;

			template<typename T>
			using json_bool = impl::decoder_t<impl::json_class_root_t<ClassType>,
			                                  impl::json_bool_t<T>>;

			template<typename T>
			using json_string = impl::decoder_t<impl::json_class_root_t<ClassType>,
			                                    impl::json_string_t<T>>;

			template<typename T>
			using json_class = impl::decoder_t<impl::json_class_root_t<ClassType>,
			                                   impl::json_class_t<T>>;

			template<typename T>
			using json_array = impl::decoder_t<impl::json_class_root_t<ClassType>,
			                                   impl::json_array_t<T>>;
		};

		namespace impl {
			template<typename T>
			using get_json_link_member_detect = decltype( T::get_json_link( ) );

			template<typename T>
			using get_json_link_func_detect = decltype( get_json_link( tag<T>{} ) );

			template<typename T>
			constexpr bool get_json_link_member_v =
			  daw::is_detected_v<get_json_link_member_detect, T>;

			template<typename T>
			constexpr bool get_json_link_func_v =
			  daw::is_detected_v<get_json_link_func_detect, T>;

			template<typename T>
			constexpr bool no_get_json_link_v =
			  !get_json_link_member_v<T> && !get_json_link_func_v<T>;

			template<typename T, std::enable_if_t<get_json_link_member_v<T>,
			                                      std::nullptr_t> = nullptr>
			constexpr decltype( auto )
			call_get_json_link( ) noexcept( noexcept( T::get_json_link( ) ) ) {

				return T::get_json_link( );
			}

			template<typename T, std::enable_if_t<get_json_link_func_v<T>,
			                                      std::nullptr_t> = nullptr>
			constexpr decltype( auto )
			call_get_json_link( ) noexcept( noexcept( get_json_link( tag<T>{} ) ) ) {
				return get_json_link( tag<T>{} );
			}

			template<typename T, std::enable_if_t<no_get_json_link_v<T>,
			                                      std::nullptr_t> = nullptr>
			constexpr void call_get_json_link( ) noexcept {
				static_assert( !no_get_json_link_v<T>,
				               "There is no member function T::get_json_link( ) or a "
				               "free function get_json_link( daw::tag<T>{} )" );
			}

			constexpr auto find_next_string( daw::string_view &str ) noexcept {
				// Must be called from outside a string
				str.remove_prefix( str.find( '"' ) );
				if( str.empty( ) ) {
					return daw::string_view( );
				}

				struct {
					bool in_escape = false;

					bool operator( )( char c ) noexcept {
						if( c == '"' && !in_escape ) {
							return true;
						}
						in_escape = c == '\\' ? !in_escape : false;
					}
				} find_end_of_string;

				auto result = str.pop_front( find_end_of_string );
				return result;
			}

			struct unknown_lablel_exception {};

			template<typename... Args>
			constexpr auto ensure_label_is_known( decoder_t<Args...> const &link_types,
			                                      daw::string_view label ) {
				auto pos = std::find( link_types.labels.cbegin( ),
				                      link_types.labels.cend( ), label );
				if( pos == link_types.labels.cend( ) ) {
					throw unknown_lablel_exception{};
				}
				return static_cast<size_t>(
				  std::distance( link_types.labels.cbegin( ), pos ) );
			}

			template<typename... Args>
			constexpr auto find_part( decoder_t<Args...> const &link_types,
			                          daw::string_view &str ) {

				auto const lbl = find_next_string( str );
				ensure_label_is_known( link_types, lbl );
				return lbl;
			}

			constexpr void skip_json_class( daw::string_view &str ) {
				// non-validating moving paste json class
				daw::exception::DebugAssert( str.front( ) == '{', "Expected {" );
				intmax_t brace_count = 1;
				while( !str.empty( ) && brace_count > 0 ) {
					str.remove_prefix( );
					impl::skip_whitespace( str );
					switch( str.front( ) ) {
					case '"':
						find_next_string( str );
						break;
					case '{':
						++brace_count;
						break;
					case '}':
						--brace_count;
						break;
					}
				}
				daw::exception::Assert( brace_count == 0, "Unexpected brace count" );
			}

			constexpr void skip_value( daw::string_view &str ) noexcept {
				impl::skip_whitespace( str );
				if( str.empty( ) ) {
					return;
				}
				switch( str.front( ) ) {
				case '"':
					find_next_string( str );
					break;
				case '{':
					skip_json_class( str );
					break;
				case '[':
					skip_json_array( str );
					break;
				}
				if( str.front( ) == '"' ) {
					find_next_string( str );
				}
				str.remove_prefix( str.find( '"' ) - 1 );
			}

			template<typename... Args>
			constexpr auto find_parts( decoder_t<Args...> const &link_types,
			                           daw::string_view str ) {
				std::array<daw::string_view, link_types.size> result{};
				auto part = find_next_string( link_types, str );
				do {
					auto const lbl_pos = ensure_label_is_known( link_types, lbl );
					results[lbl_pos] = part;

					skip_value( str );
					part = find_next_string( link_types, str );
				} while( part );

				return parts;
			}
		} // namespace impl

		template<typename T>
		constexpr decltype( auto ) parse_class( daw::string_view str ) {
			auto const link_types = impl::call_get_json_link<T>( );
			auto const parts = find_parts( link_types );

			if( str.empty( ) ) {
				throw json_link_exception{};
			}
			if( str.front( ) != '{' || str.back( ) != '}' ) {
				throw json_link_exception{};
			}
			str.remove_prefix( );
			str.remove_suffix( );
			str = daw::parser::trim_left( str str.size( ) - 2 ) );
		}
	} // namespace json
} // namespace daw

