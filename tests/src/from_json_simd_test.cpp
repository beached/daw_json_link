// Copyright (c) Darrell Wright
// Distributed under the Boost Software License, Version 1.0.
#include <cstdlib>
#include <daw/json/daw_from_json_simd.h>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>
#if defined( DAW_JSON_HAS_FROM_JSON_SIMD )
struct scan_string_length {
	std::size_t operator( )( std::string_view v ) const {
		return v.size( );
	}
};
struct scan_nullable_point {
	int x;
	std::optional<int> y;
};
struct scan_ordered_point {
	int y;
};
struct scan_exact_point {
	double x;
	double y;
};
struct scan_record {
	int id;
	std::optional<double> value;
	std::vector<std::tuple<double, double>> points;
};
inline std::vector<int> scan_construction_log;
template<int Tag>
struct scan_tracked_number {
	int operator( )( int value ) const {
		scan_construction_log.push_back( Tag );
		return value;
	}
};
class scan_constructed {
	int sum_;

public:
	scan_constructed( int first, int second )
	  : sum_( first * 10 + second ) {}
	int sum( ) const {
		return sum_;
	}
};
struct scan_builder {
	scan_constructed operator( )( int first, int second ) const {
		scan_construction_log.push_back( 3 );
		return scan_constructed( first, second );
	}
};
struct scan_pinned {
	int value;
	explicit scan_pinned( int v )
	  : value( v ) {}
	scan_pinned( scan_pinned const & ) = delete;
	scan_pinned( scan_pinned && ) = delete;
};
struct scan_pinned_outer {
	scan_pinned child;
	int tail;
};
struct scan_lifetime {
	inline static int live = 0;
	int value;
	explicit scan_lifetime( int v )
	  : value( v ) {
		++live;
	}
	scan_lifetime( scan_lifetime &&other ) noexcept
	  : value( other.value ) {
		++live;
	}
	scan_lifetime( scan_lifetime const & ) = delete;
	~scan_lifetime( ) {
		--live;
	}
};
struct scan_lifetime_converter {
	scan_lifetime operator( )( std::string_view value ) const {
		return scan_lifetime( value.empty( ) ? 0 : value.front( ) - '0' );
	}
};
struct scan_throwing_record {
	scan_lifetime first;
	scan_lifetime second;
};
struct scan_throwing_builder {
	scan_throwing_record operator( )( scan_lifetime first,
	                                  scan_lifetime second ) const {
		(void)first;
		(void)second;
		throw 42;
	}
};
struct scan_maps {
	std::map<std::string, int> by_name;
	std::map<std::int64_t, std::string> by_id;
};
struct scan_meters {
	int value;
};
struct scan_samples {
	std::vector<int> values;
};
struct scan_ids {
	std::map<std::uint64_t, int> ids;
};
struct scan_aliases {
	scan_meters meters;
	scan_samples samples;
	scan_ids ids;
};
static_assert( not std::is_aggregate_v<scan_constructed> );
static_assert( daw::json::is_pinned_type_v<scan_pinned_outer> );

