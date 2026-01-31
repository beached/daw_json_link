// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/daw_json_link_types.h"
#include "daw/json/daw_json_switches.h"
#include "daw/json/impl/daw_json_parse_common.h"
#include "daw/json/impl/daw_json_traits.h"
#include "daw/json/impl/daw_json_type_options.h"

#if defined( DAW_JSON_HAS_REFLECTION )
#include "daw/json/impl/daw_json_reflection_helpers.h"

#include <daw/daw_bind_args_at.h>
#include <daw/daw_concepts.h>
#include <daw/daw_move.h>
#include <daw/daw_pipelines.h>

#include <cstddef>
#include <meta>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace daw::json::inline DAW_JSON_VER::refl_details {
	struct reflect_all_t {};

	template<EnumType E, json_options_t Options = json_custom_opts_def>
	struct enum_string;

	struct refl_ignored_base {
		consteval refl_ignored_base( ) = default;
	};

	struct refl_annotation_base {
		consteval refl_annotation_base( ) = default;
	};

	struct refl_map_as : refl_annotation_base {
		std::meta::info type;

		explicit consteval refl_map_as( std::meta::info i )
		  : type( i ) {}
	};

	struct refl_enum_string : refl_annotation_base {
		json_options_t Options;

		explicit consteval refl_enum_string( json_options_t opts )
		  : Options( opts ) {}
	};

	struct refl_rename : refl_annotation_base {
		char const *name;

		explicit consteval refl_rename( char const *Name )
		  : name( Name ) {}
	};

	template<typename T, T... Vals>
	inline constexpr std::array<T, sizeof...( Vals )> constant_fixed_array = {
	  Vals... };

	template<std::ranges::input_range R>
	requires( std::is_constructible_v<std::ranges::range_value_t<R>,
	                                  std::ranges::range_reference_t<R>> )
	  consteval auto as_stdarray( R &&elems ) {

		auto args = std::vector{ ^^std::ranges::range_value_t<R> };
		for( const auto &V : elems ) {
			args.push_back( reflect_constant( V ) );
		}
		return substitute( ^^constant_fixed_array, args );
	}

	consteval std::vector<std::meta::info>
	annotations_of_with_base_type( std::meta::info item, std::meta::info type ) {
		auto result = std::vector<std::meta::info>{ };
		for( auto annotation : annotations_of( item ) ) {
			if( is_base_of_type( type, type_of( annotation ) ) ) {
				result.push_back( annotation );
			}
		}
		return result;
	}
#if defined( __clang__ )
	consteval std::vector<std::meta::info>
	annotations_of_with_type( std::meta::info item, std::meta::info type ) {
		auto result = std::vector<std::meta::info>{ };
		for( auto annot : annotations_of( item ) ) {
			if( type_of( annot ) == type ) {
				result.push_back( annot );
			}
		}
		return result;
	}
