#include <daw/json/daw_json_link.h>

#include <daw/daw_ensure.h>

#include <string>
#include <variant>

struct Foo {
	std::variant<std::monostate, int, std::string> member{ };
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char member[] = "member";
		using type = json_member_list<json_nullable<
		  member, std::variant<std::monostate, int, std::string>,
		  json_variant_no_name<std::variant<std::monostate, int, std::string>,
		                       json_variant_type_list<int, std::string>>>>;
	};
} // namespace daw::json

int main( ) {
	constexpr daw::string_view doc0 = R"json({ "member":null})json";
	auto v0 = daw::json::from_json<Foo>( doc0 );
	daw_ensure( v0.member.index( ) == 0 );

	constexpr daw::string_view doc1 = R"json({ "member":42})json";
	auto v1 = daw::json::from_json<Foo>( doc1 );
	daw_ensure( v1.member.index( ) == 1 );
	daw_ensure( std::get<1>( v1.member ) == 42 );

	constexpr daw::string_view doc2 = R"json({ "member":"Hello world"})json";
	auto v2 = daw::json::from_json<Foo>( doc2 );
	daw_ensure( v2.member.index( ) == 2 );
	daw_ensure( std::get<2>( v2.member ) == "Hello world" );
}