namespace daw::json {
	template<>
	struct json_data_contract<scan_maps> {
		static constexpr char by_name[] = "by_name", by_id[] = "by_id";
		using type = json_member_list<
		  json_key_value<by_name, std::map<std::string, int>, int>,
		  json_key_value<by_id, std::map<std::int64_t, std::string>, std::string,
		                 json_number_no_name<
		                   std::int64_t, options::number_opt(
		                                   options::LiteralAsStringOpt::Always )>>>;
	};
	template<>
	struct json_data_contract<scan_meters> {
		using type = json_type_alias<json_number_no_name<int>>;
	};
	template<>
	struct json_data_contract<scan_samples> {
		using type = json_type_alias<json_array_no_name<int>>;
	};
	template<>
	struct json_data_contract<scan_ids> {
		using type = json_type_alias<json_key_value_no_name<
		  std::map<std::uint64_t, int>, int,
		  json_number_no_name<std::uint64_t,
		                      options::number_opt(
		                        options::LiteralAsStringOpt::Always )>>>;
	};
	template<>
	struct json_data_contract<scan_aliases> {
		static constexpr char meters[] = "meters", samples[] = "samples",
		                      ids[] = "ids";
		using type = json_member_list<json_class<meters, scan_meters>,
		                              json_class<samples, scan_samples>,
		                              json_class<ids, scan_ids>>;
	};
	template<>
	struct json_data_contract<scan_ordered_point> {
		using type = json_tuple_member_list<json_tuple_member<1, int>>;
	};
	template<>
	struct json_data_contract<scan_constructed> {
		using constructor_t = scan_builder;
		static constexpr char first[] = "first", second[] = "second";
		using type = json_member_list<
		  json_number<first, int, number_opts_def, scan_tracked_number<1>>,
		  json_number<second, int, number_opts_def, scan_tracked_number<2>>>;
	};
	template<>
	struct json_data_contract<scan_pinned> {
		using force_aggregate_construction = void;
		static constexpr char value[] = "value";
		using type = json_member_list<json_number<value, int>>;
	};
	template<>
	struct json_data_contract<scan_pinned_outer> {
		using force_aggregate_construction = void;
		static constexpr char child[] = "child", tail[] = "tail";
		using type =
		  json_member_list<json_class<child, scan_pinned>, json_number<tail, int>>;
	};
	template<>
	struct json_data_contract<scan_throwing_record> {
		using constructor_t = scan_throwing_builder;
		static constexpr char first[] = "first", second[] = "second";
		using type = json_member_list<
		  json_custom<first, scan_lifetime, scan_lifetime_converter, use_default,
		              options::json_custom_opt(
		                options::JsonCustomTypes::Literal )>,
		  json_custom<second, scan_lifetime, scan_lifetime_converter, use_default,
		              options::json_custom_opt(
		                options::JsonCustomTypes::Literal )>>;
	};

	template<>
	struct json_data_contract<scan_nullable_point> {
		using type =
		  json_tuple_member_list<int, json_number_null_no_name<std::optional<int>>>;
		static auto to_json_data( scan_nullable_point const &v ) {
			return std::tie( v.x, v.y );
		}
	};
	template<>
	struct json_data_contract<scan_exact_point> {
		using exact_class_mapping = void;
		using type = json_tuple_member_list<double, double>;
		static auto to_json_data( scan_exact_point const &v ) {
			return std::tie( v.x, v.y );
		}
	};
	template<>
	struct json_data_contract<scan_record> {
		static constexpr char id[] = "id", value[] = "value", points[] = "points";
		using type =
		  json_member_list<json_number<id, int>,
		                   json_number_null<value, std::optional<double>>,
		                   json_array<points, std::tuple<double, double>>>;
		static auto to_json_data( scan_record const &v ) {
			return std::tie( v.id, v.value, v.points );
		}
	};
} // namespace daw::json
void check( bool v ) {
	if( !v ) {
		std::abort( );
	}
}
template<class T>
void reject( std::string const &text ) {
	bool caught = false;
	try {
		(void)daw::json::experimental::from_json_simd<T>( text );
	} catch( daw::json::json_exception const & ) { caught = true; }
	check( caught );
}
template<class T>
void reject_at( std::string const &text, std::size_t offset ) {
	bool caught = false;
	try {
		(void)daw::json::experimental::from_json_simd<T>( text );
	} catch( daw::json::json_exception const &je ) {
		caught = je.parse_location( ) == text.data( ) + offset;
	}
	check( caught );
}

