// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw/json/impl/version.h"

#include "daw/json/daw_json_switches.h"

#if defined( DAW_JSON_HAS_REFLECTION )
#include "daw/json/impl/daw_json_name.h"
#include "daw/json/impl/daw_json_option_bits.h"
#include "daw/json/impl/daw_json_traits.h"
#include "daw/json/impl/daw_json_type_options.h"

#include <daw/daw_concepts.h>

#include <meta>
#include <optional>
#include <type_traits>
#include <vector>

namespace daw::json::inline DAW_JSON_VER {
	struct reflect_base_t;
} // namespace daw::json::inline DAW_JSON_VER

namespace daw::json::inline DAW_JSON_VER::refl_details {
	/// Get the public non-static data members
	consteval std::vector<std::meta::info>
	pub_nsdm_of( std::meta::info type_class ) {
		return nonstatic_data_members_of(
		  type_class, std::meta::access_context::unprivileged( ) );
	}

	consteval std::vector<std::meta::info>
	all_nsdm_of( std::meta::info type_class ) {
		return nonstatic_data_members_of( type_class,
		                                  std::meta::access_context::unchecked( ) );
	}

	template<typename T>
	concept PublicMembersOnly = all_nsdm_of( remove_cvref( ^^T ) ).size( ) ==
	                            pub_nsdm_of( remove_cvref( ^^T ) ).size( );
	// The type may not be reflectable but we can error later
	template<typename T>
	concept PotentiallyReflectable =
	  not std::is_empty_v<T> and std::is_class_v<T> and std::is_aggregate_v<T> and
	  PublicMembersOnly<T>;

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

	/// Types that can carry reflection annotations
	template<typename T>
	concept TypeAnnotatable =
	  std::is_class_v<T> or std::is_enum_v<T> or std::is_union_v<T>;

	/// Get the annotation of type Annotation on the type T, e.g.
	/// enum class [[= reflect.enum_string]] E { A, B };
	template<typename Annotation, typename T>
	consteval std::optional<Annotation> get_type_annotation( ) {
		if constexpr( TypeAnnotatable<T> ) {
			for( auto annot : annotations_of( ^^T ) ) {
				if( remove_cv( type_of( annot ) ) == ^^Annotation ) {
					return extract<Annotation>( annot );
				}
			}
		}
		return std::nullopt;
	}

	template<typename AnnotationBase, typename T>
	consteval bool has_type_annotation_with_base( ) {
		if constexpr( TypeAnnotatable<T> ) {
			return not annotations_of_with_base_type( ^^T, ^^AnnotationBase )
			             .empty( );
		} else {
			return false;
		}
	}

	/// Types that have a reflect.* annotation that only applies to members, or
	/// that change how the type is mapped. These are handled when the mapping of
	/// the type is deduced
	template<typename T>
	concept HasTypeLevelAnnotation =
	  has_type_annotation_with_base<refl_annotation_base, T>( ) or
	  has_type_annotation_with_base<refl_ignored_base, T>( ) or
	  ( std::is_enum_v<T> and has_type_annotation_with_base<reflect_base_t, T>( ) );

	/// Enum types annotated with reflect.enum_string are mapped as strings
	/// wherever their mapping is deduced
	template<typename E>
	concept EnumStringAnnotated =
	  std::is_enum_v<E> and
	  get_type_annotation<refl_enum_string, E>( ).has_value( );

	/// Types annotated with reflect.map_as<JsonType> are mapped with JsonType
	/// wherever their mapping is deduced
	template<typename T>
	concept MapAsAnnotated =
	  get_type_annotation<refl_map_as, T>( ).has_value( );

	/// Get the mapping for a type from its type level annotations, checking that
	/// only annotations valid on a type are used
	template<HasTypeLevelAnnotation T>
	consteval auto type_annotation_mapping( ) {
		static_assert( not( std::is_enum_v<T> and
		                    has_type_annotation_with_base<reflect_base_t, T>( ) ),
		               "reflect is only valid on class types. Use "
		               "reflect.enum_string to map an enum as a string" );
		static_assert( not get_type_annotation<refl_rename, T>( ),
		               "reflect.rename is only valid on members, not types" );
		static_assert( not has_type_annotation_with_base<refl_ignored_base, T>( ),
		               "reflect.ignored is only valid on members, not types" );
		static_assert( std::is_enum_v<T> or
		                 not get_type_annotation<refl_enum_string, T>( ),
		               "reflect.enum_string is only valid on enum types or "
		               "members of enum type" );
		static_assert( not( EnumStringAnnotated<T> and MapAsAnnotated<T> ),
		               "Do not use reflect.enum_string and reflect.map_as "
		               "at the same time" );
		if constexpr( EnumStringAnnotated<T> ) {
			static constexpr auto annot = get_type_annotation<refl_enum_string, T>( );
			return daw::traits::identity<enum_string<T, annot->Options>>{ };
		} else if constexpr( MapAsAnnotated<T> ) {
			static constexpr auto annot = get_type_annotation<refl_map_as, T>( );
			using mapping_t = typename[:annot->type:];
			static_assert( json_details::is_a_json_type_v<mapping_t>,
			               "A type level reflect.map_as<JsonType> requires a JSON "
			               "type, e.g. json_number_no_name<int>" );
			static_assert( json_details::is_no_name_v<mapping_t>,
			               "A type level reflect.map_as<JsonType> requires a no "
			               "name JSON type, e.g. json_number_no_name<int>. The "
			               "member name is supplied where the type is used" );
			return daw::traits::identity<mapping_t>{ };
		} else {
			// Unreachable, the checks above will have failed
			return daw::traits::identity<void>{ };
		}
	}

	template<HasTypeLevelAnnotation T>
	using type_annotation_mapping_t =
	  typename decltype( type_annotation_mapping<T>( ) )::type;
} // namespace daw::json::inline DAW_JSON_VER::refl_details
#endif
