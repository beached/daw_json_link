// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_exception.h"
#include "daw_move.h"
#include "daw_traits.h"

#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace daw::string {
	namespace string_details {
		using string_t = std::string;
	} // namespace string_details
} // namespace daw::string

template<typename CharT, typename Traits = std::char_traits<CharT>>
void clear( std::basic_stringstream<CharT, Traits> &ss ) {
	ss.str( std::basic_string<CharT, Traits>{} );
	ss.clear( );
}

namespace daw::string {
	namespace string_details {
		template<typename To, typename From>
		To lexical_cast( From const &from ) {
			std::stringstream ss;
			ss << from;
			To result;
			ss >> result;
			return result;
		}

		template<typename Arg>
		std::vector<string_details::string_t> unknowns_to_string( Arg arg ) {
			std::vector<string_details::string_t> result;
			if constexpr( std::is_same_v<Arg, std::string> ) {
				result.emplace_back( std::move( arg ) );
			} else if constexpr( std::is_pointer_v<Arg> ) {
				result.emplace_back( arg );
			} else {
				using std::to_string;
				result.emplace_back( to_string( arg ) );
			}
			return result;
		}

		template<typename Arg, typename... Args>
		std::vector<string_details::string_t> unknowns_to_string( Arg arg,
		                                                          Args... args ) {
			std::vector<string_details::string_t> result;
			result.reserve( sizeof...( args ) + 1 );
			using std::to_string;
			if constexpr( std::is_same_v<Arg, std::string> ) {
				result.emplace_back( std::move( arg ) );
			} else if constexpr( std::is_pointer_v<Arg> ) {
				result.emplace_back( arg );
			} else {
				result.emplace_back( to_string( arg ) );
			}
			auto result2 = unknowns_to_string( args... );
			result.insert( std::end( result ), std::begin( result2 ),
			               std::end( result2 ) );
			return result;
		}
	} // namespace string_details

	template<typename String>
	auto split( String const &sv ) {
		using string_t = std::decay_t<String>;
		std::vector<string_t> result;
		auto pos = sv.begin( );
		decltype( pos ) find_iter;
		while( true ) {
			find_iter = find_if( pos, sv.end( ), []( auto const &c ) {
				return static_cast<bool>( isspace( c ) );
			} );
			result.emplace_back( pos, find_iter );
			if( find_iter == sv.end( ) ) {
				break;
			}
			pos = ++find_iter;
		}
		return result;
	}

	template<typename String>
	auto split( String const &sv, char const delimiter ) {
		using string_t = std::decay_t<String>;
		std::vector<string_t> result;
		auto pos = sv.begin( );
		decltype( pos ) find_iter;
		while( true ) {
			find_iter = find( pos, sv.end( ), delimiter );
			result.emplace_back( pos, find_iter );
			if( find_iter == sv.end( ) ) {
				break;
			}
			pos = ++find_iter;
		}
		return result;
	}

	template<typename String, typename Delimiters>
	auto split( String const &sv, Delimiters const &delimiters ) {
		using string_t = std::decay_t<String>;
		std::vector<string_t> result;
		auto pos = sv.begin( );
		decltype( pos ) find_iter;
		while( true ) {
			find_iter = find_if( pos, sv.end( ), [&delimiters]( auto const &c ) {
				return std::find( std::begin( delimiters ), std::end( delimiters ),
				                  c ) != delimiters.end( );
			} );
			result.emplace_back( pos, find_iter );
			if( find_iter == sv.end( ) ) {
				break;
			}
			pos = ++find_iter;
		}
		return result;
	}

	template<typename String, typename Predicate>
	auto split_if( String const &sv, Predicate is_delim ) {
		using string_t = std::decay_t<String>;
		std::vector<string_t> result;
		auto pos = sv.begin( );
		decltype( pos ) find_iter;
		while( true ) {
			find_iter = find_if( pos, sv.end( ), [is_delim]( auto const &c ) {
				return static_cast<bool>( is_delim( c ) );
			} );
			result.emplace_back( pos, find_iter );
			if( find_iter == sv.end( ) ) {
				break;
			}
			pos = ++find_iter;
		}
		return result;
	}

