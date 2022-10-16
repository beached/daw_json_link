// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/json/daw_from_json.h>
#include <daw/json/daw_json_link_types.h>
#include <daw/json/daw_to_json.h>

namespace {

	struct Message {
		explicit Message( const double value ) {

			m_value = value;
		}

		bool operator==( const Message &rhs ) const {
			return this->m_value == rhs.m_value;
		}

		double m_value{ };
	};
} // namespace

namespace daw::json {
	template<>
	struct json_data_contract<Message> {
		static constexpr const char number[] = "number";
		using type = json_member_list<json_number<number, double>>;
		static inline auto to_json_data( const Message &value ) {
			return std::forward_as_tuple( value.m_value );
		}
	};
} // namespace daw::json

void test_roundtrip( const double value ) {

	Message hm( value );

	const auto json = daw::json::to_json( hm );
	const auto recovered = daw::json::from_json<Message>( json );
	if( hm != recovered ) {
		std::abort( );
	}
}

int main( )
#ifdef DAW_USE_EXCEPTIONS
  try
#endif
{
	// this is ok
	test_roundtrip( 1000000 - 1 );

	// also ok
	test_roundtrip( -( 1000000 - 1 ) );

	// this fails with "Invalid Number started, expected a "0123456789-"
	test_roundtrip( 1'000'000 );

}
#ifdef DAW_USE_EXCEPTIONS
catch( daw::json::json_exception const &jex ) {
	std::cerr << "Exception thrown by parser: " << jex.reason( ) << '\n';
	exit( 1 );
} catch( std::exception const &ex ) {
	std::cerr << "Unknown exception thrown during testing: " << ex.what( )
	          << '\n';
	exit( 1 );
} catch( ... ) {
	std::cerr << "Unknown exception thrown during testing\n";
	throw;
}
#endif
