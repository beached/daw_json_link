// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_json_link.h>

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace daw::gsoc {
	struct sponsor_t {
		std::string type;
		std::string name;
		std::string disambiguatingDescription;
		std::string description;
		std::string url;
		std::string logo;
	};

	struct author_t {
		std::string type;
		std::string name;
	};

	struct gsoc_element_t {
		std::string context;
		std::string type;
		std::string name;
		std::string description;
		sponsor_t sponsor;
		author_t author;
	};
	using gsoc_object_t = std::vector<std::pair<std::uint64_t, gsoc_element_t>>;
	// using gsoc_object_t = std::unordered_map<std::uint64_t, gsoc_element_t>;
} // namespace daw::gsoc

namespace daw::json {
	template<JSONNAMETYPE Name>
	using json_sv = json_string_raw<Name, std::string>;

	template<>
	struct json_data_contract<daw::gsoc::sponsor_t> {
		static constexpr char const mem_type[] = "@type";
		static constexpr char const mem_name[] = "name";
		static constexpr char const mem_disambiguatingDescription[] =
		  "disambiguatingDescription";
		static constexpr char const mem_description[] = "description";
		static constexpr char const mem_url[] = "url";
		static constexpr char const mem_logo[] = "logo";
		using type = json_member_list<json_sv<mem_type>, json_sv<mem_name>,
		                              json_sv<mem_disambiguatingDescription>,
		                              json_sv<mem_description>, json_sv<mem_url>,
		                              json_sv<mem_logo>>;

		static inline auto to_json_data( daw::gsoc::sponsor_t const &value ) {
			return std::forward_as_tuple( value.type, value.name,
			                              value.disambiguatingDescription,
			                              value.description, value.url, value.logo );
		}
	};

	template<>
	struct json_data_contract<daw::gsoc::author_t> {
		static constexpr char const mem_type[] = "@type";
		static constexpr char const mem_name[] = "name";
		using type = json_member_list<json_sv<mem_type>, json_sv<mem_name>>;

		static inline auto to_json_data( daw::gsoc::author_t const &value ) {
			return std::forward_as_tuple( value.type, value.name );
		}
	};

	template<>
	struct json_data_contract<daw::gsoc::gsoc_element_t> {
		static constexpr char const mem_context[] = "@context";
		static constexpr char const mem_type[] = "@type";
		static constexpr char const mem_name[] = "name";
		static constexpr char const mem_description[] = "description";
		static constexpr char const mem_sponsor[] = "sponsor";
		static constexpr char const mem_author[] = "author";
		using type = json_member_list<json_sv<mem_context>, json_sv<mem_type>,
		                              json_sv<mem_name>, json_sv<mem_description>,
		                              json_class<mem_sponsor, daw::gsoc::sponsor_t>,
		                              json_class<mem_author, daw::gsoc::author_t>>;

		static inline auto to_json_data( daw::gsoc::gsoc_element_t const &value ) {
			return std::forward_as_tuple( value.context, value.type, value.name,
			                              value.description, value.sponsor,
			                              value.author );
		}
	};

	template<>
	struct json_data_contract<daw::gsoc::gsoc_object_t> {
		using type = json_class_map<
		  json_key_value_no_name<daw::gsoc::gsoc_object_t, gsoc::gsoc_element_t,
		                 json_number_no_name<std::uint64_t,
		                             number_opt( LiteralAsStringOpt::Always )>>>;

		static inline auto const &
		to_json_data( daw::gsoc::gsoc_object_t const &v ) {
			return v;
		}
	};
} // namespace daw::json
