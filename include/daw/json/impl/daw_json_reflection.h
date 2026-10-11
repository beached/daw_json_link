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
				                     { v( ) } -> std::convertible_to<T>;
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
		static constexpr auto enums =
		  reflect_constant_array( enumerators_of( ^^E ) );
		template for( constexpr auto enumerator : [:enums:] ) {
			// TODO add name formatting e.g lower/upper/first capital
			if( name == identifier_of( enumerator ) ) {
				return [:enumerator:];
			}
		}
		daw_json_error( true, ErrorReason::InvalidString );
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
		daw_json_error( true, ErrorReason::CouldNotFindEnumeratorForValue );
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

	/// Map an enum as the enumerator's name when the value has one, otherwise
	/// as a number of the underlying type. Parsing accepts either form
	template<EnumType E, json_options_t NumberOptions>
	struct reflect_enum_as_string_or_number {
		using number_t = std::underlying_type_t<E>;
		using json_number_t = json_number_no_name<number_t, NumberOptions>;

		/// Values are from a JsonCustomTypes::Any mapping, strings retain their
		/// quotes
		static constexpr E operator( )( std::string_view json_value ) {
			daw_json_ensure( not json_value.empty( ),
			                 ErrorReason::UnexpectedEndOfData );
			if( json_value.front( ) == '"' ) {
				auto const name_end = json_value.find( '"', 1 );
				daw_json_ensure( name_end != std::string_view::npos,
				                 ErrorReason::InvalidString );
				auto const name = json_value.substr( 1, name_end - 1 );
				// Identifiers cannot start with these, allow the number options to
				// decide if a number in a string is valid
				if( name.empty( ) or
				    not( name.front( ) == '-' or name.front( ) == '+' or
				         ( name.front( ) >= '0' and name.front( ) <= '9' ) ) ) {
					return enum_from_string<E>( name );
				}
			}
			return static_cast<E>( from_json<json_number_t>( json_value ) );
		}

		template<typename WritableType>
		static constexpr WritableType operator( )( WritableType it, E value ) {
			static constexpr auto enums =
			  reflect_constant_array( enumerators_of( ^^E ) );
			template for( constexpr auto enumerator : [:enums:] ) {
				if( value == [:enumerator:] ) {
					static constexpr std::string_view name = identifier_of( enumerator );
					it.put( '"' );
					it.write( name );
					it.put( '"' );
					return it;
				}
			}
			return json_details::member_to_string<json_number_t>(
			  std::move( it ), static_cast<number_t>( value ) );
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

	template<EnumType E, json_options_t NumberOptions>
	struct enum_string_or_number
	  : json_custom_no_name<
	      E, reflect_enum_as_string_or_number<E, NumberOptions>,
	      reflect_enum_as_string_or_number<E, NumberOptions>,
	      options::json_custom_opt( options::JsonCustomTypes::Any )> {};

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

		/// Map the enum as a string when the value is an enumerator, otherwise as
		/// a number of the underlying type. NumberOptions are the json_number
		/// options, e.g. number_opt( options::JsonRangeCheck::CheckForNarrowing )
		template<json_options_t NumberOptions>
		static constexpr auto enum_string_or_number_with_options =
		  refl_details::refl_enum_string_or_number{ NumberOptions };

		static constexpr auto enum_string_or_number =
		  enum_string_or_number_with_options<number_opts_def>;
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
	  std::is_class_v<T> and
	  ( enable_reflection_for<T> or
	    refl_details::has_annotation<reflect_base_t, T>( ) or
	    refl_details::has_reflected_submembers<T>( ) );

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