// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_tuple_forward.h>
#include <daw/json/daw_json_link.h>

#include <magic_enum.hpp>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

enum class test_enum : int {
	val_0 = 0,
	val_1 = 1,
};

enum class data_type : int { FOO = 0, BAR = 1 };

struct foo_type {
	int field_1;
};

struct bar_type {
	test_enum field_1;
	// std::string field_1;
};

using variant_type = std::variant<foo_type, bar_type>;

namespace daw::json {
	template<typename E>
	struct json_data_contract<E, std::enable_if_t<std::is_enum_v<E>>> {

		template<typename Enum>
		struct enum_name_converter {
			static_assert( std::is_enum_v<Enum>, "Only enum types are supported" );
			constexpr std::string_view operator( )( Enum e ) const {
				return magic_enum::enum_name( e );
			}

			inline Enum operator( )( std::string_view sv ) const {
				auto test = magic_enum::enum_cast<Enum>( sv );
				if( !test ) {
					std::string msg = "Bad enum value ";
					msg += sv;
					msg += " for enum type ";
					msg += magic_enum::enum_type_name<Enum>( );
					throw std::runtime_error( msg );
				}
				return test.value( );
			}
		};

		using type = json_type_alias<
		  json_custom_no_name<E, enum_name_converter<E>, enum_name_converter<E>>>;
	};

	template<>
	struct json_data_contract<foo_type> {
		static constexpr char const mem_dtype[] = "dtype";
		static constexpr char const mem_field_1[] = "field_1";

		struct builder {
			template<class... Args>
			constexpr auto operator( )( data_type /*dtype*/, Args &&...args ) const {
				return foo_type{ std::forward<Args>( args )... };
			}
		};

		using constructor_t = builder;
		using type = json_member_list<json_class<mem_dtype, data_type>,
		                              json_number<mem_field_1, int>>;

		static inline auto to_json_data( const foo_type &val ) {
			return daw::forward_nonrvalue_as_tuple( data_type::FOO, val.field_1 );
		}
	};

	template<>
	struct json_data_contract<bar_type> {
		static constexpr char const mem_dtype[] = "dtype";
		static constexpr char const mem_field_1[] = "field_1";

		struct builder {
			template<class... Args>
			constexpr auto operator( )( data_type /*dtype*/, Args &&...args ) const {
				return bar_type{ std::forward<Args>( args )... };
			}
		};

		using constructor_t = builder;
		using type = json_member_list<json_class<mem_dtype, data_type>,
		                              json_class<mem_field_1, test_enum>>;
		// using type = json_member_list<json_class<dtype, data_type>,
		// json_string<field_1>>;
		static inline auto to_json_data( const bar_type &val ) {
			return daw::forward_nonrvalue_as_tuple( data_type::BAR, val.field_1 );
		}
	};

	template<>
	struct json_data_contract<variant_type> {
		struct switcher {
			// Convert JSON tag member to type index
			constexpr size_t operator( )( data_type type ) const {
				return static_cast<size_t>( type );
			}
			// Get value for Tag from class value
			data_type operator( )( const variant_type &val ) const {
				return static_cast<data_type>( val.index( ) );
			}
		};

		static constexpr char const mem_dtype[] = "dtype";

		using type = json_submember_tagged_variant<json_class<mem_dtype, data_type>,
		                                           switcher, foo_type, bar_type>;
	};

} // namespace daw::json

int main( int /*argc*/, char ** /*argv*/ ) {
	constexpr std::string_view json_doc = R"json(
[
  {
    "field_1": "val_0",
    "dtype": "BAR"
  },
  {
    "field_1": 3,
    "dtype": "FOO"
  }
]
)json";

	try {
		auto obj = daw::json::from_json<std::vector<variant_type>>( json_doc );
		auto obj_json = daw::json::to_json(
		  obj, daw::json::options::output_flags<
		         daw::json::options::SerializationFormat::Pretty> );
		std::cout << obj_json.c_str( ) << std::endl;

	} catch( const daw::json::json_exception &err ) {
		std::cout << daw::json::to_formatted_string( err, json_doc.data( ) )
		          << std::endl;
		throw err;
	}
}
