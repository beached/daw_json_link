// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//
// Checks the stage 1 lexer cursor against a scalar tokenizer on random input,
// valid JSON or not.  Besides reading every event in order, first is moved the
// ways a parser moves it: past an event, and forward and back to value
// boundaries, including behind events that have been popped.

#include "defines.h"

#include <daw/json/impl/daw_json_lexer_cursor.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>

#if defined( DAW_JSON_HAS_SIMD )
namespace {
	using daw::json::json_details::lexer_cursor;

	constexpr std::size_t no_close = static_cast<std::size_t>( -1 );

	struct reference_event {
		std::size_t pos;
		bool is_string = false;
		// For strings, the closing quote and whether there is a backslash in it
		std::size_t close = no_close;
		bool escaped = false;
	};

	struct reference_tokens {
		std::vector<reference_event> events;
		// Positions where a clean reload gives the same events: not in a
		// string, not escaped and not part of a literal or number
		std::vector<std::size_t> boundaries;
		bool ends_in_string = false;
	};

	constexpr bool is_structural( char c ) {
		return c == '{' or c == '}' or c == '[' or c == ']' or c == ',' or c == ':';
	}

	constexpr bool is_whitespace( char c ) {
		return static_cast<unsigned char>( c ) >= 0x01 and
		       static_cast<unsigned char>( c ) <= 0x20;
	}

	reference_tokens tokenize( std::string const &doc ) {
		auto result = reference_tokens{ };
		bool in_string = false;
		bool escaped = false;
		bool in_token = false;
		bool string_clean = false;
		for( std::size_t n = 0; n < doc.size( ); ++n ) {
			if( not in_string and not escaped and not in_token ) {
				result.boundaries.push_back( n );
			}
			auto const c = doc[n];
			auto const is_escaped = escaped;
			escaped = not is_escaped and c == '\\';
			if( c == '"' and not is_escaped ) {
				if( in_string ) {
					result.events.back( ).close = n;
					result.events.back( ).escaped = not string_clean;
				} else {
					result.events.push_back( { n, true } );
					string_clean = true;
				}
				in_string = not in_string;
				in_token = false;
				continue;
			}
			if( in_string ) {
				if( c == '\\' ) {
					string_clean = false;
				}
				in_token = false;
				continue;
			}
			if( is_structural( c ) and not is_escaped ) {
				result.events.push_back( { n } );
				in_token = false;
			} else if( is_whitespace( c ) ) {
				in_token = false;
			} else {
				if( not in_token ) {
					result.events.push_back( { n } );
				}
				in_token = true;
			}
		}
		result.ends_in_string = in_string;
		return result;
	}

	std::string describe( std::string const &doc, std::size_t first,
	                      std::string_view what ) {
		auto result = std::string( what );
		result += " at first=" + std::to_string( first ) + " in [";
		for( char c : doc ) {
			if( c == '\0' ) {
				result += "\\0";
			} else if( c == '\n' ) {
				result += "\\n";
			} else {
				result += c;
			}
		}
		return result + "]";
	}

	template<typename Rng>
	std::string random_doc( Rng &rng ) {
		// 0x0C and 0x1A match a structural by their low four bits
		static constexpr char alphabet[] =
		  "{}[],:\"\"\"\\\\  \n\t\x01\x0c\x1atf1-.eaaaa";
		auto length = std::uniform_int_distribution<std::size_t>( 0, 700 )( rng );
		auto pick =
		  std::uniform_int_distribution<std::size_t>( 0, sizeof( alphabet ) - 1U );
		auto run = std::uniform_int_distribution<std::size_t>( 0, 99 );
		auto result = std::string( );
		while( result.size( ) < length ) {
			auto const choice = run( rng );
			if( choice < 3 ) {
				// Long strings and backslash runs cross block boundaries
				result += '"';
				result.append( run( rng ), 'x' );
				result += '"';
			} else if( choice < 6 ) {
				result.append( run( rng ) % 9U, '\\' );
			} else if( choice < 8 ) {
				result.append( run( rng ), ' ' );
			} else {
				// The index one past the end gives the terminating zero
				result += alphabet[pick( rng )];
			}
		}
		return result;
	}

