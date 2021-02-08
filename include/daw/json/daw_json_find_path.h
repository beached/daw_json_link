// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_event_parser.h"
#include "impl/daw_json_assert.h"

#include <daw/daw_algorithm.h>
#include <daw/iterator/daw_reverse_iterator.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

namespace daw::json {
	class json_path_node;

	[[nodiscard]] inline std::vector<json_path_node>
	find_json_path_stack_to( char const *parse_location, char const *doc_start );

	class json_path_node {
		std::string_view m_name{ };
		char const *m_value_start = nullptr;
		long long m_index = -1;
		JsonBaseParseTypes m_type = JsonBaseParseTypes::None;

		friend std::vector<json_path_node>
		find_json_path_stack_to( char const *parse_location,
		                         char const *doc_start );

		constexpr json_path_node( ) = default;
		constexpr json_path_node( JsonBaseParseTypes Type, std::string_view Name,
		                          long long Index, char const *ValueStart )
		  : m_name( Name )
		  , m_value_start( ValueStart )
		  , m_index( Index )
		  , m_type( Type ) {}

	public:
		/// What type of value is represented.
		[[nodiscard]] constexpr JsonBaseParseTypes type( ) const {
			return m_type;
		}

		/// The member name, only value for submembers of Class types
		[[nodiscard]] constexpr std::string_view name( ) const {
			return m_name;
		}

		/// The element index, only valid for elements of Array types
		[[nodiscard]] constexpr long long index( ) const {
			return m_index;
		}

		/// The beginning of the value's data in JSON document
		[[nodiscard]] constexpr char const *value_start( ) const {
			return m_value_start;
		}
	};

	/// Convert a json_path_node stack to a JSON Path string
	/// \param path_stack A vector with json_path_nodes representing the path in
	/// the JSON document tree
	/// \return A string in JSON Path format
	[[nodiscard]] inline std::string
	to_json_path_string( std::vector<json_path_node> const &path_stack ) {
		return daw::algorithm::accumulate(
		  path_stack.begin( ), path_stack.end( ), std::string{ },
		  []( auto &&state, json_path_node const &sv ) mutable {
			  if( sv.index( ) >= 0 ) {
				  state += "[" + std::to_string( sv.index( ) ) + "]";
			  } else if( not sv.name( ).empty( ) ) {
				  state += "." + static_cast<std::string>( sv.name( ) );
			  }
			  return std::forward<decltype( state )>( state );
		  } );
	}

	/// Get the json_path_nodes representing the path to the nearest value's
	/// position in the document
	/// \param parse_location The position in the document to find
	/// \param doc_start A pointer to the stat of the JSON document
	[[nodiscard]] inline std::vector<json_path_node>
	find_json_path_stack_to( char const *parse_location, char const *doc_start ) {
		if( parse_location == nullptr or doc_start == nullptr ) {
			return { };
		}
		if( std::less<>{ }( parse_location, doc_start ) ) {
			return { };
		}

		struct handler_t {
			char const *first;
			char const *last;
			std::vector<json_path_node> parse_stack{ };

			// This is for when we throw after array/class end, but before the next
			// value starts
			std::optional<json_path_node> last_popped{ };
			json_path_node state{ };

			JsonBaseParseTypes child_of( ) {
				if( parse_stack.empty( ) ) {
					return JsonBaseParseTypes::None;
				}
				return parse_stack.back( ).type( );
			}

			[[nodiscard]] bool handle_on_value( json_pair jp ) {
				if( auto const range = jp.value.get_range( );
				    range.empty( ) or last <= range.data( ) ) {
					return false;
				}
				if( auto const t = child_of( ); t == JsonBaseParseTypes::Class ) {
					state.m_name = *jp.name;
					state.m_index = -1;
				} else if( t == JsonBaseParseTypes::Array ) {
					state.m_name = { };
					state.m_index++;
				} else {
					state.m_name = { };
					state.m_index = -1;
				}
				state.m_value_start = jp.value.get_range( ).first;
				state.m_type = jp.value.type( );
				last_popped = std::nullopt;
				return true;
			}

			[[nodiscard]] bool handle_on_array_start( json_value const & ) {
				parse_stack.push_back( state );
				state = { };
				return true;
			}

			[[nodiscard]] bool handle_on_array_end( ) {
				if( not parse_stack.empty( ) ) {
					last_popped = parse_stack.back( );
					state = parse_stack.back( );
					parse_stack.pop_back( );
				}
				return true;
			}

			[[nodiscard]] bool handle_on_class_start( json_value const & ) {
				parse_stack.push_back( state );
				state = { };
				return true;
			}

			[[nodiscard]] bool handle_on_class_end( ) {
				if( not parse_stack.empty( ) ) {
					last_popped = parse_stack.back( );
					state = parse_stack.back( );
					parse_stack.pop_back( );
				}
				return true;
			}

			[[nodiscard]] bool handle_on_number( json_value jv ) {
				auto sv = std::string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				try {
#endif
					sv = jv.get_string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				} catch( json_exception const & ) {
					parse_stack.push_back( state );
					return false;
				}
#endif
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( state );
					return false;
				}
				return true;
			}

			[[nodiscard]] bool handle_on_bool( json_value jv ) {
				auto sv = std::string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				try {
#endif
					sv = jv.get_string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				} catch( json_exception const & ) {
					parse_stack.push_back( state );
					return false;
				}
#endif
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( state );
					return false;
				}
				return true;
			}

