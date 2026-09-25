// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <daw/daw_ensure.h>
#include <daw/daw_print.h>
#include <daw/json/daw_json_link.h>
#include <daw/json/daw_json_writer.h>

#include <array>
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

using daw::json::reflect;

struct X {
	[[= reflect.map_as<daw::json::json_number<"member1", int>>]] int m1;

	[[= reflect.rename<"member2">]] int m2;
};
static_assert(
  daw::json::refl_details::get_non_ignored_reflectible_members<X>( ).size( ) ==
  2 );

struct[[= reflect]] Y {
	X m0;
	std::string m1;
};

struct[[= reflect]] Z {
	std::map<std::string, int> kv;
};

struct[[= reflect]] Foo {
	std::optional<Y> m0;
	std::vector<X> m1;
	std::shared_ptr<int> m2;
};

class[[= reflect]] A {
	mutable int counter = 0;

public:
	std::string value = "";

	explicit A( ) = default;

	explicit A( std::string v )
	  : value( std::move( v ) ) {}

	int foo( ) const {
		return ++counter;
	}
};

struct NoRefl {
	int x = 55;
};

struct[[= reflect]] NumberHalf {
	constexpr auto operator( )( auto y ) const {
		return y / 2;
	}
};

template<>
struct daw::json::json_data_contract<NoRefl> {
	using type =
	  json_member_list<json_number<"x", int, number_opts_def, NumberHalf>>;

	static constexpr auto to_json_data( NoRefl const &nr ) {
		return std::tuple{ nr.x * 2 };
	}
};

enum class EFoo { AChoo, BlessYou };

struct[[= reflect]] EnumMember {
	EFoo foo;
};

struct EnumMemberString {
	[[= reflect.enum_string]] EFoo foo;
};

// Annotating the enum type itself maps it as a string everywhere it is used
enum class [[= reflect.enum_string]] EBar { A, B };

struct[[= reflect]] EnumTypeString {
	EBar bar;
	std::vector<EBar> bars;
};

enum class [[= reflect.map_as<daw::json::json_number_no_name<int>>]] EBaz {
	A = 1,
	B = 2
};

struct[[= reflect]] EnumTypeNumber {
	EBaz baz;
};

static_assert( daw::json::refl_details::EnumStringAnnotated<EBar> );
static_assert( not daw::json::refl_details::EnumStringAnnotated<EFoo> );
static_assert(
  daw::json::reason_message(
    daw::json::ErrorReason::CouldNotFindEnumeratorForValue ) ==
  "The enum_string mapping requires that values passed map to an enumerator "
  "in the enum definition" );

// Compile time parsing of a type annotated enum
static_assert( daw::json::from_json<EBar>( R"json("A")json" ) == EBar::A );
static_assert( daw::json::from_json<EBar>( R"json("B")json" ) == EBar::B );
static_assert(
  daw::json::from_json<EnumTypeString>( R"json({"bar":"B","bars":[]})json" )
    .bar == EBar::B );
static_assert(
  daw::json::from_json<EnumTypeNumber>( R"json({"baz":2})json" ).baz ==
  EBaz::B );
// Compile time parsing of a member annotated enum
static_assert( daw::json::from_json<EnumMemberString>(
                 R"json({"foo":"BlessYou"})json" )
                 .foo == EFoo::BlessYou );

template<typename T>
constexpr bool to_json_equals( T const &value, std::string_view expected ) {
	auto buff = std::array<char, 64>{ };
	char const *const last = daw::json::to_json( value, buff.data( ) );
	return std::string_view(
	         buff.data( ), static_cast<std::size_t>( last - buff.data( ) ) ) ==
	       expected;
}

// Compile time serialization of a type annotated enum
static_assert( to_json_equals( EBar::A, R"json("A")json" ) );
static_assert( to_json_equals( EBar::B, R"json("B")json" ) );
static_assert( to_json_equals( EnumTypeString{ EBar::A, { } },
                               R"json({"bar":"A","bars":[]})json" ) );
static_assert( to_json_equals( EBaz::A, "1" ) );
static_assert(
  to_json_equals( EnumTypeNumber{ EBaz::B }, R"json({"baz":2})json" ) );
// Compile time serialization of a member annotated enum
static_assert( to_json_equals( EnumMemberString{ EFoo::AChoo },
                               R"json({"foo":"AChoo"})json" ) );

