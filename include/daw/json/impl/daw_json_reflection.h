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
#include "daw/json/daw_json_link.h"
#include "daw/json/impl/daw_json_reflection_impl.h"

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
	template<typename D>
	struct refl_ignored_value : refl_annotation_base, refl_ignored_base {
		D default_value;

		explicit constexpr refl_ignored_value( D value )
		  : default_value( value ) {}

		template<typename T>
		constexpr operator T( ) const {
			if constexpr( std::is_convertible_v<D, T> ) {
				return default_value;
			} else if constexpr( requires( D v ) {
				                     { v( ) }->std::convertible_to<T>;
			                     } ) {
				return default_value( );
			}
		}
	};

	struct refl_ignored : refl_annotation_base, refl_ignored_base {
		explicit consteval refl_ignored( ) = default;

		template<typename T>
		static consteval auto operator( )( T &&rhs ) {
			return refl_ignored_value<T>{ DAW_FWD( rhs ) };
		}

		template<typename T>
		consteval operator T( ) const {
			return T{ };
		}
	};

	struct member_reflection_t {};

	template<typename T, std::size_t Idx>
	using submember_type_t = std::tuple_element_t<Idx, to_tuple_t<T>>;

	template<EnumType E>
	constexpr E enum_from_string( std::string_view name ) {
		template for( constexpr auto enumerator : enumerators_of( ^^E ) ) {
			// TODO add name formatting e.g lower/upper/first capital
			if( name == identifier_of( enumerator ) ) {
				return [:enumerator:];
			}
		}
		daw_json_ensure( daw::pipelines::Contains( name )( enumerators_of( ^^E ) ),
		                 ErrorReason::InvalidString );
	}

	template<EnumType E>
	constexpr std::string_view enum_to_string( E value ) {
		static constexpr auto enums =
		  reflect_constant_array( enumerators_of( ^^E ) );
		template for( constexpr auto enumerator : [:enums:] ) {
			if( value == [:enumerator:] ) {
				return identifier_of( enumerator );
			}
		}
		return std::string_view{ };
	}

	template<EnumType E>
	struct reflect_enum_as_string {
		static constexpr E operator( )( std::string_view name ) {
			return enum_from_string<E>( name );
		}

		static constexpr std::string_view operator( )( E value ) {
			return enum_to_string( value );
		}
	};
	template<typename T, std::size_t... Idx>
	consteval std::meta::info
	get_json_members_list_impl( std::index_sequence<Idx...> ) {
		return ^^json_member_list<get_member_link_t<T, Idx>...>;
	}

	template<typename T>
	consteval std::meta::info get_json_member_list( ) {
		static constexpr auto sz =
		  get_non_ignored_reflectible_members<T>( ).size( );
		return get_json_members_list_impl<T>( std::make_index_sequence<sz>{ } );
	}

	template<EnumType E, json_options_t Options>
	struct enum_string : json_custom_no_name<E, reflect_enum_as_string<E>,
	                                         reflect_enum_as_string<E>, Options> {
	};

	template<Reflectable T>
	consteval bool has_reflected_submembers( ) {
		static constexpr auto members = [:as_stdarray(
		                                    get_reflectible_members<T>( ) ):];
		template for( constexpr auto member : members ) {
			if( not annotations_of_with_base_type( member, ^^refl_annotation_base )
			          .empty( ) ) {
				return true;
			}
		}
		return false;
	}

} // namespace daw::json::inline DAW_JSON_VER::refl_details
namespace daw::json::inline DAW_JSON_VER {

	///
	/// class for daw::json::reflection that allows marking user data structures
	/// as reflectable and update the mappings of their members
	struct reflect_base_t {

		/// By default the reflection mappings use the members name.  This allows
		/// overriding that
		template<json_name Name>
		static constexpr auto rename = refl_details::refl_rename{ Name.m_data };

		template<typename JsonMember>
		static constexpr auto map_as = refl_details::refl_map_as{ ^^JsonMember };

		/// Do not map this member.  One can add a default value or provide a
		/// callable that generates a value convertible to the member.  The
		/// default is T{}
		static constexpr auto ignored = refl_details::refl_ignored{ };

		/// Map the enum as a string
		template<json_options_t Options>
		static constexpr auto enum_string_with_options =
		  refl_details::refl_enum_string{ Options };

		static constexpr auto enum_string =
		  enum_string_with_options<json_custom_opts_def>;
	};
	struct reflect_all_t : reflect_base_t, refl_details::reflect_all_t {};
	struct reflect_t : reflect_base_t {
		static constexpr auto unchecked = reflect_all_t{ };
	};

	inline constexpr auto reflect = reflect_t{ };
} // namespace daw::json::inline DAW_JSON_VER

namespace daw::json::inline DAW_JSON_VER {
	template<typename T>
	inline constexpr bool enable_reflection_for = false;

	template<typename T>
	concept ReflectionEnabled =
	  enable_reflection_for<T> or
	  refl_details::has_annotation<reflect_base_t, T>( ) or
	  refl_details::has_reflected_submembers<T>( );

	template<ReflectionEnabled T>
	struct json_data_contract<T> {
		using constructor_t = refl_details::reflected_constructor<T>;

		using type = typename[:refl_details::get_json_member_list<T>( ):];

		DAW_ATTRIB_INLINE static constexpr auto to_json_data( T const &value ) {
			return refl_details::to_tuple( value );
		}
	};
} // namespace daw::json::inline DAW_JSON_VER

#endif