			[[nodiscard]] bool handle_on_string( json_value jv ) {
				auto sv = std::string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				try {
#endif
					sv = jv.get_string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				} catch( json_exception const & ) {
					parse_stack.push_back( state );
					return false;
				}
#endif
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( state );
					return false;
				}
				return true;
			}

			[[nodiscard]] bool handle_on_null( json_value jv ) {
				auto sv = std::string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				try {
#endif
					sv = jv.get_string_view( );
#if defined( DAW_USE_JSON_EXCEPTIONS )
				} catch( json_exception const & ) {
					parse_stack.push_back( state );
					return false;
				}
#endif
				if( sv.data( ) <= last and last <= ( sv.data( ) + sv.size( ) ) ) {
					parse_stack.push_back( state );
					return false;
				}
				return true;
			}
		} handler{ doc_start, parse_location + 1 };

#if defined( DAW_USE_JSON_EXCEPTIONS )
		try {
#endif
			json_event_parser( doc_start, handler );
#if defined( DAW_USE_JSON_EXCEPTIONS )
		} catch( json_exception const & ) {
			// Ignoring because we are only looking for the stack leading up to this
			// and it may have come from an error
		}
#endif
		if( handler.last_popped ) {
			handler.parse_stack.push_back( *handler.last_popped );
		}
		return std::move( handler.parse_stack );
	}

	[[nodiscard]] inline std::vector<json_path_node>
	find_json_path_stack_to( json_exception const &jex, char const *doc_start ) {
		return find_json_path_stack_to( jex.parse_location( ), doc_start );
	}

	[[nodiscard]] inline std::string
	find_json_path_to( char const *parse_location, char const *doc_start ) {
		return to_json_path_string(
		  find_json_path_stack_to( parse_location, doc_start ) );
	}

	[[nodiscard]] inline std::string find_json_path_to( json_exception const &jex,
	                                                    char const *doc_start ) {
		return to_json_path_string(
		  find_json_path_stack_to( jex.parse_location( ), doc_start ) );
	}

	[[nodiscard]] constexpr std::size_t
	find_line_number_of( char const *doc_pos, char const *doc_start ) {
		daw_json_assert( doc_pos != nullptr and doc_start != nullptr,
		                 ErrorReason::UnexpectedEndOfData );
		daw_json_assert( std::less<>{ }( doc_start, doc_pos ),
		                 ErrorReason::UnexpectedEndOfData );

		return daw::algorithm::accumulate( doc_start, doc_pos, std::size_t{ },
		                                   []( std::size_t count, char c ) {
			                                   if( c == '\n' ) {
				                                   return count + 1;
			                                   }
			                                   return count;
		                                   } );
	}

	[[nodiscard]] constexpr std::size_t
	find_line_number_of( json_path_node const &node, char const *doc_start ) {
		return find_line_number_of( node.value_start( ), doc_start );
	}

	[[nodiscard]] constexpr std::size_t
	find_column_number_of( char const *doc_pos, char const *doc_start ) {
		daw_json_assert( doc_pos != nullptr and doc_start != nullptr,
		                 ErrorReason::UnexpectedEndOfData );
		daw_json_assert( std::less<>{ }( doc_start, doc_pos ),
		                 ErrorReason::UnexpectedEndOfData );

		auto first = daw::reverse_iterator<char const *>( doc_pos );
		auto last = daw::reverse_iterator<char const *>( doc_start );
		auto pos =
		  std::distance( first, daw::algorithm::find( first, last, '\n' ) );
		daw_json_assert( pos >= 0, ErrorReason::Unknown );
		return static_cast<std::size_t>( pos );
	}

	[[nodiscard]] constexpr std::size_t
	find_column_number_of( json_path_node const &node, char const *doc_start ) {
		return find_column_number_of( node.value_start( ), doc_start );
	}
} // namespace daw::json