int main( ) {
	using daw::json::experimental::from_json_simd;
	check( from_json_simd<scan_ordered_point>( "[9,7]" ).y == 7 );
	// Members are parsed as they are encountered, then passed to the result
	// constructor in mapping order.
	constexpr char constructed_doc[] =
	  R"({"second":2,"unknown":{"a":[1,2]},"first":1})";
	scan_construction_log.clear( );
	auto const constructed_scalar =
	  daw::json::from_json<scan_constructed>( constructed_doc );
	check( ( scan_construction_log == std::vector<int>{ 1, 2, 3 } ) );
	scan_construction_log.clear( );
	auto const constructed = from_json_simd<scan_constructed>( constructed_doc );
	check( constructed.sum( ) == constructed_scalar.sum( ) );
	check( ( scan_construction_log == std::vector<int>{ 2, 1, 3 } ) );
	auto const pinned =
	  from_json_simd<scan_pinned_outer>( R"({"tail":9,"child":{"value":7}})" );
	check( pinned.child.value == 7 and pinned.tail == 9 );
	auto const scalar_pinned = daw::json::from_json<scan_pinned_outer>(
	  R"({"tail":9,"child":{"value":7}})" );
	check( scalar_pinned.child.value == pinned.child.value );

	// Every constructed slot is destroyed when later grammar or result
	// construction throws.
	scan_lifetime::live = 0;
	reject<scan_throwing_record>( R"({"first":1 "second":2})" );
	check( scan_lifetime::live == 0 );
	bool constructor_threw = false;
	try {
		(void)from_json_simd<scan_throwing_record>( R"({"second":2,"first":1})" );
	} catch( int value ) { constructor_threw = value == 42; }
	check( constructor_threw and scan_lifetime::live == 0 );

	// A missing member or tuple element is reported at the opening of the
	// value it belongs to
	reject_at<std::vector<scan_record>>(
	  R"([{"id":1,"points":[]}, {"points":[]}])", 23 );
	reject_at<scan_record>( R"({"id":1,"points":[[1,2],[3]]})", 24 );

	// Index growth must not invalidate token ranges retained for parent scopes.
	std::string many = "[";
	for( int n = 0; n < 512; ++n ) {
		if( n != 0 ) {
			many += ',';
		}
		many += R"({"points":[[1,2],[3,4]],"id":)" + std::to_string( n ) + "}";
	}
	many += ']';
	auto const records = from_json_simd<std::vector<scan_record>>( many );
	check( records.size( ) == 512 );
	for( std::size_t n = 0; n < records.size( ); ++n ) {
		check( records[n].id == static_cast<int>( n ) and
		       std::get<1>( records[n].points[1] ) == 4 );
	}
	for( auto const length : { 0, 63, 64, 255, 256, 1023 } ) {
		auto const text = std::string( static_cast<std::size_t>( length ), 'a' );
		auto const document = std::string( "[\"" ) + text + "\\n\",\"\"]";
		auto const strings = from_json_simd<std::vector<std::string>>( document );
		check( strings.size( ) == 2 and strings[0] == text + "\n" and
		       strings[1].empty( ) );
	}
	auto const deep = std::string( R"({"id":3,"points":[],"unknown":)" ) +
	                  std::string( 512, '[' ) + "invalid" +
	                  std::string( 512, ']' ) + "}";
	check( from_json_simd<scan_record>( deep ).id == 3 );
	for( auto const &bad :
	     { R"({"id":1,"points":[],"unknown":[{]})",
	       R"({"id":1 "points":[]})",
	       R"({"id":1,"points":[[1,2] [3,4]]})",
	       R"({"id":1,"points":[],"unknown":"unterminated})" } ) {
		reject<scan_record>( bad );
	}
	// Structural validation covers skipped values before typed parsing.
	for( auto const &bad : { R"({"unknown":{"a" 1},"id":2,"points":[]})",
	                         R"({"unknown":[1 2],"id":2,"points":[]})",
	                         R"({"unknown":[,1],"id":2,"points":[]})",
	                         R"({"unknown":{1:2},"id":2,"points":[]})",
	                         R"({"unknown":[1,],"id":2 "points":[]})" } ) {
		reject<scan_record>( bad );
	}
	bool end_rejected = false;
	try {
		(void)from_json_simd<int>(
		  "1 2",
		  daw::json::options::parse_flags<
		    daw::json::options::MustVerifyEndOfDataIsValid::yes> );
	} catch( daw::json::json_exception const & ) { end_rejected = true; }
	check( end_rejected );
	check( from_json_simd<int>(
	         "1 2",
	         daw::json::options::parse_flags<
	           daw::json::options::MustVerifyEndOfDataIsValid::no> ) == 1 );
	check(
	  from_json_simd<scan_record>( R"({"points":[],"id":4})",
	                               daw::json::options::parse_flags<
	                                 daw::json::options::CheckedParseMode::no> )
	    .id == 4 );

	using custom =
	  daw::json::json_custom_no_name<std::size_t, scan_string_length>;
	check( from_json_simd<custom>( R"("timestamp")" ) == 9 );
	auto v = from_json_simd<scan_record>( R"({"points":[[1,2],[3,4]],"id":7})" );
	check( v.id == 7 && !v.value && v.points.size( ) == 2 &&
	       std::get<1>( v.points[1] ) == 4 );
	check( from_json_simd<scan_record>( R"({"id":1,"value":null,"points":[]})" )
	         .value == std::nullopt );
	reject<scan_record>( R"({"points":[]})" );
	reject<int>( "1.25" );
	reject<int>( "1e2" );
	reject<int>( "01" );
	reject<unsigned>( "01" );
	reject<int>( "-" );
	reject<unsigned>( "-1" );
	reject<bool>( "truex" );
	reject<double>( "1e" );
	reject<std::string>( "1" );
	reject<std::tuple<int, int>>( "[1]" );
	reject<scan_exact_point>( "[1,2,3]" );
	reject<scan_exact_point>( "[1]" );
	bool exact_rejected = false;
	try {
		(void)from_json_simd<scan_record>(
		  R"({"id":1,"points":[],"extra":0})",
		  daw::json::options::parse_flags<
		    daw::json::options::UseExactMappingsByDefault::yes> );
	} catch( daw::json::json_exception const & ) { exact_rejected = true; }
	check( exact_rejected );
	check( from_json_simd<scan_exact_point>( "[1,2]" ).y == 2 );
	check( from_json_simd<std::string>( R"("a\nb")" ) == "a\nb" );
	check( from_json_simd<std::vector<std::vector<int>>>(
	         "[[1,2],[],[3]]" )[2][0] == 3 );
	reject<std::vector<int>>( "[1 2]" );
	reject<std::vector<int>>( "[1,2" );
	for( auto padding : { 0, 55, 63, 64, 250, 255, 256, 511 } ) {
		std::string pad( static_cast<std::size_t>( padding ), 'x' );
		auto text =
		  std::string( R"({"unknown":{"s":")" ) + pad +
		  R"(\"[]","nested":[{},[]]},"points":[[1.5,2.5,],[3,4]],"value":2e2,"id":9,})";
		auto r = from_json_simd<scan_record>( text );
		check( r.id == 9 && r.value == 200 && r.points.size( ) == 2 );
	}