// Annotating a type with map_as maps it with that JSON type everywhere it is
// used.  The JSON type must not have a name, it is supplied by the member
struct Temperature;
struct TemperatureConv {
	static constexpr Temperature operator( )( std::string_view sv );
	static constexpr std::string_view operator( )( Temperature const &t );
};

struct[[= reflect.map_as<daw::json::json_custom_no_name<
  Temperature, TemperatureConv, TemperatureConv>>]] Temperature {
	bool is_hot;

	constexpr bool operator==( Temperature const & ) const = default;
};

constexpr Temperature TemperatureConv::operator( )( std::string_view sv ) {
	return Temperature{ sv == "hot" };
}

constexpr std::string_view
TemperatureConv::operator( )( Temperature const &t ) {
	return t.is_hot ? "hot" : "cold";
}

struct[[= reflect]] Weather {
	Temperature today;
	[[= reflect.rename<"tomorrow">]] Temperature next;
	std::vector<Temperature> week;
};

static_assert( daw::json::refl_details::MapAsAnnotated<Temperature> );
static_assert( not daw::json::refl_details::MapAsAnnotated<Weather> );

// Compile time parsing of a type level map_as
static_assert( daw::json::from_json<Temperature>( R"json("hot")json" ) ==
               Temperature{ true } );
static_assert(
  daw::json::from_json<Weather>(
    R"json({"today":"hot","tomorrow":"cold","week":["cold","hot"]})json" )
    .next == Temperature{ false } );

// Compile time serialization of a type level map_as
static_assert( to_json_equals( Temperature{ false }, R"json("cold")json" ) );
static_assert( to_json_equals( Weather{ { true }, { false }, { } },
                               R"json({"today":"hot","tomorrow":"cold",)json"
                               R"json("week":[]})json" ) );

struct HasHidden {
	int x;
	[[= reflect.ignored( 42 )]] int y;
	int z;
};
static_assert( daw::json::ReflectionEnabled<HasHidden> );

struct[[= reflect]] Value {
	int value = 21212;
	explicit Value( ) = default;
	Value( int x )
	  : value( x ) {}
};
static_assert( daw::json::ReflectionEnabled<Value> );
struct HasHidden2 {
	int x;
	[[= reflect.ignored( [] {
		return Value( 4242 );
	} )]] Value y;
	int z;
};
static_assert( daw::json::refl_details::construction_test_v<
               HasHidden2, daw::json::refl_details::to_tuple_t<HasHidden2>> );
static_assert( daw::json::ReflectionEnabled<HasHidden2> );

struct ExternallyEnabled {
	int member1;
	int member2;
};

template<>
inline constexpr bool daw::json::enable_reflection_for<ExternallyEnabled> =
  true;

