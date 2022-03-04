// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "defines.h"

#include "daw/json/daw_json_link.h"

#include <daw/daw_benchmark.h>
#include <daw/daw_read_file.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

namespace daw::cookbook_optional_values1 {
	namespace details {
		template<typename... Args>
		[[maybe_unused]] constexpr void
		is_unique_ptr_test_impl( std::unique_ptr<Args...> const & );

		template<typename T>
		using is_unique_ptr_test =
		  decltype( is_unique_ptr_test_impl( std::declval<T>( ) ) );

		template<typename T>
		constexpr bool is_unique_ptr_v = daw::is_detected_v<is_unique_ptr_test, T>;

	} // namespace details

	/**
	 * This is used for nullables who's member is a unique_ptr.
	 * @tparam T Type of value stored in unique_ptr
	 */
	template<typename T>
	struct UniquePtrConstructor {
		static_assert( not details::is_unique_ptr_v<T>,
		               "T should be the type contained in the unique_ptr" );

		inline constexpr std::unique_ptr<T> operator( )( ) const {
			return { };
		}

		template<typename Arg, typename... Args>
		inline std::unique_ptr<T> operator( )( Arg &&arg, Args &&...args ) const {
			return std::make_unique<T>( std::forward<Arg>( arg ),
			                            std::forward<Args>( args )... );
		}
	};

	struct MyOptionalStuff1 {
		std::optional<int> member0{ };
		std::string member1{ };
		std::unique_ptr<bool> member2{ };
	};

	bool operator==( MyOptionalStuff1 const &lhs, MyOptionalStuff1 const &rhs ) {
		bool result = lhs.member0 == rhs.member0;
		result = result and ( lhs.member1 == rhs.member1 );
		result = result and ( lhs.member2 and rhs.member2 and
		                      ( *lhs.member2 == *rhs.member2 ) );
		result = result or ( not rhs.member2 );
		return result;
	}
} // namespace daw::cookbook_optional_values1

namespace daw::json {
	template<>
	struct json_data_contract<daw::cookbook_optional_values1::MyOptionalStuff1> {
#if defined( __cpp_nontype_template_parameter_class )
		using type = json_member_list<
		  json_number_null<"member0", std::optional<int>>, json_string<"member1">,
		  json_bool_null<
		    "member2", std::unique_ptr<bool>, LiteralAsStringOpt::Never,
		    daw::cookbook_optional_values1::UniquePtrConstructor<bool>>>;
#else
		static constexpr char const member0[] = "member0";
		static constexpr char const member1[] = "member1";
		static constexpr char const member2[] = "member2";
		using type = json_member_list<
		  json_number_null<member0, std::optional<int>>, json_string<member1>,
		  json_bool_null<
		    member2, std::unique_ptr<bool>, LiteralAsStringOpt::Never,
		    daw::cookbook_optional_values1::UniquePtrConstructor<bool>>>;
#endif
		static inline auto to_json_data(
		  daw::cookbook_optional_values1::MyOptionalStuff1 const &value ) {
			return std::forward_as_tuple( value.member0, value.member1,
			                              value.member2 );
		}
	};
} // namespace daw::json

int main( int argc, char **argv )
#ifdef DAW_USE_JSON_EXCEPTIONS
  try
#endif
{
	if( argc <= 1 ) {
		puts( "Must supply path to cookbook_optional_values1.json file\n" );
		exit( EXIT_FAILURE );
	}
	auto data = *daw::read_file( argv[1] );
	puts( "Original" );
	puts( std::string( data.data( ), data.size( ) ).c_str( ) );

	std::vector<daw::cookbook_optional_values1::MyOptionalStuff1> stuff =
	  daw::json::from_json_array<
	    daw::cookbook_optional_values1::MyOptionalStuff1>( data );

	test_assert( stuff.size( ) == 2, "Unexpected size" );
	test_assert( not stuff.front( ).member2, "Unexpected value" );
	test_assert( not stuff.back( ).member0, "Unexpected value" );

	std::string const str = daw::json::to_json_array( stuff );

	puts( "After" );
	puts( str.c_str( ) );

	std::vector<daw::cookbook_optional_values1::MyOptionalStuff1> stuff2 =
	  daw::json::from_json_array<
	    daw::cookbook_optional_values1::MyOptionalStuff1>( str );

	test_assert( stuff == stuff2, "Unexpected round trip error" );
	return 0;
}
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << std::endl;
	exit( 1 );
}
