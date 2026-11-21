// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_enums.h"
#include "daw/json/impl/daw_json_find_result.h"
#include "daw/json/impl/daw_json_parse_name.h"
#include "daw/json/impl/daw_json_traits.h"

#include <daw/daw_consteval.h>
#include <daw/daw_likely.h>
#include <daw/daw_logic.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>

#include <array>
#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <string>

#if defined( DAW_JSON_PARSER_DIAGNOSTICS )
#include <cmath>
#include <iostream>
#endif

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			/// @brief Exact name comparison.  The size check rejects most
			/// candidates, so only names of equal size are compared
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
			names_equal( daw::string_view lhs, daw::string_view rhs ) {
				return std::size( lhs ) == std::size( rhs ) and
				       std::char_traits<char>::compare( std::data( lhs ),
				                                        std::data( rhs ),
				                                        std::size( lhs ) ) == 0;
			}

			template<typename Iterator = char const *>
			struct location_info_t {
				Iterator first = nullptr;
				Iterator last = nullptr;
				Iterator class_first = nullptr;
				Iterator class_last = nullptr;
				std::size_t counter = 0;

				[[nodiscard]] constexpr bool missing( ) const {
					return first == nullptr;
				}

				template<typename ParseState>
				constexpr void set_range( ParseState const &parse_state ) {
					first = parse_state.first;
					last = parse_state.last;
					class_first = parse_state.class_first;
					class_last = parse_state.class_last;
					counter = parse_state.counter;
				}

				template<typename ParseState>
				constexpr auto get_range( ) const {
					using range_t = typename ParseState::without_allocator_type;
					return range_t( first, last, class_first, class_last, counter );
				}
			};

			/// @brief The members of a class that need another member of the same
			/// class to be parsed: the tag of a tagged variant, or the size of a
			/// sized array.  Also when nullable.  Members given by their position
			/// in an ordered class are not included
			/// @brief Is there a char in name that JSON has to escape, so name can
			/// never be in the input as written
			[[nodiscard]] DAW_CONSTEVAL bool
			name_needs_escaping( daw::string_view name ) {
				for( char const c : name ) {
					if( c == '"' or c == '\\' or
					    static_cast<unsigned char>( c ) < 0x20U ) {
						return true;
					}
				}
				return false;
			}

			template<typename JsonMember, typename = void>
			struct class_dependent_member {
				static constexpr bool value = false;
			};

			template<typename JsonMember>
			struct class_dependent_member<JsonMember,
			                              std::enable_if_t<daw::is_detected_v<
			                                json_nullable_member_type_t, JsonMember>>>
			  : class_dependent_member<json_nullable_member_type_t<JsonMember>> {};

			template<typename JsonMember>
			struct class_dependent_member<
			  JsonMember,
			  std::enable_if_t<
			    not daw::is_detected_v<json_nullable_member_type_t, JsonMember> and
			    JsonMember::expected_type == JsonParseTypes::VariantTagged>> {
				using base_member = JsonMember;
				using dependent_member = typename JsonMember::tag_member;
				static constexpr bool value =
				  not is_an_ordered_member_v<dependent_member>;
			};

			template<typename JsonMember>
			struct class_dependent_member<
			  JsonMember,
			  std::enable_if_t<
			    not daw::is_detected_v<json_nullable_member_type_t, JsonMember> and
			    JsonMember::expected_type == JsonParseTypes::SizedArray>> {
				using base_member = JsonMember;
				using dependent_member = typename JsonMember::dependent_member;
				static constexpr bool value =
				  not is_an_ordered_member_v<dependent_member>;
			};

			template<typename JsonMember>
			inline constexpr bool has_class_dependent_v =
			  class_dependent_member<JsonMember>::value;

			template<typename JsonMember>
			[[nodiscard]] DAW_CONSTEVAL daw::string_view class_dependent_name( ) {
				return daw::string_view(
				  class_dependent_member<JsonMember>::dependent_member::name );
			}

			template<std::size_t Most>
			struct class_dependent_list {
				std::array<daw::string_view, Most> names{ };
				std::size_t count = 0;
			};

			/// @brief The different names of the members that other members of a
			/// class depend on.  One can be shared, or also be a mapped member
			template<typename... JsonMembers>
			[[nodiscard]] DAW_CONSTEVAL auto make_class_dependent_list( ) {
				constexpr std::size_t most =
				  ( std::size_t{ 0 } + ... +
				    static_cast<std::size_t>( has_class_dependent_v<JsonMembers> ) );
				class_dependent_list<most> result{ };
				auto const add = [&]( daw::string_view name ) {
					for( std::size_t n = 0; n < result.count; ++n ) {
						if( names_equal( result.names[n], name ) ) {
							return;
						}
					}
					result.names[result.count] = name;
					++result.count;
				};
				(
				  [&] {
					  if constexpr( has_class_dependent_v<JsonMembers> ) {
						  add( class_dependent_name<JsonMembers>( ) );
					  }
				  }( ),
				  ... );
				return result;
			}

			/// @brief The size of the longest of the mapped and dependent names.  A
			/// decoded member name longer than this cannot be any of them
			template<typename... JsonMembers>
			[[nodiscard]] DAW_CONSTEVAL std::size_t longest_member_name( ) {
				std::size_t result = 0;
				auto const add = [&]( daw::string_view name ) {
					if( std::size( name ) > result ) {
						result = std::size( name );
					}
				};
				( add( daw::string_view( JsonMembers::name ) ), ... );
				auto const dependents = make_class_dependent_list<JsonMembers...>( );
				for( std::size_t n = 0; n < dependents.count; ++n ) {
					add( dependents.names[n] );
				}
				return result;
			}

			/// @brief The names of a class's mapped members and of the members they
			/// depend on.  They are known when the class is mapped, so they are not
			/// stored with the locations
			template<typename... JsonMembers>
			struct class_member_names {
				static constexpr daw::string_view names[] = {
				  daw::string_view( JsonMembers::name )... };

				static constexpr auto dependent_list =
				  make_class_dependent_list<JsonMembers...>( );
				static constexpr std::size_t dependent_count = dependent_list.count;

				static constexpr std::size_t max_name_size =
				  longest_member_name<JsonMembers...>( );

				/// The slot of the member that JsonMember depends on
				template<typename JsonMember>
				[[nodiscard]] static DAW_CONSTEVAL std::size_t dependent_index( ) {
					for( std::size_t n = 0; n < dependent_count; ++n ) {
						if( names_equal( dependent_list.names[n],
						                 class_dependent_name<JsonMember>( ) ) ) {
							return n;
						}
					}
					return dependent_count;
				}

				/// The dependent slot named like member pos, or dependent_count when
				/// there is none
				[[nodiscard]] static DAW_CONSTEVAL std::size_t
				member_dependent_index( std::size_t pos ) {
					for( std::size_t n = 0; n < dependent_count; ++n ) {
						if( names_equal( dependent_list.names[n], names[pos] ) ) {
							return n;
						}
					}
					return dependent_count;
				}
			};

			/***
			 * The locations of a class's mapped members
			 * @tparam MemberCount Number of mapped members from json_class
			 * @tparam Names class_member_names of the mapped members
			 */
			template<std::size_t MemberCount, typename Iterator, typename Names>
			struct locations_members {
				using value_type = location_info_t<Iterator>;
				using reference = value_type &;
				using const_reference = value_type const &;
				using names_type = Names;
				value_type members[MemberCount];

				constexpr const_reference operator[]( std::size_t idx ) const {
					daw_json_ensure( idx < MemberCount, ErrorReason::NumberOutOfRange );
					return members[idx];
				}

				constexpr reference operator[]( std::size_t idx ) {
					daw_json_ensure( idx < MemberCount, ErrorReason::NumberOutOfRange );
					return members[idx];
				}

				static constexpr std::size_t size( ) {
					return MemberCount;
				}

				[[nodiscard]] static constexpr daw::string_view
				name( std::size_t idx ) {
					return Names::names[idx];
				}

				template<std::size_t start_pos>
				[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::size_t
				find_name( daw::string_view key ) const {
#if defined( DAW_JSON_BUGFIX_MSVC_EVAL_ORDER_002 )
					(void)start_pos;
					constexpr std::size_t first_pos = 0;
#else
					constexpr std::size_t first_pos = start_pos;
#endif
					for( std::size_t n = first_pos; n < MemberCount; ++n ) {
						if( names_equal( key, Names::names[n] ) ) {
							return n;
						}
					}
					return MemberCount;
				}
			};

			/***
			 * Contains an array of member location_info mapped in a json_class, and
			 * where the value of each member that other members depend on starts.
			 * They are recorded as the class is scanned, so they are never looked
			 * for by reading the class again
			 * @tparam MemberCount Number of mapped members from json_class
			 * @tparam Names class_member_names of the mapped members
			 */
			template<std::size_t MemberCount, typename Iterator, typename Names,
			         std::size_t DependentCount = Names::dependent_count>
			struct locations_info_t
			  : locations_members<MemberCount, Iterator, Names> {
				Iterator dependent_first[DependentCount]{ };

				/// The first member of a dependent name is the one used
				constexpr void record_dependent( daw::string_view name,
				                                 Iterator value_first ) {
					for( std::size_t n = 0; n < DependentCount; ++n ) {
						if( dependent_first[n] == nullptr and
						    names_equal( name, Names::dependent_list.names[n] ) ) {
							dependent_first[n] = value_first;
						}
					}
				}

				/// The name of member pos was read, value_first is its value
				template<std::size_t pos>
				constexpr void record_member_dependent( Iterator value_first ) {
					constexpr std::size_t dependent_pos =
					  Names::member_dependent_index( pos );
					if constexpr( dependent_pos < DependentCount ) {
						if( dependent_first[dependent_pos] == nullptr ) {
							dependent_first[dependent_pos] = value_first;
						}
					} else {
						(void)value_first;
					}
				}
			};

			template<std::size_t MemberCount, typename Iterator, typename Names>
			struct locations_info_t<MemberCount, Iterator, Names, 0>
			  : locations_members<MemberCount, Iterator, Names> {
				constexpr void record_dependent( daw::string_view, Iterator ) {}

				template<std::size_t pos>
				constexpr void record_member_dependent( Iterator ) {}
			};

			/// @brief Are the names of a class's mapped members all different.  Two
			/// members with one name are not supported when parsing, use a
			/// Constructor to make more than one value from a member
			template<typename... JsonMembers>
			[[nodiscard]] DAW_CONSTEVAL bool member_names_are_unique( ) {
				daw::string_view const names[] = {
				  daw::string_view( JsonMembers::name )... };
				for( std::size_t n = 0; n < sizeof...( JsonMembers ); ++n ) {
					for( std::size_t m = n + 1; m < sizeof...( JsonMembers ); ++m ) {
						if( names_equal( names[n], names[m] ) ) {
							return false;
						}
					}
				}
				return true;
			}

			/// @brief Does a mapped name, or the name of a member one depends on,
			/// have a char that JSON has to escape
			template<typename... JsonMembers>
			[[nodiscard]] DAW_CONSTEVAL bool member_names_need_escaping( ) {
				if( ( name_needs_escaping( daw::string_view( JsonMembers::name ) ) or
				      ... ) ) {
					return true;
				}
				auto const dependents = make_class_dependent_list<JsonMembers...>( );
				for( std::size_t n = 0; n < dependents.count; ++n ) {
					if( name_needs_escaping( dependents.names[n] ) ) {
						return true;
					}
				}
				return false;
			}

			// Should never be called outside a consteval context
			template<typename ParseState, typename... JsonMembers>
			DAW_ATTRIB_FLATINLINE static DAW_JSON_MAKE_LOC_INFO_CONSTEVAL auto
			make_locations_info( ) {
				static_assert( member_names_are_unique<JsonMembers...>( ),
				               "Each member of a parsed class needs a different name.  "
				               "Use a Constructor to make more than one value from a "
				               "member" );
				static_assert(
				  ParseState::allow_escaped_names or
				    not member_names_need_escaping<JsonMembers...>( ),
				  "A member name has a \", \\ or control char, which JSON input must "
				  "escape.  Names are only decoded before they are compared when "
				  "parsing with options::AllowEscapedNames::yes" );
				return locations_info_t<sizeof...( JsonMembers ),
				                        typename ParseState::iterator,
				                        class_member_names<JsonMembers...>>{ };
			}

			/***
			 * Get the position from already seen JSON members or move the parser
			 * forward until we reach the end of the class or the member.
			 * @tparam N Number of members in json_class
			 * @tparam ParseState see IteratorRange
			 * @param locations members location and names
			 * @param parse_state Current JSON data
			 * @return IteratorRange with begin( ) being start of value
			 */
			enum class AllMembersMustExist { yes, no };

			/// @brief Does the input start with the quoted name of member pos.  A
			/// name ending in a backslash is left to parse_name as the quote after
			/// it would be escaped.  When names are decoded, a name with a char
			/// that has to be escaped is left to parse_name too
			template<typename Names, std::size_t pos, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr bool
			match_expected_name( ParseState const &parse_state ) {
				constexpr daw::string_view expected = Names::names[pos];
				constexpr std::size_t sz = std::size( expected );
				constexpr bool never_as_written =
				  ParseState::allow_escaped_names
				    ? name_needs_escaping( expected )
				    : ( sz > 0 and expected[sz - 1U] == '\\' );
				if constexpr( never_as_written ) {
					(void)parse_state;
					return false;
				} else {
					return parse_state.size( ) > sz + 1U and
					       parse_state.first[0] == '"' and
					       parse_state.first[sz + 1U] == '"' and
					       std::char_traits<char>::compare(
					         parse_state.first + 1, std::data( expected ), sz ) == 0;
				}
			}

			/// @brief The mapped member whose name has escapes in the input.  The
			/// mapped names are decoded text, so the name is decoded before it is
			/// compared, into a buffer the size of the longest mapped name
			template<std::size_t start_pos, typename Locations, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_NOINLINE constexpr std::size_t
			find_escaped_name( Locations &locations, daw::string_view raw,
			                   ParseState const &parse_state ) {
				using names_t = typename Locations::names_type;
				auto buffer = name::name_buffer<names_t::max_name_size>{ };
				if( not name::decode_name( raw, buffer, parse_state ) ) {
					// Longer than any mapped name
					return std::size( locations );
				}
				locations.record_dependent( buffer.view( ), parse_state.first );
				return locations.template find_name<start_pos>( buffer.view( ) );
			}

			/// @brief The position of the mapped member with name, or the member
			/// count when there is none.  When name is a member that others depend
			/// on, where its value starts is kept
			template<std::size_t start_pos, typename Locations, typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::size_t
			find_and_record_name( Locations &locations, daw::string_view name,
			                      ParseState const &parse_state ) {
				if constexpr( ParseState::allow_escaped_names ) {
					for( char const c : name ) {
						if( c == '\\' ) {
							return find_escaped_name<start_pos>(
							  locations, name, parse_state );
						}
					}
				}
				locations.record_dependent( name, parse_state.first );
				return locations.template find_name<start_pos>( name );
			}

			template<std::size_t pos, AllMembersMustExist must_exist,
			         bool from_start = false, std::size_t N, typename Names,
			         typename ParseState>
			[[nodiscard]] DAW_ATTRIB_INLINE static constexpr find_result<ParseState>
			find_class_member(
			  ParseState &parse_state,
			  locations_info_t<N, typename ParseState::iterator, Names> &locations,
			  bool is_nullable, daw::string_view member_name ) {

				// silencing gcc9 warning as these are selectively used
				(void)is_nullable;
				(void)member_name;

				daw_json_assert_weak(
				  nsc_or( is_nullable,
				          ( not locations[pos].missing( ) ),
				          ( not parse_state.is_closing_brace_checked( ) ) ),
				  missing_member( member_name ),
				  parse_state );

				parse_state.trim_left_unchecked( );
				bool known = not locations[pos].missing( );
				while( nsc_and(
				  locations[pos].missing( ),
				  ( not parse_state.empty( ) and parse_state.front( ) != '}' ) ) ) {
					// Members usually arrive in mapped order, so try the expected name
					// first.  Its size is known, so only the closing quote needs
					// checking and the name does not need to be scanned
					// name_pos is initialized for C++17 constexpr, the store is elided
					std::size_t name_pos = std::size( locations );
					if( match_expected_name<Names, pos>( parse_state ) ) {
						parse_state.remove_prefix( std::size( Names::names[pos] ) + 2U );
						name::name_parser::trim_end_of_name( parse_state );
						name_pos = pos;
						locations.template record_member_dependent<pos>(
						  parse_state.first );
					} else {
						// parse_name checks if we have more and are quotes
						auto const name = parse_name( parse_state );
						name_pos =
						  find_and_record_name<( from_start ? 0 : pos )>(
						    locations, name, parse_state );
					}
					if constexpr( must_exist == AllMembersMustExist::yes ) {
						daw_json_assert_weak( name_pos < std::size( locations ),
						                      ErrorReason::UnknownMember,
						                      parse_state );
					} else {
#if defined( DAW_JSON_PARSER_DIAGNOSTICS )
						std::cerr << "DEBUG: Unknown member '" << name << '\n';
#endif
						if( name_pos >= std::size( locations ) ) {
							// This is not a member we are concerned with
							(void)skip_value( parse_state );
							parse_state.move_next_member_or_end_after_value( );
							continue;
						}
					}
					if( name_pos == pos ) {
						locations[pos].set_range( parse_state );
						break;
					} else {
#if defined( DAW_JSON_PARSER_DIAGNOSTICS )
						std::cerr << "DEBUG:	Out of order member '"
						          << locations.name( name_pos ) << "' found, looking for '"
						          << locations.name( pos ) << ". It is "
						          << std::abs( static_cast<long long>( pos ) -
						                       static_cast<long long>( name_pos ) )
						          << " members ahead in constructor\n";
#endif
						// We are out of order, store position for later
						// OLDTODO:	use type knowledge to speed up skip
						// OLDTODO:	on skipped classes see if way to store
						// 				member positions so that we don't have to
						//				re-parse them after
						// RESULT: storing preparsed is slower, don't try 3 times
						// it also limits the type of things we can parse potentially
						// Using locations to switch on BaseType is slower too
						locations[name_pos].set_range( skip_value( parse_state ) );

						if constexpr( ParseState::is_unchecked_input ) {
							if( name_pos + 1 < std::size( locations ) ) {
								parse_state.move_next_member( );
							} else {
								parse_state.move_next_member_or_end( );
							}
						} else {
							parse_state.move_next_member_or_end_after_value( );
						}
					}
				}
				if( locations[pos].missing( ) ) {
					known = true;
				}
				if constexpr( ParseState::has_allocator ) {
					return find_result{
					  locations[pos].template get_range<ParseState>( ).with_allocator(
					    parse_state ),
					  known };
				} else {
					return find_result<ParseState>{
					  locations[pos].template get_range<ParseState>( ), known };
				}
			}

			/// @brief Move forward through the class until the value of dependent
			/// member dependent_pos
			/// has been seen, keeping the positions of the mapped members that are
			/// passed
			template<std::size_t dependent_pos, AllMembersMustExist must_exist,
			         std::size_t N, typename Names, typename ParseState>
			constexpr void find_class_dependent(
			  ParseState &parse_state,
			  locations_info_t<N, typename ParseState::iterator, Names> &locations ) {
				parse_state.trim_left_unchecked( );
				while( nsc_and(
				  locations.dependent_first[dependent_pos] == nullptr,
				  ( not parse_state.empty( ) and parse_state.front( ) != '}' ) ) ) {
					auto const name = parse_name( parse_state );
					auto const name_pos =
					  find_and_record_name<0>( locations, name, parse_state );
					if constexpr( must_exist == AllMembersMustExist::yes ) {
						daw_json_assert_weak( name_pos < std::size( locations ),
						                      ErrorReason::UnknownMember,
						                      parse_state );
					}
					if( name_pos < std::size( locations ) ) {
						locations[name_pos].set_range( skip_value( parse_state ) );
					} else {
						(void)skip_value( parse_state );
					}
					parse_state.move_next_member_or_end_after_value( );
				}
			}
		} // namespace json_details
	} // namespace DAW_JSON_VER
} // namespace daw::json