	// Remove the event the last peek returned and check the closing quote of a
	// string.  Gives where the model of the cursor is after it
	std::size_t check_pop( lexer_cursor &cursor, std::string const &doc,
	                       char const *data, std::size_t first,
	                       reference_event const &expected ) {
		if( not expected.is_string ) {
			cursor.pop( );
			return expected.pos + 1U;
		}
		auto const close = cursor.pop_string( );
		if( expected.close == no_close ) {
			if( close != nullptr ) {
				daw_ensure_error( true,
				                  describe( doc,
				                            first,
				                            "Unexpected closing quote for " +
				                              std::to_string( expected.pos ) ) );
			}
			return doc.size( );
		}
		if( close != data + expected.close ) {
			daw_ensure_error( true,
			                  describe( doc,
			                            first,
			                            "Expected closing quote " +
			                              std::to_string( expected.close ) ) );
		}
		if( cursor.string_has_escape( close ) != expected.escaped ) {
			daw_ensure_error( true,
			                  describe( doc,
			                            first,
			                            "Wrong escape flag at " +
			                              std::to_string( expected.close ) ) );
		}
		return expected.close + 1U;
	}

	// Compare the event the cursor gave with the first reference event at or
	// after pos
	std::vector<reference_event>::const_iterator
	check_event( std::string const &doc, char const *data,
	             reference_tokens const &ref, std::size_t pos, char const *event,
	             std::size_t first ) {
		auto expected = ref.events.begin( );
		while( expected != ref.events.end( ) and expected->pos < pos ) {
			++expected;
		}
		auto const want =
		  expected == ref.events.end( ) ? nullptr : data + expected->pos;
		if( event != want ) {
			daw_ensure_error(
			  true,
			  describe( doc,
			            first,
			            "Expected event " +
			              ( want == nullptr ? std::string( "none" )
			                                : std::to_string( expected->pos ) ) +
			              " got " +
			              ( event == nullptr ? std::string( "none" )
			                                 : std::to_string( event - data ) ) ) );
		}
		return expected;
	}

	// Read every event from start, popping each and continuing with peek( )
	void check_all_events( std::string const &doc, char const *data,
	                       reference_tokens const &ref, std::size_t start ) {
		auto cursor = lexer_cursor( data + doc.size( ) );
		auto event = cursor.peek( data + start );
		auto pos = start;
		while( true ) {
			auto const expected = check_event( doc, data, ref, pos, event, start );
			if( expected == ref.events.end( ) ) {
				if( cursor.in_string( ) != ref.ends_in_string ) {
					daw_ensure_error( true, describe( doc, start, "Wrong end state" ) );
				}
				return;
			}
			pos = check_pop( cursor, doc, data, start, *expected );
			event = cursor.peek( );
		}
	}

	void check_structural_index( std::string const &doc, char const *data,
	                             reference_tokens const &ref ) {
		auto const capacity =
		  doc.size( ) + lexer_cursor::structural_index_padding;
		auto offsets = std::vector<std::uint32_t>( capacity );
		auto const result = lexer_cursor::build_structural_index(
		  std::string_view( data, doc.size( ) ), offsets.data( ) );

		auto expected_offsets = std::vector<std::size_t>{ };
		for( auto const &event : ref.events ) {
			expected_offsets.push_back( event.pos );
		}
		std::sort( expected_offsets.begin( ), expected_offsets.end( ) );
		daw_ensure( result.size == expected_offsets.size( ) );
		for( std::size_t n = 0; n < expected_offsets.size( ); ++n ) {
			daw_ensure( offsets[n] == expected_offsets[n] );
		}
		daw_ensure( offsets[result.size] == doc.size( ) );
		daw_ensure( result.in_string == ref.ends_in_string );
	}