int main( ) try {
	constexpr daw::string_view json_doc0 = R"json(
{
	"member1": 55,
	"member2": 123
}
)json";
	constexpr auto val0 = daw::json::from_json<X>( json_doc0 );
	daw_ensure( val0.m1 == 55 );
	daw_ensure( val0.m2 == 123 );
	auto const val0_json = daw::json::to_json( val0 );
	daw::println( "json: {}", val0_json );

	constexpr auto val0b = daw::json::from_json<ExternallyEnabled>( json_doc0 );
	daw_ensure( val0b.member1 == 55 );
	daw_ensure( val0b.member2 == 123 );
	auto const val0b_json = daw::json::to_json( val0b );
	daw::println( "json: {}", val0b_json );

	constexpr daw::string_view json_doc1 = R"json(
	{
	  "m0": { "member1": 55, "member2": 123 },
	  "m1": "Hello World!"
	}
	)json";
	auto const val1 = daw::json::from_json<Y>( json_doc1 );
	daw_ensure( val1.m0.m1 == 55 );
	daw_ensure( val1.m0.m2 == 123 );
	daw_ensure( val1.m1 == "Hello World!" );
	daw::println( "json: {}", daw::json::to_json( val1 ) );

	constexpr daw::string_view json_doc2 = R"json(
	{
	  "kv": { "key0": 0, "key1": 1, "key2": 2 },
	}
	)json";
	auto val2 = daw::json::from_json<Z>( json_doc2 );
	auto p = val2.kv.find( "key0" );
	daw_ensure( p != val2.kv.end( ) );
	daw_ensure( p->second == 0 );

	p = val2.kv.find( "key1" );
	daw_ensure( p != val2.kv.end( ) );
	daw_ensure( p->second == 1 );

	p = val2.kv.find( "key2" );
	daw_ensure( p != val2.kv.end( ) );
	daw_ensure( p->second == 2 );
	daw::println( "json: {}", daw::json::to_json( val2 ) );

	constexpr daw::string_view json_doc3 = R"json(
	{
	  "m1": [ { "member1": 0, "member2": 1 }, { "member1": 2, "member2": 3 } ]
	}
	)json";
	auto val3 = daw::json::from_json<Foo>( json_doc3 );
	daw_ensure( not val3.m0 );
	daw_ensure( val3.m1.size( ) == 2 );
	daw_ensure( val3.m1[0].m1 == 0 );
	daw_ensure( val3.m1[0].m2 == 1 );
	daw_ensure( val3.m1[1].m1 == 2 );
	daw_ensure( val3.m1[1].m2 == 3 );
	daw::println( "json: {}", daw::json::to_json( val3 ) );

	using namespace daw::json::options;
	auto json_doc3b =
	  daw::json::to_json( val3, output_flags<SerializationFormat::Pretty> );
	daw::println( "pretty json: {}", json_doc3b );
	auto val3b = daw::json::from_json<Foo>( json_doc3b );
	daw_ensure( not val3b.m0 );
	daw_ensure( val3b.m1.size( ) == 2 );
	daw_ensure( val3b.m1[0].m1 == 0 );
	daw_ensure( val3b.m1[0].m2 == 1 );
	daw_ensure( val3b.m1[1].m1 == 2 );
	daw_ensure( val3b.m1[1].m2 == 3 );

	static constexpr daw::string_view json_doc4 = R"json({"value": "42"})json";
	daw::println( "json_doc4: {}", json_doc4 );
	auto val4 = daw::json::from_json<A>( json_doc4 );
	daw_ensure( val4.value == "42" );
	auto const val4_json = daw::json::to_json( val4 );
	daw::println( "json: {}", val4_json );

	static constexpr daw::string_view json_doc5 = R"json({"x": 42})json";
	daw::println( "json_doc5: {}", json_doc5 );
	auto val5 = daw::json::from_json<NoRefl>( json_doc5 );
	// If refletion is used, x will be 42
	daw_ensure( val5.x == 21 );
	auto const val5_json = daw::json::to_json( val4 );
	daw::println( "json: {}", val5_json );

	auto afoo0 = EnumMember{ EFoo::AChoo };
	auto afoo1 = EnumMember{ EFoo::BlessYou };
	auto const val6_json = daw::json::to_json( afoo0 );
	daw::println( "EnumMember{{ EFoo::AChoo }}; as json: {}", val6_json );
	auto const val7_json = daw::json::to_json( afoo1 );
	daw::println( "EnumMember{{ EFoo::BlessYou }}; as json: {}", val7_json );

	static constexpr auto achoo =
	  daw::json::refl_details::enum_to_string( EFoo::AChoo );
	static_assert( achoo == "AChoo" );

	auto bfoo0 = EnumMemberString{ EFoo::AChoo };
	auto bfoo1 = EnumMemberString{ EFoo::BlessYou };
	auto const val8_json = daw::json::to_json( bfoo0 );
	daw::println( "EnumMemberString{{ EFoo::AChoo }}; as json: {}", val8_json );
	auto const val9_json = daw::json::to_json( bfoo1 );
	daw::println( "EnumMemberString{{ EFoo::BlessYou }}; as json: {}",
	              val9_json );

	auto const bfoo2 = daw::json::from_json<EnumMemberString>( val9_json );
	daw_ensure( bfoo2.foo == EFoo::BlessYou );

	auto const ets0 = EnumTypeString{ EBar::B, { EBar::A, EBar::B, EBar::A } };
	auto const ets0_json = daw::json::to_json( ets0 );
	daw::println( "EnumTypeString as json: {}", ets0_json );
	daw_ensure( ets0_json == R"json({"bar":"B","bars":["A","B","A"]})json" );
	auto const ets1 = daw::json::from_json<EnumTypeString>( ets0_json );
	daw_ensure( ets1.bar == ets0.bar );
	daw_ensure( ets1.bars == ets0.bars );

	auto const ebars = daw::json::from_json_array<EBar>( R"json(["B","A"])json" );
	daw_ensure( ebars == std::vector{ EBar::B, EBar::A } );
	daw_ensure( daw::json::to_json_array( ebars ) == R"json(["B","A"])json" );

	bool bad_enum_string_threw = false;
	try {
		(void)daw::json::from_json<EBar>( R"json("C")json" );
	} catch( daw::json::json_exception const & ) {
		bad_enum_string_threw = true;
	}
	daw_ensure( bad_enum_string_threw );

	auto const invalid_ebar = static_cast<EBar>( 42 );
	bool invalid_enum_to_json_threw = false;
	try {
		(void)daw::json::to_json( invalid_ebar );
	} catch( daw::json::json_exception const &jex ) {
		invalid_enum_to_json_threw = true;
		daw_ensure(
		  jex.reason_type( ) ==
		  daw::json::ErrorReason::CouldNotFindEnumeratorForValue );
	}
	daw_ensure( invalid_enum_to_json_threw );

	bool invalid_enum_writer_threw = false;
	try {
		auto output = std::string{ };
		auto writer = daw::json::json_writer( output );
		writer.write_enum_string( invalid_ebar );
	} catch( daw::json::json_exception const &jex ) {
		invalid_enum_writer_threw = true;
		daw_ensure(
		  jex.reason_type( ) ==
		  daw::json::ErrorReason::CouldNotFindEnumeratorForValue );
	}
	daw_ensure( invalid_enum_writer_threw );

	auto const w0 =
	  Weather{ { true }, { false }, { { false }, { true }, { true } } };
	auto const w0_json = daw::json::to_json( w0 );
	daw::println( "Weather as json: {}", w0_json );
	daw_ensure(
	  w0_json ==
	  R"json({"today":"hot","tomorrow":"cold","week":["cold","hot","hot"]})json" );
	auto const w1 = daw::json::from_json<Weather>( w0_json );
	daw_ensure( w1.today == w0.today );
	daw_ensure( w1.next == w0.next );
	daw_ensure( w1.week == w0.week );

	static constexpr daw::string_view h0_doc = R"json({"x": 55, "z": 66 })json";
	daw::println( "json: {}", h0_doc );
	auto const h0 = daw::json::from_json<HasHidden>( h0_doc );
	daw::println(
	  "HasHidden parsed x: {}, defaulted to 42 y: {}, z: {}", h0.x, h0.y, h0.z );
	daw_ensure( h0.x == 55 );
	daw_ensure( h0.y == 42 );
	daw_ensure( h0.z == 66 );

	auto const h1 = daw::json::from_json<HasHidden2>( h0_doc );
	daw::println( "HasHidden2 parsed x: {}, defaulted to 4242 y: {}, z: {}",
	              h1.x,
	              h1.y.value,
	              h1.z );
	daw_ensure( h1.x == 55 );
	daw_ensure( h1.y.value == 4242 );
	daw_ensure( h1.z == 66 );

	class[[= reflect.unchecked]] PrivateRefl {
		int x;

	public:
		constexpr PrivateRefl( int v )
		  : x( v ) {}
		int const &value( ) const noexcept {
			return x;
		}
		bool operator==( PrivateRefl const & ) const = default;
	};
	static_assert( daw::json::ReflectionEnabled<PrivateRefl> );
	static_assert(
	  daw::json::refl_details::has_annotation<daw::json::reflect_base_t,
	                                          PrivateRefl>( ) );
	static_assert(
	  daw::json::refl_details::
	    has_annotation<daw::json::refl_details::reflect_all_t, PrivateRefl>( ) );
	static_assert( not daw::json::refl_details::PublicMembersOnly<PrivateRefl> );

	struct Fallback {
		int x;
		std::vector<int> y;
		PrivateRefl z;

		bool operator==( Fallback const & ) const = default;
	};
	auto const fb1 = daw::json::from_json<Fallback>(
	  R"json({"z":{"x":42},"x":42,"y":[1,2,3]})json" );
	daw_ensure( fb1.x == 42 );
	daw_ensure( fb1.y == std::vector<int>{ 1, 2, 3 } );
	daw_ensure( fb1.z.value( ) == 42 );
	auto const fb1_json = daw::json::to_json( fb1 );
	auto const fb2 = daw::json::from_json<Fallback>( fb1_json );
	daw_ensure( fb1 == fb2 );

	return EXIT_SUCCESS;
} catch( daw::json::json_exception const &jex ) {
	daw::println( "unexpected JSON Exception: {}", to_formatted_string( jex ) );
	return EXIT_FAILURE;
}
