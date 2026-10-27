// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.
#pragma once

#include "daw/json/impl/daw_json_arrow_proxy.h"
#include "daw/json/impl/daw_json_assert.h"
#include "daw/json/impl/daw_json_lexer_cursor.h"
#include "daw/json/impl/daw_json_parse_policy.h"
#include "daw/json/impl/daw_json_parse_value.h"

#include <daw/daw_span.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <tuple>
#include <type_traits>
#include <utility>

#if defined( DAW_HAS_SIMD )
namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details::simd_parse {
			using index_type = std::uint32_t;

			template<typename ParseState>
			inline constexpr bool policy_supports_simd_v =
			  std::is_same_v<typename ParseState::CommentPolicy,
			                 NoCommentSkippingPolicy> and
			  not ParseState::has_allocator and not ParseState::allow_escaped_names;

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
				struct byte_bounds {
					char const *first;
					char const *last;
				};

				std::string_view doc;
				unique_array_t<index_type> entries;

				[[nodiscard]] inline index_type size( ) const noexcept {
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

				[[nodiscard]] inline byte_bounds
				bounds( index_type token ) const noexcept {
					return { get( token ), get( static_cast<index_type>( token + 1 ) ) };
				}
			};

			template<typename State>
			[[noreturn]] DAW_ATTRIB_NOINLINE inline void
			index_error( char const *position, char const *last,
			             ErrorReason reason ) {
				using error_state = typename State::without_string_mutation_type;
				daw_json_error( true, reason, error_state( position, last ) );
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
				// maximum possible entry count. Allocate the table once.
				auto const capacity =
				  doc.size( ) + lexer_cursor::structural_index_padding;

				auto result = structural_index{
				  doc, unique_array_t<index_type>( memory_resource, capacity ) };

				auto const built =
				  lexer_cursor::build_structural_index( doc, result.entries );

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

			// A forward cursor over the immutable structural entries. A consumer owns
			// the grammar for its mapped type and leaves position at the first event
			// after its value.  The entries and document are held by value, so an
			// entry is one load from the reader instead of a chain through the index.
			// The document is the policy's iterator, so a policy allowing string
			// mutation can decode in-situ strings in place.  The index is complete
			// before any value is read and a decoded string never grows, so the
			// entries after it stay valid
			template<typename ParseState>
			struct reader {
				using iterator = typename ParseState::iterator;

				struct byte_bounds {
					iterator first;
					iterator last;
				};

				iterator document;
				index_type const *entries;
				index_type size;
				iterator last;
				index_type position = 0;
				bool source_is_zero_terminated = false;

				explicit inline reader( structural_index const &input, iterator doc,
				                        bool is_zero_terminated = false )
				  : document( doc )
				  , entries( input.entries.data( ) )
				  , size( input.size( ) )
				  , last( doc + input.entries[input.size( )] )
				  , source_is_zero_terminated( is_zero_terminated ) {}

				[[nodiscard]] DAW_ATTRIB_INLINE iterator
				operator[]( index_type token ) const noexcept {
					return document + entries[token];
				}

				/// A token's first byte and the first byte of the entry after it
				[[nodiscard]] DAW_ATTRIB_INLINE byte_bounds
				bounds( index_type token ) const noexcept {
					return { ( *this )[token],
					         ( *this )[static_cast<index_type>( token + 1 )] };
				}

				[[nodiscard]] static inline ParseState state_at( iterator f,
				                                                 iterator l ) {
					return ParseState( f, l );
				}

				[[nodiscard]] DAW_ATTRIB_INLINE iterator peek( ) const noexcept {
					return position < size ? ( *this )[position] : nullptr;
				}

				[[nodiscard]] DAW_ATTRIB_INLINE iterator end( ) const noexcept {
					return last;
				}

				DAW_ATTRIB_INLINE void pop( ) noexcept {
					++position;
				}
			};

			/// Errors only report a position, so they are given without mutation
			template<typename ParseState>
			[[nodiscard]] inline auto error_state_at( reader<ParseState> const &r,
			                                          char const *position ) {
				using error_state = typename ParseState::without_string_mutation_type;
				return error_state( position == nullptr ? r.end( ) : position,
				                    r.end( ) );
			}

			template<typename ParseState>
			[[noreturn]] DAW_ATTRIB_NOINLINE inline void
			error_at( reader<ParseState> const &r, char const *position,
			          ErrorReason reason ) {
				daw_json_error( true, reason, error_state_at( r, position ) );
			}

			template<typename ParseState>
			[[noreturn]] DAW_ATTRIB_NOINLINE inline void
			missing_member_at( reader<ParseState> const &r, char const *position,
			                   daw::string_view name ) {
				daw_json_error( true,
				                missing_member( std::string_view( std::data( name ),
				                                                  std::size( name ) ) ),
				                error_state_at( r, position ) );
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
			struct class_members<JsonMember,
			                     std::void_t<typename JsonMember::wrapped_type,
			                                 json_class_member_list_t<JsonMember>>>
			  : member_list_of<json_class_member_list_t<JsonMember>> {};

			/// A class whose json_data_contract is a json_type_alias, it parses as
			/// the member it aliases and is constructed from that result.
			template<typename JsonMember, typename = void>
			inline constexpr bool is_type_alias_class_v = false;

			template<typename JsonMember>
			inline constexpr bool is_type_alias_class_v<
			  JsonMember, std::void_t<json_data_contract_trait_t<
			                typename JsonMember::wrapped_type>>> =
			  is_a_json_map_alias_v<
			    json_data_contract_trait_t<typename JsonMember::wrapped_type>>;

			template<typename JsonMember>
			using aliased_member_t = typename json_data_contract_trait_t<
			  typename JsonMember::wrapped_type>::json_member;

			template<typename Members, std::size_t... Is>
			[[nodiscard]] inline constexpr bool
			walkable_members( std::index_sequence<Is...> ) {
				return (
				  not has_class_dependent_v<typename Members::template member<Is>> and
				  ... );
			}

			/// Independent json_member_list mappings can be consumed in input order.
			template<typename JsonMember>
			[[nodiscard]] DAW_CONSTEVAL bool is_walkable_class( ) {
				if constexpr( not is_json_class_mapping_v<JsonMember> ) {
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

#if defined( DAW_JSON_BUGFIX_MSVC_EVAL_ORDER_002 )
			// MSVC does not always evaluate a braced list in order, so tuples keep
			// their members in slots and positions are left to the scalar parser
			template<typename Members, std::size_t... Is>
			DAW_CONSTEVAL bool independent_tuple( std::index_sequence<Is...> ) {
				return (
				  ( not is_an_ordered_member_v<std::tuple_element_t<Is, Members>> and
				    not has_class_dependent_v<std::tuple_element_t<Is, Members>> ) and
				  ... );
			}
#else
			template<typename Members, std::size_t I>
			DAW_CONSTEVAL std::size_t tuple_position( );

			/// The array position just after the tuple member before I
			template<typename Members, std::size_t I>
			DAW_CONSTEVAL std::size_t tuple_start( ) {
				if constexpr( I == 0 ) {
					return 0;
				} else {
					return tuple_position<Members, I - 1>( ) + 1;
				}
			}

			/// The array position of the tuple member at I.  A json_tuple_member
			/// gives its own, the others follow the member before them
			template<typename Members, std::size_t I>
			DAW_CONSTEVAL std::size_t tuple_position( ) {
				using member = std::tuple_element_t<I, Members>;
				if constexpr( is_an_ordered_member_v<member> ) {
					return member::member_index;
				} else {
					return tuple_start<Members, I>( );
				}
			}

			/// The members are read in order, so their positions must increase
			template<typename Members, std::size_t... Is>
			DAW_CONSTEVAL bool independent_tuple( std::index_sequence<Is...> ) {
				return (
				  ( not has_class_dependent_v<
				      ordered_member_subtype_t<std::tuple_element_t<Is, Members>>> and
				    tuple_position<Members, Is>( ) >= tuple_start<Members, Is>( ) ) and
				  ... );
			}
#endif

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

			template<typename JsonMember, typename ParseState>
			inline json_result_t<JsonMember> consume_value( reader<ParseState> &r );

			template<typename T>
			struct raw_value_slot {
				alignas( T ) std::byte storage[sizeof( T )];

				[[nodiscard]] T *get( ) noexcept {
					return std::launder( reinterpret_cast<T *>( storage ) );
				}
			};

			template<typename... Ts>
			class value_slots {
				static constexpr std::size_t count = sizeof...( Ts );
				std::tuple<raw_value_slot<Ts>...> m_slots;
				std::array<std::uint64_t, ( count + 63U ) / 64U> m_built{ };

				template<std::size_t I>
				using value_t = std::tuple_element_t<I, std::tuple<Ts...>>;

				template<std::size_t... Is>
				void destroy_all( std::index_sequence<Is...> ) noexcept {
					( ( built<Is>( )
					      ? ( std::destroy_at( std::get<Is>( m_slots ).get( ) ), void( ) )
					      : void( ) ),
					  ... );
				}

			public:
				value_slots( ) = default;
				value_slots( value_slots const & ) = delete;
				value_slots &operator=( value_slots const & ) = delete;

				~value_slots( ) {
					destroy_all( std::index_sequence_for<Ts...>{ } );
				}

				template<std::size_t I>
				[[nodiscard]] bool built( ) const noexcept {
					return built( I );
				}

				[[nodiscard]] bool built( std::size_t index ) const noexcept {
					return ( m_built[index / 64U] &
					         ( std::uint64_t{ 1 } << ( index % 64U ) ) ) != 0;
				}

				template<std::size_t I, typename U>
				void emplace( U &&value ) {
					auto const bit = std::uint64_t{ 1 } << ( I % 64U );
					auto &word = m_built[I / 64U];
					if( ( word & bit ) != 0 ) {
						std::destroy_at( std::get<I>( m_slots ).get( ) );
						word &= ~bit;
					}
					std::construct_at( std::get<I>( m_slots ).get( ), DAW_FWD( value ) );
					word |= bit;
				}

				template<std::size_t I>
				[[nodiscard]] value_t<I> &&take( ) noexcept {
					return std::move( *std::get<I>( m_slots ).get( ) );
				}
			};

#if defined( DAW_JSON_BUGFIX_MSVC_EVAL_ORDER_002 )
			template<typename Members, std::size_t... Is>
			auto make_tuple_slots( std::index_sequence<Is...> )
			  -> value_slots<json_result_t<std::tuple_element_t<Is, Members>>...>;
#endif

			template<typename JsonMember>
			DAW_CONSTEVAL bool forward_supported( );

			template<typename T>
			inline constexpr bool is_storable_result_v =
			  std::is_object_v<T> and std::is_move_constructible_v<T> and
			  std::is_destructible_v<T>;

			template<typename Members, std::size_t... Is>
			DAW_CONSTEVAL bool forward_class_members( std::index_sequence<Is...> ) {
				return (
				  ( is_storable_result_v<json_result_t<
				      without_name<typename Members::template member<Is>>>> and
				    forward_supported<
				      without_name<typename Members::template member<Is>>>( ) ) and
				  ... );
			}

			template<typename Members, std::size_t... Is>
			DAW_CONSTEVAL bool forward_tuple_members( std::index_sequence<Is...> ) {
				return (
				  forward_supported<
				    ordered_member_subtype_t<std::tuple_element_t<Is, Members>>>( ) and
				  ... );
			}

			template<typename... Alternatives>
			DAW_CONSTEVAL bool
			forward_variant_alternatives( fwd_pack<Alternatives...> const * ) {
				return ( forward_supported<Alternatives>( ) and ... );
			}

			template<typename JsonMember>
			using variant_alternatives_t =
			  typename JsonMember::json_elements::element_map_t;

			template<typename JsonMember>
			DAW_CONSTEVAL bool forward_supported( ) {
				constexpr auto type = JsonMember::expected_type;
				if constexpr( not is_storable_result_v<json_result_t<JsonMember>> ) {
					return false;
				} else if constexpr( type == JsonParseTypes::Null ) {
					return forward_supported<typename JsonMember::member_type>( );
				} else if constexpr( is_walkable_tuple<JsonMember>( ) ) {
					using members = typename tuple_members<JsonMember>::types;
					return forward_tuple_members<members>(
					  std::make_index_sequence<std::tuple_size_v<members>>{ } );
				} else if constexpr( type == JsonParseTypes::Variant ) {
					return forward_variant_alternatives(
					  static_cast<variant_alternatives_t<JsonMember> const *>(
					    nullptr ) );
				} else if constexpr( type == JsonParseTypes::KeyValueArray ) {
					return forward_supported<typename JsonMember::json_class_t>( );
				} else if constexpr( is_json_class_mapping_v<JsonMember> ) {
					if constexpr( is_type_alias_class_v<JsonMember> ) {
						return forward_supported<aliased_member_t<JsonMember>>( );
					} else if constexpr( not is_walkable_class<JsonMember>( ) ) {
						return false;
					} else {
						using members = class_members<JsonMember>;
						return forward_class_members<members>(
						  std::make_index_sequence<members::size>{ } );
					}
				} else if constexpr( type == JsonParseTypes::Array ) {
					return forward_supported<typename JsonMember::json_element_t>( );
				} else if constexpr( type == JsonParseTypes::KeyValue ) {
					return forward_supported<typename JsonMember::json_key_t>( ) and
					       forward_supported<typename JsonMember::json_element_t>( );
				} else {
					return type == JsonParseTypes::Real or
					       type == JsonParseTypes::Signed or
					       type == JsonParseTypes::Unsigned or
					       type == JsonParseTypes::Bool or
					       type == JsonParseTypes::StringRaw or
					       type == JsonParseTypes::StringEscaped or
					       type == JsonParseTypes::StringInsitu or
					       type == JsonParseTypes::Date or
					       type == JsonParseTypes::Custom or
					       type == JsonParseTypes::Unknown;
				}
			}

			template<typename State>
			inline daw::string_view consume_member_name( reader<State> &r ) {
				auto const first = r.peek( );
				if( DAW_UNLIKELY( first == nullptr or *first != '"' or
				                  r.position + 1 >= r.size or
				                  *r[r.position + 1] != ':' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, first, ErrorReason::MissingMemberNameOrEndOfClass );
				}
				auto const colon = r[r.position + 1];
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

			/// Skip a value.  By default, like simdjson's On-Demand, a class or
			/// array only has its depth counted: { and [ add one, } and ] take one
			/// away.  With options::StrictSkipChecks::yes the bracket kinds, member
			/// names, colons and commas are checked too, nesting at most
			/// DAW_JSON_STRICT_SKIP_MAX_DEPTH deep.  A string, number or literal is
			/// one entry and its spelling is not checked.  Both are loops, so deep
			/// input cannot exhaust the stack
			template<typename State>
			inline void skip_indexed_value( reader<State> &r ) {
				auto event = r.peek( );
				if( DAW_UNLIKELY( event == nullptr or *event == ',' or *event == ':' or
				                  *event == '}' or *event == ']' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, event, ErrorReason::InvalidStartOfValue );
				}
				r.pop( );
				if( *event != '{' and *event != '[' ) {
					return;
				}
				if constexpr( not State::strict_skip_checks ) {
					auto depth = std::size_t{ 1 };
					while( depth != 0U ) {
						event = r.peek( );
						if( DAW_UNLIKELY( event == nullptr ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, event, ErrorReason::UnexpectedEndOfData );
						}
						r.pop( );
						if( *event == '{' or *event == '[' ) {
							++depth;
						} else if( *event == '}' or *event == ']' ) {
							--depth;
						}
					}
				} else {
					auto brackets = strict_skip_brackets{ };
					(void)brackets.push( *event == '{' );
					// After an opening bracket or a comma, where the close may come
					auto item_start = true;
					for( ;; ) {
						event = r.peek( );
						auto const in_class = brackets.in_class( );
						if( event != nullptr and *event == ( in_class ? '}' : ']' ) ) {
							r.pop( );
							if( brackets.pop( ) ) {
								return;
							}
							item_start = false;
							continue;
						}
						if( not item_start ) {
							if( DAW_UNLIKELY( event == nullptr or *event != ',' ) ) {
								DAW_UNLIKELY_BRANCH
								error_at( r, event, ErrorReason::InvalidEndOfValue );
							}
							r.pop( );
							item_start = true;
							continue;
						}
						if( in_class ) {
							(void)consume_member_name( r );
							event = r.peek( );
						}
						if( DAW_UNLIKELY( event == nullptr or *event == ',' or
						                  *event == ':' or *event == '}' or
						                  *event == ']' ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, event, ErrorReason::InvalidStartOfValue );
						}
						r.pop( );
						item_start = *event == '{' or *event == '[';
						if( item_start and
						    DAW_UNLIKELY( not brackets.push( *event == '{' ) ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, event, ErrorReason::MaxDepthExceeded );
						}
					}
				}
			}

			template<typename Members, std::size_t... Is>
			inline std::size_t find_forward_member( daw::string_view name,
			                                        std::size_t expected,
			                                        std::index_sequence<Is...> ) {
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

			/// Where the members of a class are.  Values are read in constructor
			/// order; a member found before its turn has its value's position
			/// recorded and is skipped, then read when its turn comes
			template<std::size_t N, bool Exact>
			struct class_locations {
				static constexpr bool exact = Exact;
				char const *opening;
				// 0 is never a value's position, as the { comes first
				std::array<index_type, N> value_at{ };
				// Just after the last value scanned past, or the {
				index_type frontier = 0;
				bool reader_at_frontier = true;
				bool first = true;
				bool closed = false;
			};

			/// Move to the next member name from the frontier, or close the class.
			/// Returns false when the class is closed
			template<typename State, typename Locations>
			DAW_ATTRIB_INLINE bool next_member( reader<State> &r, Locations &locs ) {
				auto event = r.peek( );
				if( locs.first ) {
					locs.first = false;
				} else {
					if( event != nullptr and *event == '}' ) {
						r.pop( );
						locs.closed = true;
						return false;
					}
					if( DAW_UNLIKELY( event == nullptr or *event != ',' ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( r, event, ErrorReason::InvalidEndOfValue );
					}
					r.pop( );
					event = r.peek( );
				}
				if( event != nullptr and *event == '}' ) {
					r.pop( );
					locs.closed = true;
					return false;
				}
				return true;
			}

			/// Read member names from the frontier until member I is found,
			/// recording the others' values and skipping them.  Returns false when
			/// the class closes first
			template<typename Members, std::size_t I, bool Exact, typename State,
			         typename Locations, std::size_t... Is>
			DAW_ATTRIB_INLINE bool
			scan_to_member( reader<State> &r, Locations &locs,
			                std::index_sequence<Is...> indices ) {
				while( not locs.closed and next_member( r, locs ) ) {
					auto const name_event = r.peek( );
					auto const name = consume_member_name( r );
					auto const found = find_forward_member<Members>( name, I, indices );
					bool const known =
					  found < Members::size and locs.value_at[found] == 0;
					if constexpr( Exact ) {
						if( DAW_UNLIKELY( not known ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, name_event, ErrorReason::UnknownMember );
						}
					}
					if( known ) {
						locs.value_at[found] = r.position;
						if( found == I ) {
							return true;
						}
					}
					skip_indexed_value( r );
				}
				return false;
			}

			/// When the next member at the frontier is spelled exactly as Member,
			/// with only whitespace between its closing quote and the colon, move
			/// to its value.  Anything else is left to scan_to_member
			template<typename Member, typename State, typename Locations>
			DAW_ATTRIB_INLINE bool at_expected_member( reader<State> &r,
			                                           Locations &locs ) {
				constexpr auto size = std::size( Member::name );
				if( locs.closed ) {
					return false;
				}
				auto name = r.position;
				if( not locs.first ) {
					auto const comma = r.peek( );
					if( comma == nullptr or *comma != ',' ) {
						return false;
					}
					++name;
				}
				if( name + 1U >= r.size ) {
					return false;
				}
				auto const first = r[name];
				auto const colon = r[name + 1U];
				auto last = first + size + 2U;
				if( colon < last or *first != '"' or first[size + 1U] != '"' or
				    std::memcmp( first + 1, std::data( Member::name ), size ) != 0 ) {
					return false;
				}
				for( ; last < colon; ++last ) {
					if( static_cast<unsigned char>( *last ) - 1U >= 0x20U ) {
						return false;
					}
				}
				locs.first = false;
				r.position = static_cast<index_type>( name + 2U );
				return true;
			}

			// A missing member is reported at the opening brace of its class
			template<typename Members, std::size_t I, typename State,
			         typename Locations, std::size_t... Is>
			DAW_ATTRIB_INLINE
			  json_result_t<without_name<typename Members::template member<I>>>
			  consume_class_member( reader<State> &r, Locations &locs,
			                        std::index_sequence<Is...> indices ) {
				using member = typename Members::template member<I>;
				using value = without_name<member>;
				if( auto const at = locs.value_at[I]; at != 0 ) {
					if( locs.reader_at_frontier ) {
						locs.frontier = r.position;
						locs.reader_at_frontier = false;
					}
					r.position = at;
					return consume_value<value>( r );
				}
				if( not locs.reader_at_frontier ) {
					r.position = locs.frontier;
					locs.reader_at_frontier = true;
				}
				if( DAW_LIKELY( at_expected_member<member>( r, locs ) ) ) {
					locs.value_at[I] = r.position;
					return consume_value<value>( r );
				}
				if( DAW_LIKELY( scan_to_member<Members, I, Locations::exact>(
				      r, locs, indices ) ) ) {
					return consume_value<value>( r );
				}
				if constexpr( is_json_nullable_v<value> ) {
					auto state = State( );
					return parse_value_null<value, true>( state );
				} else {
					missing_member_at( r,
					                   locs.opening,
					                   daw::string_view( std::data( member::name ),
					                                     std::size( member::name ) ) );
				}
			}

			/// Check the members after the last one read and move past the }
			template<typename Members, typename State, typename Locations,
			         std::size_t... Is>
			inline void finish_class( reader<State> &r, Locations &locs,
			                          std::index_sequence<Is...> indices ) {
				if( not locs.reader_at_frontier ) {
					r.position = locs.frontier;
				}
				(void)scan_to_member<Members, Members::size, Locations::exact>(
				  r, locs, indices );
			}

			template<typename Member, typename State, std::size_t... Is>
			inline json_result_t<Member>
			consume_class( reader<State> &r, std::index_sequence<Is...> indices ) {
				using members = class_members<Member>;
				using T = json_result_t<Member>;
				using Constructor = json_constructor_t<Member>;
				static_assert(
				  State::allow_escaped_names or
				    not( name_needs_escaping( daw::string_view(
				           std::data( members::template member<Is>::name ),
				           std::size( members::template member<Is>::name ) ) ) or
				         ... ),
				  "A member name has a \", \\ or control char, which JSON input must "
				  "escape.  Names are only decoded before they are compared when "
				  "parsing with options::AllowEscapedNames::yes" );
				auto const opening = r.peek( );
				if( DAW_UNLIKELY( opening == nullptr or *opening != '{' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, opening, ErrorReason::InvalidClassStart );
				}
				r.pop( );
				auto locs =
				  class_locations<members::size,
				                  all_json_members_must_exist_v<T, State>>{ opening };
				if constexpr( should_construct_explicitly_v<Constructor, T, State> ) {
					auto result =
					  T{ consume_class_member<members, Is>( r, locs, indices )... };
					finish_class<members>( r, locs, indices );
					return result;
				} else {
					auto state = r.state_at( opening, r.end( ) );
					auto result = construct_value_tp<T, Constructor>(
					  state,
					  fwd_pack{
					    consume_class_member<members, Is>( r, locs, indices )... } );
					finish_class<members>( r, locs, indices );
					return result;
				}
			}

			template<typename Element, typename State>
			struct forward_array_iterator {
				using iterator_category = std::input_iterator_tag;
				using iterator_concept = std::input_iterator_tag;
				using value_type = json_result_t<Element>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using difference_type = std::ptrdiff_t;
				reader<State> *r = nullptr;

				forward_array_iterator( ) = default;
				explicit forward_array_iterator( reader<State> &input )
				  : r( &input ) {
					if( auto event = r->peek( ); event != nullptr and *event == ']' ) {
						r->pop( );
						r = nullptr;
					}
				}

				value_type operator*( ) const {
					return consume_value<Element>( *r );
				}

				forward_array_iterator &operator++( ) {
					auto event = r->peek( );
					if( event != nullptr and *event == ']' ) {
						r->pop( );
						r = nullptr;
						return *this;
					}
					if( DAW_UNLIKELY( event == nullptr or *event != ',' ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( *r, event, ErrorReason::InvalidEndOfValue );
					}
					r->pop( );
					event = r->peek( );
					if( event != nullptr and *event == ']' ) {
						r->pop( );
						r = nullptr;
					}
					return *this;
				}

				void operator++( int ) {
					(void)operator++( );
				}

				[[nodiscard]] std::size_t size_hint( ) const noexcept {
					return State::array_reserve_amount;
				}

				friend bool operator==( forward_array_iterator const &lhs,
				                        forward_array_iterator const &rhs ) {
					return lhs.r == rhs.r;
				}
			};

			/// The members of a json_key_value object as key/value pairs, read from
			/// the index in document order.  The reader starts after the opening
			/// brace.  Each key goes through consume_value as its own token, so
			/// escaped and literal as string keys are parsed as they are elsewhere.
			template<typename Member, typename State>
			struct forward_kv_iterator {
				using key_t = typename Member::json_key_t;
				using value_t = typename Member::json_element_t;
				using iterator_category = std::input_iterator_tag;
				using iterator_concept = std::input_iterator_tag;
				using value_type =
				  kv_class_iter_impl::container_value_type_or<Member,
				                                              json_base_type_t<Member>>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;
				using difference_type = std::ptrdiff_t;
				reader<State> *r = nullptr;

				forward_kv_iterator( ) = default;
				explicit forward_kv_iterator( reader<State> &input )
				  : r( &input ) {
					if( auto event = r->peek( ); event != nullptr and *event == '}' ) {
						r->pop( );
						r = nullptr;
					}
				}

				value_type operator*( ) const {
					auto const name = r->peek( );
					if( DAW_UNLIKELY( name == nullptr or *name != '"' ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( *r, name, ErrorReason::MissingMemberNameOrEndOfClass );
					}
					auto key = consume_value<key_t>( *r );
					auto const colon = r->peek( );
					if( DAW_UNLIKELY( colon == nullptr or *colon != ':' ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( *r, name, ErrorReason::InvalidMemberName );
					}
					r->pop( );
					return json_class_constructor<value_type,
					                              default_constructor<value_type>>(
					  std::move( key ), consume_value<value_t>( *r ) );
				}

				forward_kv_iterator &operator++( ) {
					auto event = r->peek( );
					if( event != nullptr and *event == '}' ) {
						r->pop( );
						r = nullptr;
						return *this;
					}
					if( DAW_UNLIKELY( event == nullptr or *event != ',' ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( *r, event, ErrorReason::InvalidEndOfValue );
					}
					r->pop( );
					event = r->peek( );
					if( event != nullptr and *event == '}' ) {
						r->pop( );
						r = nullptr;
					}
					return *this;
				}

				void operator++( int ) {
					(void)operator++( );
				}

				friend bool operator==( forward_kv_iterator const &lhs,
				                        forward_kv_iterator const &rhs ) {
					return lhs.r == rhs.r;
				}
			};

			template<typename Member, typename State>
			inline json_result_t<Member> consume_array( reader<State> &r ) {
				using Constructor = json_constructor_t<Member>;
				auto const opening = r.peek( );
				if( DAW_UNLIKELY( opening == nullptr or *opening != '[' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, opening, ErrorReason::InvalidArrayStart );
				}
				r.pop( );
				auto state = r.state_at( opening, r.end( ) );
				using iterator =
				  forward_array_iterator<typename Member::json_element_t, State>;
				return construct_value<json_result_t<Member>, Constructor>(
				  state, iterator( r ), iterator( ) );
			}

			template<typename Member, typename State>
			inline json_result_t<Member> consume_kv( reader<State> &r ) {
				using Constructor = json_constructor_t<Member>;
				auto const opening = r.peek( );
				if( DAW_UNLIKELY( opening == nullptr or *opening != '{' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, opening, ErrorReason::ExpectedKeyValueToStartWithBrace );
				}
				r.pop( );
				auto state = r.state_at( opening, r.end( ) );
				using iterator = forward_kv_iterator<Member, State>;
				return construct_value<json_result_t<Member>, Constructor>(
				  state, iterator( r ), iterator( ) );
			}

			/// The elements of a json_key_value_array as key/value pairs.  Each
			/// element is the key and value class json_parse_kv_array_iterator
			/// parses, so its members may come in either order
			template<typename Member, typename State>
			struct forward_kv_array_iterator
			  : forward_array_iterator<typename Member::json_class_t, State> {
				using base =
				  forward_array_iterator<typename Member::json_class_t, State>;
				using value_type =
				  std::pair<json_result_t<typename Member::json_key_t> const,
				            json_result_t<typename Member::json_value_t>>;
				using reference = value_type;
				using pointer = arrow_proxy<value_type>;

				using base::base;

				value_type operator*( ) const {
					auto kv = base::operator*( );
					return value_type( std::get<0>( std::move( kv.members ) ),
					                   std::get<1>( std::move( kv.members ) ) );
				}

				forward_kv_array_iterator &operator++( ) {
					(void)base::operator++( );
					return *this;
				}

				void operator++( int ) {
					(void)operator++( );
				}
			};

			template<typename Member, typename State>
			inline json_result_t<Member> consume_kv_array( reader<State> &r ) {
				using Constructor = json_constructor_t<Member>;
				auto const opening = r.peek( );
				if( DAW_UNLIKELY( opening == nullptr or *opening != '[' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at(
					  r, opening, ErrorReason::ExpectedKeyValueArrayToStartWithBracket );
				}
				r.pop( );
				auto state = r.state_at( opening, r.end( ) );
				using iterator = forward_kv_array_iterator<Member, State>;
				return construct_value<json_result_t<Member>, Constructor>(
				  state, iterator( r ), iterator( ) );
			}

			/// As parse_value_variant does, the alternative is chosen by the first
			/// byte of the value
			template<JsonBaseParseTypes BPT, typename Member, typename State>
			inline json_result_t<Member> consume_variant_value( reader<State> &r ) {
				using alternatives = variant_alternatives_t<Member>;
				constexpr auto idx = static_cast<std::size_t>(
				  Member::base_map::base_map[static_cast<std::int_fast8_t>( BPT )] );
				if constexpr( idx < pack_size_v<alternatives> ) {
					return consume_value<pack_element_t<idx, alternatives>>( r );
				} else {
					error_at( r, r.peek( ), ErrorReason::UnexpectedJSONVariantType );
				}
			}

			template<typename Member, typename State>
			inline json_result_t<Member> consume_variant( reader<State> &r ) {
				auto const first = r.peek( );
				if( DAW_UNLIKELY( first == nullptr ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, first, ErrorReason::UnexpectedEndOfData );
				}
				switch( *first ) {
				case '{':
					return consume_variant_value<JsonBaseParseTypes::Class, Member>( r );
				case '[':
					return consume_variant_value<JsonBaseParseTypes::Array, Member>( r );
				case 't':
				case 'f':
					return consume_variant_value<JsonBaseParseTypes::Bool, Member>( r );
				case '"':
					return consume_variant_value<JsonBaseParseTypes::String, Member>( r );
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
					return consume_variant_value<JsonBaseParseTypes::Number, Member>( r );
				}
				error_at( r, first, ErrorReason::InvalidStartOfValue );
			}

			/// A json_raw/json_value is parsed, not skipped, so as
			/// parse_value_unknown does, the scalar skip checks the value and gives
			/// its bytes.  The index skip only moves the reader past it
			template<typename Member, typename State>
			inline json_result_t<Member> consume_raw( reader<State> &r ) {
				using Constructor = json_constructor_t<Member>;
				auto const first = r.peek( );
				if( DAW_UNLIKELY( first == nullptr ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, first, ErrorReason::UnexpectedEndOfData );
				}
				auto state = r.state_at( first, r.end( ) );
				auto value = skip_value<true>( state );
				skip_indexed_value( r );
				return construct_value<json_result_t<Member>, Constructor>(
				  state, std::data( value ), std::size( value ) );
			}

#if defined( DAW_JSON_BUGFIX_MSVC_EVAL_ORDER_002 )
			template<typename Members, typename Slots, typename State,
			         std::size_t... Is>
			inline void consume_tuple_member( std::size_t member, reader<State> &r,
			                                  Slots &slots,
			                                  std::index_sequence<Is...> ) {
				(void)( ( member == Is
				            ? ( slots.template emplace<Is>(
				                  consume_value<std::tuple_element_t<Is, Members>>(
				                    r ) ),
				                true )
				            : false ) or
				        ... );
			}

			// A missing element is reported at the opening bracket of its tuple
			template<std::size_t I, typename Members, typename Slots, typename State>
			inline void finish_tuple_slot( Slots &slots, reader<State> const &r,
			                               char const *opening ) {
				using value = std::tuple_element_t<I, Members>;
				if( slots.template built<I>( ) ) {
					return;
				}
				if constexpr( is_json_nullable_v<value> ) {
					auto state = State( );
					slots.template emplace<I>( parse_value_null<value, true>( state ) );
				} else {
					error_at( r, opening, ErrorReason::UnexpectedEndOfData );
				}
			}

			template<typename Member, typename State, std::size_t... Is>
			inline json_result_t<Member>
			consume_tuple( reader<State> &r, std::index_sequence<Is...> indices ) {
				using members = typename tuple_members<Member>::types;
				using T = json_base_type_t<Member>;
				using Constructor = json_constructor_t<Member>;
				using slots_t = decltype( make_tuple_slots<members>( indices ) );
				auto const opening = r.peek( );
				if( DAW_UNLIKELY( opening == nullptr or *opening != '[' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, opening, ErrorReason::InvalidArrayStart );
				}
				r.pop( );
				auto slots = slots_t{ };
				auto member = std::size_t{ 0 };
				if( auto first_event = r.peek( );
				    first_event != nullptr and *first_event == ']' ) {
					r.pop( );
				} else {
					for( ;; ++member ) {
						if( member < sizeof...( Is ) ) {
							consume_tuple_member<members>( member, r, slots, indices );
						} else {
							if constexpr( all_json_members_must_exist_v<T, State> ) {
								error_at( r, r.peek( ), ErrorReason::UnknownMember );
							}
							skip_indexed_value( r );
						}
						auto event = r.peek( );
						if( event != nullptr and *event == ']' ) {
							r.pop( );
							break;
						}
						if( DAW_UNLIKELY( event == nullptr or *event != ',' ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, event, ErrorReason::InvalidEndOfValue );
						}
						r.pop( );
						event = r.peek( );
						if( event != nullptr and *event == ']' ) {
							r.pop( );
							break;
						}
					}
				}
				( finish_tuple_slot<Is, members>( slots, r, opening ), ... );
				auto state =
				  r.state_at( opening, r.peek( ) == nullptr ? r.end( ) : r.peek( ) );
				if constexpr( should_construct_explicitly_v<Constructor, T, State> ) {
					return T{ slots.template take<Is>( )... };
				} else {
					return construct_value_tp<T, Constructor>(
					  state, fwd_pack{ slots.template take<Is>( )... } );
				}
			}
#else
			/// Where the elements of a tuple are read from
			struct tuple_cursor {
				char const *opening;
				bool closed = false;
			};

			/// Move past the separator after a tuple element, closing the tuple at
			/// its ']'.  A trailing comma is accepted
			template<typename State>
			inline void end_tuple_element( reader<State> &r, tuple_cursor &c ) {
				auto event = r.peek( );
				if( event != nullptr and *event == ']' ) {
					r.pop( );
					c.closed = true;
					return;
				}
				if( DAW_UNLIKELY( event == nullptr or *event != ',' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, event, ErrorReason::InvalidEndOfValue );
				}
				r.pop( );
				event = r.peek( );
				if( event != nullptr and *event == ']' ) {
					r.pop( );
					c.closed = true;
				}
			}

			/// The tuple member at I after skipping the elements before its
			/// position.  A missing element is null when nullable, otherwise it is
			/// reported at the opening bracket of its tuple
			template<typename Members, std::size_t I, typename State>
			inline json_result_t<
			  ordered_member_subtype_t<std::tuple_element_t<I, Members>>>
			consume_tuple_element( reader<State> &r, tuple_cursor &c ) {
				using value =
				  ordered_member_subtype_t<std::tuple_element_t<I, Members>>;
				for( auto n = tuple_start<Members, I>( );
				     n < tuple_position<Members, I>( ) and not c.closed;
				     ++n ) {
					skip_indexed_value( r );
					end_tuple_element( r, c );
				}
				if( c.closed ) {
					if constexpr( is_json_nullable_v<value> ) {
						auto state = State( );
						return parse_value_null<value, true>( state );
					} else {
						error_at( r, c.opening, ErrorReason::UnexpectedEndOfData );
					}
				}
				auto result = consume_value<value>( r );
				end_tuple_element( r, c );
				return result;
			}

			/// Skip the elements past the last member, which exact mappings reject
			template<typename T, typename State>
			inline void finish_tuple( reader<State> &r, tuple_cursor &c ) {
				while( not c.closed ) {
					if constexpr( all_json_members_must_exist_v<T, State> ) {
						error_at( r, r.peek( ), ErrorReason::UnknownMember );
					}
					skip_indexed_value( r );
					end_tuple_element( r, c );
				}
			}

			/// The members are read in mapping order, which is array order, straight
			/// into the result's construction.  The elements of a braced list are
			/// evaluated in order
			template<typename Member, typename State, std::size_t... Is>
			inline json_result_t<Member> consume_tuple( reader<State> &r,
			                                            std::index_sequence<Is...> ) {
				using members = typename tuple_members<Member>::types;
				using T = json_base_type_t<Member>;
				using Constructor = json_constructor_t<Member>;
				auto const opening = r.peek( );
				if( DAW_UNLIKELY( opening == nullptr or *opening != '[' ) ) {
					DAW_UNLIKELY_BRANCH
					error_at( r, opening, ErrorReason::InvalidArrayStart );
				}
				r.pop( );
				auto c = tuple_cursor{ opening };
				if( auto const first_event = r.peek( );
				    first_event != nullptr and *first_event == ']' ) {
					r.pop( );
					c.closed = true;
				}
				if constexpr( should_construct_explicitly_v<Constructor, T, State> ) {
					auto result = T{ consume_tuple_element<members, Is>( r, c )... };
					finish_tuple<T>( r, c );
					return result;
				} else {
					auto state = r.state_at( opening, r.end( ) );
					auto result = construct_value_tp<T, Constructor>(
					  state, fwd_pack{ consume_tuple_element<members, Is>( r, c )... } );
					finish_tuple<T>( r, c );
					return result;
				}
			}
#endif

			template<typename JsonMember, typename ParseState>
			inline json_result_t<JsonMember> consume_value( reader<ParseState> &r ) {
				constexpr auto type = JsonMember::expected_type;
				if constexpr( type == JsonParseTypes::Null ) {
					auto const first = r.peek( );
					if( DAW_UNLIKELY( first == nullptr ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( r, first, ErrorReason::UnexpectedEndOfData );
					}
					if( *first == 'n' ) {
						auto const token = r.position++;
						auto bounds = r.bounds( token );
						auto state = ParseState( bounds.first, bounds.last );
						return parse_value<JsonMember, false, type>( state );
					}
					using base_member = typename JsonMember::member_type;
					using constructor = json_constructor_t<JsonMember>;
					auto value = consume_value<base_member>( r );
					auto state =
					  r.state_at( first, r.peek( ) == nullptr ? r.end( ) : r.peek( ) );
					return construct_value<base_member, constructor>(
					  state, std::move( value ) );
				} else if constexpr( is_walkable_tuple<JsonMember>( ) ) {
					using members = typename tuple_members<JsonMember>::types;
					return consume_tuple<JsonMember>(
					  r, std::make_index_sequence<std::tuple_size_v<members>>{ } );
				} else if constexpr( type == JsonParseTypes::Class and
				                     is_type_alias_class_v<JsonMember> ) {
					// As json_type_alias::parse_to_class does, construct the result
					// from the aliased member's
					auto const first = r.peek( );
					auto value = consume_value<aliased_member_t<JsonMember>>( r );
					auto state =
					  r.state_at( first, r.peek( ) == nullptr ? r.end( ) : r.peek( ) );
					return construct_value<JsonMember,
					                       daw::construct_a_t<json_result_t<JsonMember>>>(
					  state, std::move( value ) );
				} else if constexpr( is_json_class_mapping_v<JsonMember> ) {
					using members = class_members<JsonMember>;
					return consume_class<JsonMember>(
					  r, std::make_index_sequence<members::size>{ } );
				} else if constexpr( type == JsonParseTypes::Array ) {
					return consume_array<JsonMember>( r );
				} else if constexpr( type == JsonParseTypes::KeyValue ) {
					return consume_kv<JsonMember>( r );
				} else if constexpr( type == JsonParseTypes::KeyValueArray ) {
					return consume_kv_array<JsonMember>( r );
				} else if constexpr( type == JsonParseTypes::Variant ) {
					return consume_variant<JsonMember>( r );
				} else if constexpr( type == JsonParseTypes::Unknown ) {
					return consume_raw<JsonMember>( r );
				} else {
					if( DAW_UNLIKELY( r.peek( ) == nullptr ) ) {
						DAW_UNLIKELY_BRANCH
						error_at( r, nullptr, ErrorReason::UnexpectedEndOfData );
					}
					auto const token = r.position++;
					auto bounds = r.bounds( token );
					if constexpr( type == JsonParseTypes::Real or
					              type == JsonParseTypes::Signed or
					              type == JsonParseTypes::Unsigned ) {
						// A number in a container is followed by a structural byte, so
						// the zero-terminated policy can scan until that non-digit without
						// checking the token bound on every character. A root number may
						// end at the document sentinel and still needs bounded parsing.
						if( bounds.last != r.end( ) or r.source_is_zero_terminated ) {
							using number_state = json_details::zstring_policy_t<ParseState>;
							auto state = number_state( bounds.first, bounds.last );
							return parse_value<JsonMember, false, type>( state );
						}
					}
					auto state = ParseState( bounds.first, bounds.last );
					if constexpr( type == JsonParseTypes::StringRaw or
					              type == JsonParseTypes::StringEscaped or
					              type == JsonParseTypes::StringInsitu ) {
						daw_json_assert_weak( *state.first == '"',
						                      ErrorReason::InvalidString );
						++state.first;
						do {
							--state.last;
						} while( *state.last != '"' );
						if constexpr( type != JsonParseTypes::StringRaw ) {
							auto const slash = static_cast<char const *>( std::memchr(
							  +state.first,
							  '\\',
							  static_cast<std::size_t>( state.last - state.first ) ) );
							state.counter =
							  slash == nullptr
							    ? static_cast<std::size_t>( -1 )
							    : static_cast<std::size_t>( slash - state.first );
						}
						return parse_value<JsonMember, true, type>( state );
					} else {
						return parse_value<JsonMember, false, type>( state );
					}
				}
			}

			/// The document, writable when the policy allows string mutation
			template<typename ParseState>
			using document_span_t =
			  daw::span<std::remove_pointer_t<typename ParseState::iterator>>;

			template<typename JsonMember, typename ParseState>
			inline json_result_t<JsonMember>
			scalar_fallback( document_span_t<ParseState> doc ) {
				auto state = ParseState( doc.data( ), daw::data_end( doc ) );
				DAW_ASSUME( state.first != nullptr );
				DAW_ASSUME( state.last != nullptr );
				DAW_ASSUME( state.class_first != nullptr );
				DAW_ASSUME( state.class_last != nullptr );
				if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
					auto result =
					  parse_value<JsonMember, false, JsonMember::expected_type>( state );
					state.trim_left( );
					daw_json_ensure(
					  state.empty( ), ErrorReason::InvalidEndOfValue, state );
					return result;
				} else {
					return parse_value<JsonMember, false, JsonMember::expected_type>(
					  state );
				}
			}

			template<typename JsonMember, typename ParseState>
			inline json_result_t<JsonMember> from_json_simd_impl(
			  document_span_t<ParseState> doc,
			  daw::not_null<std::pmr::memory_resource *> memory_resource ) {
				// A root json_raw/json_value would only be given the whole document
				if constexpr( not forward_supported<JsonMember>( ) or
				              JsonMember::expected_type == JsonParseTypes::Unknown ) {
					return scalar_fallback<JsonMember, ParseState>( doc );
				} else {
					using indexed_state = json_details::apply_policy_option_t<
					  ParseState,
					  options::ZeroTerminatedString::no>;
					auto index = build_index<indexed_state>(
					  std::string_view( doc.data( ), doc.size( ) ), memory_resource );
					auto r = reader<indexed_state>(
					  index, doc.data( ), ParseState::is_zero_terminated_string );
					auto result = consume_value<JsonMember>( r );
					if constexpr( ParseState::must_verify_end_of_data_is_valid ) {
						if( DAW_UNLIKELY( r.position != index.size( ) ) ) {
							DAW_UNLIKELY_BRANCH
							error_at( r, r.peek( ), ErrorReason::InvalidEndOfValue );
						}
					}
					return result;
				}
			}
		} // namespace json_details::simd_parse
	} // namespace DAW_JSON_VER
} // namespace daw::json
#endif
