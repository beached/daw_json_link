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

#include <algorithm>
#include <exception>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include "daw_exception.h"
#include "daw_move.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		template<typename T, typename Iterator>
		struct parser_result {
			Iterator first;
			Iterator last;
			T result;

			parser_result( ) = default;
			parser_result( T value, Iterator f, Iterator l )
			  : first{daw::move( f )}
			  , last{daw::move( l )}
			  , result{daw::move( value )} {}

			~parser_result( );
			parser_result( parser_result const & ) = default;
			parser_result( parser_result && ) noexcept = default;
			parser_result &operator=( parser_result const & ) = default;
			parser_result &operator=( parser_result && ) noexcept = default;
		}; // parser_result

		template<typename T, typename Iterator>
		parser_result<T, Iterator>::~parser_result( ) = default;

		struct ParserException {};
		struct ParserEmptyException : public ParserException {};
		struct ParserOverflowException : public ParserException {};
		using ParserOutOfRangeException = std::out_of_range;

		template<typename ForwardIterator>
		struct find_result_t {
			ForwardIterator first;
			ForwardIterator last;
			bool found;

			constexpr find_result_t( ForwardIterator First, ForwardIterator Last,
			                         bool Found ) noexcept
			  : first{First}
			  , last{Last}
			  , found{Found} {}

			constexpr explicit operator bool( ) const noexcept {
				return found;
			}

			constexpr bool empty( ) noexcept {
				return first == last;
			}

			constexpr bool empty( bool throw_on_empty ) {
				daw::exception::precondition_check<ParserEmptyException>(
				  !( throw_on_empty and empty( ) ) );
				return false;
			}

			template<typename Container>
			auto as( ) const {
				Container result;
				std::copy( first, last, std::back_inserter( result ) );
				return result;
			}

			std::string to_string( ) const {
				return std::string(
				  static_cast<char const *>( &( *first ) ),
				  static_cast<std::string::size_type>( std::distance( first, last ) ) );
			}

			daw::string_view to_string_view( ) const {
				return daw::make_string_view_it( first, last );
			}

			constexpr ForwardIterator begin( ) noexcept {
				return first;
			}

			constexpr ForwardIterator const begin( ) const noexcept {
				return first;
			}

			constexpr ForwardIterator end( ) noexcept {
				return last;
			}

			constexpr ForwardIterator const end( ) const noexcept {
				return last;
			}
		}; // find_result_t

		template<typename ForwardIterator>
		constexpr find_result_t<ForwardIterator>
		make_find_result( ForwardIterator first, ForwardIterator last,
		                  bool result = false ) noexcept {
			return find_result_t<ForwardIterator>{first, last, result};
		}

		template<typename ForwardIterator, typename Predicate>
		constexpr auto until( ForwardIterator first, ForwardIterator last,
		                      Predicate is_last ) {
			//		                      std::function<bool(
			// daw::traits::root_type_t<decltype( *first )> )> is_last )
			//{

			auto result = make_find_result( first, last );
			for( auto it = first; it != last; ++it ) {
				if( ( result.found = is_last( *it ) ) ) {
					result.last = it;
					return result;
				}
			}
			return result;
		}

		template<typename ForwardIterator, typename Predicate>
		constexpr auto until( ForwardIterator first, ForwardIterator last,
		                      Predicate is_last, bool throw_if_end_reached ) {
			auto result = make_find_result( first, last );
			for( auto it = first; it != last; ++it ) {
				if( is_last( *it ) ) {
					result.found = true;
					result.last = it;
					return result;
				}
			}
			daw::exception::precondition_check<ParserException>(
			  !throw_if_end_reached );
			return result;
		}

		template<typename ForwardIterator, typename Predicate>
		constexpr auto until_false( ForwardIterator first, ForwardIterator last,
		                            Predicate is_last ) {
			auto result = make_find_result( first, last );
			for( auto it = first; it != last; ++it ) {
				if( !is_last( *it ) ) {
					result.found = true;
					result.last = it;
					return result;
				}
			}
			return result;
		}

		template<typename T, typename Arg>
		constexpr bool is_a( T const &value, Arg const &tst ) noexcept {
			// static_assert( traits::is_comparable_v<T, Arg>, "value is not
			// comparable to tst" );
			using val_t = typename daw::traits::max_sizeof<T, Arg>::type;
			return ( static_cast<val_t>( value ) == static_cast<val_t>( tst ) );
		}

		template<typename T, typename Arg, typename... Args>
		constexpr bool is_a( T const &value, Arg const &tst,
		                     Args const &... tsts ) noexcept {
			return is_a( value, tst ) or is_a( value, tsts... );
		}

		template<typename Arg>
		class one_of_t {
			std::vector<Arg> m_args;

		public:
			one_of_t( std::initializer_list<Arg> args )
			  : m_args{args} {}

			template<typename T>
			bool operator( )( T &&value ) const {
				return value_in( std::forward<T>( value ), m_args );
			}
		}; // one_of

		template<typename... Arg>
		auto one_of( Arg &&... args ) {
			return one_of_t<Arg...>{std::forward<Arg>( args )...};
		}

		template<typename ForwardIterator, typename Value, typename... Values>
		constexpr find_result_t<ForwardIterator>
		until_value( ForwardIterator first, ForwardIterator last, Value &&value,
		             Values &&... values ) {
			auto result = make_find_result( first, last );
			for( auto it = first; it != last; ++it ) {
				if( is_a( *it, std::forward<Value>( value ),
				          std::forward<Values>( values )... ) ) {
					result.last = it;
					result.found = true;
					break;
				}
			}
			return result;
		}

		template<typename T, typename Container>
		constexpr auto value_in( T &&value, Container &&container )
		  -> decltype( std::begin( container ) == std::end( container ) ) {

			return std::find_if( std::begin( container ), std::end( container ),
			                     [&]( auto const &v ) {
				                     return is_a( value, v );
			                     } ) == std::end( container );
		}

		template<typename T, typename U, size_t N>
		constexpr bool value_in( T &&value, U ( &container )[N] ) {
			return std::find_if( container, container + N, [&]( auto const &v ) {
				       return is_a( value, v );
			       } ) == std::end( container );
		}

		class in_t {
			daw::string_view container;

		public:
			constexpr in_t( daw::string_view values ) noexcept
			  : container{daw::move( values )} {}

			template<typename U>
			constexpr bool operator( )( U const &value ) const noexcept {
				return value_in( value, container );
			}
		}; // in_t

		constexpr auto in( daw::string_view container ) noexcept {
			return in_t{daw::move( container )};
		}

		/* old
		template<typename T>
		class in_t {
		    std::vector<T> container;

		  public:
		    in_t( std::vector<T> values ) : container{daw::move( values )} {}

		    template<typename U>
		    bool operator( )( U const &value ) const {
		        return value_in( value, container );
		    }
		}; // in_t

		template<typename T>
		auto in( std::vector<T> container ) {
		    return in_t<T>{daw::move( container )};
		}

		template<typename T, size_t N>
		auto in( T ( &container )[N] ) {
		    using value_t = daw::traits::root_type_t<T>;
		    std::vector<value_t> values;
		    std::copy_n( std::begin( container ), N, std::back_inserter( values ) );

		    return in_t<value_t>{daw::move( values )};
		}

		template<typename Container>
		auto in( Container container ) {
		    using value_t = daw::traits::root_type_t<decltype( container[0] )>;
		    std::vector<value_t> values;
		    std::copy( std::begin( container ), std::end( container ),
		std::back_inserter( values ) );

		    return in_t<value_t>{daw::move( values )};
		}
		*/

		template<typename ForwardIterator, typename Container>
		constexpr find_result_t<ForwardIterator>
		until_values( ForwardIterator first, ForwardIterator last,
		              Container &&container ) {
			auto result = make_find_result( first, last );
			for( auto it = first; it != last; ++it ) {
				if( value_in( *it, container ) ) {
					result.last = it;
					result.found = true;
					return result;
				}
			}
			return result;
		}

		template<typename T, typename Predicate>
		constexpr bool
		is_true( T &&value,
		         Predicate /*std::function<bool( daw::traits::root_type_t<T> )>*/
		           predicate ) {
			return predicate( value );
		}

		template<typename T, typename Predicate, typename... Predicates>
		constexpr bool is_true( T &&value, Predicate &&predicate,
		                        Predicates &&... predicates ) {
			return is_true( std::forward<Predicate>( predicate ),
			                std::forward<T>( value ) ) or
			       is_true( std::forward<T>( value ),
			                std::forward<Predicates>( predicates )... );
		}

		template<typename T, typename U, typename... Args>
		constexpr void expect( T &&value, U &&check, Args &&... args ) {
			if( !is_a( std::forward<T>( value ), std::forward<U>( check ),
			           std::forward<Args>( args )... ) ) {

				daw::exception::daw_throw<ParserException>( );
			}
		}

		template<typename T, typename Predicate, typename... Predicates>
		constexpr void expect_true( T &&value, Predicate &&predicate,
		                            Predicates &&... predicates ) {
			if( !is_true( std::forward<T>( value ),
			              std::forward<Predicate>( predicate ),
			              std::forward<Predicates>( predicates )... ) ) {

				daw::exception::daw_throw<ParserException>( );
			}
		}

		template<typename T>
		constexpr bool is_alpha( T &&value ) noexcept {
			using val_t = typename daw::traits::max_sizeof<T, decltype( 'a' )>::type;
			constexpr auto const a = static_cast<val_t>( 'a' );
			constexpr auto const A = static_cast<val_t>( 'A' );
			constexpr auto const z = static_cast<val_t>( 'z' );
			constexpr auto const Z = static_cast<val_t>( 'Z' );
			auto const tmp = static_cast<val_t>( value );
			return ( a <= tmp and tmp <= z ) or ( A <= tmp and tmp <= Z );
		}

		template<typename T, typename Min, typename Max>
		constexpr bool in_range( T value, Min min_value, Max max_value ) noexcept {
			using val_t = typename daw::traits::max_sizeof<T, Min, Max>::type;
			return static_cast<val_t>( min_value ) <= static_cast<val_t>( value ) &&
			       static_cast<val_t>( value ) <= static_cast<val_t>( max_value );
		}

		template<typename T>
		constexpr bool is_number( T const &value ) noexcept {
			return in_range( value, '0', '9' );
		}

		template<typename T>
		constexpr bool is_hex( T const &value ) noexcept {
			return in_range( value, '0', '9' ) or in_range( value, 'a', 'f' ) or
			       in_range( value, 'A', 'F' );
		}

		template<typename T>
		constexpr bool is_alphanumeric( T &&value ) noexcept {
			return is_alpha( std::forward<T>( value ) ) or
			       is_number( std::forward<T>( value ) );
		}

		template<typename T, typename U>
		constexpr void assert_not_equal( T &&lhs, U &&rhs ) {
			static_assert( traits::is_comparable_v<T, U>,
			               "lhs is not comparable to rhs" );

			daw::exception::precondition_check<ParserException>( lhs == rhs );
		}

		template<typename T, typename U>
		constexpr void assert_equal( T &&lhs, U &&rhs ) {
			static_assert( traits::is_comparable_v<T, U>,
			               "lhs is not comparable to rhs" );
			daw::exception::precondition_check<ParserException>( lhs != rhs );
		}

		template<typename Iterator>
		constexpr void assert_not_empty( Iterator first, Iterator last ) {

			daw::exception::precondition_check<ParserEmptyException>( first != last );
		}

		template<typename Iterator>
		constexpr void assert_empty( Iterator first, Iterator last ) {
			assert_equal( first, last );
		}

		template<typename ForwardIterator, typename StartFrom, typename GoUntil>
		constexpr auto from_to( ForwardIterator first, ForwardIterator last,
		                        StartFrom &&start_from, GoUntil &&go_until,
		                        bool throw_if_end_reached = false )
		  -> std::enable_if_t<
		    traits::is_comparable_v<decltype( *first ), StartFrom> &&
		      traits::is_comparable_v<decltype( *first ), GoUntil>,
		    find_result_t<ForwardIterator>> {

			auto start =
			  until_value( first, last, std::forward<StartFrom>( start_from ) );

			daw::exception::precondition_check<ParserException>( start );

			auto finish =
			  until_value( start.last, last, std::forward<GoUntil>( go_until ) );

			daw::exception::precondition_check<ParserException>(
			  !( throw_if_end_reached and !finish ) );

			auto result = make_find_result( start.last, finish.last );
			result.found = !result.empty( );
			return result;
		}

		template<typename ForwardIterator, typename IsFirstPredicate,
		         typename IsLastPredicate>
		constexpr auto from_to_pred( ForwardIterator first, ForwardIterator last,
		                             IsFirstPredicate is_first,
		                             IsLastPredicate is_last,
		                             bool throw_if_end_reached = false ) {
			/*
			std::function<bool( daw::traits::root_type_t<decltype( *first )> )>
			is_first, std::function<bool( daw::traits::root_type_t<decltype( *first )>
			)> is_last,
			*/
			auto start = until( first, last, is_first );

			daw::exception::precondition_check<ParserException>( start );

			auto finish = until( start.last, last, is_last );

			daw::exception::precondition_check<ParserException>(
			  !( throw_if_end_reached and !finish ) );

			auto result = make_find_result( start.last, finish.last );
			result.found = !result.empty( );
			return result;
		}

		template<typename ForwardIterator, typename Divider, typename... Dividers>
		auto split_on( ForwardIterator first, ForwardIterator last,
		               Divider &&divider, Dividers &&... dividers ) {
			std::vector<ForwardIterator> endings;
			auto result = until_value( first, last, std::forward<Divider>( divider ),
			                           std::forward<Dividers>( dividers )... );
			while( result ) {
				endings.push_back( result.last );
				result =
				  until_value( result.last, last, std::forward<Divider>( divider ),
				               std::forward<Dividers>( dividers )... );
			}
			if( result.first != result.last ) {
				endings.push_back( result.last );
			}
			return endings;
		}

		template<typename ForwardIterator, typename Predicate>
		auto split_if( ForwardIterator first, ForwardIterator last,
		               Predicate /*std::function<bool(
		                          *daw::traits::root_type_t<decltype( first )> )>*/
		                 is_divider ) {
			std::vector<ForwardIterator> endings;
			auto result = until( first, last, is_divider );
			while( result ) {
				endings.push_back( result.last );
				result = until( result.last, last, is_divider );
			}
			if( result.first != result.last ) {
				endings.push_back( result.last );
			}
			return endings;
		}

		template<typename T>
		constexpr bool pred_true( T const & ) noexcept {
			return true;
		}

		template<typename T>
		constexpr bool pred_false( T const & ) noexcept {
			return false;
		}

		template<typename T>
		constexpr bool is_cr( T const &value ) noexcept {
			return is_a( value, '\n' );
		}

		template<typename T>
		constexpr bool is_unicode_whitespace( T val ) noexcept {
			switch( static_cast<uint32_t>( val ) ) {
			case 0x00000009: // CHARACTER TABULATION
			case 0x0000000A: // LINE FEED
			case 0x0000000B: // LINE TABULATION
			case 0x0000000C: // FORM FEED
			case 0x0000000D: // CARRIAGE RETURN
			case 0x00000020: // SPACE
			case 0x00000085: // NEXT LINE
			case 0x000000A0: // NO-BREAK SPACE
			case 0x00001680: // OGHAM SPACE MARK
			case 0x00002000: // EN QUAD
			case 0x00002001: // EM QUAD
			case 0x00002002: // EN SPACE
			case 0x00002003: // EM SPACE
			case 0x00002004: // THREE-PER-EM SPACE
			case 0x00002005: // FOUR-PER-EM SPACE
			case 0x00002006: // SIX-PER-EM SPACE
			case 0x00002007: // FIGURE SPACE
			case 0x00002008: // PUNCTUATION SPACE
			case 0x00002009: // THIN SPACE
			case 0x0000200A: // HAIR SPACE
			case 0x00002028: // LINE SEPARATOR
			case 0x00002029: // PARAGRAPH SEPARATOR
			case 0x0000202F: // NARROW NO-BREAK SPACE
			case 0x0000205F: // MEDIUM MATHEMATICAL SPACE
			case 0x00003000: // IDEOGRAPHIC SPACE
				return true;
			default:
				return false;
			}
		}

		template<typename T>
		constexpr bool is_escape( T val ) noexcept {
			return '\\' == static_cast<uint32_t>( val );
		}

		template<typename T>
		constexpr bool is_quote( T val ) noexcept {
			return '"' == static_cast<uint32_t>( val );
		}

		template<typename ForwardIterator>
		constexpr auto skip_ws( ForwardIterator first, ForwardIterator last ) {
			auto result =
			  until_false( first, last, is_unicode_whitespace<decltype( *first )> );
			result.first = result.last;
			result.last = last;
			return result;
		}

		template<typename T>
		constexpr bool not_unicode_whitespace( T val ) noexcept {
			return !is_unicode_whitespace( val );
		}

		template<typename ForwardIterator>
		constexpr auto trim_left( ForwardIterator first, ForwardIterator last ) {
			using val_t = daw::traits::root_type_t<decltype( *first )>;
			auto start = until( first, last, &not_unicode_whitespace<val_t> );
			if( !start ) {
				return make_find_result( first, last );
			}
			return make_find_result( start.last, last );
		}

		template<typename ForwardIterator>
		constexpr auto trim_right( ForwardIterator first, ForwardIterator last ) {
			// Cannot start from right most index in unicode.  Must parse
			// left to right.  So that sucks for perf, try not to just right trim
			//
			using val_t = daw::traits::root_type_t<decltype( *first )>;
			auto start = until( first, last, &not_unicode_whitespace<val_t> );
			if( !start ) {
				return make_find_result( first, last, false );
			}
			auto finish = until( start.last, last, &is_unicode_whitespace<val_t> );
			return make_find_result( first, finish.last, true );
		}

		template<typename ForwardIterator>
		constexpr auto trim( ForwardIterator first, ForwardIterator last ) {
			auto start = trim_left( first, last );
			auto finish = trim_right( start.first, last );
			return make_find_result( start.first, finish.last,
			                         static_cast<bool>( start ) or
			                           static_cast<bool>( finish ) );
		}

		template<typename T>
		constexpr bool is_space( T const &value ) noexcept {
			return is_a( value, ' ' );
		}

		template<typename T>
		constexpr bool not_space( T const &value ) noexcept {
			return !is_space( value );
		}

		template<typename T>
		class is_crlf {
			using value_type = daw::traits::root_type_t<T>;
			std::list<value_type> m_last_values;
			size_t m_count;

			bool match( ) const noexcept {
				if( m_last_values.size( ) != 2 * m_count ) {
					return false;
				}
				bool result = true;
				for( auto it = m_last_values.begin( );
				     result and it != m_last_values.end( ); ++it ) {
					result = result and is_a( *( it ), '\r' );
					++it;
					result = result and is_a( *( it ), '\n' );
				}
				return result;
			}

		public:
			is_crlf( size_t count = 1 )
			  : m_last_values{}
			  , m_count{count} {}

			bool operator( )( value_type const &v ) {
				m_last_values.push_back( daw::move( v ) );
				while( m_last_values.size( ) > 4 ) {
					m_last_values.pop_front( );
				}
				return match( );
			}
		}; // is_cr

		namespace impl {
			template<typename Predicate>
			class negate_t {
				Predicate predicate;

			public:
				constexpr negate_t( Predicate pred )
				  : predicate{daw::move( pred )} {}

				template<typename... Args>
				constexpr bool operator( )( Args &&... args ) const {
					return !predicate( std::forward<Args>( args )... );
				}

				template<typename... Args>
				constexpr bool operator( )( Args &&... args ) {
					return !predicate( std::forward<Args>( args )... );
				}
			}; // negate_t
		}    // namespace impl

		template<typename Predicate>
		constexpr auto negate( Predicate predicate ) {
			return impl::negate_t<Predicate>{daw::move( predicate )};
		}

		template<typename T>
		struct matcher_t {
			using value_t = daw::traits::root_type_t<T>;

		private:
			std::vector<value_t> m_to_match;

		public:
			matcher_t( std::vector<value_t> to_match )
			  : m_to_match{daw::move( to_match )} {}

			template<typename ForwardIterator>
			find_result_t<ForwardIterator> operator( )( ForwardIterator first,
			                                            ForwardIterator last ) const {
				auto result = make_find_result( first, last );

				auto pos = std::search(
				  result.first, result.last, m_to_match.begin( ), m_to_match.end( ),
				  []( auto const &lhs, auto const &rhs ) { return is_a( lhs, rhs ); } );

				if( pos != result.last ) {
					result.last = pos;
					result.found = true;
				}
				return result;
			}
		}; // matcher_t

		template<typename Container>
		auto matcher( Container const &container ) {
			using value_t = daw::traits::root_type_t<decltype( *container.begin( ) )>;
			std::vector<value_t> values;
			std::copy( container.begin( ), container.end( ),
			           std::back_inserter( values ) );
			return matcher_t<value_t>{daw::move( values )};
		}

		template<typename T, size_t N>
		auto matcher( T ( &container )[N] ) {
			using value_t = daw::traits::root_type_t<T>;
			std::vector<value_t> values;
			std::copy_n( container, N, std::back_inserter( values ) );
			return matcher_t<value_t>{daw::move( values )};
		}

		///
		// Does the first range (first1, last1] start with the second range (first2,
		// last2]
		//
		template<typename ForwardIterator1, typename ForwardIterator2,
		         typename BinaryPredicate>
		constexpr bool starts_with( ForwardIterator1 first1, ForwardIterator1 last1,
		                            ForwardIterator2 first2, ForwardIterator2 last2,
		                            BinaryPredicate pred ) {
			while( first1 != last1 and first2 != last2 ) {
				if( !pred( *first1, *first2 ) ) {
					return false;
				}
				++first1;
				++first2;
			}
			return first2 == last2; // end of second item
		}

		template<typename ForwardIterator, typename Value>
		constexpr find_result_t<ForwardIterator>
		until_last_of( ForwardIterator first, ForwardIterator last,
		               Value const &value ) {
			auto result = until_value( first, last, value );
			bool found = static_cast<bool>( result );
			while( result and result.end( ) != last ) {
				auto tmp = until_value( std::next( result.end( ) ), last, value );
				result.found = static_cast<bool>( tmp );
				if( tmp ) {
					result.last = tmp.end( );
				}
			}
			result.found = found;
			return result;
		}

		template<typename ForwardIterator, typename Sequence>
		constexpr find_result_t<ForwardIterator>
		find_first_of_any( ForwardIterator first, ForwardIterator last,
		                   Sequence values ) {
			auto first_val = *std::begin( values );
			auto result = until_value( first, last, first_val );
			if( !result ) {
				return result;
			}
			auto last_first = result.first;
			auto new_end = std::next( result.first );
			while( new_end != last ) {
				auto seq_pos = std::next( std::begin( values ) );
				while( seq_pos != std::end( values ) and new_end != last and
				       *seq_pos == *new_end ) {
					if( *new_end == first_val ) {
						last_first = new_end;
					}
					++seq_pos;
					++new_end;
				}
				if( seq_pos == std::end( values ) ) {
					result.last = new_end;
					return result;
				}
				result = until_value( first, last, first_val );
			}
			return make_find_result( first, last, false );
		}
	} // namespace parser
} // namespace daw
