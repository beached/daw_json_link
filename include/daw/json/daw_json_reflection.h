// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/daw_pipelines.h"
#include "daw_json_link.h"

#include <cstddef>
#include <experimental/meta>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace daw::json::inline DAW_JSON_VER {
  inline namespace experimental {
    struct refl_map_as {
      std::meta::info type;
    };
    
    struct refl_rename {
			char const* name;
		};

    struct refl_ignore_with_default { };

    template<typename T>
    struct refl_ignore_with_value {
      T value;
    };

		namespace refl_details {
				template<auto... vals>
				struct replicator_type {
					template<typename F>
					constexpr void operator>>( F body ) const {
						(body.template operator()<vals>( ), ...);
					}
				};

				template<auto... vals>
				replicator_type<vals...> replicator = { };
			  
				/// Get the public non-static data members
				consteval std::vector<std::meta::info>
				pub_nsdm_of( std::meta::info type_class ) {
					auto members = std::meta::nonstatic_data_members_of( type_class );
					std::erase_if( members, [] ( std::meta::info const& i )	{
						return not std::meta::is_public( i );
					} );
					return members;
				}

				template<typename R>
				consteval auto expand( R range ) {
					auto args = std::vector<std::meta::info>( );
					for( auto r: range ) {
						args.push_back( std::meta::reflect_value( r ) );
					}
					return std::meta::substitute(^^replicator, args );
				}

				template<typename T>
				consteval std::optional<T> get_annotaion( std::meta::info r ) {
					for( std::meta::info a: std::meta::annotations_of( r ) ) {
						if( std::meta::type_of( a ) == ^^T ) {
							return std::meta::extract<T>( a );
						}
					}
					return std::nullopt;
				}

				template<typename T>
				consteval bool has_annotation( std::meta::info r, T const& value ) {
					auto expected = std::meta::reflect_value( value );
					for( std::meta::info a: std::meta::annotations_of( r ) ) {
						if( value_of( a ) == expected ) {
							return true;
						}
					}
					return false;
				}

				template<typename T, std::size_t... Is>
				consteval auto to_tuple( T const& value, std::index_sequence<Is...> )
					-> decltype( std::tuple( value.[:pub_nsdm_of(^^T )[Is]:]... ) ) {
					return std::tuple( value.[:pub_nsdm_of(^^T )[Is]:]... );
				}

				template<typename T>
				consteval auto to_tuple( T const& value )
					-> decltype( refl_details::to_tuple(
						value, std::make_index_sequence<pub_nsdm_of(^^T ).size( )>{ } ) ) {
					return refl_details::to_tuple(
						value, std::make_index_sequence<pub_nsdm_of(^^T ).size( )>{ } );
				}

				template<JSONNAMETYPE Name, typename T>
				using deduce_t = typename json_details::ensure_mapped_t<
					json_details::json_deduced_type<T>>::template with_name<Name>;

				template<typename T, std::size_t Idx>
				using submember_type_t =
				std::tuple_element_t<Idx, DAW_TYPEOF( refl_details::to_tuple(
															std::declval<T>( ) ) )>;

				template<typename T, std::size_t Idx>
				consteval auto get_member_link_func( ) {
					static constexpr auto m = pub_nsdm_of(^^T )[Idx];
					static constexpr auto annot_rename = get_annotaion<daw::json::refl_rename>( m );
					static constexpr auto name = [] {
						if constexpr( annot_rename ) {
							return std::string_view( annot_rename->name );
						} else {
							return std::meta::identifier_of( m );
						}
					}( );
					static_assert( not name.empty( ) );

					static constexpr auto annot_map_as = get_annotaion<refl_map_as>( m );
				  if constexpr( annot_map_as ) {
				      static constexpr auto result = daw::traits::identity<[: annot_map_as->type :]>{};
				    if constexpr( not annot_rename ) {
				       return result; 
				    } else {
				      static constexpr auto n = json_name<name.size( ) + 1>(
				        name.data( ), std::make_index_sequence<name.size( ) + 1>{}
				      );
				      return daw::traits::identity<typename DAW_TYPEOF(result)::type::template with_name<n>>{};
				    }
				  } else {
				    return daw::traits::identity<deduce_t<
                                            json_name<name.size( ) + 1>(
                                                    name.data( ), std::make_index_sequence<name.size( ) + 1>{ } ),
                                            submember_type_t<T, Idx>>>{ };
				  }
				}

				template<typename T, std::size_t Idx>
				using get_member_link_t =
				typename DAW_TYPEOF( get_member_link_func<T, Idx>( ) )::type;

				template<typename, typename>
				struct make_data_contract;

				template<typename, typename...>
				inline constexpr bool construction_test_v = false;

				template<typename T, typename... Ts>
				inline constexpr bool construction_test_v<T, std::tuple<Ts...>> = requires {
					T{ std::declval<Ts>( )... };
				};

				template<typename T>
				concept Reflectable =
					not std::is_empty_v<T> and std::is_class_v<T> and requires {
						refl_details::to_tuple( std::declval<T>( ) );
					} and construction_test_v<T, DAW_TYPEOF( refl_details::to_tuple(
																		std::declval<T>( ) ) )>;

				template<Reflectable T, std::size_t... Is>
				struct make_data_contract<T, std::index_sequence<Is...>> {
					using type = json_member_list<get_member_link_t<T, Is>...>;

					DAW_ATTRIB_INLINE static constexpr auto to_json_data( T const& value ) {
						return daw::forward_nonrvalue_as_tuple(
							value.[:pub_nsdm_of(^^T )[Is]:]... );
					}
				};
			}

			inline constexpr struct reflect_t {
        static consteval refl_rename rename( char const * name ) {
          return refl_rename{ name };
        }

		    template<typename JsonMember>
		    static constexpr auto map_as = refl_map_as{ ^^JsonMember };

		    static constexpr auto ignore_with_default = daw::json::refl_ignore_with_default{};

		    template<typename T>
		    static consteval auto ignore_with_value( T && v ) {
		      return refl_ignore_with_value{ DAW_FWD( v ) };
		    }
			} reflect{ };

			// Trait that specifies a type is to be reflected on for parse info
			template<refl_details::Reflectable T>
			inline constexpr bool is_reflectible_type_v = refl_details::has_annotation(^^T, reflect );
	}

	template<typename T>
		requires is_reflectible_type_v<T> //
	struct json_data_contract<T>
		: refl_details::make_data_contract<
			T,
			std::make_index_sequence<refl_details::pub_nsdm_of(^^T ).size( )>> { };

	namespace json_details {
		template<typename E>
			requires std::is_enum_v<E>
		struct reflect_enum_as_string {
			constexpr E operator()( std::string_view name ) const {
				template for( constexpr auto e: std::meta::enumerators_of(^^E ) ) {
					if( name == std::meta::identifier_of( e ) ) {
						return [:e:];
					}
				}
				daw_json_error( ErrorReason::InvalidString );
			}

			constexpr std::string_view operator()( E value ) const {
				template for( constexpr auto e: std::meta::enumerators_of(^^E ) ) {
					if( value == [:e:] ) {
						return std::meta::identifier_of( e );
					}
				}
				return std::string_view{ };
			}
		};
	}

	namespace refl {
		using namespace daw::json::json_base;

		template<typename E,
						json_options_t Options = json_custom_opts_def>
			requires std::is_enum_v<E>
		using enum_string = json_custom_no_name<
			E,
			json_details::reflect_enum_as_string<E>,
			json_details::reflect_enum_as_string<E>,
			Options
		>;
	}
}