#if defined( DAW_JSON_HAS_FROM_JSON_SIMD )
	// The explicit experiment gives skipped literals structural validation only.
	check( from_json_simd<scan_record>(
	         R"({"unknown":[invalid,1e],"id":2,"points":[]})" )
	         .id == 2 );
#endif
	auto t = from_json_simd<scan_nullable_point>( "[1]" );
	check( !t.y );
	check( std::get<0>( from_json_simd<std::tuple<int, int>>( "[1,2,3]" ) ) ==
	       1 );

	// json_key_value members are read from the index rather than falling back
	static_assert( daw::json::json_details::simd_parse::forward_supported<
	               daw::json::json_details::json_deduced_type<scan_maps>>( ) );
	for( std::string const maps_doc :
	     { R"({"by_name":{"a":1,"b\"c":2,"a":3},"by_id":{"7":"x","-3":"y\n"}})",
	       R"({"by_name":{},"by_id":{}})",
	       R"({"by_id":{"1":"z",},"by_name":{"q":4}})" } ) {
		auto const maps = from_json_simd<scan_maps>( maps_doc );
		auto const scalar_maps = daw::json::from_json<scan_maps>( maps_doc );
		check( maps.by_name == scalar_maps.by_name );
		check( maps.by_id == scalar_maps.by_id );
	}
	check(
	  from_json_simd<scan_maps>( R"({"by_name":{"b\"c":2},"by_id":{"-3":"y"}})" )
	    .by_name.at( "b\"c" ) == 2 );
	check( from_json_simd<scan_maps>( R"({"by_name":{},"by_id":{"-3":"y"}})" )
	         .by_id.at( -3 ) == "y" );
	reject<scan_maps>( R"({"by_name":{"a" 1},"by_id":{}})" );
	reject<scan_maps>( R"({"by_name":{"a":1 "b":2},"by_id":{}})" );
	reject<scan_maps>( R"({"by_name":{1:1},"by_id":{}})" );
	reject<scan_maps>( R"({"by_name":{},"by_id":{"x":"y"}})" );
	reject<scan_maps>( R"({"by_name":[],"by_id":{}})" );

	// A json_type_alias goes forward when the member it aliases does
	{
		namespace sp = daw::json::json_details::simd_parse;
		using daw::json::json_details::json_deduced_type;
		static_assert( sp::forward_supported<json_deduced_type<scan_meters>>( ) );
		static_assert( sp::forward_supported<json_deduced_type<scan_samples>>( ) );
		static_assert( sp::forward_supported<json_deduced_type<scan_ids>>( ) );
		static_assert( sp::forward_supported<json_deduced_type<scan_aliases>>( ) );
	}
	check( from_json_simd<scan_meters>( "42" ).value == 42 );
	check( ( from_json_simd<scan_samples>( "[3,1,2]" ).values ==
	         std::vector<int>{ 3, 1, 2 } ) );
	check( from_json_simd<scan_ids>( R"({"18446744073709551615":1,"0":2})" )
	         .ids.at( 18446744073709551615ULL ) == 1 );
	for( std::string const aliases_doc :
	     { R"({"meters":5,"samples":[1,2],"ids":{"9":3,"4":5}})",
	       R"({"ids":{},"samples":[],"meters":-1})" } ) {
		auto const aliases = from_json_simd<scan_aliases>( aliases_doc );
		auto const scalar_aliases =
		  daw::json::from_json<scan_aliases>( aliases_doc );
		check( aliases.meters.value == scalar_aliases.meters.value );
		check( aliases.samples.values == scalar_aliases.samples.values );
		check( aliases.ids.ids == scalar_aliases.ids.ids );
	}
	reject<scan_meters>( R"("42")" );
	reject<scan_samples>( R"({"values":[1]})" );
	reject<scan_ids>( R"({"x":1})" );

	// Preserve daw::string_view's runtime zero-termination information when
	// selecting the SIMD parse policy.
	std::string const zero_terminated_numbers = "[10,20,30]";
	auto const zero_terminated_view = daw::string_view( zero_terminated_numbers );
	check( zero_terminated_view.is_zero_terminated( ) ==
	       daw::ZeroTerminated::Yes );
	check( from_json_simd<std::vector<int>>( zero_terminated_view )[2] == 30 );

	// A root number can end exactly at a non-terminated view's bound and must
	// continue to use the bounded number parser.
	char const bounded_number[] = { '4', '2' };
	check( from_json_simd<int>( daw::string_view( bounded_number, 2 ) ) == 42 );
}

#else
int main( ) {}
#endif
