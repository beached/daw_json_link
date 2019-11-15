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

#include <optional>
#include <string>
#include <type_traits>

#include <daw/daw_traits.h>

#include "daw_json_parse_common.h"

namespace daw::json {
	namespace to_strings {
		namespace {
			using std::to_string;
		}

		template<typename T>
		[[nodiscard]] auto to_string( std::optional<T> const &v )
		  -> decltype( to_string( *v ) ) {
			if( not v ) {
				return {"null"};
			}
			return to_string( *v );
		}
	} // namespace to_strings
	template<typename T>
	struct custom_to_converter_t {
		[[nodiscard]] constexpr decltype( auto ) operator( )( T &&value ) const {
			using std::to_string;
			return to_string( daw::move( value ) );
		}

		[[nodiscard]] constexpr decltype( auto )
		operator( )( T const &value ) const {
			using std::to_string;
			return to_string( value );
		}
	};

} // namespace daw::json

namespace daw::json::impl {

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Null>, OutputIterator it,
	           parse_to_t const &container );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValueArray>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Real>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::String>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
	           parse_to_t const &value );

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
	           parse_to_t const &value );

	//************************************************
	template<typename Container, typename OutputIterator,
	         daw::enable_when_t<daw::traits::is_container_like_v<
	           daw::remove_cvref_t<Container>>> = nullptr>
	[[nodiscard]] static constexpr OutputIterator
	copy_to_iterator( Container const &c, OutputIterator it ) {
		for( auto const &value : c ) {
			*it++ = value;
		}
		return it;
	}

	template<typename OutputIterator>
	[[nodiscard]] static constexpr OutputIterator
	copy_to_iterator( char const *ptr, OutputIterator it ) {
		if( ptr == nullptr ) {
			return it;
		}
		while( *ptr != '\0' ) {
			*it = *ptr;
			++it;
			++ptr;
		}
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Bool>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		if( value ) {
			return copy_to_iterator( "true", it );
		}
		return copy_to_iterator( "false", it );
	}

	template<typename T>
	[[nodiscard]] static constexpr auto deref_detect( T &&value ) noexcept
	  -> decltype( *value ) {
		return *value;
	}

	template<typename Optional>
	using deref_t = decltype(
	  deref_detect( std::declval<daw::remove_cvref_t<Optional> &>( ) ) );

	template<typename Optional>
	static inline constexpr bool is_valid_optional_v =
	  daw::is_detected_v<deref_t, Optional>;

	template<typename JsonMember, typename OutputIterator, typename Optional>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Null>, OutputIterator it,
	           Optional const &value ) {
		static_assert( is_valid_optional_v<Optional> );
		if( not value ) {
			it = copy_to_iterator( "null", it );
			return it;
		}
		using sub_type = typename JsonMember::sub_type;
		using tag_type = ParseTag<sub_type::expected_type>;
		return to_string<sub_type>( tag_type{}, it, *value );
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Real>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using ::daw::json::to_strings::to_string;
		using std::to_string;
		return copy_to_iterator( to_string( value ), it );
	}
	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Signed>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using ::daw::json::to_strings::to_string;
		using std::to_string;
		return copy_to_iterator( to_string( value ), it );
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Unsigned>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using ::daw::json::to_strings::to_string;
		using std::to_string;
		return copy_to_iterator( to_string( value ), it );
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::String>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		*it++ = '"';
		it = copy_to_iterator( value, it );
		*it++ = '"';
		return it;
	}

	template<typename T>
	[[nodiscard]] static constexpr bool is_null( std::optional<T> const &v ) {
		return not static_cast<bool>( v );
	}

	template<typename T>
	[[nodiscard]] static constexpr bool is_null( T const & ) {
		return false;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Date>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		using ::daw::json::impl::is_null;
		if( is_null( value ) ) {
			it = copy_to_iterator( "null", it );
		} else {
			*it++ = '"';
			using namespace ::daw::date_formatting::formats;
			it = copy_to_iterator( ::daw::date_formatting::fmt_string(
			                         "{0}T{1}:{2}:{3}Z", value, YearMonthDay<>{},
			                         Hour<>{}, Minute<>{}, Second<>{} ),
			                       it );
			*it++ = '"';
		}
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Class>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		return json_parser_description_t<
		  typename JsonMember::parse_to_t>::serialize( it, to_json_data( value ) );
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Custom>, OutputIterator it,
	           parse_to_t const &value ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		if constexpr( JsonMember::is_string ) {
			*it++ = '"';
			it =
			  copy_to_iterator( typename JsonMember::to_converter_t{}( value ), it );
			*it++ = '"';
			return it;
		} else {
			return copy_to_iterator( typename JsonMember::to_converter_t{}( value ),
			                         it );
		}
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::Array>, OutputIterator it,
	           parse_to_t const &container ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		*it++ = '[';
		if( not std::empty( container ) ) {
			auto count = std::size( container ) - 1U;
			for( auto const &v : container ) {
				it = to_string<typename JsonMember::json_element_t>(
				  ParseTag<JsonMember::json_element_t::expected_type>{}, it, v );
				if( count-- > 0 ) {
					*it++ = ',';
				}
			}
		}
		*it++ = ']';
		return it;
	}

	template<typename Key, typename Value>
	auto const &json_get_key( std::pair<Key, Value> const &kv ) {
		return kv.first;
	}

	template<typename Key, typename Value>
	auto const &json_get_value( std::pair<Key, Value> const &kv ) {
		return kv.second;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValueArray>, OutputIterator it,
	           parse_to_t const &container ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );
		using key_t = typename JsonMember::json_key_t;
		using value_t = typename JsonMember::json_value_t;
		*it++ = '[';
		if( not std::empty( container ) ) {
			auto count = std::size( container ) - 1U;
			for( auto const &v : container ) {
				*it++ = '{';
				// Append Key Name
				*it++ = '"';
				it = copy_to_iterator( key_t::name, it );
				*it++ = '"';
				*it++ = ':';
				// Append Key Value
				it = to_string<key_t>( ParseTag<key_t::expected_type>{}, it,
				                       json_get_key( v ) );

				*it++ = ',';
				// Append Value Name
				*it++ = '"';
				it = copy_to_iterator( value_t::name, it );
				*it++ = '"';
				*it++ = ':';
				// Append Value Value
				it = to_string<value_t>( ParseTag<value_t::expected_type>{}, it,
				                         json_get_value( v ) );

				*it++ = '}';
				if( count-- > 0 ) {
					*it++ = ',';
				}
			}
		}
		*it++ = ']';
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename parse_to_t>
	[[nodiscard]] static constexpr OutputIterator
	to_string( ParseTag<JsonParseTypes::KeyValue>, OutputIterator it,
	           parse_to_t const &container ) {

		static_assert(
		  std::is_convertible_v<parse_to_t, typename JsonMember::parse_to_t>,
		  "value must be convertible to specified type in class contract" );

		*it++ = '{';
		if( not std::empty( container ) ) {
			auto count = std::size( container ) - 1U;
			for( auto const &v : container ) {
				it = to_string<typename JsonMember::json_key_t>(
				  ParseTag<JsonMember::json_key_t::expected_type>{}, it,
				  json_get_key( v ) );
				*it++ = ':';
				it = to_string<typename JsonMember::json_element_t>(
				  ParseTag<JsonMember::json_element_t::expected_type>{}, it,
				  json_get_value( v ) );
				if( count-- > 0 ) {
					*it++ = ',';
				}
			}
		}
		*it++ = '}';
		return it;
	}

	template<typename JsonMember, typename OutputIterator, typename T>
	[[nodiscard]] static constexpr OutputIterator
	member_to_string( OutputIterator it, T const &value ) {
		it = to_string<JsonMember>( ParseTag<JsonMember::expected_type>{},
		                            daw::move( it ), value );
		return it;
	}

	template<typename T>
	using is_a_json_type_detect = typename T::i_am_a_json_type;

	template<typename T>
	inline constexpr bool is_a_json_type_v =
	  daw::is_detected_v<is_a_json_type_detect, T>;

	template<typename JsonMember, size_t pos, typename OutputIterator,
	         typename... Args>
	static constexpr void to_json_str( OutputIterator it,
	                                   std::tuple<Args...> const &args ) {

		static_assert( is_a_json_type_v<JsonMember>, "Unsupported data type" );
		*it++ = '"';
		it = copy_to_iterator( JsonMember::name, it );
		it = copy_to_iterator( "\":", it );
		it = member_to_string<JsonMember>( daw::move( it ), std::get<pos>( args ) );
		if constexpr( pos < ( sizeof...( Args ) - 1U ) ) {
			*it++ = ',';
		}
	}
} // namespace daw::json::impl