	template<typename Arg>
	string_details::string_t to_string( Arg const &arg ) {
		if constexpr( std::is_same_v<Arg, std::string> ) {
			return arg;
		} else if constexpr( std::is_pointer_v<Arg> ) {
			return string_details::string_t( arg );
		} else {
			return std::to_string( arg );
		}
	}

	template<size_t N>
	string_details::string_t to_string( char const ( &sl )[N] ) {
		return std::string( sl );
	}

	template<typename Arg>
	decltype( auto ) string_join( Arg const &arg ) {
		return to_string( arg );
	}

	template<typename Arg1, typename Arg2>
	auto string_join( Arg1 const &arg1, Arg2 const &arg2 )
	  -> decltype( to_string( arg1 ) + to_string( arg2 ) ) {
		return to_string( arg1 ) + to_string( arg2 );
	}

	template<typename Arg1, typename Arg2, typename... Args>
	auto string_join( Arg1 const &arg1, Arg2 const &arg2, Args const &... args )
	  -> decltype( string_join( string_join( arg1, arg2 ),
	                            string_join( args... ) ) ) {
		return string_join( string_join( arg1, arg2 ), string_join( args... ) );
	}

	template<typename StringType>
	struct ends_with_t {
		ends_with_t( ) noexcept {}
		~ends_with_t( ) = default;
		ends_with_t( ends_with_t const & ) noexcept {}
		ends_with_t( ends_with_t && ) noexcept {}
		ends_with_t &operator=( ends_with_t ) const noexcept {
			return *this;
		}
		bool operator==( ends_with_t const & ) const noexcept {
			return true;
		}
		bool operator<( ends_with_t const & ) const noexcept {
			return false;
		}

		bool operator( )( StringType const &src, const char ending ) const
		  noexcept {
			return 0 < src.size( ) and ending == src[src.size( ) - 1];
		}

		bool operator( )( StringType const &src, StringType const &ending ) const
		  noexcept {
			auto pos = src.find_last_of( ending );
			return string_details::string_t::npos != pos and pos == src.size( ) - 1;
		}
	};

	template<typename StringType, typename Ending>
	bool ends_with( StringType const &src, Ending const &ending ) {
		const static auto func = ends_with_t<StringType>( );
		return func( src, ending );
	}

	template<typename String>
	constexpr decltype( auto ) fmt( String &&fmt ) noexcept {
		return std::forward<String>( fmt );
	}

	//////////////////////////////////////////////////////////////////////////
	// Summary: takes a format string like "{0} {1} {2}" and in this case 3
	// parameters
	template<typename String, typename Arg, typename... Args>
	string_details::string_t fmt( String &&format_, Arg arg, Args... args ) {
		std::regex const reg( R"^(\{(\d+(:\d)*)\})^" );
		std::stringstream ss;
		auto const arguments = string_details::unknowns_to_string( arg, args... );
		std::smatch sm;
		std::string format{format_};
		while( std::regex_search( format, sm, reg ) ) {
			auto const &prefix = sm.prefix( ).str( );
			ss << prefix;
			if( ends_with( prefix, "{" ) ) {
				ss << sm[0].str( );
			} else {
				auto delims = split( sm[1].str( ), ':' );
				if( 1 >= delims.size( ) ) {
					std::stringstream ss2;
					ss2 << sm[1].str( );
					size_t v;
					ss2 >> v;
					ss << arguments[v];
				} else if( 2 == delims.size( ) ) {
					// Assumes the argument at pos is a double.  If not, will crash
					size_t pos = string_details::lexical_cast<size_t>( delims[0] );
					int precision = string_details::lexical_cast<int>( delims[1] );
					ss << std::fixed << std::setprecision( precision )
					   << string_details::lexical_cast<double>( arguments[pos] );
				} else {
					daw::exception::daw_throw<std::out_of_range>(
					  fmt( "Unknown string format.  Too many colons(", delims.size( ),
					       "): ", sm[1].str( ) ) );
				}
			}
			format = sm.suffix( ).str( );
		}
		ss << format;
		return ss.str( );
	}

