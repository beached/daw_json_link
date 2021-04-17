// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <daw/daw_json_link>
#include <string_view>
#include <vector>

/*
 * {
    "obj/isBeach/archives/xgGroundCover_archiveRock0002_geo.obj": {
        "xgPrim13021986_xgGroundCover": [
            -0.027957417791725492,
            -0.00010995744572427291,
            0.008245142748421388,
            0.0,
            0.0004173780477003665,
            0.02908927727233923,
            0.0018031701253846634,
            ]*/
namespace daw::island {
	struct point_t {
		double x;
		double y;
		double z;
		double w;
	};

	template<typename First, typename Last>
	struct IteratorPair {
		First first;
		Last last;
	};

	struct obj_t {
		std::string_view name, point_t vertices[4];

		template<typename First, typename Last>
		inline constexpr obj_t( std::string_view n,
		                        IteratorPairpoint_t<First, Last> points )
		  : name( n )
		  , vertices {
			{ *( points.first++ ), *( points.first++ ), *( points.first++ ),
			  *( points.first++ ) },
			  { *( points.first++ ), *( points.first++ ), *( points.first++ ),
			    *( points.first++ ) },
			  { *( points.first++ ), *( points.first++ ), *( points.first++ ),
			    *( points.first++ ) },
			{
				*( points.first++ ), *( points.first++ ), *( points.first++ ),
				  *( points.first++ )
			}
		}
	}
};

struct object_t {
	std::string object_name;
	std::vector<obj_t> objs;
};

struct island_t {
	std::vector<object_t> objects;
};
} // namespace daw::island

namespace daw::json {
	template<>
	daw::json::json_data_contract<daw::island::obj_t>{ using type = };
}
