// copyright (c) darrell wright
//
// distributed under the boost software license, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_string_view.h>

#include <optional>
#include <string>
#include <vector>

namespace daw {
	namespace parse_arg_details {
		struct is_ws_t {
			constexpr bool operator( )( char c ) const {
				return c <= 0x20;
			}
		};
		inline constexpr auto is_ws = is_ws_t{ };
		struct is_eq_t {
			constexpr bool operator( )( char c ) const {
				return c == '=';
			}
		};
		inline constexpr auto is_eq = is_eq_t{ };
	} // namespace parse_arg_details

	class Arguments;

	struct Argument {
		daw::string_view name{ };
		daw::string_view value{ };

	private:
		friend class ::daw::Arguments;

		constexpr Argument( daw::string_view args ) {
			if( args.size( ) >= 2 and ( ( args[0] == '-' ) & ( args[1] == '-' ) ) ) {
				// We are a named argument, starting with '--'
				args.remove_prefix( 2 );
				name = args.pop_front( parse_arg_details::is_eq );
				if( args.empty( ) ) {
					return;
				}
				args.remove_prefix( );
			}
			// We have an assigned value
			value = args;
		}
	};

	class Arguments {
		daw::string_view m_prog_name = "";
		std::vector<Argument> m_arguments{ };

	public:
		using value_type = Argument;
		using reference = Argument &;
		using const_reference = Argument const &;
		using values_type = std::vector<value_type>;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;
		using size_type = std::size_t;
		using differnce_type = std::ptrdiff_t;

		inline Arguments( int argc, char **argv ) {
			if( argc > 0 and argv[0] != nullptr ) {
				m_prog_name = argv[0];
			}
			for( int n = 1; n < argc; ++n ) {
				m_arguments.push_back( { argv[n] } );
			}
		}

		daw::string_view program_name( ) const {
			return m_prog_name;
		}

		const_iterator begin( ) const {
			return m_arguments.begin( );
		}

		const_iterator cbegin( ) const {
			return m_arguments.cbegin( );
		}

		const_iterator end( ) const {
			return m_arguments.end( );
		}

		const_iterator cend( ) const {
			return m_arguments.cend( );
		}

		size_type size( ) const {
			return m_arguments.size( );
		}

		bool empty( ) const {
			return m_arguments.empty( );
		}

		const_reference operator[]( std::size_t idx ) const {
			return m_arguments[idx];
		}

		std::optional<size_type>
		find_argument_position( daw::string_view name ) const {
			auto pos = std::find_if( begin( ), end( ), [name]( Argument const &arg ) {
				return arg.name == name;
			} );
			if( pos == end( ) ) {
				return std::nullopt;
			}
			return static_cast<size_type>( std::distance( begin( ), pos ) );
		}

		std::optional<size_type> first_named_argument( ) const {
			auto pos = std::find_if( begin( ), end( ), []( Argument const &arg ) {
				return not arg.name.empty( );
			} );
			if( pos == end( ) ) {
				return std::nullopt;
			}
			return static_cast<size_type>( std::distance( begin( ), pos ) );
		}

		bool has_named_argument( ) const {
			return static_cast<bool>( first_named_argument( ) );
		}
	};
} // namespace daw
