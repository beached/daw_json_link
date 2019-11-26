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

#include "daw_json_parse_common.h"

namespace daw::json::impl {
	template<typename T, LiteralAsStringOpt LiteralAsString, typename Constructor,
	         JsonParseTypes ExpectedType, bool RangeCheck>
	struct json_number_base {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JsonParseTypes expected_type = ExpectedType;
		static constexpr LiteralAsStringOpt literal_as_string = LiteralAsString;
		static constexpr bool range_check = RangeCheck;
		using base = json_number_base;
	};

	template<typename T, LiteralAsStringOpt LiteralAsString, typename Constructor>
	struct json_bool_base {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr LiteralAsStringOpt literal_as_string = LiteralAsString;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Bool;
		using base = json_bool_base;
	};

	template<typename String, typename Constructor, bool EmptyStringNull,
	         bool DisallowHighEightBit>
	struct json_string_raw_base {
		using i_am_a_json_type = void;
		using parse_to_t = String;
		using constructor_t = Constructor;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::String;
		static constexpr bool empty_is_null = EmptyStringNull;
		static constexpr bool disallow_high_eight_bit = DisallowHighEightBit;
		using base = json_string_raw_base;
	};

	template<typename String, typename Constructor, typename Appender,
	         bool EmptyStringNull, bool DisallowHighEightBit>
	struct json_string_base {
		using i_am_a_json_type = void;
		using parse_to_t = String;
		using constructor_t = Constructor;
		using appender_t = Appender;
		static constexpr JsonParseTypes expected_type =
		  JsonParseTypes::StringEscaped;
		static constexpr bool empty_is_null = EmptyStringNull;
		static constexpr bool disallow_high_eight_bit = DisallowHighEightBit;
		using base = json_string_base;
	};

	template<typename T, typename Constructor>
	struct json_date_base {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Date;
		using base = json_date_base;
	};

	template<typename T, typename Constructor>
	struct json_class_base {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Class;
		using base = json_class_base;
	};

	template<typename T, typename FromConverter, typename ToConverter,
	         bool IsString>
	struct json_custom_base {
		using i_am_a_json_type = void;
		using parse_to_t = T;
		using to_converter_t = ToConverter;
		using from_converter_t = FromConverter;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Custom;
		static constexpr bool const is_string = IsString;
		using base = json_custom_base;
	};

	template<typename JsonElement, typename Container, typename Constructor,
	         typename Appender>
	struct json_array_base {
		using i_am_a_json_type = void;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_element_t = JsonElement;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::Array;
		using base = json_array_base;
	};

	template<typename Container, typename JsonValueType, typename JsonKeyType,
	         typename Constructor, typename Appender>
	struct json_key_value_base {
		using i_am_a_json_type = void;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_element_t = JsonValueType;
		using json_key_t = JsonKeyType;
		static constexpr JsonParseTypes expected_type = JsonParseTypes::KeyValue;
		using base = json_key_value_base;
	};

	template<typename Container, typename JsonValueType, typename JsonKeyType,
	         typename Constructor, typename Appender>
	struct json_key_value_array_base {
		using i_am_a_json_type = void;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_key_t = JsonKeyType;
		using json_value_t = JsonValueType;
		static constexpr JsonParseTypes expected_type =
		  JsonParseTypes::KeyValueArray;
		using base = json_key_value_array_base;
	};

} // namespace daw::json::impl
