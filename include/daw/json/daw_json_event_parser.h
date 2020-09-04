// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/daw_json_link.h"

namespace daw::json {
	struct handler_result {
		constexpr handler_result( ) = default;
		constexpr handler_result( bool ) {}
	};

	namespace json_details {
		namespace hnd_checks {
			// On Next Value
			template<typename Handler, typename ParsePolicy>
			using has_on_next_value_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_next_value(
			    std::declval<daw::json::basic_json_pair<ParsePolicy>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_next_value_handler_v =
			  daw::is_detected_v<has_on_next_value_handler_detect, Handler, Range>;

			// On Array Start
			template<typename Handler, typename Range>
			using has_on_array_start_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_array_start(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_array_start_handler_v =
			  daw::is_detected_v<has_on_array_start_handler_detect, Handler, Range>;

			// On Array End
			template<typename Handler>
			using has_on_array_end_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_array_end( ) );

			template<typename Handler>
			inline constexpr bool has_on_array_end_handler_v =
			  daw::is_detected_v<has_on_array_end_handler_detect, Handler>;

			// On Class Start
			template<typename Handler, typename Range>
			using has_on_class_start_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_class_start(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_class_start_handler_v =
			  daw::is_detected_v<has_on_class_start_handler_detect, Handler, Range>;

			// On Class End
			template<typename Handler>
			using has_on_class_end_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_class_end( ) );

			template<typename Handler>
			inline constexpr bool has_on_class_end_handler_v =
			  daw::is_detected_v<has_on_class_end_handler_detect, Handler>;

			// On Number
			template<typename Handler, typename Range>
			using has_on_number_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_number(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_number_handler_v =
			  daw::is_detected_v<has_on_number_handler_detect, Handler, Range>;

			// On Bool
			template<typename Handler, typename Range>
			using has_on_bool_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_bool(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_bool_handler_v =
			  daw::is_detected_v<has_on_bool_handler_detect, Handler, Range>;

			// On String
			template<typename Handler, typename Range>
			using has_on_string_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_string(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_string_handler_v =
			  daw::is_detected_v<has_on_string_handler_detect, Handler, Range>;

			// On Null
			template<typename Handler, typename Range>
			using has_on_null_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_null(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_null_handler_v =
			  daw::is_detected_v<has_on_null_handler_detect, Handler, Range>;

			// On Error
			template<typename Handler, typename Range>
			using has_on_error_handler_detect =
			  decltype( std::declval<Handler>( ).handle_on_error(
			    std::declval<daw::json::basic_json_value<Range>>( ) ) );

			template<typename Handler, typename Range>
			inline constexpr bool has_on_error_handler_v =
			  daw::is_detected_v<has_on_error_handler_detect, Handler, Range>;
		} // namespace hnd_checks

		template<typename Handler, typename ParsePolicy>
		handler_result
		handle_on_next_value( Handler &&handler,
		                      daw::json::basic_json_pair<ParsePolicy> p ) {
			if constexpr( hnd_checks::has_on_next_value_handler_v<Handler,
			                                                      ParsePolicy> ) {
				return handler.handle_on_next_value( std::move( p ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result
		handle_on_array_start( Handler &&handler,
		                       daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_array_start_handler_v<Handler, Range> ) {
				return handler.handle_on_array_start( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler>
		handler_result handle_on_array_end( Handler &&handler ) {
			if constexpr( hnd_checks::has_on_array_end_handler_v<Handler> ) {
				return handler.handle_on_array_start( );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result
		handle_on_class_start( Handler &&handler,
		                       daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_class_start_handler_v<Handler, Range> ) {
				return handler.handle_on_class_start( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler>
		handler_result handle_on_class_end( Handler &&handler ) {
			if constexpr( hnd_checks::has_on_class_end_handler_v<Handler> ) {
				return handler.handle_on_class_start( );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result handle_on_number( Handler &&handler,
		                                 daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_number_handler_v<Handler, Range> ) {
				return handler.handle_on_number( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result handle_on_bool( Handler &&handler,
		                               daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_bool_handler_v<Handler, Range> ) {
				return handler.handle_on_bool( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result handle_on_string( Handler &&handler,
		                                 daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_string_handler_v<Handler, Range> ) {
				return handler.handle_on_string( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result handle_on_null( Handler &&handler,
		                               daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_null_handler_v<Handler, Range> ) {
				return handler.handle_on_null( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}

		template<typename Handler, typename Range>
		handler_result handle_on_error( Handler &&handler,
		                                daw::json::basic_json_value<Range> jv ) {
			if constexpr( hnd_checks::has_on_error_handler_v<Handler, Range> ) {
				return handler.handle_on_error( std::move( jv ) );
			} else {
				return handler_result{ };
			}
		}
	} // namespace json_details

	template<typename ParsePolicy = NoCommentSkippingPolicyChecked,
	         typename StringView, typename Handler>
	inline void json_event_parser( StringView const &json_document,
	                               Handler &&handler ) {
		using iterator = daw::json::basic_json_value_iterator<ParsePolicy>;
		using json_value_t = daw::json::basic_json_pair<ParsePolicy>;
		enum class RangeType { Class, Array };
		struct stack_value_t {
			RangeType type;
			std::pair<iterator, iterator> value;
		};
		using min_stack_t = std::vector<stack_value_t>;

		auto parent_stack = min_stack_t( );

		auto const process_value = [&]( json_value_t p ) {
			auto h_result = json_details::handle_on_next_value( handler, p );
			// TODO: handler result
			(void)h_result;
			auto &jv = p.value;
			switch( jv.type( ) ) {
			case daw::json::JsonBaseParseTypes::Array: {
				auto result = json_details::handle_on_array_start( handler, jv );
				// TODO: handler result
				(void)result;
				parent_stack.push_back(
				  { RangeType::Array,
				    std::pair<iterator, iterator>( jv.begin( ), jv.end( ) ) } );
			} break;
			case daw::json::JsonBaseParseTypes::Class: {
				auto result = json_details::handle_on_class_start( handler, jv );
				// TODO: handler result
				(void)result;
				parent_stack.push_back(
				  { RangeType::Class,
				    std::pair<iterator, iterator>( jv.begin( ), jv.end( ) ) } );
			} break;
			case daw::json::JsonBaseParseTypes::Number: {
				auto result = json_details::handle_on_number( handler, jv );
				// TODO: handler result
				(void)result;
			} break;
			case daw::json::JsonBaseParseTypes::Bool: {
				auto result = json_details::handle_on_bool( handler, jv );
				// TODO: handler result
				(void)result;
			} break;
			case daw::json::JsonBaseParseTypes::String: {
				auto result = json_details::handle_on_string( handler, jv );
				// TODO: handler result
				(void)result;
			} break;
			case daw::json::JsonBaseParseTypes::Null: {
				auto result = json_details::handle_on_null( handler, jv );
				// TODO: handler result
				(void)result;
			} break;
			case daw::json::JsonBaseParseTypes::None:
			default: {
				auto result = json_details::handle_on_error( handler, jv );
				// TODO: handler result
				(void)result;
			} break;
			}
		};

		auto const process_range = [&]( stack_value_t v ) {
			if( v.value.first != v.value.second ) {
				auto jv = *v.value.first;
				v.value.first++;
				parent_stack.push_back( std::move( v ) );
				process_value( std::move( jv ) );
			} else {
				switch( v.type ) {
				case RangeType::Class: {
					auto result = json_details::handle_on_class_end( handler );
					// TODO: handler result
					(void)result;
				} break;
				case RangeType::Array: {
					auto result = json_details::handle_on_array_end( handler );
					// TODO: handler result
					(void)result;
				} break;
				}
			}
		};

		process_value(
		  { std::nullopt,
		    daw::json::basic_json_value<ParsePolicy>( std::string_view(
		      std::data( json_document ), std::size( json_document ) ) ) } );

		while( not parent_stack.empty( ) ) {
			auto v = std::move( parent_stack.back( ) );
			parent_stack.pop_back( );
			process_range( v );
		}
	}
} // namespace daw::json
