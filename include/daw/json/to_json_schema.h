// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link_types.h"
#include "impl/to_daw_json_string.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Class>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Null>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::KeyValue>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Array>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Bool>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Custom>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Date>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::KeyValueArray>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Real>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Signed>,
			                                         OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::StringEscaped>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::StringRaw>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::Unsigned>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::Variant>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::VariantTagged>,
			                OutputIterator out_it );

			template<typename JsonMember, typename OutputIterator>
			constexpr void member_to_json_schema( bool &is_first,
			                                      OutputIterator &out_it ) {
				if( is_first ) {
					is_first = false;
				} else {
					*out_it++ = ',';
				}
				*out_it++ = '"';
				out_it = utils::copy_to_iterator( out_it, JsonMember::name );
				out_it = utils::copy_to_iterator( out_it, R"(":)" );
				out_it = to_json_schema<JsonMember>(
				  ParseTag<JsonMember::expected_type>{ }, out_it );
				return out_it;
			}

			template<typename... JsonMembers, typename OutputIterator>
			constexpr OutputIterator members_to_json_schema( OutputIterator out_it ) {
				bool is_first = true;
				daw::Empty expander[]{
				  ( member_to_json_schema<JsonMembers>( is_first, out_it ),
				    daw::Empty{ } )...,
				  daw::Empty{} };
				(void)expander;
				return out_it;
			}

			template<typename JsonMember, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Class>,
			                                         OutputIterator out_it ) {
				out_it = utils::copy_to_iterator(
				  out_it, R"({"type":"object","properties":{)" );
				*out_it++ = '}';
				return out_it;
			}

		} // namespace json_details
	}   // namespace DAW_JSON_VER
} // namespace daw::json

namespace daw::json {
	inline namespace DAW_JSON_VER {
		template<typename T, typename OutputIterator>
		constexpr OutputIterator to_json_schema( OutputIterator out_it,
		                                         std::string_view id,
		                                         std::string_view title ) {
			*out_it++ = '{';
			out_it = utils::copy_to_iterator(
			  out_it,
			  R"("$schema":"https://json-schema.org/draft/2020-12/schema",)" );
			out_it = utils::copy_to_iterator( out_it, R"("$id":")" );
			out_it = utils::copy_to_iterator( out_it, id );
			out_it = utils::copy_to_iterator( out_it, R"(","title":")" );
			out_it = utils::copy_to_iterator( out_it, title );
			out_it = utils::copy_to_iterator( out_it, R"(",)" );

			using json_type = json_link_no_name<T>;
			DAW_FWD( )
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
