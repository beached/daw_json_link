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

#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

namespace daw {
	namespace json {
		enum class Optional : bool { True = true, False = false };
		namespace impl {
			template<typename T>
			struct json_number_t {};

			template<typename T>
			struct json_bool_t {};

			template<typename T>
			struct json_string_t {};

			template<typename T>
			struct json_class_t {};

			template<typename T>
			struct json_array_t {};

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
			struct decoder {
				std::array<daw::string_view, sizeof...( Args )> labels = {};
				using types = std::tuple<Args...>;

				template<typename T>
				using json_number = decoder<Args..., json_number_t<T>>;

				template<typename T>
				using json_bool = decoder<Args..., json_bool_t<T>>;

				template<typename T>
				using json_string = decoder<Args..., json_string_t<T>>;

				template<typename T>
				using json_class = decoder<Args..., json_class_t<T>>;

				template<typename T>
				using json_array = decoder<Args..., json_array_t<T>>;

				// TODO use SFINAE to lock down to string_view interface
				template<typename... JsonLabels>
				constexpr decoder( JsonLabels &&... lbls ) noexcept
				  : labels{daw::string_view( lbls )...} {

					/*static_assert(
					  ( sizeof...( JsonLabels ) + 1 ) == sizeof...( Args ),
					  "There is a mismatch between arg count and the label count" );
					*/
					static_assert(
					  impl::is_first_json_class_root_v<Args...>,
					  "Unexpected class, first must be json_class_root_t<T>" );
				}
			};
		} // namespace impl

		template<typename ClassType>
		struct link {
			template<typename T>
			using json_number = impl::decoder<impl::json_class_root_t<ClassType>,
			                                  impl::json_number_t<T>>;

			template<typename T>
			using json_bool =
			  impl::decoder<impl::json_class_root_t<ClassType>, impl::json_bool_t<T>>;

			template<typename T>
			using json_string = impl::decoder<impl::json_class_root_t<ClassType>,
			                                  impl::json_string_t<T>>;

			template<typename T>
			using json_class = impl::decoder<impl::json_class_root_t<ClassType>,
			                                 impl::json_class_t<T>>;

			template<typename T>
			using json_array = impl::decoder<impl::json_class_root_t<ClassType>,
			                                 impl::json_array_t<T>>;
		};

		namespace impl {
			template<typename T>
			using get_json_link_member_detect = decltype( T::get_json_link( ) );

			template<typename T>
			using get_json_link_func_detect =
			  decltype( get_json_link( static_cast<T *>( nullptr ) ) );

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
			constexpr decltype( auto ) call_get_json_link( ) noexcept(
			  noexcept( get_json_link( static_cast<T *>( nullptr ) ) ) ) {

				return get_json_link( static_cast<T *>( nullptr ) );
			}

			template<typename T, std::enable_if_t<no_get_json_link_v<T>,
			                                      std::nullptr_t> = nullptr>
			constexpr void call_get_json_link( ) noexcept {
				static_assert( !no_get_json_link_v<T>,
				               "There is no member function T::get_json_link( ) or a "
				               "free function get_json_link( T* )" );
			}

		} // namespace impl

		template<typename T>
		auto import_class( daw::string_view str ) noexcept {
			return impl::call_get_json_link<T>( );
		}
	} // namespace json
} // namespace daw

