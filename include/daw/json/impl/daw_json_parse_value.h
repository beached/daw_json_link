// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "../daw_json_assert.h"
#include "daw_json_iterator_range.h"
#include "daw_json_parse_array_iterator.h"
#include "daw_json_parse_kv_array_iterator.h"
#include "daw_json_parse_kv_class_iterator.h"
#include "daw_json_parse_name.h"
#include "daw_json_parse_real.h"
#include "daw_json_parse_std_string.h"
#include "daw_json_parse_string_need_slow.h"
#include "daw_json_parse_string_quote.h"
#include "daw_json_parse_unsigned_int.h"
#include "daw_json_parse_value_fwd.h"

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>

namespace daw::json::json_details {
	template<LiteralAsStringOpt literal_as_string, bool KnownBounds = false,
	         typename Range>
	constexpr void skip_quote_when_literal_as_string( Range &rng ) {
		if constexpr( literal_as_string == LiteralAsStringOpt::Always ) {
			daw_json_assert_weak( rng.is_quotes_checked( ),
			                      "Unexpected quote prior to number", rng );
			rng.remove_prefix( );
			if constexpr( KnownBounds ) {
				rng.last = std::prev( rng.last );
			}
		} else if constexpr( literal_as_string == LiteralAsStringOpt::Maybe ) {
			daw_json_assert_weak( not rng.empty( ), "Unexpected end of stream", rng );
			if( rng.front( ) == '"' ) {
				rng.remove_prefix( );
				if constexpr( KnownBounds ) {
					rng.last = std::prev( rng.last );
				}
			}
		} else {
			daw_json_assert_weak( rng.front( ) != '"',
			                      "Unexpected quote prior to number", rng );
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Real>, Range &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::base_type;

		if constexpr( KnownBounds ) {
			daw_json_assert_weak(
			  parse_policy_details::is_number_start( rng.front( ) ),
			  "Expected number to start with on of \"0123456789-\"", rng );
			return constructor_t{ }( parse_real<element_t, true>( rng ) );
		} else {
			daw_json_assert_weak( rng.has_more( ), "Could not find value", rng );
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			daw_json_assert_weak(
			  parse_policy_details::is_number_start( rng.front( ) ),
			  "Expected number to start with on of \"0123456789-\"", rng );
			auto result = constructor_t{ }( parse_real<element_t, false>( rng ) );
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			daw_json_assert_weak(
			  parse_policy_details::at_end_of_item( rng.front( ) ),
			  "Expected whitespace or one of \",}]\" at end of number", rng );
			return result;
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Signed>, Range &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::base_type;
		static_assert( daw::is_signed_v<element_t>, "Expected signed type" );
		if constexpr( KnownBounds ) {
			daw_json_assert_weak(
			  parse_policy_details::is_number_start( rng.front( ) ),
			  "Expected number to start with on of \"0123456789e+-\"" );
		} else {
			daw_json_assert_weak( rng.has_more( ), "Could not find value" );
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			daw_json_assert_weak(
			  parse_policy_details::is_number_start( rng.front( ) ),
			  "Expected number to start with on of \"0123456789e+-\"", rng );
		}
		element_t sign = 1;
		if( rng.front( ) == '-' ) {
			rng.remove_prefix( );
			sign = -1;
		}

		if constexpr( KnownBounds ) {
			return constructor_t{ }(
			  sign * unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
			           Range::exec_tag, rng ) );
		} else {
			auto result = constructor_t{ }(
			  sign * unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
			           Range::exec_tag, rng ) );
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			rng.trim_left( );
			daw_json_assert_weak(
			  parse_policy_details::at_end_of_item( rng.front( ) ),
			  "Expected whitespace or one of \",}]\" at end of number", rng );
			return result;
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unsigned>, Range &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		using element_t = typename JsonMember::base_type;

		if constexpr( KnownBounds ) {
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			daw_json_assert_weak(
			  parse_policy_details::is_number( rng.front( ) ),
			  "Expected number to start with on of \"0123456789\"", rng );
			return constructor_t{ }(
			  unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
			    Range::exec_tag, rng ) );
		} else {
			daw_json_assert_weak( rng.has_more( ), "Could not find value", rng );
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			daw_json_assert_weak(
			  parse_policy_details::is_number( rng.front( ) ),
			  "Expected number to start with on of \"0123456789\"", rng );
			auto result = constructor_t{ }(
			  unsigned_parser<element_t, JsonMember::range_check, KnownBounds>(
			    Range::exec_tag, rng ) );
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			daw_json_assert_weak(
			  parse_policy_details::at_end_of_item( rng.front( ) ),
			  "Expected whitespace or one of \",}]\" at end of number", rng );
			return result;
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Null>, Range &rng ) {

		using constructor_t = typename JsonMember::constructor_t;
		if constexpr( KnownBounds ) {
			if( rng.is_null( ) ) {
				return constructor_t{ }( );
			}
			return parse_value<JsonMember, true>(
			  ParseTag<JsonMember::base_expected_type>{ }, rng );
		} else if constexpr( Range::is_unchecked_input ) {
			if( not rng.has_more( ) ) {
				return constructor_t{ }( );
			} else if( rng.front( ) == 'n' ) {
				rng.remove_prefix( 4 );
				rng.trim_left_unchecked( );
				rng.remove_prefix( );
				return constructor_t{ }( );
			}
			return parse_value<JsonMember>(
			  ParseTag<JsonMember::base_expected_type>{ }, rng );
		} else {
			if( rng.empty( ) ) {
				return constructor_t{ }( );
			} else if( rng.starts_with( "null" ) ) {
				rng.remove_prefix( 4 );
				daw_json_assert_weak(
				  parse_policy_details::at_end_of_item( rng.front( ) ),
				  "Unexpected value", rng );
				rng.trim_left_checked( );
				return constructor_t{ }( );
			}
			return parse_value<JsonMember>(
			  ParseTag<JsonMember::base_expected_type>{ }, rng );
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Bool>, Range &rng ) {
		daw_json_assert_weak( rng.size( ) >= 4, "Range to small to be a bool",
		                      rng );

		using constructor_t = typename JsonMember::constructor_t;

		if constexpr( KnownBounds ) {
			// We have already checked if it is a true/false
			if constexpr( Range::is_unchecked_input ) {
				return rng.front( ) == 't';
			} else {
				switch( rng.front( ) ) {
				case 't':
					return constructor_t{ }( true );
				case 'f':
					return constructor_t{ }( false );
				}
				daw_json_error( "Expected a literal true or false" );
			}
		} else {
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			bool result = false;
			if constexpr( Range::is_unchecked_input ) {
				if( rng.front( ) == 't' ) /* true */ {
					result = true;
					rng.remove_prefix( 4 );
				} else /* false */ {
					rng.remove_prefix( 5 );
				}
			} else {
				if( rng.starts_with( "true" ) ) {
					rng.remove_prefix( 4 );
					result = true;
				} else if( rng.starts_with( "false" ) ) {
					rng.remove_prefix( 5 );
				} else {
					daw_json_error( "Invalid boolean value, expected true or false" );
				}
			}
			skip_quote_when_literal_as_string<JsonMember::literal_as_string>( rng );
			rng.trim_left( );
			daw_json_assert_weak(
			  parse_policy_details::at_end_of_item( rng.front( ) ),
			  "Unexpected value", rng );
			return constructor_t{ }( result );
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringRaw>, Range &rng ) {

		using constructor_t = typename JsonMember::constructor_t;
		if constexpr( KnownBounds ) {
			if constexpr( JsonMember::empty_is_null == JsonNullable::Nullable ) {
				if( rng.empty( ) ) {
					return constructor_t{ }( );
				}
				return constructor_t{ }( rng.first, rng.size( ) );
			} else {
				return constructor_t{ }( rng.first, rng.size( ) );
			}
		} else {
			if constexpr( JsonMember::allow_escape_character ==
			              AllowEscapeCharacter::Allow ) {
				auto const str = skip_string( rng );
				if constexpr( JsonMember::empty_is_null == JsonNullable::Nullable ) {
					if( str.empty( ) ) {
						return constructor_t{ }( );
					}
					return constructor_t{ }( str.first, str.size( ) );
				} else {
					return constructor_t{ }( str.first, str.size( ) );
				}
			} else {
				rng.remove_prefix( );

				char const *const first = rng.first;
				rng.template move_to_next_of<'"'>( );
				char const *const last = rng.first;
				rng.remove_prefix( );
				if constexpr( JsonMember::empty_is_null == JsonNullable::Nullable ) {
					if( first == last ) {
						return constructor_t{ }( );
					}
					return constructor_t{ }( first,
					                         static_cast<std::size_t>( last - first ) );
				} else {
					return constructor_t{ }( first,
					                         static_cast<std::size_t>( last - first ) );
				}
			}
		}
	}

	template<typename... Args>
	std::true_type is_string( std::basic_string<Args...> const & ) {
		return { };
	}
	template<typename T>
	std::false_type is_string( T const & ) {
		return { };
	}

	/***
	 * We know that we are constructing a std::string or
	 * std::optional<std::string> We can take advantage of this and reduce the
	 * allocator time by presizing the string up front and then using a
	 * pointer to the data( ).
	 */
	template<typename JsonMember>
	struct can_parse_to_stdstring_fast
	  : std::conjunction<
	      std::disjunction<
	        std::is_same<std::string, json_result<JsonMember>>,
	        std::is_same<std::optional<std::string>, json_result<JsonMember>>>,
	      std::is_same<typename JsonMember::appender_t,
	                   basic_appender<std::string>>,
	      std::disjunction<
	        std::is_same<typename JsonMember::constructor_t,
	                     daw::construct_a_t<std::string>>,
	        std::is_same<typename JsonMember::constructor_t,
	                     daw::construct_a_t<std::optional<std::string>>>>> {};

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::StringEscaped>, Range &rng ) {
		if constexpr( can_parse_to_stdstring_fast<JsonMember>::value ) {
			constexpr bool AllowHighEightbits =
			  JsonMember::eight_bit_mode != EightBitModes::DisallowHigh;
			auto rng2 = KnownBounds ? rng : skip_string( rng );
			if( not AllowHighEightbits or needs_slow_path( rng2 ) ) {
				// There are escapes in the string
				return parse_string_known_stdstring<AllowHighEightbits,
				                                    json_result<JsonMember>, true>(
				  rng2 );
			}
			// There are no escapes in the string, we can just use the ptr/size ctor
			if constexpr( std::is_same_v<std::string, json_result<JsonMember>> ) {
				return std::string( rng2.first, rng2.size( ) );
			} else {
				return std::optional<std::string>( std::in_place, rng2.first,
				                                   rng2.size( ) );
			}
		} else {
			using constructor_t = typename JsonMember::constructor_t;
			using appender_t = typename JsonMember::appender_t;
			constexpr EightBitModes eight_bit_mode = JsonMember::eight_bit_mode;

			auto result = constructor_t{ }( );
			auto const app = [&] {
				if constexpr( std::is_same_v<typename JsonMember::parse_to_t,
				                             typename JsonMember::base_type> ) {
					return daw::construct_a<appender_t>( result );
				} else {
					return daw::construct_a<appender_t>( *result );
				}
			}( );

			bool const has_quote = rng.is_quotes_checked( );
			if( has_quote ) {
				rng.remove_prefix( );
			}
			daw_json_assert_weak( not rng.empty( ), "Unexpected end of data", rng );
			while( rng.front( ) != '"' ) {
				// TODO look at move_to_next_of
				while( not parse_policy_details::in<'\\', '"'>( rng.front( ) ) ) {
					daw_json_assert_weak( not rng.empty( ), "Unexpected end of data",
					                      rng );
					app( rng.front( ) );
					rng.remove_prefix( );
				}
				if( rng.front( ) == '\\' ) {
					daw_json_assert_weak( not rng.is_space_unchecked( ),
					                      "Invalid codepoint", rng );
					rng.remove_prefix( );
					switch( rng.front( ) ) {
					case 'b':
						app( '\b' );
						rng.remove_prefix( );
						break;
					case 'f':
						app( '\f' );
						rng.remove_prefix( );
						break;
					case 'n':
						app( '\n' );
						rng.remove_prefix( );
						break;
					case 'r':
						app( '\r' );
						rng.remove_prefix( );
						break;
					case 't':
						app( '\t' );
						rng.remove_prefix( );
						break;
					case 'u':
						decode_utf16( rng, app );
						break;
					case '\\':
					case '/':
					case '"':
						app( rng.front( ) );
						rng.remove_prefix( );
						break;
					default:
						if constexpr( eight_bit_mode == EightBitModes::DisallowHigh ) {
							daw_json_assert_weak(
							  not rng.is_space_unchecked( ) and
							    static_cast<unsigned>( rng.front( ) ) <= 0x7FU,
							  "string support limited to 0x20 < chr <= 0x7F when "
							  "DisallowHighEightBit is true",
							  rng );
						}
						app( rng.front( ) );
						rng.remove_prefix( );
					}
				} else {
					daw_json_assert_weak( not has_quote or rng.is_quotes_checked( ),
					                      "Unexpected end of string", rng );
				}
				daw_json_assert_weak( not has_quote or rng.has_more( ),
				                      "Unexpected end of data", rng );
			}

			if constexpr( not KnownBounds ) {
				daw_json_assert_weak( not has_quote or rng.is_quotes_checked( ),
				                      "Unexpected state, no \"", rng );
				rng.remove_prefix( );
			}
			return result;
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Date>, Range &rng ) {

		daw_json_assert_weak( rng.has_more( ), "Could not find value", rng );
		auto str = skip_string( rng );
		using constructor_t = typename JsonMember::constructor_t;
		return constructor_t{ }( str.first, str.size( ) );
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Custom>, Range &rng ) {

		if( ( ( rng.front( ) != '"' ) & ( rng.class_first != nullptr ) ) &
		    ( ( rng.first > rng.class_first ) and
		      ( *std::prev( rng.first ) == '"' ) ) ) {
			rng.first = std::prev( rng.first );
		}
		auto const str = skip_value( rng );

		using constructor_t = typename JsonMember::from_converter_t;
		return constructor_t{ }( std::string_view( str.first, str.size( ) ) );
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Class>, Range &rng ) {

		using element_t = typename JsonMember::base_type;
		daw_json_assert_weak( rng.has_more( ), "Attempt to parse empty string",
		                      rng );

		if constexpr( is_guaranteed_rvo_v<Range> ) {
			// This relies on non-trivial dtor's being allowed.  So C++20 constexpr or
			// not in a constant expression.  It does allow for construction of
			// classes without move/copy special members
			if constexpr( not KnownBounds ) {
				auto const oe =
				  daw::on_exit_success( [&] { rng.trim_left_checked( ); } );
			}
			return json_data_contract_trait_t<element_t>::template parse<element_t>(
			  rng );
		} else {
			if constexpr( KnownBounds ) {
				return json_data_contract_trait_t<element_t>::template parse<element_t>(
				  rng );
			} else {
				auto result =
				  json_data_contract_trait_t<element_t>::template parse<element_t>(
				    rng );
				// TODO: make trim_left
				rng.trim_left_checked( );
				return result;
			}
		}
	}

	/**
	 * Parse a key_value pair encoded as a json object where the keys are the
	 * member names
	 * @tparam JsonMember json_key_value type
	 * @tparam Range Input range typee
	 * @param rng Range of input to parse
	 * @return Constructed key_value container
	 */
	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValue>, Range &rng ) {

		static_assert( JsonMember::expected_type == JsonParseTypes::KeyValue,
		               "Expected a json_key_value" );
		daw_json_assert_weak(
		  rng.is_opening_brace_checked( ),
		  "Expected keyvalue type to be of class type and beginning with '{'",
		  rng );

		rng.remove_prefix( );
		// We are inside a KV map, we can expected a quoted name next
		rng.template move_to_next_of<'"'>( );

		using iter_t = json_parse_kv_class_iterator<JsonMember, Range, KnownBounds>;
		return typename JsonMember::constructor_t{ }( iter_t( rng ), iter_t( ) );
	}

	/**
	 * Parse a key_value pair encoded as a json object where the keys are the
	 * member names
	 * @tparam JsonMember json_key_value type
	 * @tparam Range Input Range type
	 * @param rng Range of input to parse
	 * @return Constructed key_value container
	 */
	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::KeyValueArray>, Range &rng ) {

		static_assert( JsonMember::expected_type == JsonParseTypes::KeyValueArray,
		               "Expected a json_key_value" );
		daw_json_assert_weak(
		  rng.is_opening_bracket_checked( ),
		  "Expected key/value type to be of class type and beginning with '{'",
		  rng );

		rng.remove_prefix( );

		using iter_t = json_parse_kv_array_iterator<JsonMember, Range, KnownBounds>;
		return typename JsonMember::constructor_t{ }( iter_t( rng ), iter_t( ) );
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Array>, Range &rng ) {
		rng.trim_left( );
		daw_json_assert_weak( rng.is_opening_bracket_checked( ),
		                      "Expected array to start with a '['", rng );
		rng.remove_prefix( );
		rng.trim_left_unchecked( );

		using iterator_t =
		  json_parse_array_iterator<JsonMember, Range, KnownBounds>;
		return
		  typename JsonMember::constructor_t{ }( iterator_t( rng ), iterator_t( ) );
	}

	template<JsonBaseParseTypes BPT, typename JsonMembers, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMembers>
	parse_variant_value( Range &rng ) {

		using element_t = typename JsonMembers::json_elements;
		constexpr std::size_t idx =
		  element_t::base_map[static_cast<int_fast8_t>( BPT )];
		if constexpr( idx < std::tuple_size_v<typename element_t::element_map_t> ) {
			using JsonMember =
			  std::tuple_element_t<idx, typename element_t::element_map_t>;
			return parse_value<JsonMember>(
			  ParseTag<JsonMember::base_expected_type>{ }, rng );
		} else {
			daw_json_error( "Unexpected JSON Variant type." );
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Variant>, Range &rng ) {

		switch( rng.front( ) ) {
		case '{':
			return parse_variant_value<JsonBaseParseTypes::Class, JsonMember>( rng );
		case '[':
			return parse_variant_value<JsonBaseParseTypes::Array, JsonMember>( rng );
		case 't':
		case 'f':
			return parse_variant_value<JsonBaseParseTypes::Bool, JsonMember>( rng );
		case '"':
			return parse_variant_value<JsonBaseParseTypes::String, JsonMember>( rng );
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '+':
		case '-':
			return parse_variant_value<JsonBaseParseTypes::Number, JsonMember>( rng );
		}
		daw_json_error( "Unexcepted data at start of json member" );
	}

	template<typename Result, typename TypeList, std::size_t pos = 0,
	         typename Range>
	constexpr Result parse_visit( std::size_t idx, Range &rng ) {
		if( idx == pos ) {
			using JsonMember = std::tuple_element_t<pos, TypeList>;
			return { parse_value<JsonMember>( ParseTag<JsonMember::expected_type>{ },
			                                  rng ) };
		}
		if constexpr( pos + 1 < std::tuple_size_v<TypeList> ) {
			return parse_visit<Result, TypeList, pos + 1>( idx, rng );
		} else {
			// Should never happen
			std::abort( );
		}
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	[[nodiscard, maybe_unused]] inline constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::VariantTagged>, Range &rng ) {

		using tag_member = typename JsonMember::tag_member;
		auto [is_found, rng2] = find_range<Range>(
		  daw::string_view( rng.class_first, static_cast<std::size_t>(
		                                       rng.last - rng.class_first ) ),
		  tag_member::name );

		daw_json_assert( is_found, "Tag Member is mandatory", rng );
		auto index = typename JsonMember::switcher{ }(
		  parse_value<tag_member>( ParseTag<tag_member::expected_type>{ }, rng2 ) );

		return parse_visit<json_result<JsonMember>,
		                   typename JsonMember::json_elements::element_map_t>(
		  index, rng );
	}

	template<typename JsonMember, bool KnownBounds, typename Range>
	constexpr json_result<JsonMember>
	parse_value( ParseTag<JsonParseTypes::Unknown>, Range &rng ) {
		using constructor_t = typename JsonMember::constructor_t;
		if constexpr( KnownBounds ) {
			return constructor_t{ }( rng.first, rng.size( ) );
		} else {
			auto value_rng = skip_value( rng );
			return constructor_t{ }( value_rng.first, value_rng.size( ) );
		}
	}
} // namespace daw::json::json_details
