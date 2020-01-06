// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include <daw/json/daw_json_link.h>

#include <array>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace daw::canada {
	namespace {
		struct properties_t {
			std::string_view name;
		}; // properties_t

		struct geometry_t {
			std::string_view type;
			std::vector<std::vector<std::array<double, 2>>> coordinates;
		}; // geometry_t

		struct features_element_t {
			std::string_view type;
			properties_t properties;
			geometry_t geometry;
		}; // features_element_t

		struct canada_object_t {
			std::string_view type;
			std::vector<features_element_t> features;
		}; // canada_object_t

		template<typename T>
		struct array_appender {
			T *ptr;

			template<size_t N>
			inline explicit array_appender( std::array<T, N> &ary ) noexcept
			  : ptr( ary.data( ) ) {}

			template<typename U>
			inline void operator( )( U &&item ) noexcept {
				*ptr++ = std::forward<U>( item );
			}
		};
	} // namespace
} // namespace daw::canada

namespace daw::json {
	template<>
	struct json_data_contract<daw::canada::properties_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<json_string_raw<"name", std::string_view>>;
#else
		static inline constexpr char const name[] = "name";
		using type = json_member_list<json_string_raw<name, std::string_view>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::canada::properties_t const &value ) {
			return std::forward_as_tuple( value.name );
		}
	};

	template<>
	struct json_data_contract<daw::canada::geometry_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type = json_member_list<
		  json_string_raw<"type", std::string_view>,
		  json_array<
		    "coordinates",
		    json_array<no_name,
		               json_array<no_name, double, std::array<double, 2>,
		                          daw::construct_a_t<std::array<double, 2>>,
		                          daw::canada::array_appender<double>>>>>;
#else
		static inline constexpr char const type_sym[] = "type";
		static inline constexpr char const coordinates[] = "coordinates";
		using type = json_member_list<
		  json_string_raw<type_sym, std::string_view>,
		  json_array<
		    coordinates,
		    json_array<no_name,
		               json_array<no_name, double, std::array<double, 2>,
		                          daw::construct_a_t<std::array<double, 2>>,
		                          daw::canada::array_appender<double>>>>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::canada::geometry_t const &value ) {
			return std::forward_as_tuple( value.type, value.coordinates );
		}
	};

	template<>
	struct json_data_contract<daw::canada::features_element_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string_raw<"type", std::string_view>,
		                   json_class<"properties", daw::canada::properties_t>,
		                   json_class<"geometry", daw::canada::geometry_t>>;
#else
		static inline constexpr char const type_sym[] = "type";
		static inline constexpr char const properties[] = "properties";
		static inline constexpr char const geometry[] = "geometry";
		using type =
		  json_member_list<json_string_raw<type_sym, std::string_view>,
		                   json_class<properties, daw::canada::properties_t>,
		                   json_class<geometry, daw::canada::geometry_t>>;

		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::canada::features_element_t const &value ) {
			return std::forward_as_tuple( value.type, value.properties,
			                              value.geometry );
		}
#endif
	};

	template<>
	struct json_data_contract<daw::canada::canada_object_t> {
#ifdef __cpp_nontype_template_parameter_class
		using type =
		  json_member_list<json_string_raw<"type", std::string_view>,
		                   json_array<"features", daw::canada::features_element_t>>;
#else
		static inline constexpr char const type_sym[] = "type";
		static inline constexpr char const features[] = "features";
		using type =
		  json_member_list<json_string_raw<type_sym, std::string_view>,
		                   json_array<features, daw::canada::features_element_t>>;
#endif
		[[nodiscard, maybe_unused]] static inline auto
		to_json_data( daw::canada::canada_object_t const &value ) {
			return std::forward_as_tuple( value.type, value.features );
		}
	};
} // namespace daw::json