	namespace string_details {
		constexpr auto standard_split_delimiters( char ) {
			return " \f\n\r\t\v\0";
		}

		constexpr auto standard_split_delimiters( wchar_t ) {
			return L" \f\n\r\t\v\0";
		}
	} // namespace string_details

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	auto trim_right_copy(
	  std::basic_string<CharT, Traits, Allocator> const &str,
	  std::basic_string<CharT, Traits, Allocator> const &delimiters =
	    string_details::standard_split_delimiters( CharT{} ) ) {
		if( str.empty( ) ) {
			return str;
		}
		return str.substr( 0, str.find_last_not_of( delimiters ) + 1 );
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	void
	trim_right( std::basic_string<CharT, Traits, Allocator> &str,
	            std::basic_string<CharT, Traits, Allocator> const &delimiters =
	              string_details::standard_split_delimiters( CharT{} ) ) {
		if( str.empty( ) ) {
			return;
		}
		str = str.substr( 0, str.find_last_not_of( delimiters ) + 1 );
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	auto trim_left_copy(
	  std::basic_string<CharT, Traits, Allocator> const &str,
	  std::basic_string<CharT, Traits, Allocator> const &delimiters =
	    string_details::standard_split_delimiters( CharT{} ) ) {
		if( str.empty( ) ) {
			return str;
		}
		return str.substr( str.find_first_not_of( delimiters ) );
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	void
	trim_left( std::basic_string<CharT, Traits, Allocator> &str,
	           std::basic_string<CharT, Traits, Allocator> const &delimiters =
	             string_details::standard_split_delimiters( CharT{} ) ) {
		if( str.empty( ) ) {
			return;
		}
		str = str.substr( str.find_first_not_of( delimiters ) );
	}

	template<class StringElementType, class StringType>
	bool in( StringElementType const &val, StringType const &values ) {
		return StringType::npos != values.find( val );
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	void trim( std::basic_string<CharT, Traits, Allocator> &str,
	           std::basic_string<CharT, Traits, Allocator> const &delimiters =
	             string_details::standard_split_delimiters( CharT{} ) ) {
		auto const start = str.find_first_not_of( delimiters );
		if( std::basic_string<CharT, Traits, Allocator>::npos == start ) {
			str.clear( );
			return;
		}
		auto const end = str.find_last_not_of( delimiters );
		auto const len = end - start + 1;
		str = str.substr( start, len );
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	auto
	trim_copy( std::basic_string<CharT, Traits, Allocator> str,
	           std::basic_string<CharT, Traits, Allocator> const &delimiters =
	             string_details::standard_split_delimiters( CharT{} ) ) {
		trim( str, delimiters );
		return str;
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>,
	         typename Allocator = std::allocator<CharT>>
	bool contains( std::basic_string<CharT, Traits, Allocator> const &str,
	               std::basic_string<CharT, Traits, Allocator> const &match ) {
		return str.find( match ) !=
		       std::basic_string<CharT, Traits, Allocator>::npos;
	}

	template<typename CharT = char, typename traits = std::char_traits<CharT>,
	         typename Alloc = std::allocator<CharT>>
	void search_replace( std::basic_string<CharT, traits, Alloc> &in_str,
	                     CharT const *search_for, CharT const *replace_with ) {
		struct {
			inline auto operator( )( wchar_t const *ptr ) const noexcept {
				return wcslen( ptr );
			}
			inline auto operator( )( char const *ptr ) const noexcept {
				return strlen( ptr );
			}
		} str_size;

		size_t pos = 0u;
		auto const search_for_len = str_size( search_for );
		auto const replace_with_len = str_size( replace_with );

		while( ( pos = in_str.find( search_for, pos ) ) !=
		       std::basic_string<CharT, traits, Alloc>::npos ) {
			in_str.replace( pos, search_for_len, replace_with );
			pos += replace_with_len;
		}
	}

	template<typename CharT = char, typename traits = std::char_traits<CharT>,
	         typename Alloc = std::allocator<CharT>>
	auto search_replace_copy( std::basic_string<CharT, traits, Alloc> in_str,
	                          CharT const *search_for,
	                          CharT const *replace_with ) {
		search_replace( in_str, search_for, replace_with );
		return in_str;
	}

	namespace string_details {
		template<typename... Args>
		using can_construct_string =
		  decltype( std::string( std::declval<Args>( )... ) );

		template<typename CharT, typename Traits = std::char_traits<CharT>,
		         typename Allocator = std::allocator<CharT>>
		struct BasicString {
			using values_type = std::basic_string<CharT, Traits, Allocator>;
			using value_type = typename values_type::value_type;
			using reference = value_type &;
			using const_reference = value_type const &;
			using pointer = value_type *;
			using const_pointer = value_type const *;
			using iterator = typename values_type::iterator;
			using const_iterator = typename values_type::const_iterator;

			values_type m_string;

			template<
			  typename... Args,
			  std::enable_if_t<daw::is_detected_v<can_construct_string, Args...>,
			                   std::nullptr_t> = nullptr>
			BasicString( Args &&... args )
			  : m_string( std::forward<Args>( args )... ) {}

			BasicString( BasicString const & ) = default;

			BasicString( BasicString && ) noexcept = default;

			BasicString( value_type other )
			  : m_string{daw::move( other )} {}

			~BasicString( ) = default;

			BasicString &operator=( BasicString const & ) = default;

			BasicString &operator=( BasicString && ) noexcept = default;

			BasicString &operator=( value_type rhs ) {
				m_string = daw::move( rhs );
				return *this;
			}

			BasicString &search_replace( CharT const *search_for,
			                             CharT const *replace_with ) {
				daw::string::search_replace( m_string, search_for, replace_with );
				return *this;
			}

			BasicString &
			trim_left( std::basic_string<CharT, Traits, Allocator> const &delimiters =
			             string_details::standard_split_delimiters( CharT{} ) ) {
				daw::string::trim_left( m_string, delimiters );
				return *this;
			}

			BasicString &trim_right(
			  std::basic_string<CharT, Traits, Allocator> const &delimiters =
			    string_details::standard_split_delimiters( CharT{} ) ) {
				daw::string::trim_right( m_string, delimiters );
				return *this;
			}

			BasicString &
			trim( std::basic_string<CharT, Traits, Allocator> const &delimiters =
			        string_details::standard_split_delimiters( CharT{} ) ) {
				daw::string::trim( m_string, delimiters );
				return *this;
			}

			auto const &to_string( ) const {
				return m_string;
			}
		}; // BasicString

		template<typename OStream, typename CharT,
		         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, BasicString<CharT> const &str ) {
			os << str.to_string( );
			return os;
		}
	} // namespace string_details

	template<typename StrArray, typename Char>
	auto combine( StrArray const &strings, Char chr ) {
		std::stringstream ss;
		bool is_first = true;
		for( auto const &str : strings ) {
			if( !is_first ) {
				ss << chr;
			} else {
				is_first = false;
			}
			ss << str;
		}
		return ss.str( );
	}
} // namespace daw::string

namespace daw {
	using String = daw::string::string_details::BasicString<char>;
	using WString = daw::string::string_details::BasicString<wchar_t>;
} // namespace daw