#endif

	template<typename AnnotationType, typename T>
	consteval bool has_annotation( ) {
		return not annotations_of_with_base_type( ^^T, ^^AnnotationType ).empty( );
	}

	template<typename T>
	consteval std::vector<std::meta::info> get_reflectible_members( ) {
		using namespace daw::pipelines;
		auto result = std::vector<std::meta::info>{ };
		if( not annotations_of_with_base_type( ^^T, ^^reflect_all_t ).empty( ) ) {
			return all_nsdm_of( ^^T );
		}
		return pub_nsdm_of( ^^T );
	}

	template<typename T>
	consteval std::vector<std::meta::info>
	get_non_ignored_reflectible_members( ) {
		using namespace daw::pipelines;
		auto result = std::vector<std::meta::info>{ };
		auto const members = get_reflectible_members<T>( );
		for( auto const member : members ) {
			if( annotations_of_with_base_type( member, ^^refl_ignored_base )
			      .empty( ) ) {
				result.push_back( member );
			}
		}
		return result;
	}

	template<typename T>
	constexpr auto to_tuple( T const &value ) {
		static constexpr auto
		  members = [:as_stdarray( get_non_ignored_reflectible_members<T>( ) ):];
		/* This currently fails to compile
		static constexpr auto [... Is] =
		  std::make_index_sequence<members.size( )>{ };
		return daw::forward_nonrvalue_as_tuple( value.[:members[Is]:]... );
		*/
		return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
			return daw::forward_nonrvalue_as_tuple( value.[:members[Is]:]... );
		}( std::make_index_sequence<members.size( )>{ } );
	}

	template<typename T>
	using to_tuple_t = DAW_TYPEOF( to_tuple( std::declval<T>( ) ) );

	template<std::meta::info members_i>
	consteval auto make_arg_indexes( ) {
		static constexpr auto members = [:members_i:];
		auto r = std::array<std::size_t, members.size( )>{ };
		for( auto const [index, member] : daw::pipelines::Enumerate( members ) ) {
			if( not annotations_of_with_base_type( member, ^^refl_ignored_base )
			          .empty( ) ) {
				r[index] = daw::max_value<std::size_t>;
			} else {
				r[index] = static_cast<std::size_t>(
				  std::count_if( r.data( ),
				                 r.data( ) + static_cast<std::ptrdiff_t>( index ),
				                 []( std::size_t s ) {
					                 return s != daw::max_value<std::size_t>;
				                 } ) );
			}
		}
		return r;
	};

	template<typename T, typename... Fns>
	struct construct_t {
		static constexpr T operator( )( auto &&fns, auto &&...args ) {
			auto const &[... member_fns] = fns;
			return T{ member_fns( DAW_FWD( args )... )... };
		}
	};

	// Function to return the Nth parsed JSON member
	template<typename result_t, std::size_t index>
	struct ArgReturn {
		static constexpr result_t operator( )( auto &&...arguments ) {
			return static_cast<result_t>( DAW_FWD( arguments...[index] ) );
		}
	};

	template<typename result_t, std::meta::info annotation>
	struct DefaultReturn {
		static constexpr result_t operator( )( auto &&... ) {
			static constexpr auto const ignored_default = [:constant_of(
			                                                  annotation ):];
			return static_cast<result_t>( ignored_default );
		}
	};

	template<std::meta::info members_i, std::meta::info arg_indexes_i,
	         typename... Args, typename C>
	consteval auto fn_maker( C ) {
		static constexpr auto members = [:members_i:];
		static constexpr auto index = C::value;
		static constexpr auto member = members[index];
		static constexpr auto annotations = [:as_stdarray(
		                                        annotations_of_with_base_type(
		                                          member, ^^refl_ignored_base ) ):];
		using result_t = [:type_of( member ):];
		if constexpr( annotations.empty( ) ) {
			static constexpr auto arg_indexes = [:arg_indexes_i:];
			static constexpr auto i = arg_indexes[index];
			static_assert( i != daw::max_value<std::size_t> );
			static constexpr auto fn = ArgReturn<result_t, i>{ };
			return fn;
		} else {
			static_assert( annotations.size( ) == 1 );
			static constexpr std::meta::info annotation = annotations.front( );
			static constexpr auto fn = DefaultReturn<result_t, annotation>{ };
			return fn;
		}
	}

	template<std::meta::info members_i, std::meta::info arg_indexes_i,
	         typename... Args, std::size_t... Is>
	consteval auto make_member_fns( std::index_sequence<Is...> ) {
		return std::tuple{ fn_maker<members_i, arg_indexes_i, Args...>(
		  std::integral_constant<std::size_t, Is>{ } )... };
	}

	template<typename T>
	struct reflected_constructor {
		static constexpr auto members_b = [:as_stdarray(
		                                      get_reflectible_members<T>( ) ):];
		static constexpr auto arg_indexes = make_arg_indexes<^^members_b>( );
		using result_t = std::remove_cvref_t<T>;

		template<typename... Args>
		static constexpr result_t operator( )( Args &&...args ) {
			static constexpr auto const member_fns =
			  make_member_fns<^^members_b, ^^arg_indexes, Args...>(
			    std::make_index_sequence<members_b.size( )>{ } );
			return construct_t<result_t>{ }( member_fns, DAW_FWD( args )... );
		}
	};

	template<typename, typename...>
	inline constexpr bool construction_test_v = false;

	template<typename T, typename... Ts>
	inline constexpr bool construction_test_v<T, std::tuple<Ts...>> =
	  requires( Ts... ts ) {
		reflected_constructor<T>{ }( ts... );
	};

	template<typename T>
	concept Reflectable =
	  not std::is_empty_v<T> and std::is_class_v<T> and requires( T v ) {
		to_tuple( v );
	}
	and construction_test_v<T, to_tuple_t<T>>;

	// Checks for an annotation of a specific type and returns it if it exists
	template<typename AnnotationType, std::meta::info r>
	consteval std::optional<AnnotationType> get_annotation( ) {
		auto annotations = annotations_of_with_type( r, ^^AnnotationType );
		if( annotations.empty( ) ) {
			return std::nullopt;
		}
		return extract<AnnotationType>( annotations.front( ) );
	}

	template<auto member_info, json_name name>
	consteval std::optional<refl_map_as> get_map_as_annotation( ) {
		static constexpr auto refl_map_as_annot =
		  get_annotation<refl_map_as, member_info>( );

		static constexpr auto refl_enum_string_annot =
		  get_annotation<refl_enum_string, member_info>( );

		if constexpr( refl_map_as_annot ) {
			static_assert( not refl_enum_string_annot,
			               "Do not use reflect.enum_string and reflect.map_as "
			               "at the same time" );
			return refl_map_as_annot;
		} else if constexpr( refl_enum_string_annot ) {
			using json_member_no_name = enum_string<
			  typename[:type_of( member_info ):], refl_enum_string_annot->Options>;
			static constexpr auto info =
			  ^^typename json_member_no_name::template with_name<name>;
			return refl_map_as{ info };
		} else {
			return std::nullopt;
		}
	}

	template<JSONNAMETYPE Name, typename T>
	using deduce_t = typename json_details::ensure_mapped_t<
	  json_details::json_deduced_type<T>>::template with_name<Name>;

	template<auto member_info>
	consteval std::meta::info get_member_link_func( ) {
		static constexpr auto annot_rename =
		  get_annotation<refl_rename, member_info>( );

		static constexpr std::string_view svname =
		  annot_rename ? std::string_view( annot_rename->name )
		               : identifier_of( member_info );
		static constexpr auto name = json_name<svname.size( ) + 1>(
		  svname.data( ), std::make_index_sequence<svname.size( ) + 1>{ } );
		static_assert( not name.empty( ), "Unexpected empty name" );

		static constexpr auto annot_map_as =
		  get_map_as_annotation<member_info, name>( );

		if constexpr( annot_map_as ) {
			static_assert(
			  not annot_rename,
			  "Do not use reflect.rename and reflect.map_as at the same time" );
			return annot_map_as->type;
		} else {
			return ^^deduce_t<name, typename[:type_of( member_info ):]>;
		}
	}

	template<typename T, std::size_t Idx>
	using get_member_link_t = typename
	[:get_member_link_func<get_non_ignored_reflectible_members<T>( )[Idx]>( ):];

} // namespace daw::json::inline DAW_JSON_VER::refl_details

#endif