	// Move first around like a parser.  peek( first ) gives the first event at
	// or after first, whatever was popped, and peek( ) continues after the
	// popped events.  A first that is not at a value boundary, just past an
	// opening quote say, is only valid right after the peek that gave the event
	// it steps past, so the move after it goes back to a boundary
	template<typename Rng>
	void check_moves( std::string const &doc, char const *data,
	                  reference_tokens const &ref, Rng &rng ) {
		auto cursor = lexer_cursor( data + doc.size( ) );
		auto const &boundaries = ref.boundaries;
		auto choose = std::uniform_int_distribution<int>( 0, 4 );
		std::size_t first = boundaries.empty( ) ? doc.size( ) : boundaries[0];
		auto event = cursor.peek( data + first );
		auto expected = check_event( doc, data, ref, first, event, first );
		for( int step = 0; step < 96; ++step ) {
			auto choice = choose( rng );
			if( choice >= 2 and not boundaries.empty( ) and
			    not std::binary_search(
			      boundaries.begin( ), boundaries.end( ), first ) ) {
				first = boundaries[std::uniform_int_distribution<std::size_t>(
				  0, boundaries.size( ) - 1U )( rng )];
				choice = -1;
			}
			switch( choice ) {
			case -1:
				break;
			case 0:
			case 1: {
				// Pop and continue, so the cursor runs ahead of first
				if( expected == ref.events.end( ) ) {
					continue;
				}
				auto const pos = check_pop( cursor, doc, data, first, *expected );
				event = cursor.peek( );
				expected = check_event( doc, data, ref, pos, event, first );
				continue;
			}
			case 2:
				// Just past the event, inside a string after an opening quote
				if( event != nullptr ) {
					first = static_cast<std::size_t>( event - data ) + 1U;
				}
				break;
			case 3: {
				// Forward to a value boundary, possibly far away
				auto const lo =
				  std::lower_bound( boundaries.begin( ), boundaries.end( ), first );
				if( lo != boundaries.end( ) ) {
					auto const span = static_cast<std::size_t>( boundaries.end( ) - lo );
					first = lo[std::uniform_int_distribution<std::size_t>( 0, span - 1U )(
					  rng )];
				}
				break;
			}
			default: {
				// Back to a value boundary, possibly behind where pops got to
				auto const hi =
				  std::lower_bound( boundaries.begin( ), boundaries.end( ), first );
				if( hi != boundaries.begin( ) ) {
					auto const span =
					  static_cast<std::size_t>( hi - boundaries.begin( ) );
					first = boundaries[std::uniform_int_distribution<std::size_t>(
					  0, span - 1U )( rng )];
				}
				break;
			}
			}
			event = cursor.peek( data + first );
			expected = check_event( doc, data, ref, first, event, first );
		}
	}

	// Count events across strings and block boundaries.
	std::size_t count_events( ) {
		constexpr char doc[] =
		  R"({"a\"b": [1, true, "x"], "c" :{ } , "long string that crosses the first 64 byte block": null})";
		auto cursor = lexer_cursor( doc + sizeof( doc ) - 1U );
		std::size_t result = 0;
		for( auto event = cursor.peek( doc ); event != nullptr;
		     event = cursor.peek( ) ) {
			if( *event == '"' ) {
				(void)cursor.pop_string( );
			} else {
				cursor.pop( );
			}
			++result;
		}
		return result;
	}
} // namespace

int main( ) {
	daw_ensure( count_events( ) == 20 );
	auto rng = std::mt19937_64( 0x5eed );
	for( int n = 0; n < 50'000; ++n ) {
		auto const doc = random_doc( rng );
		// Exactly sized so that reading past the end is caught by sanitizers
		auto const buffer = std::make_unique<char[]>( doc.size( ) );
		std::copy( doc.begin( ), doc.end( ), buffer.get( ) );
		auto const ref = tokenize( doc );
		check_structural_index( doc, buffer.get( ), ref );
		check_all_events( doc, buffer.get( ), ref, 0 );
		if( not ref.boundaries.empty( ) ) {
			auto const start =
			  ref.boundaries[std::uniform_int_distribution<std::size_t>(
			    0, ref.boundaries.size( ) - 1U )( rng )];
			check_all_events( doc, buffer.get( ), ref, start );
		}
		check_moves( doc, buffer.get( ), ref, rng );
	}
}
#else
int main( ) {}
#endif
