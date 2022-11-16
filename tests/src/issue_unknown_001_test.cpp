// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//
#include <daw/json/daw_json_iterator.h>
#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

struct arg_t {
	std::string channel;
	std::string instId;
}; // arg_t

struct data_element_t {
	std::vector<std::vector<std::string>> asks;
	std::vector<std::vector<std::string>> bids;
	std::string ts;
	int64_t checksum;
}; // data_element_t

struct root_object_t {
	arg_t arg;
	std::string action;
	std::vector<data_element_t> data;
}; // root_object_t

namespace daw::json {
	template<>
	struct json_data_contract<arg_t> {
		static constexpr char const mem_channel[] = "channel";
		static constexpr char const mem_instId[] = "instId";
		using type =
		  json_member_list<json_string<mem_channel>, json_string<mem_instId>>;

		static inline auto to_json_data( arg_t const &value ) {
			return std::forward_as_tuple( value.channel, value.instId );
		}
	};

	template<>
	struct json_data_contract<data_element_t> {
		static constexpr char const mem_asks[] = "asks";
		static constexpr char const mem_bids[] = "bids";
		static constexpr char const mem_ts[] = "ts";
		static constexpr char const mem_checksum[] = "checksum";
		using type = json_member_list<
		  json_array<mem_asks,
		             json_array_no_name<std::string, std::vector<std::string>>,
		             std::vector<std::vector<std::string>>>,
		  json_array<mem_bids,
		             json_array_no_name<std::string, std::vector<std::string>>,
		             std::vector<std::vector<std::string>>>,
		  json_string<mem_ts>, json_number<mem_checksum, int64_t>>;

		static inline auto to_json_data( data_element_t const &value ) {
			return std::forward_as_tuple( value.asks, value.bids, value.ts,
			                              value.checksum );
		}
	};

	template<>
	struct json_data_contract<root_object_t> {
		static constexpr char const mem_arg[] = "arg";
		static constexpr char const mem_action[] = "action";
		static constexpr char const mem_data[] = "data";
		using type =
		  json_member_list<json_class<mem_arg, arg_t>, json_string<mem_action>,
		                   json_array<mem_data, json_class_no_name<data_element_t>,
		                              std::vector<data_element_t>>>;

		static inline auto to_json_data( root_object_t const &value ) {
			return std::forward_as_tuple( value.arg, value.action, value.data );
		}
	};
} // namespace daw::json

static constexpr daw::string_view json_doc = R"json(
{
	"arg": {
		"channel": "books50-l2-tbt",
		"instId": "ETH-USDT-221118"
	},
	"action": "snapshot",
	"data": [{
		"asks": [
			["1209.19", "1", "0", "1"],
			["1209.2", "14", "0", "1"],
			["1209.33", "116", "0", "1"],
			["1209.62", "36", "0", "1"],
			["1209.72", "90", "0", "1"],
			["1209.83", "91", "0", "1"],
			["1209.94", "241", "0", "1"],
			["1209.98", "9", "0", "1"],
			["1210.2", "14", "0", "1"],
			["1210.54", "351", "0", "1"],
			["1210.89", "187", "0", "1"],
			["1211.14", "476", "0", "1"],
			["1211.18", "6", "0", "1"],
			["1211.67", "375", "0", "1"],
			["1212", "100", "0", "1"],
			["1213.48", "144", "0", "2"],
			["1213.49", "375", "0", "1"],
			["1213.7", "20", "0", "1"],
			["1214.65", "32", "0", "1"],
			["1215.36", "1174", "0", "1"],
			["1216.26", "213", "0", "1"],
			["1217.8", "90", "0", "1"],
			["1218.32", "750", "0", "1"],
			["1218.42", "1228", "0", "1"],
			["1218.67", "205", "0", "1"],
			["1220.21", "90", "0", "1"],
			["1221.09", "209", "0", "1"],
			["1221.3", "727", "0", "1"],
			["1222.63", "820", "0", "1"],
			["1225", "1", "0", "1"],
			["1226.25", "817", "0", "1"],
			["1226.99", "473", "0", "1"],
			["1229.87", "819", "0", "1"],
			["1230", "87", "0", "1"],
			["1232.29", "237", "0", "1"],
			["1233.16", "1", "0", "1"],
			["1233.5", "815", "0", "1"],
			["1235", "57", "0", "1"],
			["1237.12", "810", "0", "1"],
			["1241.7", "123", "0", "1"],
			["1249.03", "1", "0", "1"],
			["1250.74", "118", "0", "1"],
			["1256.44", "19933", "0", "1"],
			["1258.68", "1", "0", "1"],
			["1259.01", "8", "0", "1"],
			["1260", "7", "0", "1"],
			["1264.89", "1", "0", "1"],
			["1266.66", "2", "0", "1"],
			["1269", "8", "0", "1"],
			["1270.6", "10", "0", "1"]
		],
		"bids": [
			["1208.18", "1", "0", "1"],
			["1208.01", "115", "0", "1"],
			["1207.99", "14", "0", "1"],
			["1207.33", "9", "0", "1"],
			["1207.26", "4", "0", "1"],
			["1207.2", "14", "0", "1"],
			["1207.17", "241", "0", "1"],
			["1206.78", "36", "0", "1"],
			["1206.7", "351", "0", "1"],
			["1206.35", "476", "0", "1"],
			["1205.86", "187", "0", "1"],
			["1205.65", "108", "0", "1"],
			["1205.08", "6", "0", "1"],
			["1204.24", "375", "0", "1"],
			["1202.43", "465", "0", "2"],
			["1202.09", "1234", "0", "1"],
			["1201.54", "1217", "0", "1"],
			["1201.43", "1", "0", "1"],
			["1201", "58", "0", "1"],
			["1200.75", "208", "0", "1"],
			["1200", "165", "0", "6"],
			["1198.37", "90", "0", "1"],
			["1198.34", "213", "0", "1"],
			["1197.6", "750", "0", "1"],
			["1196", "15", "0", "1"],
			["1195.92", "211", "0", "1"],
			["1195.7", "32", "0", "1"],
			["1195", "1", "0", "1"],
			["1192.85", "699", "0", "1"],
			["1192.76", "842", "0", "1"],
			["1189.66", "490", "0", "1"],
			["1189.14", "842", "0", "1"],
			["1185.74", "237", "0", "1"],
			["1185.57", "1", "0", "1"],
			["1185.52", "844", "0", "1"],
			["1183.33", "2", "0", "1"],
			["1182", "2", "0", "1"],
			["1181.89", "847", "0", "1"],
			["1180", "2", "0", "2"],
			["1178.27", "856", "0", "1"],
			["1175", "5", "0", "1"],
			["1171", "30", "0", "2"],
			["1170", "6", "0", "1"],
			["1169.7", "1", "0", "1"],
			["1166.88", "117", "0", "1"],
			["1158.95", "21596", "0", "1"],
			["1158.71", "1", "0", "1"],
			["1158.58", "119", "0", "1"],
			["1154.74", "2", "0", "1"],
			["1153.84", "1", "0", "1"]
		],
		"ts": "1668612447909",
		"checksum": -414529088
	}]
}
)json";

int main( ) {
	auto val = daw::json::from_json<root_object_t>( json_doc );
	auto json_doc2 = daw::json::to_json( val );
	auto val2 = daw::json::from_json<root_object_t>( json_doc2 );
	assert( val.data[0].checksum == val2.data[0].checksum );
	return 0;
}
