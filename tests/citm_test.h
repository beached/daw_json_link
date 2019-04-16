// Code auto generated from json file '/Users/dwright/Downloads/citm_catalog.json'

#pragma once

#include <tuple>
#include <optional>
#include <cstdint>
#include <string_view>
#include <vector>
#include <daw/json/daw_json_link.h>

struct audienceSubCategoryNames_t {
	std::string_view _json337100890;
};	// audienceSubCategoryNames_t

inline auto describe_json_class( audienceSubCategoryNames_t ) {
	using namespace daw::json;
	static constexpr char const _json337100890[] = "337100890";
	return daw::json::class_description_t<
		json_string<_json337100890, std::string_view>
>{};
}

inline auto to_json_data( audienceSubCategoryNames_t const & value ) {
	return std::forward_as_tuple( value._json337100890 );
}

struct blockNames_t {
};	// blockNames_t

inline auto describe_json_class( blockNames_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
>{};
}

inline auto to_json_data( blockNames_t const & value ) {
	return std::forward_as_tuple(  );
}

struct _json138586341_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586341_t

inline auto describe_json_class( _json138586341_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586341_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586345_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586345_t

inline auto describe_json_class( _json138586345_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586345_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586349_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586349_t

inline auto describe_json_class( _json138586349_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586349_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586353_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586353_t

inline auto describe_json_class( _json138586353_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586353_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586357_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586357_t

inline auto describe_json_class( _json138586357_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586357_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586361_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586361_t

inline auto describe_json_class( _json138586361_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586361_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586365_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586365_t

inline auto describe_json_class( _json138586365_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586365_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586369_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586369_t

inline auto describe_json_class( _json138586369_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586369_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586373_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586373_t

inline auto describe_json_class( _json138586373_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586373_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586377_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586377_t

inline auto describe_json_class( _json138586377_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586377_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586381_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586381_t

inline auto describe_json_class( _json138586381_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586381_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586385_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586385_t

inline auto describe_json_class( _json138586385_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586385_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586389_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586389_t

inline auto describe_json_class( _json138586389_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586389_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586393_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586393_t

inline auto describe_json_class( _json138586393_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586393_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586397_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586397_t

inline auto describe_json_class( _json138586397_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586397_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586401_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586401_t

inline auto describe_json_class( _json138586401_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586401_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586405_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586405_t

inline auto describe_json_class( _json138586405_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586405_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586409_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586409_t

inline auto describe_json_class( _json138586409_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586409_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586413_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586413_t

inline auto describe_json_class( _json138586413_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586413_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586417_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586417_t

inline auto describe_json_class( _json138586417_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586417_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586421_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586421_t

inline auto describe_json_class( _json138586421_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586421_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586425_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586425_t

inline auto describe_json_class( _json138586425_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586425_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586429_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586429_t

inline auto describe_json_class( _json138586429_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586429_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586433_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586433_t

inline auto describe_json_class( _json138586433_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586433_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586437_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586437_t

inline auto describe_json_class( _json138586437_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586437_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586441_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586441_t

inline auto describe_json_class( _json138586441_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586441_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586445_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586445_t

inline auto describe_json_class( _json138586445_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586445_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586449_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586449_t

inline auto describe_json_class( _json138586449_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586449_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586453_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586453_t

inline auto describe_json_class( _json138586453_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586453_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586457_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586457_t

inline auto describe_json_class( _json138586457_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586457_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586461_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586461_t

inline auto describe_json_class( _json138586461_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586461_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586465_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586465_t

inline auto describe_json_class( _json138586465_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586465_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586469_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586469_t

inline auto describe_json_class( _json138586469_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586469_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586473_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586473_t

inline auto describe_json_class( _json138586473_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586473_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586477_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586477_t

inline auto describe_json_class( _json138586477_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586477_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586481_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586481_t

inline auto describe_json_class( _json138586481_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586481_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586485_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586485_t

inline auto describe_json_class( _json138586485_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586485_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586501_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586501_t

inline auto describe_json_class( _json138586501_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586501_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586505_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586505_t

inline auto describe_json_class( _json138586505_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586505_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586509_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586509_t

inline auto describe_json_class( _json138586509_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586509_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586513_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586513_t

inline auto describe_json_class( _json138586513_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586513_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586517_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586517_t

inline auto describe_json_class( _json138586517_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586517_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586521_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586521_t

inline auto describe_json_class( _json138586521_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586521_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586525_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586525_t

inline auto describe_json_class( _json138586525_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586525_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586529_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586529_t

inline auto describe_json_class( _json138586529_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586529_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586533_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586533_t

inline auto describe_json_class( _json138586533_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586533_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586537_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586537_t

inline auto describe_json_class( _json138586537_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586537_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586541_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586541_t

inline auto describe_json_class( _json138586541_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586541_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586545_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586545_t

inline auto describe_json_class( _json138586545_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586545_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586549_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586549_t

inline auto describe_json_class( _json138586549_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586549_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586553_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586553_t

inline auto describe_json_class( _json138586553_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586553_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586557_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586557_t

inline auto describe_json_class( _json138586557_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586557_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586561_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586561_t

inline auto describe_json_class( _json138586561_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586561_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586565_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586565_t

inline auto describe_json_class( _json138586565_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586565_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586569_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586569_t

inline auto describe_json_class( _json138586569_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586569_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586581_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586581_t

inline auto describe_json_class( _json138586581_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586581_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586585_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586585_t

inline auto describe_json_class( _json138586585_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586585_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586589_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586589_t

inline auto describe_json_class( _json138586589_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586589_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586593_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586593_t

inline auto describe_json_class( _json138586593_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586593_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586597_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586597_t

inline auto describe_json_class( _json138586597_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586597_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586601_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586601_t

inline auto describe_json_class( _json138586601_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586601_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586605_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586605_t

inline auto describe_json_class( _json138586605_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586605_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586609_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586609_t

inline auto describe_json_class( _json138586609_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586609_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586613_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586613_t

inline auto describe_json_class( _json138586613_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586613_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586617_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586617_t

inline auto describe_json_class( _json138586617_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586617_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586625_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586625_t

inline auto describe_json_class( _json138586625_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586625_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586629_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586629_t

inline auto describe_json_class( _json138586629_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586629_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586633_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586633_t

inline auto describe_json_class( _json138586633_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586633_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586641_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586641_t

inline auto describe_json_class( _json138586641_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586641_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586645_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586645_t

inline auto describe_json_class( _json138586645_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586645_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586649_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586649_t

inline auto describe_json_class( _json138586649_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586649_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586653_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586653_t

inline auto describe_json_class( _json138586653_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586653_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586657_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586657_t

inline auto describe_json_class( _json138586657_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586657_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586661_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586661_t

inline auto describe_json_class( _json138586661_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586661_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586667_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586667_t

inline auto describe_json_class( _json138586667_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586667_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586671_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586671_t

inline auto describe_json_class( _json138586671_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586671_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586675_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586675_t

inline auto describe_json_class( _json138586675_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586675_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586679_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586679_t

inline auto describe_json_class( _json138586679_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586679_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586687_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586687_t

inline auto describe_json_class( _json138586687_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586687_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586691_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586691_t

inline auto describe_json_class( _json138586691_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586691_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586695_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586695_t

inline auto describe_json_class( _json138586695_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586695_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586699_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586699_t

inline auto describe_json_class( _json138586699_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586699_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586703_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586703_t

inline auto describe_json_class( _json138586703_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586703_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586707_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586707_t

inline auto describe_json_class( _json138586707_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586707_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586711_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586711_t

inline auto describe_json_class( _json138586711_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586711_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586715_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586715_t

inline auto describe_json_class( _json138586715_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586715_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586719_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586719_t

inline auto describe_json_class( _json138586719_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586719_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586723_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586723_t

inline auto describe_json_class( _json138586723_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586723_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586731_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586731_t

inline auto describe_json_class( _json138586731_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586731_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586735_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586735_t

inline auto describe_json_class( _json138586735_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586735_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586743_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586743_t

inline auto describe_json_class( _json138586743_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586743_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586747_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586747_t

inline auto describe_json_class( _json138586747_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586747_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586751_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586751_t

inline auto describe_json_class( _json138586751_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586751_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586755_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586755_t

inline auto describe_json_class( _json138586755_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586755_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586759_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586759_t

inline auto describe_json_class( _json138586759_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586759_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586771_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586771_t

inline auto describe_json_class( _json138586771_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586771_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586775_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586775_t

inline auto describe_json_class( _json138586775_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586775_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586779_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586779_t

inline auto describe_json_class( _json138586779_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586779_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586783_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586783_t

inline auto describe_json_class( _json138586783_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586783_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586787_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586787_t

inline auto describe_json_class( _json138586787_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586787_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586795_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586795_t

inline auto describe_json_class( _json138586795_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586795_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586799_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586799_t

inline auto describe_json_class( _json138586799_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586799_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586803_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586803_t

inline auto describe_json_class( _json138586803_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586803_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586807_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586807_t

inline auto describe_json_class( _json138586807_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586807_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586811_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586811_t

inline auto describe_json_class( _json138586811_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586811_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586815_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586815_t

inline auto describe_json_class( _json138586815_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586815_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586823_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586823_t

inline auto describe_json_class( _json138586823_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586823_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586827_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586827_t

inline auto describe_json_class( _json138586827_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586827_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586831_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586831_t

inline auto describe_json_class( _json138586831_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586831_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586835_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586835_t

inline auto describe_json_class( _json138586835_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586835_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586839_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586839_t

inline auto describe_json_class( _json138586839_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586839_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586843_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586843_t

inline auto describe_json_class( _json138586843_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586843_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586847_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586847_t

inline auto describe_json_class( _json138586847_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586847_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586851_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586851_t

inline auto describe_json_class( _json138586851_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586851_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586855_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586855_t

inline auto describe_json_class( _json138586855_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586855_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586859_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586859_t

inline auto describe_json_class( _json138586859_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586859_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586863_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586863_t

inline auto describe_json_class( _json138586863_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586863_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586867_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586867_t

inline auto describe_json_class( _json138586867_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586867_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586871_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586871_t

inline auto describe_json_class( _json138586871_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586871_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586875_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586875_t

inline auto describe_json_class( _json138586875_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586875_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586879_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586879_t

inline auto describe_json_class( _json138586879_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586879_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586883_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586883_t

inline auto describe_json_class( _json138586883_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586883_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586889_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586889_t

inline auto describe_json_class( _json138586889_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586889_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586893_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586893_t

inline auto describe_json_class( _json138586893_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586893_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586897_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586897_t

inline auto describe_json_class( _json138586897_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586897_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586901_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586901_t

inline auto describe_json_class( _json138586901_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586901_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586905_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586905_t

inline auto describe_json_class( _json138586905_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586905_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586909_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586909_t

inline auto describe_json_class( _json138586909_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586909_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586917_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586917_t

inline auto describe_json_class( _json138586917_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586917_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586921_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586921_t

inline auto describe_json_class( _json138586921_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586921_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586929_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586929_t

inline auto describe_json_class( _json138586929_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586929_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586933_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586933_t

inline auto describe_json_class( _json138586933_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586933_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586937_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586937_t

inline auto describe_json_class( _json138586937_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586937_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586941_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586941_t

inline auto describe_json_class( _json138586941_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586941_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586945_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586945_t

inline auto describe_json_class( _json138586945_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586945_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586949_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586949_t

inline auto describe_json_class( _json138586949_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586949_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586953_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586953_t

inline auto describe_json_class( _json138586953_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586953_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586957_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586957_t

inline auto describe_json_class( _json138586957_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586957_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586961_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586961_t

inline auto describe_json_class( _json138586961_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586961_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586965_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586965_t

inline auto describe_json_class( _json138586965_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586965_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586969_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586969_t

inline auto describe_json_class( _json138586969_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586969_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586973_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586973_t

inline auto describe_json_class( _json138586973_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586973_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586977_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586977_t

inline auto describe_json_class( _json138586977_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586977_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586981_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586981_t

inline auto describe_json_class( _json138586981_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586981_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586989_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586989_t

inline auto describe_json_class( _json138586989_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586989_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586993_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586993_t

inline auto describe_json_class( _json138586993_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586993_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json138586997_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json138586997_t

inline auto describe_json_class( _json138586997_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json138586997_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json151183114_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json151183114_t

inline auto describe_json_class( _json151183114_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json151183114_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json339420802_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json339420802_t

inline auto describe_json_class( _json339420802_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json339420802_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json339420805_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json339420805_t

inline auto describe_json_class( _json339420805_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json339420805_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341069930_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341069930_t

inline auto describe_json_class( _json341069930_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341069930_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181232_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181232_t

inline auto describe_json_class( _json341181232_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181232_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181233_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181233_t

inline auto describe_json_class( _json341181233_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181233_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181234_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181234_t

inline auto describe_json_class( _json341181234_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181234_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181235_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181235_t

inline auto describe_json_class( _json341181235_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181235_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181236_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181236_t

inline auto describe_json_class( _json341181236_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181236_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181237_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181237_t

inline auto describe_json_class( _json341181237_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181237_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181238_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181238_t

inline auto describe_json_class( _json341181238_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181238_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181239_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181239_t

inline auto describe_json_class( _json341181239_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181239_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181240_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181240_t

inline auto describe_json_class( _json341181240_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181240_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181241_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181241_t

inline auto describe_json_class( _json341181241_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181241_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181242_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181242_t

inline auto describe_json_class( _json341181242_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181242_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181243_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181243_t

inline auto describe_json_class( _json341181243_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181243_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181244_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181244_t

inline auto describe_json_class( _json341181244_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181244_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181245_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181245_t

inline auto describe_json_class( _json341181245_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181245_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181246_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181246_t

inline auto describe_json_class( _json341181246_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181246_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181247_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181247_t

inline auto describe_json_class( _json341181247_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181247_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181248_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181248_t

inline auto describe_json_class( _json341181248_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181248_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181249_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181249_t

inline auto describe_json_class( _json341181249_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181249_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181250_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181250_t

inline auto describe_json_class( _json341181250_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181250_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181251_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181251_t

inline auto describe_json_class( _json341181251_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181251_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181252_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181252_t

inline auto describe_json_class( _json341181252_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181252_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181253_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181253_t

inline auto describe_json_class( _json341181253_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181253_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181254_t {
	int64_t id;
	std::string_view logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181254_t

inline auto describe_json_class( _json341181254_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<logo, std::string_view>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181254_t const & value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181255_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181255_t

inline auto describe_json_class( _json341181255_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181255_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181256_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181256_t

inline auto describe_json_class( _json341181256_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181256_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181257_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181257_t

inline auto describe_json_class( _json341181257_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181257_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181258_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181258_t

inline auto describe_json_class( _json341181258_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181258_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json341181259_t {
	int64_t id;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
};	// _json341181259_t

inline auto describe_json_class( _json341181259_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
		,json_array<subTopicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
		,json_array<topicIds, std::vector<int64_t>, json_number<no_name, int64_t>>
>{};
}

inline auto to_json_data( _json341181259_t const & value ) {
	return std::forward_as_tuple( value.id, value.name, value.subTopicIds, value.topicIds );
}

struct _json342742592_t {
	int64_t id;
	std::string_view name;
};	// _json342742592_t

inline auto describe_json_class( _json342742592_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
>{};
}

inline auto to_json_data( _json342742592_t const & value ) {
	return std::forward_as_tuple( value.id, value.name );
}

struct _json342742593_t {
	int64_t id;
	std::string_view name;
};	// _json342742593_t

inline auto describe_json_class( _json342742593_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
>{};
}

inline auto to_json_data( _json342742593_t const & value ) {
	return std::forward_as_tuple( value.id, value.name );
}

struct _json342742594_t {
	int64_t id;
	std::string_view name;
};	// _json342742594_t

inline auto describe_json_class( _json342742594_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
>{};
}

inline auto to_json_data( _json342742594_t const & value ) {
	return std::forward_as_tuple( value.id, value.name );
}

struct _json342742595_t {
	int64_t id;
	std::string_view name;
};	// _json342742595_t

inline auto describe_json_class( _json342742595_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
>{};
}

inline auto to_json_data( _json342742595_t const & value ) {
	return std::forward_as_tuple( value.id, value.name );
}

struct _json342742596_t {
	int64_t id;
	std::string_view name;
};	// _json342742596_t

inline auto describe_json_class( _json342742596_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const name[] = "name";
	return daw::json::class_description_t<
		json_number<id, intmax_t>
		,json_string<name, std::string_view>
>{};
}

inline auto to_json_data( _json342742596_t const & value ) {
	return std::forward_as_tuple( value.id, value.name );
}

struct events_t {
	_json138586341_t _json138586341;
	_json138586345_t _json138586345;
	_json138586349_t _json138586349;
	_json138586353_t _json138586353;
	_json138586357_t _json138586357;
	_json138586361_t _json138586361;
	_json138586365_t _json138586365;
	_json138586369_t _json138586369;
	_json138586373_t _json138586373;
	_json138586377_t _json138586377;
	_json138586381_t _json138586381;
	_json138586385_t _json138586385;
	_json138586389_t _json138586389;
	_json138586393_t _json138586393;
	_json138586397_t _json138586397;
	_json138586401_t _json138586401;
	_json138586405_t _json138586405;
	_json138586409_t _json138586409;
	_json138586413_t _json138586413;
	_json138586417_t _json138586417;
	_json138586421_t _json138586421;
	_json138586425_t _json138586425;
	_json138586429_t _json138586429;
	_json138586433_t _json138586433;
	_json138586437_t _json138586437;
	_json138586441_t _json138586441;
	_json138586445_t _json138586445;
	_json138586449_t _json138586449;
	_json138586453_t _json138586453;
	_json138586457_t _json138586457;
	_json138586461_t _json138586461;
	_json138586465_t _json138586465;
	_json138586469_t _json138586469;
	_json138586473_t _json138586473;
	_json138586477_t _json138586477;
	_json138586481_t _json138586481;
	_json138586485_t _json138586485;
	_json138586501_t _json138586501;
	_json138586505_t _json138586505;
	_json138586509_t _json138586509;
	_json138586513_t _json138586513;
	_json138586517_t _json138586517;
	_json138586521_t _json138586521;
	_json138586525_t _json138586525;
	_json138586529_t _json138586529;
	_json138586533_t _json138586533;
	_json138586537_t _json138586537;
	_json138586541_t _json138586541;
	_json138586545_t _json138586545;
	_json138586549_t _json138586549;
	_json138586553_t _json138586553;
	_json138586557_t _json138586557;
	_json138586561_t _json138586561;
	_json138586565_t _json138586565;
	_json138586569_t _json138586569;
	_json138586581_t _json138586581;
	_json138586585_t _json138586585;
	_json138586589_t _json138586589;
	_json138586593_t _json138586593;
	_json138586597_t _json138586597;
	_json138586601_t _json138586601;
	_json138586605_t _json138586605;
	_json138586609_t _json138586609;
	_json138586613_t _json138586613;
	_json138586617_t _json138586617;
	_json138586625_t _json138586625;
	_json138586629_t _json138586629;
	_json138586633_t _json138586633;
	_json138586641_t _json138586641;
	_json138586645_t _json138586645;
	_json138586649_t _json138586649;
	_json138586653_t _json138586653;
	_json138586657_t _json138586657;
	_json138586661_t _json138586661;
	_json138586667_t _json138586667;
	_json138586671_t _json138586671;
	_json138586675_t _json138586675;
	_json138586679_t _json138586679;
	_json138586687_t _json138586687;
	_json138586691_t _json138586691;
	_json138586695_t _json138586695;
	_json138586699_t _json138586699;
	_json138586703_t _json138586703;
	_json138586707_t _json138586707;
	_json138586711_t _json138586711;
	_json138586715_t _json138586715;
	_json138586719_t _json138586719;
	_json138586723_t _json138586723;
	_json138586731_t _json138586731;
	_json138586735_t _json138586735;
	_json138586743_t _json138586743;
	_json138586747_t _json138586747;
	_json138586751_t _json138586751;
	_json138586755_t _json138586755;
	_json138586759_t _json138586759;
	_json138586771_t _json138586771;
	_json138586775_t _json138586775;
	_json138586779_t _json138586779;
	_json138586783_t _json138586783;
	_json138586787_t _json138586787;
	_json138586795_t _json138586795;
	_json138586799_t _json138586799;
	_json138586803_t _json138586803;
	_json138586807_t _json138586807;
	_json138586811_t _json138586811;
	_json138586815_t _json138586815;
	_json138586823_t _json138586823;
	_json138586827_t _json138586827;
	_json138586831_t _json138586831;
	_json138586835_t _json138586835;
	_json138586839_t _json138586839;
	_json138586843_t _json138586843;
	_json138586847_t _json138586847;
	_json138586851_t _json138586851;
	_json138586855_t _json138586855;
	_json138586859_t _json138586859;
	_json138586863_t _json138586863;
	_json138586867_t _json138586867;
	_json138586871_t _json138586871;
	_json138586875_t _json138586875;
	_json138586879_t _json138586879;
	_json138586883_t _json138586883;
	_json138586889_t _json138586889;
	_json138586893_t _json138586893;
	_json138586897_t _json138586897;
	_json138586901_t _json138586901;
	_json138586905_t _json138586905;
	_json138586909_t _json138586909;
	_json138586917_t _json138586917;
	_json138586921_t _json138586921;
	_json138586929_t _json138586929;
	_json138586933_t _json138586933;
	_json138586937_t _json138586937;
	_json138586941_t _json138586941;
	_json138586945_t _json138586945;
	_json138586949_t _json138586949;
	_json138586953_t _json138586953;
	_json138586957_t _json138586957;
	_json138586961_t _json138586961;
	_json138586965_t _json138586965;
	_json138586969_t _json138586969;
	_json138586973_t _json138586973;
	_json138586977_t _json138586977;
	_json138586981_t _json138586981;
	_json138586989_t _json138586989;
	_json138586993_t _json138586993;
	_json138586997_t _json138586997;
	_json151183114_t _json151183114;
	_json339420802_t _json339420802;
	_json339420805_t _json339420805;
	_json341069930_t _json341069930;
	_json341181232_t _json341181232;
	_json341181233_t _json341181233;
	_json341181234_t _json341181234;
	_json341181235_t _json341181235;
	_json341181236_t _json341181236;
	_json341181237_t _json341181237;
	_json341181238_t _json341181238;
	_json341181239_t _json341181239;
	_json341181240_t _json341181240;
	_json341181241_t _json341181241;
	_json341181242_t _json341181242;
	_json341181243_t _json341181243;
	_json341181244_t _json341181244;
	_json341181245_t _json341181245;
	_json341181246_t _json341181246;
	_json341181247_t _json341181247;
	_json341181248_t _json341181248;
	_json341181249_t _json341181249;
	_json341181250_t _json341181250;
	_json341181251_t _json341181251;
	_json341181252_t _json341181252;
	_json341181253_t _json341181253;
	_json341181254_t _json341181254;
	_json341181255_t _json341181255;
	_json341181256_t _json341181256;
	_json341181257_t _json341181257;
	_json341181258_t _json341181258;
	_json341181259_t _json341181259;
	_json342742592_t _json342742592;
	_json342742593_t _json342742593;
	_json342742594_t _json342742594;
	_json342742595_t _json342742595;
	_json342742596_t _json342742596;
};	// events_t

inline auto describe_json_class( events_t ) {
	using namespace daw::json;
	static constexpr char const _json138586341[] = "138586341";
	static constexpr char const _json138586345[] = "138586345";
	static constexpr char const _json138586349[] = "138586349";
	static constexpr char const _json138586353[] = "138586353";
	static constexpr char const _json138586357[] = "138586357";
	static constexpr char const _json138586361[] = "138586361";
	static constexpr char const _json138586365[] = "138586365";
	static constexpr char const _json138586369[] = "138586369";
	static constexpr char const _json138586373[] = "138586373";
	static constexpr char const _json138586377[] = "138586377";
	static constexpr char const _json138586381[] = "138586381";
	static constexpr char const _json138586385[] = "138586385";
	static constexpr char const _json138586389[] = "138586389";
	static constexpr char const _json138586393[] = "138586393";
	static constexpr char const _json138586397[] = "138586397";
	static constexpr char const _json138586401[] = "138586401";
	static constexpr char const _json138586405[] = "138586405";
	static constexpr char const _json138586409[] = "138586409";
	static constexpr char const _json138586413[] = "138586413";
	static constexpr char const _json138586417[] = "138586417";
	static constexpr char const _json138586421[] = "138586421";
	static constexpr char const _json138586425[] = "138586425";
	static constexpr char const _json138586429[] = "138586429";
	static constexpr char const _json138586433[] = "138586433";
	static constexpr char const _json138586437[] = "138586437";
	static constexpr char const _json138586441[] = "138586441";
	static constexpr char const _json138586445[] = "138586445";
	static constexpr char const _json138586449[] = "138586449";
	static constexpr char const _json138586453[] = "138586453";
	static constexpr char const _json138586457[] = "138586457";
	static constexpr char const _json138586461[] = "138586461";
	static constexpr char const _json138586465[] = "138586465";
	static constexpr char const _json138586469[] = "138586469";
	static constexpr char const _json138586473[] = "138586473";
	static constexpr char const _json138586477[] = "138586477";
	static constexpr char const _json138586481[] = "138586481";
	static constexpr char const _json138586485[] = "138586485";
	static constexpr char const _json138586501[] = "138586501";
	static constexpr char const _json138586505[] = "138586505";
	static constexpr char const _json138586509[] = "138586509";
	static constexpr char const _json138586513[] = "138586513";
	static constexpr char const _json138586517[] = "138586517";
	static constexpr char const _json138586521[] = "138586521";
	static constexpr char const _json138586525[] = "138586525";
	static constexpr char const _json138586529[] = "138586529";
	static constexpr char const _json138586533[] = "138586533";
	static constexpr char const _json138586537[] = "138586537";
	static constexpr char const _json138586541[] = "138586541";
	static constexpr char const _json138586545[] = "138586545";
	static constexpr char const _json138586549[] = "138586549";
	static constexpr char const _json138586553[] = "138586553";
	static constexpr char const _json138586557[] = "138586557";
	static constexpr char const _json138586561[] = "138586561";
	static constexpr char const _json138586565[] = "138586565";
	static constexpr char const _json138586569[] = "138586569";
	static constexpr char const _json138586581[] = "138586581";
	static constexpr char const _json138586585[] = "138586585";
	static constexpr char const _json138586589[] = "138586589";
	static constexpr char const _json138586593[] = "138586593";
	static constexpr char const _json138586597[] = "138586597";
	static constexpr char const _json138586601[] = "138586601";
	static constexpr char const _json138586605[] = "138586605";
	static constexpr char const _json138586609[] = "138586609";
	static constexpr char const _json138586613[] = "138586613";
	static constexpr char const _json138586617[] = "138586617";
	static constexpr char const _json138586625[] = "138586625";
	static constexpr char const _json138586629[] = "138586629";
	static constexpr char const _json138586633[] = "138586633";
	static constexpr char const _json138586641[] = "138586641";
	static constexpr char const _json138586645[] = "138586645";
	static constexpr char const _json138586649[] = "138586649";
	static constexpr char const _json138586653[] = "138586653";
	static constexpr char const _json138586657[] = "138586657";
	static constexpr char const _json138586661[] = "138586661";
	static constexpr char const _json138586667[] = "138586667";
	static constexpr char const _json138586671[] = "138586671";
	static constexpr char const _json138586675[] = "138586675";
	static constexpr char const _json138586679[] = "138586679";
	static constexpr char const _json138586687[] = "138586687";
	static constexpr char const _json138586691[] = "138586691";
	static constexpr char const _json138586695[] = "138586695";
	static constexpr char const _json138586699[] = "138586699";
	static constexpr char const _json138586703[] = "138586703";
	static constexpr char const _json138586707[] = "138586707";
	static constexpr char const _json138586711[] = "138586711";
	static constexpr char const _json138586715[] = "138586715";
	static constexpr char const _json138586719[] = "138586719";
	static constexpr char const _json138586723[] = "138586723";
	static constexpr char const _json138586731[] = "138586731";
	static constexpr char const _json138586735[] = "138586735";
	static constexpr char const _json138586743[] = "138586743";
	static constexpr char const _json138586747[] = "138586747";
	static constexpr char const _json138586751[] = "138586751";
	static constexpr char const _json138586755[] = "138586755";
	static constexpr char const _json138586759[] = "138586759";
	static constexpr char const _json138586771[] = "138586771";
	static constexpr char const _json138586775[] = "138586775";
	static constexpr char const _json138586779[] = "138586779";
	static constexpr char const _json138586783[] = "138586783";
	static constexpr char const _json138586787[] = "138586787";
	static constexpr char const _json138586795[] = "138586795";
	static constexpr char const _json138586799[] = "138586799";
	static constexpr char const _json138586803[] = "138586803";
	static constexpr char const _json138586807[] = "138586807";
	static constexpr char const _json138586811[] = "138586811";
	static constexpr char const _json138586815[] = "138586815";
	static constexpr char const _json138586823[] = "138586823";
	static constexpr char const _json138586827[] = "138586827";
	static constexpr char const _json138586831[] = "138586831";
	static constexpr char const _json138586835[] = "138586835";
	static constexpr char const _json138586839[] = "138586839";
	static constexpr char const _json138586843[] = "138586843";
	static constexpr char const _json138586847[] = "138586847";
	static constexpr char const _json138586851[] = "138586851";
	static constexpr char const _json138586855[] = "138586855";
	static constexpr char const _json138586859[] = "138586859";
	static constexpr char const _json138586863[] = "138586863";
	static constexpr char const _json138586867[] = "138586867";
	static constexpr char const _json138586871[] = "138586871";
	static constexpr char const _json138586875[] = "138586875";
	static constexpr char const _json138586879[] = "138586879";
	static constexpr char const _json138586883[] = "138586883";
	static constexpr char const _json138586889[] = "138586889";
	static constexpr char const _json138586893[] = "138586893";
	static constexpr char const _json138586897[] = "138586897";
	static constexpr char const _json138586901[] = "138586901";
	static constexpr char const _json138586905[] = "138586905";
	static constexpr char const _json138586909[] = "138586909";
	static constexpr char const _json138586917[] = "138586917";
	static constexpr char const _json138586921[] = "138586921";
	static constexpr char const _json138586929[] = "138586929";
	static constexpr char const _json138586933[] = "138586933";
	static constexpr char const _json138586937[] = "138586937";
	static constexpr char const _json138586941[] = "138586941";
	static constexpr char const _json138586945[] = "138586945";
	static constexpr char const _json138586949[] = "138586949";
	static constexpr char const _json138586953[] = "138586953";
	static constexpr char const _json138586957[] = "138586957";
	static constexpr char const _json138586961[] = "138586961";
	static constexpr char const _json138586965[] = "138586965";
	static constexpr char const _json138586969[] = "138586969";
	static constexpr char const _json138586973[] = "138586973";
	static constexpr char const _json138586977[] = "138586977";
	static constexpr char const _json138586981[] = "138586981";
	static constexpr char const _json138586989[] = "138586989";
	static constexpr char const _json138586993[] = "138586993";
	static constexpr char const _json138586997[] = "138586997";
	static constexpr char const _json151183114[] = "151183114";
	static constexpr char const _json339420802[] = "339420802";
	static constexpr char const _json339420805[] = "339420805";
	static constexpr char const _json341069930[] = "341069930";
	static constexpr char const _json341181232[] = "341181232";
	static constexpr char const _json341181233[] = "341181233";
	static constexpr char const _json341181234[] = "341181234";
	static constexpr char const _json341181235[] = "341181235";
	static constexpr char const _json341181236[] = "341181236";
	static constexpr char const _json341181237[] = "341181237";
	static constexpr char const _json341181238[] = "341181238";
	static constexpr char const _json341181239[] = "341181239";
	static constexpr char const _json341181240[] = "341181240";
	static constexpr char const _json341181241[] = "341181241";
	static constexpr char const _json341181242[] = "341181242";
	static constexpr char const _json341181243[] = "341181243";
	static constexpr char const _json341181244[] = "341181244";
	static constexpr char const _json341181245[] = "341181245";
	static constexpr char const _json341181246[] = "341181246";
	static constexpr char const _json341181247[] = "341181247";
	static constexpr char const _json341181248[] = "341181248";
	static constexpr char const _json341181249[] = "341181249";
	static constexpr char const _json341181250[] = "341181250";
	static constexpr char const _json341181251[] = "341181251";
	static constexpr char const _json341181252[] = "341181252";
	static constexpr char const _json341181253[] = "341181253";
	static constexpr char const _json341181254[] = "341181254";
	static constexpr char const _json341181255[] = "341181255";
	static constexpr char const _json341181256[] = "341181256";
	static constexpr char const _json341181257[] = "341181257";
	static constexpr char const _json341181258[] = "341181258";
	static constexpr char const _json341181259[] = "341181259";
	static constexpr char const _json342742592[] = "342742592";
	static constexpr char const _json342742593[] = "342742593";
	static constexpr char const _json342742594[] = "342742594";
	static constexpr char const _json342742595[] = "342742595";
	static constexpr char const _json342742596[] = "342742596";
	return daw::json::class_description_t<
		json_class<_json138586341, _json138586341_t>
		,json_class<_json138586345, _json138586345_t>
		,json_class<_json138586349, _json138586349_t>
		,json_class<_json138586353, _json138586353_t>
		,json_class<_json138586357, _json138586357_t>
		,json_class<_json138586361, _json138586361_t>
		,json_class<_json138586365, _json138586365_t>
		,json_class<_json138586369, _json138586369_t>
		,json_class<_json138586373, _json138586373_t>
		,json_class<_json138586377, _json138586377_t>
		,json_class<_json138586381, _json138586381_t>
		,json_class<_json138586385, _json138586385_t>
		,json_class<_json138586389, _json138586389_t>
		,json_class<_json138586393, _json138586393_t>
		,json_class<_json138586397, _json138586397_t>
		,json_class<_json138586401, _json138586401_t>
		,json_class<_json138586405, _json138586405_t>
		,json_class<_json138586409, _json138586409_t>
		,json_class<_json138586413, _json138586413_t>
		,json_class<_json138586417, _json138586417_t>
		,json_class<_json138586421, _json138586421_t>
		,json_class<_json138586425, _json138586425_t>
		,json_class<_json138586429, _json138586429_t>
		,json_class<_json138586433, _json138586433_t>
		,json_class<_json138586437, _json138586437_t>
		,json_class<_json138586441, _json138586441_t>
		,json_class<_json138586445, _json138586445_t>
		,json_class<_json138586449, _json138586449_t>
		,json_class<_json138586453, _json138586453_t>
		,json_class<_json138586457, _json138586457_t>
		,json_class<_json138586461, _json138586461_t>
		,json_class<_json138586465, _json138586465_t>
		,json_class<_json138586469, _json138586469_t>
		,json_class<_json138586473, _json138586473_t>
		,json_class<_json138586477, _json138586477_t>
		,json_class<_json138586481, _json138586481_t>
		,json_class<_json138586485, _json138586485_t>
		,json_class<_json138586501, _json138586501_t>
		,json_class<_json138586505, _json138586505_t>
		,json_class<_json138586509, _json138586509_t>
		,json_class<_json138586513, _json138586513_t>
		,json_class<_json138586517, _json138586517_t>
		,json_class<_json138586521, _json138586521_t>
		,json_class<_json138586525, _json138586525_t>
		,json_class<_json138586529, _json138586529_t>
		,json_class<_json138586533, _json138586533_t>
		,json_class<_json138586537, _json138586537_t>
		,json_class<_json138586541, _json138586541_t>
		,json_class<_json138586545, _json138586545_t>
		,json_class<_json138586549, _json138586549_t>
		,json_class<_json138586553, _json138586553_t>
		,json_class<_json138586557, _json138586557_t>
		,json_class<_json138586561, _json138586561_t>
		,json_class<_json138586565, _json138586565_t>
		,json_class<_json138586569, _json138586569_t>
		,json_class<_json138586581, _json138586581_t>
		,json_class<_json138586585, _json138586585_t>
		,json_class<_json138586589, _json138586589_t>
		,json_class<_json138586593, _json138586593_t>
		,json_class<_json138586597, _json138586597_t>
		,json_class<_json138586601, _json138586601_t>
		,json_class<_json138586605, _json138586605_t>
		,json_class<_json138586609, _json138586609_t>
		,json_class<_json138586613, _json138586613_t>
		,json_class<_json138586617, _json138586617_t>
		,json_class<_json138586625, _json138586625_t>
		,json_class<_json138586629, _json138586629_t>
		,json_class<_json138586633, _json138586633_t>
		,json_class<_json138586641, _json138586641_t>
		,json_class<_json138586645, _json138586645_t>
		,json_class<_json138586649, _json138586649_t>
		,json_class<_json138586653, _json138586653_t>
		,json_class<_json138586657, _json138586657_t>
		,json_class<_json138586661, _json138586661_t>
		,json_class<_json138586667, _json138586667_t>
		,json_class<_json138586671, _json138586671_t>
		,json_class<_json138586675, _json138586675_t>
		,json_class<_json138586679, _json138586679_t>
		,json_class<_json138586687, _json138586687_t>
		,json_class<_json138586691, _json138586691_t>
		,json_class<_json138586695, _json138586695_t>
		,json_class<_json138586699, _json138586699_t>
		,json_class<_json138586703, _json138586703_t>
		,json_class<_json138586707, _json138586707_t>
		,json_class<_json138586711, _json138586711_t>
		,json_class<_json138586715, _json138586715_t>
		,json_class<_json138586719, _json138586719_t>
		,json_class<_json138586723, _json138586723_t>
		,json_class<_json138586731, _json138586731_t>
		,json_class<_json138586735, _json138586735_t>
		,json_class<_json138586743, _json138586743_t>
		,json_class<_json138586747, _json138586747_t>
		,json_class<_json138586751, _json138586751_t>
		,json_class<_json138586755, _json138586755_t>
		,json_class<_json138586759, _json138586759_t>
		,json_class<_json138586771, _json138586771_t>
		,json_class<_json138586775, _json138586775_t>
		,json_class<_json138586779, _json138586779_t>
		,json_class<_json138586783, _json138586783_t>
		,json_class<_json138586787, _json138586787_t>
		,json_class<_json138586795, _json138586795_t>
		,json_class<_json138586799, _json138586799_t>
		,json_class<_json138586803, _json138586803_t>
		,json_class<_json138586807, _json138586807_t>
		,json_class<_json138586811, _json138586811_t>
		,json_class<_json138586815, _json138586815_t>
		,json_class<_json138586823, _json138586823_t>
		,json_class<_json138586827, _json138586827_t>
		,json_class<_json138586831, _json138586831_t>
		,json_class<_json138586835, _json138586835_t>
		,json_class<_json138586839, _json138586839_t>
		,json_class<_json138586843, _json138586843_t>
		,json_class<_json138586847, _json138586847_t>
		,json_class<_json138586851, _json138586851_t>
		,json_class<_json138586855, _json138586855_t>
		,json_class<_json138586859, _json138586859_t>
		,json_class<_json138586863, _json138586863_t>
		,json_class<_json138586867, _json138586867_t>
		,json_class<_json138586871, _json138586871_t>
		,json_class<_json138586875, _json138586875_t>
		,json_class<_json138586879, _json138586879_t>
		,json_class<_json138586883, _json138586883_t>
		,json_class<_json138586889, _json138586889_t>
		,json_class<_json138586893, _json138586893_t>
		,json_class<_json138586897, _json138586897_t>
		,json_class<_json138586901, _json138586901_t>
		,json_class<_json138586905, _json138586905_t>
		,json_class<_json138586909, _json138586909_t>
		,json_class<_json138586917, _json138586917_t>
		,json_class<_json138586921, _json138586921_t>
		,json_class<_json138586929, _json138586929_t>
		,json_class<_json138586933, _json138586933_t>
		,json_class<_json138586937, _json138586937_t>
		,json_class<_json138586941, _json138586941_t>
		,json_class<_json138586945, _json138586945_t>
		,json_class<_json138586949, _json138586949_t>
		,json_class<_json138586953, _json138586953_t>
		,json_class<_json138586957, _json138586957_t>
		,json_class<_json138586961, _json138586961_t>
		,json_class<_json138586965, _json138586965_t>
		,json_class<_json138586969, _json138586969_t>
		,json_class<_json138586973, _json138586973_t>
		,json_class<_json138586977, _json138586977_t>
		,json_class<_json138586981, _json138586981_t>
		,json_class<_json138586989, _json138586989_t>
		,json_class<_json138586993, _json138586993_t>
		,json_class<_json138586997, _json138586997_t>
		,json_class<_json151183114, _json151183114_t>
		,json_class<_json339420802, _json339420802_t>
		,json_class<_json339420805, _json339420805_t>
		,json_class<_json341069930, _json341069930_t>
		,json_class<_json341181232, _json341181232_t>
		,json_class<_json341181233, _json341181233_t>
		,json_class<_json341181234, _json341181234_t>
		,json_class<_json341181235, _json341181235_t>
		,json_class<_json341181236, _json341181236_t>
		,json_class<_json341181237, _json341181237_t>
		,json_class<_json341181238, _json341181238_t>
		,json_class<_json341181239, _json341181239_t>
		,json_class<_json341181240, _json341181240_t>
		,json_class<_json341181241, _json341181241_t>
		,json_class<_json341181242, _json341181242_t>
		,json_class<_json341181243, _json341181243_t>
		,json_class<_json341181244, _json341181244_t>
		,json_class<_json341181245, _json341181245_t>
		,json_class<_json341181246, _json341181246_t>
		,json_class<_json341181247, _json341181247_t>
		,json_class<_json341181248, _json341181248_t>
		,json_class<_json341181249, _json341181249_t>
		,json_class<_json341181250, _json341181250_t>
		,json_class<_json341181251, _json341181251_t>
		,json_class<_json341181252, _json341181252_t>
		,json_class<_json341181253, _json341181253_t>
		,json_class<_json341181254, _json341181254_t>
		,json_class<_json341181255, _json341181255_t>
		,json_class<_json341181256, _json341181256_t>
		,json_class<_json341181257, _json341181257_t>
		,json_class<_json341181258, _json341181258_t>
		,json_class<_json341181259, _json341181259_t>
		,json_class<_json342742592, _json342742592_t>
		,json_class<_json342742593, _json342742593_t>
		,json_class<_json342742594, _json342742594_t>
		,json_class<_json342742595, _json342742595_t>
		,json_class<_json342742596, _json342742596_t>
>{};
}

inline auto to_json_data( events_t const & value ) {
	return std::forward_as_tuple( value._json138586341, value._json138586345, value._json138586349, value._json138586353, value._json138586357, value._json138586361, value._json138586365, value._json138586369, value._json138586373, value._json138586377, value._json138586381, value._json138586385, value._json138586389, value._json138586393, value._json138586397, value._json138586401, value._json138586405, value._json138586409, value._json138586413, value._json138586417, value._json138586421, value._json138586425, value._json138586429, value._json138586433, value._json138586437, value._json138586441, value._json138586445, value._json138586449, value._json138586453, value._json138586457, value._json138586461, value._json138586465, value._json138586469, value._json138586473, value._json138586477, value._json138586481, value._json138586485, value._json138586501, value._json138586505, value._json138586509, value._json138586513, value._json138586517, value._json138586521, value._json138586525, value._json138586529, value._json138586533, value._json138586537, value._json138586541, value._json138586545, value._json138586549, value._json138586553, value._json138586557, value._json138586561, value._json138586565, value._json138586569, value._json138586581, value._json138586585, value._json138586589, value._json138586593, value._json138586597, value._json138586601, value._json138586605, value._json138586609, value._json138586613, value._json138586617, value._json138586625, value._json138586629, value._json138586633, value._json138586641, value._json138586645, value._json138586649, value._json138586653, value._json138586657, value._json138586661, value._json138586667, value._json138586671, value._json138586675, value._json138586679, value._json138586687, value._json138586691, value._json138586695, value._json138586699, value._json138586703, value._json138586707, value._json138586711, value._json138586715, value._json138586719, value._json138586723, value._json138586731, value._json138586735, value._json138586743, value._json138586747, value._json138586751, value._json138586755, value._json138586759, value._json138586771, value._json138586775, value._json138586779, value._json138586783, value._json138586787, value._json138586795, value._json138586799, value._json138586803, value._json138586807, value._json138586811, value._json138586815, value._json138586823, value._json138586827, value._json138586831, value._json138586835, value._json138586839, value._json138586843, value._json138586847, value._json138586851, value._json138586855, value._json138586859, value._json138586863, value._json138586867, value._json138586871, value._json138586875, value._json138586879, value._json138586883, value._json138586889, value._json138586893, value._json138586897, value._json138586901, value._json138586905, value._json138586909, value._json138586917, value._json138586921, value._json138586929, value._json138586933, value._json138586937, value._json138586941, value._json138586945, value._json138586949, value._json138586953, value._json138586957, value._json138586961, value._json138586965, value._json138586969, value._json138586973, value._json138586977, value._json138586981, value._json138586989, value._json138586993, value._json138586997, value._json151183114, value._json339420802, value._json339420805, value._json341069930, value._json341181232, value._json341181233, value._json341181234, value._json341181235, value._json341181236, value._json341181237, value._json341181238, value._json341181239, value._json341181240, value._json341181241, value._json341181242, value._json341181243, value._json341181244, value._json341181245, value._json341181246, value._json341181247, value._json341181248, value._json341181249, value._json341181250, value._json341181251, value._json341181252, value._json341181253, value._json341181254, value._json341181255, value._json341181256, value._json341181257, value._json341181258, value._json341181259, value._json342742592, value._json342742593, value._json342742594, value._json342742595, value._json342742596 );
}

struct prices_element_t {
	int64_t amount;
	int64_t audienceSubCategoryId;
	int64_t seatCategoryId;
};	// prices_element_t

inline auto describe_json_class( prices_element_t ) {
	using namespace daw::json;
	static constexpr char const amount[] = "amount";
	static constexpr char const audienceSubCategoryId[] = "audienceSubCategoryId";
	static constexpr char const seatCategoryId[] = "seatCategoryId";
	return daw::json::class_description_t<
		json_number<amount, intmax_t>
		,json_number<audienceSubCategoryId, intmax_t>
		,json_number<seatCategoryId, intmax_t>
>{};
}

inline auto to_json_data( prices_element_t const & value ) {
	return std::forward_as_tuple( value.amount, value.audienceSubCategoryId, value.seatCategoryId );
}

struct areas_element_t {
	int64_t areaId;
};	// areas_element_t

inline auto describe_json_class( areas_element_t ) {
	using namespace daw::json;
	static constexpr char const areaId[] = "areaId";
	return daw::json::class_description_t<
		json_number<areaId, intmax_t>
>{};
}

inline auto to_json_data( areas_element_t const & value ) {
	return std::forward_as_tuple( value.areaId );
}

struct seatCategories_element_t {
	std::vector<areas_element_t> areas;
	int64_t seatCategoryId;
};	// seatCategories_element_t

inline auto describe_json_class( seatCategories_element_t ) {
	using namespace daw::json;
	static constexpr char const areas[] = "areas";
	static constexpr char const seatCategoryId[] = "seatCategoryId";
	return daw::json::class_description_t<
		json_array<areas, std::vector<areas_element_t>, json_class<no_name, areas_element_t>>
		,json_number<seatCategoryId, intmax_t>
>{};
}

inline auto to_json_data( seatCategories_element_t const & value ) {
	return std::forward_as_tuple( value.areas, value.seatCategoryId );
}

struct performances_element_t {
	int64_t eventId;
	int64_t id;
	std::optional<std::string_view> logo;
	std::vector<prices_element_t> prices;
	std::vector<seatCategories_element_t> seatCategories;
	int64_t start;
	std::string_view venueCode;
};	// performances_element_t

inline auto describe_json_class( performances_element_t ) {
	using namespace daw::json;
	static constexpr char const eventId[] = "eventId";
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const prices[] = "prices";
	static constexpr char const seatCategories[] = "seatCategories";
	static constexpr char const start[] = "start";
	static constexpr char const venueCode[] = "venueCode";
	return daw::json::class_description_t<
		json_number<eventId, intmax_t>
		,json_number<id, intmax_t>
		,json_nullable<json_string<logo, std::string_view>>
		,json_array<prices, std::vector<prices_element_t>, json_class<no_name, prices_element_t>>
		,json_array<seatCategories, std::vector<seatCategories_element_t>, json_class<no_name, seatCategories_element_t>>
		,json_number<start, intmax_t>
		,json_string<venueCode, std::string_view>
>{};
}

inline auto to_json_data( performances_element_t const & value ) {
	return std::forward_as_tuple( value.eventId, value.id, value.logo, value.prices, value.seatCategories, value.start, value.venueCode );
}

struct subjectNames_t {
};	// subjectNames_t

inline auto describe_json_class( subjectNames_t ) {
	using namespace daw::json;
	return daw::json::class_description_t<
>{};
}

inline auto to_json_data( subjectNames_t const & value ) {
	return std::forward_as_tuple(  );
}

struct venueNames_t {
	std::string_view PLEYEL_PLEYEL;
};	// venueNames_t

inline auto describe_json_class( venueNames_t ) {
	using namespace daw::json;
	static constexpr char const PLEYEL_PLEYEL[] = "PLEYEL_PLEYEL";
	return daw::json::class_description_t<
		json_string<PLEYEL_PLEYEL, std::string_view>
>{};
}

inline auto to_json_data( venueNames_t const & value ) {
	return std::forward_as_tuple( value.PLEYEL_PLEYEL );
}

struct citm_object_t {
	std::unordered_map<std::string_view, std::string_view> areaNames;
	audienceSubCategoryNames_t audienceSubCategoryNames;
	blockNames_t blockNames;
	events_t events;
	std::vector<performances_element_t> performances;
	std::map<std::string, std::string> seatCategoryNames;
	std::map<std::string, std::string> subTopicNames;
	subjectNames_t subjectNames;
	std::map<std::string, std::string> topicNames;
	std::map<std::string, std::vector<int64_t>> topicSubTopics;
	venueNames_t venueNames;
};	// root_object_t

inline auto describe_json_class( citm_object_t ) {
	using namespace daw::json;
	static constexpr char const areaNames[] = "areaNames";
	static constexpr char const audienceSubCategoryNames[] = "audienceSubCategoryNames";
	static constexpr char const blockNames[] = "blockNames";
	static constexpr char const events[] = "events";
	static constexpr char const performances[] = "performances";
	static constexpr char const seatCategoryNames[] = "seatCategoryNames";
	static constexpr char const subTopicNames[] = "subTopicNames";
	static constexpr char const subjectNames[] = "subjectNames";
	static constexpr char const topicNames[] = "topicNames";
	static constexpr char const topicSubTopics[] = "topicSubTopics";
	static constexpr char const venueNames[] = "venueNames";
	return daw::json::class_description_t<
		json_key_value<areaNames, >
		,json_class<audienceSubCategoryNames, audienceSubCategoryNames_t>
		,json_class<blockNames, blockNames_t>
		,json_class<events, events_t>
		,json_array<performances, std::vector<performances_element_t>, json_class<no_name, performances_element_t>>
		,json_class<seatCategoryNames, std::map<std::string, std::string>, json_string<no_name>>
		,json_key_value<subTopicNames, std::map<std::string, std::string>, json_string<no_name>>
		,json_class<subjectNames, subjectNames_t>
		,json_key_value<topicNames, std::map<std::string, std::string>, json_string<no_name>>
		,json_key_value<topicSubTopics, std::map<std::string, std::vector<int64_t>>, json_array<no_name, int64_t>>
		,json_class<venueNames, venueNames_t>
>{};
}

inline auto to_json_data( citm_object_t const & value ) {
	return std::forward_as_tuple( value.areaNames, value.audienceSubCategoryNames, value.blockNames, value.events, value.performances, value.seatCategoryNames, value.subTopicNames, value.subjectNames, value.topicNames, value.topicSubTopics, value.venueNames );
}

