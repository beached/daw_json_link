// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

namespace {
	struct properties_t {
		std::string_view name;
	}; // properties_t

#ifdef __cpp_nontype_template_parameter_class
	[[nodiscard, maybe_unused]] inline auto describe_json_class( properties_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<"name", std::string_view>>{};
	}
#else
	namespace symbols_properties_t {
		static constexpr char const name[] = "name";
	}

	[[nodiscard, maybe_unused]] inline auto describe_json_class( properties_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<symbols_properties_t::name, std::string_view>>{};
	}
#endif

	[[nodiscard, maybe_unused]] inline auto
	to_json_data( properties_t const &value ) {
		return std::forward_as_tuple( value.name );
	}

	struct geometry_t {
		std::string_view type;
		std::vector<std::vector<std::array<double, 2>>> coordinates;
	}; // geometry_t

	template<typename T>
	struct array_appender {
		T *ptr;

		template<size_t N>
		constexpr array_appender( std::array<T, N> &ary ) noexcept
		  : ptr( ary.data( ) ) {}

		template<typename U>
		constexpr void operator( )( U &&item ) noexcept {
			*ptr++ = std::forward<U>( item );
		}
	};

#ifdef __cpp_nontype_template_parameter_class
	[[nodiscard, maybe_unused]] inline auto describe_json_class( geometry_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<"type", std::string_view>,
		  json_array<
		    "coordinates",
		    json_array<no_name,
		               json_array<no_name, double, std::array<double, 2>,
		                          daw::construct_a_t<std::array<double, 2>>,
		                          array_appender<double>>>>>{};
	}
#else
	namespace symbols_geometry_t {
		static constexpr char const type[] = "type";
		static constexpr char const coordinates[] = "coordinates";
	} // namespace symbols_geometry_t

	[[nodiscard, maybe_unused]] inline auto describe_json_class( geometry_t ) {
		using namespace daw::json;

		return daw::json::class_description_t<
		  json_string_raw<symbols_geometry_t::type, std::string_view>,
		  json_array<
		    symbols_geometry_t::coordinates,
		    json_array<no_name,
		               json_array<no_name, double, std::array<double, 2>,
		                          daw::construct_a_t<std::array<double, 2>>,
		                          array_appender<double>>>>>{};
	}
#endif

	[[nodiscard, maybe_unused]] inline auto
	to_json_data( geometry_t const &value ) {
		return std::forward_as_tuple( value.type, value.coordinates );
	}

	struct features_element_t {
		std::string_view type;
		properties_t properties;
		geometry_t geometry;
	}; // features_element_t

#ifdef __cpp_nontype_template_parameter_class
	[[nodiscard, maybe_unused]] inline auto
	describe_json_class( features_element_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<"type", std::string_view>,
		  json_class<"properties", properties_t>,
		  json_class<"geometry", geometry_t>>{};
	}
#else
	namespace symbols_features_element_t {
		static constexpr char const type[] = "type";
		static constexpr char const properties[] = "properties";
		static constexpr char const geometry[] = "geometry";
	} // namespace symbols_features_element_t

	[[nodiscard, maybe_unused]] inline auto
	describe_json_class( features_element_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<symbols_features_element_t::type, std::string_view>,
		  json_class<symbols_features_element_t::properties, properties_t>,
		  json_class<symbols_features_element_t::geometry, geometry_t>>{};
	}
#endif

	[[nodiscard, maybe_unused]] inline auto
	to_json_data( features_element_t const &value ) {
		return std::forward_as_tuple( value.type, value.properties,
		                              value.geometry );
	}

	struct canada_object_t {
		std::string_view type;
		std::vector<features_element_t> features;
	}; // canada_object_t

#ifdef __cpp_nontype_template_parameter_class
	[[nodiscard, maybe_unused]] inline auto
	describe_json_class( canada_object_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<"type", std::string_view>,
		  json_array<"features", features_element_t>>{};
	}
#else
	namespace symbols_canada_object_t {
		static constexpr char const type[] = "type";
		static constexpr char const features[] = "features";
	} // namespace symbols_canada_object_t

	[[nodiscard, maybe_unused]] inline auto
	describe_json_class( canada_object_t ) {
		using namespace daw::json;
		return daw::json::class_description_t<
		  json_string_raw<symbols_canada_object_t::type, std::string_view>,
		  json_array<symbols_canada_object_t::features, features_element_t>>{};
	}
#endif

	[[nodiscard, maybe_unused]] inline auto
	to_json_data( canada_object_t const &value ) {
		return std::forward_as_tuple( value.type, value.features );
	}
} // namespace
