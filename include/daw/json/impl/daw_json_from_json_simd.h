// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.
#pragma once

#include "daw/json/impl/daw_json_arrow_proxy.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_lexer_cursor.h"
#include "daw/json/impl/daw_json_location_info.h"
#include "daw/json/impl/daw_json_parse_policy.h"
#include "daw/json/impl/daw_json_parse_value.h"
#include "daw/json/impl/daw_json_skip.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <memory_resource>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if defined( DAW_JSON_HAS_SIMD )
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_parse {
			using index_type = std::uint32_t;

			template<typename ParseState>
			inline constexpr bool policy_supports_simd_v =
			  std::is_same_v<typename ParseState::CommentPolicy,
			                 NoCommentSkippingPolicy> and
			  not ParseState::allow_string_mutation and
			  not ParseState::has_allocator and not ParseState::allow_escaped_names;

			// Logical positions in an immutable structural table. Missing members
			// have no range; byte bounds are derived only when a leaf parser needs
			// them.
			struct location {
				static constexpr index_type absent = daw::max_value<index_type>;
				index_type first = absent;
				index_type last = 0;
				index_type count = 0;

				[[nodiscard]] bool missing( ) const noexcept {
					return first == absent;
				}
			};
			static_assert( sizeof( location ) == 3U * sizeof( index_type ) );

			template<typename T>
			class unique_array_t {
				daw::not_null<std::pmr::memory_resource *> m_memory_resource;
				std::size_t m_size = 0;
				std::size_t m_capacity = 0;
				T *m_data = nullptr;

				inline void release( ) noexcept {
					if( m_data ) {
						m_memory_resource->deallocate(
						  m_data, sizeof( T ) * m_capacity, alignof( T ) );
					}
				}

			public:
				explicit unique_array_t(
				  daw::not_null<std::pmr::memory_resource *> memory_resource )
				  : m_memory_resource( memory_resource ) {}

				explicit unique_array_t(
				  daw::not_null<std::pmr::memory_resource *> memory_resource,
				  std::size_t capacity )
				  : m_memory_resource( memory_resource )
				  , m_capacity( capacity )
				  , m_data( static_cast<T *>( m_memory_resource->allocate(
				      sizeof( T ) * capacity, alignof( T ) ) ) ) {}

				unique_array_t( unique_array_t &&other ) noexcept
				  : m_memory_resource( other.m_memory_resource )
				  , m_size( std::exchange( other.m_size, std::size_t{ } ) )
				  , m_capacity( std::exchange( other.m_capacity, std::size_t{ } ) )
				  , m_data( std::exchange( other.m_data, nullptr ) ) {}

				unique_array_t &operator=( unique_array_t &&rhs ) noexcept {
					if( this != &rhs ) {
						release( );
						m_memory_resource = rhs.m_memory_resource;
						m_size = std::exchange( rhs.m_size, std::size_t{ } );
						m_capacity = std::exchange( rhs.m_capacity, std::size_t{ } );
						m_data = std::exchange( rhs.m_data, nullptr );
					}
					return *this;
				}

				~unique_array_t( ) {
					release( );
				}

				unique_array_t( unique_array_t const & ) = delete;
				unique_array_t &operator=( unique_array_t const & ) = delete;

				[[nodiscard]] inline T &operator[]( std::size_t idx ) noexcept {
					daw_dbg_ensure( m_size < m_capacity );
					return m_data[idx];
				}

				[[nodiscard]] inline T const &
				operator[]( std::size_t idx ) const noexcept {
					daw_dbg_ensure( m_size < m_capacity );
					return m_data[idx];
				}

				template<typename... Args>
				inline T &construct_at( std::size_t idx, Args &&...args ) {
					daw_dbg_ensure( m_size < m_capacity );
					return *std::construct_at(
					  m_data + static_cast<std::ptrdiff_t>( idx ), DAW_FWD( args )... );
				}

				template<typename U>
				inline T &push_back( U &&value ) {
					return construct_at( m_size++, DAW_FWD( value ) );
				}

				template<typename... Args>
				inline T &emplace_back( Args &&...args ) {
					return construct_at( m_size++, DAW_FWD( args )... );
				}

				[[nodiscard]] inline T *data( ) const noexcept {
					return m_data;
				}

				[[nodiscard]] std::size_t size( ) const noexcept {
					return m_size;
				}

				inline void set_size( std::size_t size ) noexcept {
					daw_dbg_ensure( size <= m_capacity );
					m_size = size;
				}

				[[nodiscard]] std::size_t capacity( ) const noexcept {
					return m_capacity;
				}
			};
			struct structural_index {
				std::string_view doc;
				// Opening containers store the token after their close and closing
				// containers store their immediate value count. Scalars and strings
				// have no metadata; they end before the next entry.
				unique_array_t<index_type> entries;
				unique_array_t<index_type> meta_data;

				[[nodiscard]] inline index_type size( ) const noexcept {
					daw_dbg_ensure( entries.size( ) == meta_data.size( ) );
					return static_cast<index_type>( entries.size( ) );
				}

				[[nodiscard]] inline char const *
				get( index_type token ) const noexcept {
					return doc.data( ) + entries[token];
				}

				[[nodiscard]] inline char const *
				operator[]( index_type token ) const noexcept {
					return doc.data( ) + entries[token];
				}

				[[nodiscard]] inline index_type &metadata( index_type token ) noexcept {
					return meta_data[token];
				}

				[[nodiscard]] inline index_type
				metadata( index_type token ) const noexcept {
					return meta_data[token];
				}

				[[nodiscard]] inline char const *
				byte_end( location loc ) const noexcept {
					auto const kind = *get( loc.first );
					if( kind == '{' or kind == '[' ) {
						return get( loc.last - 1 ) + 1;
					}
					// The next entry, whitespace may come before it
					return get( loc.last );
				}

				[[nodiscard]] inline location_info_t<>
				bounds( location loc ) const noexcept {
					location_info_t<> result;
					result.first = get( loc.first );
					result.last = byte_end( loc );
					return result;
				}
			};

			template<typename State>
			[[noreturn]] DAW_ATTRIB_NOINLINE inline void
			index_error( char const *position, char const *last,
			             ErrorReason reason ) {
				daw_json_error( true, reason, State( position, last ) );
			}

			// Build a minified structural view. Strings occupy one entry at their
			// opening quote; all other entries are structural
			// characters or scalar starts. A scalar ends before the next entry.
			// Scalar validation stays lazy.
			template<typename ParseState>
			inline structural_index build_index(
			  std::string_view doc,
			  daw::not_null<std::pmr::memory_resource *> memory_resource ) {

				if( DAW_UNLIKELY( doc.size( ) > daw::max_value<index_type> ) ) {
					DAW_UNLIKELY_BRANCH
					index_error<ParseState>(
					  doc.data( ), daw::data_end( doc ), ErrorReason::DocumentTooLarge );
				}
				// Every offset names a distinct input byte, so document_size is the
				// maximum possible entry count. Allocate each table once.
				auto const capacity =
				  doc.size( ) + lexer_cursor::structural_index_padding;

				auto result = structural_index{
				  doc,
				  unique_array_t<index_type>( memory_resource, capacity ),
				  unique_array_t<index_type>( memory_resource, capacity ) };

				auto const built =
				  lexer_cursor::build_structural_index( doc, result.entries );

				result.meta_data.set_size( built.size );

				if( DAW_UNLIKELY( built.in_string ) ) {
					DAW_UNLIKELY_BRANCH
					index_error<ParseState>( doc.data( ),
					                         daw::data_end( doc ),
					                         ErrorReason::UnexpectedEndOfData );
				}
				if( DAW_UNLIKELY( built.size == 0 ) ) {
					DAW_UNLIKELY_BRANCH
					index_error<ParseState>(
					  doc.data( ), daw::data_end( doc ), ErrorReason::EmptyJSONDocument );
				}
				return result;
			}

			template<typename State>
			inline location value_range( structural_index const &index,
			                             index_type token ) noexcept {
				auto const kind = *index[token];
				if( kind != '{' and kind != '[' ) {
					return { token, static_cast<index_type>( token + 1 ), 0 };
				}
				auto const end = index.metadata( token );
				return { token, end, index.metadata( end - 1 ) };
			}

			template<typename State>
			inline location prepare_root( structural_index &index ) {
				auto const kind = *index[0];
				if( DAW_UNLIKELY( kind != '{' and kind != '[' ) ) {
					DAW_UNLIKELY_BRANCH
					return value_range<State>( index, 0 );
				}

				enum class container_state : std::uint8_t {
					array_value_or_end,
					array_comma_or_end,
					object_name_or_end,
					object_value,
					object_comma_or_end
				};

				struct open_container {
					index_type token;
					index_type count;
					char close;
					container_state state;
				};

				auto containers = std::vector<open_container>{ };
				containers.reserve( 64 );
				containers.push_back( { 0,
				                        0,
				                        static_cast<char>( kind == '{' ? '}' : ']' ),
				                        kind == '{'
				                          ? container_state::object_name_or_end
				                          : container_state::array_value_or_end } );

				auto pos = index_type{ 1 };
				auto close_container = [&]( char current ) -> location {
					auto const opening = containers.back( );
					if( DAW_UNLIKELY( opening.close != current ) ) {
						DAW_UNLIKELY_BRANCH
						index_error<State>( index[pos],
						                    daw::data_end( index.doc ),
						                    ErrorReason::InvalidBracketing );
					}
					containers.pop_back( );
					index.metadata( opening.token ) = static_cast<index_type>( pos + 1 );
					index.metadata( pos ) = opening.count;
					++pos;
					return { opening.token, pos, opening.count };
				};

				auto take_structural_value = [&]( open_container &container ) {
					++container.count;
					auto const current = *index[pos];
					switch( current ) {
					case ',':
					case ':':
					case '}':
					case ']':
						DAW_UNLIKELY_BRANCH
						index_error<State>( index[pos],
						                    daw::data_end( index.doc ),
						                    ErrorReason::InvalidStartOfValue );
					case '"':
						++pos;
						break;
					case '{':
					case '[': {
						auto const opening = pos++;
						containers.push_back(
						  { opening,
						    0,
						    static_cast<char>( current == '{' ? '}' : ']' ),
						    current == '{' ? container_state::object_name_or_end
						                   : container_state::array_value_or_end } );
					} //
					break;
					default:
						++pos;
						break;
					}
				};

				while( pos < index.size( ) ) {
					auto &container = containers.back( );
					auto const current = *index[pos];
					switch( container.state ) {
					case container_state::array_value_or_end:
						if( current == container.close ) {
							auto const result = close_container( current );
							if( containers.empty( ) ) {
								return result;
							}
						} else {
							container.state = container_state::array_comma_or_end;
							take_structural_value( container );
						}
						break;
					case container_state::array_comma_or_end:
						if( current == ',' ) {
							container.state = container_state::array_value_or_end;
							++pos;
						} else if( current == container.close ) {
							auto const result = close_container( current );
							if( containers.empty( ) ) {
								return result;
							}
						} else {
							DAW_UNLIKELY_BRANCH
							index_error<State>( index[pos],
							                    daw::data_end( index.doc ),
							                    ErrorReason::InvalidEndOfValue );
						}
						break;
					case container_state::object_name_or_end:
						if( current == container.close ) {
							auto const result = close_container( current );
							if( containers.empty( ) ) {
								return result;
							}
							break;
						}
						if( DAW_UNLIKELY( current != '"' or pos + 1 >= index.size( ) or
						                  *index[pos + 1] != ':' ) ) {
							DAW_UNLIKELY_BRANCH
							index_error<State>( index[pos],
							                    daw::data_end( index.doc ),
							                    ErrorReason::MissingMemberNameOrEndOfClass );
						}
						pos = static_cast<index_type>( pos + 2 );
						container.state = container_state::object_value;
						break;
					case container_state::object_value:
						container.state = container_state::object_comma_or_end;
						take_structural_value( container );
						break;
					case container_state::object_comma_or_end:
						if( current == ',' ) {
							container.state = container_state::object_name_or_end;
							++pos;
						} else if( current == container.close ) {
							auto const result = close_container( current );
							if( containers.empty( ) ) {
								return result;
							}
						} else {
							index_error<State>( index[pos],
							                    daw::data_end( index.doc ),
							                    ErrorReason::InvalidEndOfValue );
						}
						break;
					default:
						break;
					}
				}
				DAW_UNLIKELY_BRANCH
				index_error<State>( std::data( index.doc ),
				                    daw::data_end( index.doc ),
				                    ErrorReason::UnexpectedEndOfData );
			}

			// A view over shared structural entries. Deferred parsing never creates a
			// lexer cursor and cannot invalidate a parent's table or continuation.
			template<typename ParseState>
			struct reader {
				structural_index const &index;
				location range;
				index_type position;
				char const *last;

				explicit inline reader( structural_index const &input, location value )
				  : index( input )
				  , range( value )
				  , position( value.first )
				  , last( input.byte_end( value ) ) {}

				[[nodiscard]] inline ParseState state_at( char const *first ) const {
					return ParseState( first, last );
				}

				[[nodiscard]] DAW_ATTRIB_INLINE char const *peek( ) const noexcept {
					return position < range.last ? index[position] : nullptr;
				}

				DAW_ATTRIB_INLINE void pop( ) noexcept {
					++position;
				}
			};

			template<typename ParseState>
			[[noreturn]] DAW_ATTRIB_NOINLINE inline void
			error_at( reader<ParseState> const &r, char const *position,
			          ErrorReason reason ) {
				daw_json_error(
				  true, reason, r.state_at( position == nullptr ? r.last : position ) );
			}

			template<typename ParseState>
			[[noreturn]] DAW_ATTRIB_NOINLINE inline void
			missing_member_at( reader<ParseState> const &r, char const *position,
			                   daw::string_view name ) {
				daw_json_error( true,
				                missing_member( std::string_view( std::data( name ),
				                                                  std::size( name ) ) ),
				                r.state_at( position == nullptr ? r.last : position ) );
			}

			template<typename State>
			inline location take_value( reader<State> &r ) noexcept {
				auto const result = value_range<State>( r.index, r.position );
				r.position = result.last;
				return result;
			}

			/// The members of a json_member_list mapping
			template<typename T>
			struct member_list_of {
				static constexpr bool value = false;
			};

			template<typename... Members>
			struct member_list_of<json_member_list<Members...>> {
				static constexpr bool value = true;
				static constexpr std::size_t size = sizeof...( Members );

				template<std::size_t N>
				using member = daw::traits::nth_element<N, Members...>;
			};

			template<typename JsonMember, typename = void>
			struct class_members {
				static constexpr bool value = false;
			};

			template<typename JsonMember>
			struct class_members<JsonMember, std::void_t<json_data_contract_trait_t<
			                                   typename JsonMember::wrapped_type>>>
			  : member_list_of<
			      json_data_contract_trait_t<typename JsonMember::wrapped_type>> {};

			template<typename Members, std::size_t... Is>
			[[nodiscard]] inline constexpr bool
			walkable_members( std::index_sequence<Is...> ) {
				return (
				  not has_class_dependent_v<typename Members::template member<Is>> and
				  ... );
			}

			/// Independent json_member_list mappings can be constructed from
			/// saved member locations.
			template<typename JsonMember>
			[[nodiscard]] DAW_CONSTEVAL bool is_walkable_class( ) {
				if constexpr( JsonMember::expected_type != JsonParseTypes::Class ) {
					return false;
				} else if constexpr( not class_members<JsonMember>::value ) {
					return false;
				} else if constexpr( is_deduced_empty_class_v<JsonMember> ) {
					return false;
				} else {
					using members = class_members<JsonMember>;
					return walkable_members<members>(
					  std::make_index_sequence<members::size>{ } );
				}
			}

			/// A number or bool that is never in quotes, its value is one event
			template<typename JsonMember>
			[[nodiscard]] DAW_CONSTEVAL bool unquoted_scalar( ) {
				constexpr JsonParseTypes type = JsonMember::expected_type;
				if constexpr( type == JsonParseTypes::Real or
				              type == JsonParseTypes::Signed or
				              type == JsonParseTypes::Unsigned or
				              type == JsonParseTypes::Bool ) {
					return JsonMember::literal_as_string ==
					       options::LiteralAsStringOpt::Never;
				} else {
					return false;
				}
			}

			template<typename State>
			inline daw::string_view read_member_name( reader<State> &r ) {
				auto const first = r.index[r.position];
				auto const colon = r.index[r.position + 1];
				auto last = colon;
				while( last > first and
				       static_cast<unsigned char>( last[-1] ) - 1U < 0x20U ) {
					--last;
				}
				if( DAW_UNLIKELY( last <= first + 1 or last[-1] != '"' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, first, ErrorReason::InvalidMemberName );
				}
				r.position = static_cast<index_type>( r.position + 2 );
				return daw::string_view( first + 1,
				                         static_cast<std::size_t>( last - first - 2 ) );
			}

			// Name has fixed length at each instantiation, allowing the compiler
			// to lower short names to byte/word comparisons.
			template<typename Member>
			[[nodiscard]] inline bool matches_member( daw::string_view name ) {
				constexpr auto size = std::size( Member::name );
				return name.size( ) == size and
				       std::memcmp( name.data( ), std::data( Member::name ), size ) ==
				         0;
			}

			template<typename Members, std::size_t... Is>
			[[nodiscard]] constexpr std::size_t
			find_member_index( daw::string_view name, std::index_sequence<Is...> ) {
				std::size_t result = sizeof...( Is );
				(void)( ( matches_member<typename Members::template member<Is>>( name )
				            ? ( static_cast<void>( result = Is ), true )
				            : false ) or
				        ... );
				return result;
			}

			template<typename List>
			struct tuple_list_of {
				static constexpr bool value = false;
			};

			template<typename... Members>
			struct tuple_list_of<json_tuple_member_list<Members...>> {
				static constexpr bool value = true;
				using types = std::tuple<json_deduced_type<Members>...>;
			};

			template<typename Member, JsonParseTypes Type = Member::expected_type,
			         typename = void>
			struct tuple_members {
				static constexpr bool value = false;
			};

			template<typename Member>
			struct tuple_members<Member, JsonParseTypes::Tuple> {
				static constexpr bool value = true;
				using types = typename Member::sub_member_list;
			};

			template<typename Member>
			struct tuple_members<
			  Member, JsonParseTypes::Class,
			  std::void_t<json_data_contract_trait_t<typename Member::wrapped_type>>>
			  : tuple_list_of<
			      json_data_contract_trait_t<typename Member::wrapped_type>> {};

			template<typename Members, std::size_t... Is>
			DAW_CONSTEVAL bool independent_tuple( std::index_sequence<Is...> ) {
				return (
				  ( not is_an_ordered_member_v<std::tuple_element_t<Is, Members>> and
				    not has_class_dependent_v<std::tuple_element_t<Is, Members>> ) and
				  ... );
			}

			template<typename Member>
			DAW_CONSTEVAL bool is_walkable_tuple( ) {
				if constexpr( not tuple_members<Member>::value ) {
					return false;
				} else {
					using members = typename tuple_members<Member>::types;
					return independent_tuple<members>(
					  std::make_index_sequence<std::tuple_size_v<members>>{ } );
				}
			}

			// A scalar's bounds end at the next entry, so only whitespace may follow
			// the literal
			inline bool literal( location_info_t<> const &loc, char const *value,
			                     std::size_t length ) {
				auto const size = static_cast<std::size_t>( loc.last - loc.first );
				return size >= length and
				       std::memcmp( loc.first, value, length ) == 0 and
				       ( size == length or
				         static_cast<unsigned char>( loc.first[length] ) - 1U < 0x20U );
			}

			template<typename Member, typename State>
			inline json_result_t<Member> read_location( location const &,
			                                            reader<State> const & );

			template<typename Member, typename State>
			struct nullable_parser {
				location const &loc;
				reader<State> const &r;
				template<bool, typename OtherState>
				json_result_t<Member> parse( OtherState & ) const {
					return read_location<Member>( loc, r );
				}
			};

			template<typename Members, typename State, std::size_t... Is>
			inline std::size_t find_member( daw::string_view name,
			                                std::size_t expected,
			                                std::index_sequence<Is...> ) {
				// Preserve the expected-name path during the up-front scan.
				std::size_t found = Members::size;
				(void)( ( expected == Is and
				              matches_member<typename Members::template member<Is>>(
				                name )
				            ? ( static_cast<void>( found = Is ), true )
				            : false ) or
				        ... );
				if( found == Members::size ) {
					found =
					  find_member_index<Members>( name, std::index_sequence<Is...>{ } );
				}
				return found;
			}

			template<std::size_t Index, typename Members, typename State,
			         std::size_t N>
			inline json_result_t<
			  without_name<typename Members::template member<Index>>>
			read_member( std::array<location, N> const &locations,
			             reader<State> const &r ) {
				using member = typename Members::template member<Index>;
				using value = without_name<member>;
				if constexpr( not is_json_nullable_v<value> ) {
					if( DAW_UNLIKELY( locations[Index].missing( ) ) ) {
						DAW_UNLIKELY_BRANCH
						missing_member_at( r,
						                   r.last,
						                   daw::string_view( std::data( member::name ),
						                                     std::size( member::name ) ) );
					}
				}
				return read_location<value>( locations[Index], r );
			}

			template<typename Member, typename State, std::size_t... Is>
			inline json_result_t<Member> read_class( location const &loc,
			                                         reader<State> const &parent,
			                                         std::index_sequence<Is...> ) {
				using members = class_members<Member>;
				using T = json_result_t<Member>;
				using Constructor = json_constructor_t<Member>;
				DAW_CPP23_STATIC_LOCAL constexpr bool exact =
				  all_json_members_must_exist_v<T, State>;
				auto r = reader<State>( parent.index, loc );
				auto event = r.peek( );
				if( DAW_UNLIKELY( event == nullptr or *event != '{' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, event, ErrorReason::InvalidClassStart );
				}
				r.pop( );
				auto locations = std::array<location, members::size>{ };
				std::size_t expected = 0;
				for( auto member_number = index_type{ 0 }; member_number < loc.count;
				     ++member_number ) {
					event = r.peek( );
					auto const name = read_member_name( r );
					auto const found = find_member<members, State>(
					  name, expected, std::index_sequence<Is...>{ } );
					if constexpr( exact ) {
						if( DAW_UNLIKELY( found == members::size or
						                  not locations[found].missing( ) ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, event, ErrorReason::UnknownMember );
						}
					}
					auto const current_loc = take_value( r );
					if( found < members::size ) {
						locations[found] = current_loc;
						expected = found + 1;
					}
					if( member_number + 1 < loc.count ) {
						r.pop( );
					}
				}
				if constexpr( should_construct_explicitly_v<Constructor, T, State> ) {
					return T{ read_member<Is, members>( locations, r )... };
				} else {
					auto state = r.state_at( r.index[loc.first] );
					return construct_value_tp<T, Constructor>(
					  state, fwd_pack{ read_member<Is, members>( locations, r )... } );
				}
			}

			template<typename Element, typename State>
			struct array_iterator {
				using iterator_category = std::input_iterator_tag;
				using value_type = json_result_t<Element>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using difference_type = std::ptrdiff_t;
				using size_type = std::size_t;
				reader<State> *r = nullptr;
				location value;
				std::size_t remaining = 0;

				array_iterator( ) = default;

				inline explicit array_iterator( reader<State> &input, std::size_t hint )
				  : r( &input )
				  , remaining( hint ) {
					if( remaining == 0 ) {
						r = nullptr;
					} else {
						value = take_value( *r );
					}
				}

				inline value_type operator*( ) const {
					return read_location<Element>( value, *r );
				}

				inline value_type operator*( ) {
					return read_location<Element>( value, *r );
				}

				inline array_iterator &operator++( ) {
					if( --remaining == 0 ) {
						r = nullptr;
					} else {
						r->pop( );
						value = take_value( *r );
					}
					return *this;
				}

				inline void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] DAW_ATTRIB_INLINE std::size_t size_hint( ) const {
					return remaining;
				}

				[[nodiscard]] DAW_ATTRIB_INLINE bool
				operator==( array_iterator const &rhs ) const {
					return r == rhs.r;
				}

				[[nodiscard]] DAW_ATTRIB_INLINE bool
				operator!=( array_iterator const &rhs ) const {
					return r != rhs.r;
				}
			};

			template<std::size_t Index, typename Members, typename State,
			         std::size_t N>
			inline json_result_t<std::tuple_element_t<Index, Members>>
			read_tuple_member( std::array<location, N> const &values,
			                   reader<State> const &r ) {
				return read_location<std::tuple_element_t<Index, Members>>(
				  values[Index], r );
			}

			template<typename Member, typename State, std::size_t... Is>
			inline json_result_t<Member> read_tuple( location const &loc,
			                                         reader<State> const &parent,
			                                         std::index_sequence<Is...> ) {
				using members = typename tuple_members<Member>::types;
				using T = json_base_type_t<Member>;
				using Constructor = json_constructor_t<Member>;
				auto r = reader<State>( parent.index, loc );
				auto event = r.peek( );
				if( event == nullptr or *event != '[' ) {
					error_at( r, event, ErrorReason::InvalidArrayStart );
				}
				r.pop( );
				std::array<location, sizeof...( Is )> values{ };
				auto captured = index_type{ 0 };
				auto capture = [&]( auto index ) {
					constexpr auto I = decltype( index )::value;
					if( I >= loc.count ) {
						return;
					}
					values[I] = take_value( r );
					++captured;
					if( captured < loc.count ) {
						r.pop( );
					}
				};
				( capture( daw::constant_v<Is> ), ... );
				while( captured < loc.count ) {
					if constexpr( all_json_members_must_exist_v<T, State> ) {
						error_at( r, r.peek( ), ErrorReason::UnknownMember );
					}
					(void)take_value( r );
					++captured;
					if( captured < loc.count ) {
						r.pop( );
					}
				}
				if constexpr( should_construct_explicitly_v<Constructor, T, State> ) {
					return T{ read_tuple_member<Is, members>( values, r )... };
				} else {
					auto state = r.state_at( r.index[loc.first] );
					return construct_value_tp<T, Constructor>(
					  state, fwd_pack{ read_tuple_member<Is, members>( values, r )... } );
				}
			}

			template<typename JsonMember, typename ParseState>
			inline json_result_t<JsonMember>
			read_location( location const &loc, reader<ParseState> const &parent ) {
				DAW_CPP23_STATIC_LOCAL constexpr auto type = JsonMember::expected_type;
				if constexpr( type == JsonParseTypes::Null ) {
					auto state = ParseState( );
					if( not loc.missing( ) ) {
						auto const first = parent.index[loc.first];
						if( *first == 'n' ) {
							auto const bounds = parent.index.bounds( loc );
							if( DAW_UNLIKELY( not literal( bounds, "null", 4 ) ) ) {
								DAW_UNLIKELY_BRANCH
								error_at( parent, bounds.first, ErrorReason::InvalidNull );
							}
						} else {
							state = ParseState( first, parent.last );
						}
					}
					return parse_value_null<JsonMember, true>(
					  state,
					  nullable_parser<typename JsonMember::member_type, ParseState>{
					    loc, parent } );
				} else {
					if( DAW_UNLIKELY( loc.missing( ) ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( parent, parent.last, ErrorReason::UnexpectedEndOfData );
					}
					if constexpr( is_walkable_class<JsonMember>( ) ) {
						return read_class<JsonMember>(
						  loc,
						  parent,
						  std::make_index_sequence<class_members<JsonMember>::size>{ } );
					} else if constexpr( is_walkable_tuple<JsonMember>( ) ) {
						return read_tuple<JsonMember>(
						  loc,
						  parent,
						  std::make_index_sequence<std::tuple_size_v<
						    typename tuple_members<JsonMember>::types>>{ } );
					} else if constexpr( type == JsonParseTypes::Array ) {
						auto r = reader<ParseState>( parent.index, loc );
						auto const event = r.peek( );
						if( DAW_UNLIKELY( event == nullptr or *event != '[' ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, event, ErrorReason::InvalidArrayStart );
						}
						r.pop( );
						auto state = r.state_at( event );
						using iterator =
						  array_iterator<typename JsonMember::json_element_t, ParseState>;
						return construct_value<json_result_t<JsonMember>,
						                       json_constructor_t<JsonMember>>(
						  state, iterator( r, loc.count ), iterator( ) );
					} else if constexpr( type == JsonParseTypes::StringRaw or
					                     type == JsonParseTypes::StringEscaped ) {
						auto const first = parent.index[loc.first];
						if( DAW_UNLIKELY( *first != '"' ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( parent, first, ErrorReason::InvalidString );
						}
						auto const bounds = parent.index.bounds( loc );
						auto state = ParseState( bounds.first, bounds.last );
						return parse_value<JsonMember, false, type>( state );
					} else {
						auto bounds = parent.index.bounds( loc );
						auto state = ParseState( bounds.first,
						                         bounds.last,
						                         bounds.class_first,
						                         bounds.class_last,
						                         bounds.counter );
						if constexpr( unquoted_scalar<JsonMember>( ) ) {
							// The bounds end at the next entry, not the number, so the
							// parsers that find the end are used
							return parse_value<JsonMember, false, type>( state );
						} else {
							// Dependent mappings, custom converters and key/value mappings
							// retain the existing bounded parser and its required class
							// context.
							state.class_first = bounds.first;
							state.class_last = bounds.last;
							return parse_value<JsonMember, false, type>( state );
						}
					}
				}
			}

			template<typename JsonMember, typename ParseState>
			inline json_result_t<JsonMember> from_json_simd_impl(
			  std::string_view doc,
			  daw::not_null<std::pmr::memory_resource *> memory_resource ) {

				auto index = build_index<ParseState>( doc, memory_resource );
				auto const loc = prepare_root<ParseState>( index );
				if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
					if( DAW_UNLIKELY( loc.last != index.size( ) ) ) {
						DAW_UNLIKELY_BRANCH
						index_error<ParseState>( index[loc.last],
						                         daw::data_end( index.doc ),
						                         ErrorReason::InvalidEndOfValue );
					}
				}
				auto const r = reader<ParseState>( index, loc );
				return read_location<JsonMember>( loc, r );
			}
		} // namespace json_details::simd_parse
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
