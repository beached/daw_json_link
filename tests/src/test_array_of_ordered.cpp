// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

struct Fixed8JsonConverter {
	double operator( )( std::string_view sv ) const {
		return stod( std::string( sv ) );
	}

	template<typename OutputIterator>
	constexpr OutputIterator operator( )( OutputIterator _it, double _f ) const {
		return daw::json::utils::copy_to_iterator( _it, std::to_string( _f ) );
	}
};

template<JSONNAMETYPE name>
using json_fixed8 = daw::json::json_custom<name, double, Fixed8JsonConverter,
                                           Fixed8JsonConverter>;

struct Change {
	double rate;
	double amount;
};

namespace daw::json {
	template<>
	struct json_data_contract<Change> {
		using type =
		  json_ordered_member_list<json_fixed8<no_name>, json_fixed8<no_name>>;
	};
} // namespace daw::json

class DepthUpdateJson {
public:
	std::int64_t time;       // E
	std::string pairName;    // s
	std::int64_t idTo;       // u
	std::int64_t idFrom;     // U
	std::vector<Change> bid; // b
	std::vector<Change> ask; // a
};

namespace daw::json {
	template<>
	struct json_data_contract<DepthUpdateJson> {
		static constexpr char const E[] = "E";
		static constexpr char const s[] = "s";
		static constexpr char const u[] = "u";
		static constexpr char const U[] = "U";
		static constexpr char const b[] = "b";
		static constexpr char const a[] = "a";
		using type = json_member_list<json_number<E, std::int64_t>, json_string<s>,
		                              json_number<u, std::int64_t>,
		                              json_number<U, std::int64_t>,
		                              json_array<b, Change>, json_array<a, Change>>;
	};
} // namespace daw::json

int main( ) {
	try {
		std::string testJson1 =
		  R"({"e":"depthUpdate","E":1609884707320,"s":"BTCBUSD","U":2544556159,"u":2544556219,"a":[["34198.19000000","0.00000000"],["34198.23000000","0.00000000"],["34198.25000000","0.00000000"],["34198.27000000","0.00000000"],["34198.30000000","0.00000000"],["34198.32000000","0.00958500"],["34198.40000000","0.01232200"],["34198.41000000","0.01000000"],["34198.87000000","0.00000000"],["34199.12000000","0.00000000"],["34199.16000000","0.00000000"],["34199.42000000","0.00000000"],["34200.25000000","0.00000000"],["34200.71000000","0.03199900"],["34201.27000000","0.03100000"],["34201.62000000","0.00000000"],["34202.58000000","0.00000000"],["34204.45000000","0.00952700"],["34207.64000000","0.00000000"],["34207.74000000","0.00000000"],["34209.77000000","0.00000000"],["34209.81000000","0.20400000"],["34225.94000000","0.20200000"],["34226.60000000","0.91050000"],["34236.08000000","0.30000000"]],"b":[["34198.31000000","0.00000000"],["34196.54000000","0.00453200"],["34193.34000000","0.00000000"],["34189.89000000","0.00000000"],["34188.82000000","0.00000000"],["34185.32000000","0.00000000"],["34184.84000000","0.06350200"],["34184.83000000","0.20000000"],["34180.61000000","0.08622700"],["34180.60000000","0.00000000"],["34180.59000000","0.19200000"],["34180.02000000","0.00000000"],["34180.01000000","0.00000000"],["34176.88000000","0.00000000"],["34166.48000000","0.00000000"],["34166.47000000","0.00000000"],["34159.85000000","0.03317500"],["34159.24000000","0.09394900"],["34158.29000000","1.00000000"],["34154.86000000","0.00000000"]]})";

		std::string testJson2 =
		  R"({"e":"depthUpdate","E":1609884707320,"s":"BTCBUSD","U":2544556159,"u":2544556219,"b":[["34198.31000000","0.00000000"],["34196.54000000","0.00453200"],["34193.34000000","0.00000000"],["34189.89000000","0.00000000"],["34188.82000000","0.00000000"],["34185.32000000","0.00000000"],["34184.84000000","0.06350200"],["34184.83000000","0.20000000"],["34180.61000000","0.08622700"],["34180.60000000","0.00000000"],["34180.59000000","0.19200000"],["34180.02000000","0.00000000"],["34180.01000000","0.00000000"],["34176.88000000","0.00000000"],["34166.48000000","0.00000000"],["34166.47000000","0.00000000"],["34159.85000000","0.03317500"],["34159.24000000","0.09394900"],["34158.29000000","1.00000000"],["34154.86000000","0.00000000"]],"a":[["34198.19000000","0.00000000"],["34198.23000000","0.00000000"],["34198.25000000","0.00000000"],["34198.27000000","0.00000000"],["34198.30000000","0.00000000"],["34198.32000000","0.00958500"],["34198.40000000","0.01232200"],["34198.41000000","0.01000000"],["34198.87000000","0.00000000"],["34199.12000000","0.00000000"],["34199.16000000","0.00000000"],["34199.42000000","0.00000000"],["34200.25000000","0.00000000"],["34200.71000000","0.03199900"],["34201.27000000","0.03100000"],["34201.62000000","0.00000000"],["34202.58000000","0.00000000"],["34204.45000000","0.00952700"],["34207.64000000","0.00000000"],["34207.74000000","0.00000000"],["34209.77000000","0.00000000"],["34209.81000000","0.20400000"],["34225.94000000","0.20200000"],["34226.60000000","0.91050000"],["34236.08000000","0.30000000"]])";

		auto parsed = daw::json::from_json<DepthUpdateJson>( testJson1 );
		std::cout << "";
	} catch( daw::json::json_exception const &e ) {
		std::cout << "formatted: " << to_formatted_string( e ) << "\n\n";
		std::cout << "daw error: " << e.reason( ) << " near: '"
		          << e.parse_location( ) << "'" << std::endl;
	}
}
