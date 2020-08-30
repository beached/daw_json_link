#pragma once

#include "daw_json_assert.h"

namespace daw::json {
	struct throw_on_error_parse_policy {
		[[noreturn]] static void handle_error( ErrorType err,
		                                       std::string_view data ) {
			throw json_exception( data, err );
		}
	};

	struct abort_on_error_parse_policy {
		[[noreturn]] static void handle_error( ErrorType, std::string_view ) {
			std::abort( );
		}
	};

} // namespace daw::json
