// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_from_json_fwd.h"
#include "impl/daw_json_parse_policy.h"
#include "impl/daw_json_value.h"
#include "impl/version.h"

#include <daw/daw_move.h>

#include <ciso646>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		enum json_parse_handler_result { Continue, SkipClassArray, Complete };
		namespace json_details {
			struct handler_result_holder {
				json_parse_handler_result value = json_parse_handler_result::Continue;

				constexpr handler_result_holder( ) = default;

				constexpr handler_result_holder( bool b )
				  : value( b ? Continue : Complete ) {}

				constexpr handler_result_holder( json_parse_handler_result r )
				  : value( r ) {}

				constexpr explicit operator bool( ) const {
					return value == json_parse_handler_result::Continue;
				}
			};

			namespace hnd_checks {
				// On Next Value
				template<typename Handler, typename ParseState>
				using has_on_value_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_value(
				    std::declval<basic_json_pair<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_value_handler_v =
				  daw::is_detected_v<has_on_value_handler_detect, Handler, ParseState>;

				// On Array Start
				template<typename Handler, typename ParseState>
				using has_on_array_start_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_array_start(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_array_start_handler_v =
				  daw::is_detected_v<has_on_array_start_handler_detect, Handler,
				                     ParseState>;

				// On Array End
				template<typename Handler>
				using has_on_array_end_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_array_end( ) );

				template<typename Handler>
				inline constexpr bool has_on_array_end_handler_v =
				  daw::is_detected_v<has_on_array_end_handler_detect, Handler>;

				// On Class Start
				template<typename Handler, typename ParseState>
				using has_on_class_start_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_class_start(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_class_start_handler_v =
				  daw::is_detected_v<has_on_class_start_handler_detect, Handler,
				                     ParseState>;

				// On Class End
				template<typename Handler>
				using has_on_class_end_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_class_end( ) );

				template<typename Handler>
				inline constexpr bool has_on_class_end_handler_v =
				  daw::is_detected_v<has_on_class_end_handler_detect, Handler>;

				// On Number
				template<typename Handler, typename ParseState>
				using has_on_number_handler_detect_jv =
				  decltype( std::declval<Handler>( ).handle_on_number(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_number_handler_jv_v =
				  daw::is_detected_v<has_on_number_handler_detect_jv, Handler,
				                     ParseState>;

				template<typename Handler>
				using has_on_number_handler_detect_dbl =
				  decltype( std::declval<Handler>( ).handle_on_number( 0.0 ) );

				template<typename Handler>
				inline constexpr bool has_on_number_handler_dbl_v =
				  daw::is_detected_v<has_on_number_handler_detect_dbl, Handler>;

				// On Bool
				template<typename Handler, typename ParseState>
				using has_on_bool_handler_detect_jv =
				  decltype( std::declval<Handler>( ).handle_on_bool(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_bool_handler_jv_v =
				  daw::is_detected_v<has_on_bool_handler_detect_jv, Handler,
				                     ParseState>;

				template<typename Handler>
				using has_on_bool_handler_detect_bl =
				  decltype( std::declval<Handler>( ).handle_on_bool( true ) );

				template<typename Handler>
				inline constexpr bool has_on_bool_handler_bl_v =
				  daw::is_detected_v<has_on_bool_handler_detect_bl, Handler>;

				// On String
				template<typename Handler, typename ParseState>
				using has_on_string_handler_detect_jv =
				  decltype( std::declval<Handler>( ).handle_on_string(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_string_handler_jv_v =
				  daw::is_detected_v<has_on_string_handler_detect_jv, Handler,
				                     ParseState>;

				template<typename Handler>
				using has_on_string_handler_detect_str =
				  decltype( std::declval<Handler>( ).handle_on_string(
				    std::declval<std::string>( ) ) );

				template<typename Handler>
				inline constexpr bool has_on_string_handler_str_v =
				  daw::is_detected_v<has_on_string_handler_detect_str, Handler>;

				// On Null
				template<typename Handler, typename ParseState>
				using has_on_null_handler_detect_jv =
				  decltype( std::declval<Handler>( ).handle_on_null(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_null_handler_jv_v =
				  daw::is_detected_v<has_on_null_handler_detect_jv, Handler,
				                     ParseState>;

				template<typename Handler>
				using has_on_null_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_null( ) );

				template<typename Handler>
				inline constexpr bool has_on_null_handler_v =
				  daw::is_detected_v<has_on_null_handler_detect, Handler>;

				// On Error
				template<typename Handler, typename ParseState>
				using has_on_error_handler_detect =
				  decltype( std::declval<Handler>( ).handle_on_error(
				    std::declval<basic_json_value<ParseState>>( ) ) );

				template<typename Handler, typename ParseState>
				inline constexpr bool has_on_error_handler_v =
				  daw::is_detected_v<has_on_error_handler_detect, Handler, ParseState>;
			} // namespace hnd_checks

			template<typename T>
			constexpr daw::remove_cvref_t<T> as_copy( T &&value ) {
				return value;
			}

			template<typename Handler, typename ParsePolicy>
			inline constexpr handler_result_holder
			handle_on_value( Handler &&handler, basic_json_pair<ParsePolicy> p ) {
				if constexpr( hnd_checks::has_on_value_handler_v<Handler,
				                                                 ParsePolicy> ) {
					return handler.handle_on_value( DAW_MOVE( p ) );
				} else {
					(void)p;
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_array_start( Handler &&handler,
			                       basic_json_value<ParseState> jv ) {
				if constexpr( hnd_checks::has_on_array_start_handler_v<Handler,
				                                                       ParseState> ) {
					return handler.handle_on_array_start( DAW_MOVE( jv ) );
				} else {
					(void)jv;
					return handler_result_holder{ };
				}
			}

			template<typename Handler>
			inline constexpr handler_result_holder
			handle_on_array_end( Handler &&handler ) {
				if constexpr( hnd_checks::has_on_array_end_handler_v<Handler> ) {
					return handler.handle_on_array_end( );
				} else {
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_class_start( Handler &&handler,
			                       basic_json_value<ParseState> jv ) {
				if constexpr( hnd_checks::has_on_class_start_handler_v<Handler,
				                                                       ParseState> ) {
					return handler.handle_on_class_start( DAW_MOVE( jv ) );
				} else {
					(void)jv;
					return handler_result_holder{ };
				}
			}

			template<typename Handler>
			inline constexpr handler_result_holder
			handle_on_class_end( Handler &&handler ) {
				if constexpr( hnd_checks::has_on_class_end_handler_v<Handler> ) {
					return handler.handle_on_class_end( );
				} else {
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_number( Handler &&handler, basic_json_value<ParseState> &jv ) {
				if constexpr( hnd_checks::has_on_number_handler_jv_v<Handler,
				                                                     ParseState> ) {
					return handler.handle_on_number( as_copy( jv ) );
				} else if constexpr( hnd_checks::has_on_number_handler_dbl_v<
				                       Handler> ) {
					return handler.handle_on_number( from_json<double>( jv ) );
				} else {
					(void)jv;
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_bool( Handler &&handler, basic_json_value<ParseState> jv ) {
				if constexpr( hnd_checks::has_on_bool_handler_jv_v<Handler,
				                                                   ParseState> ) {
					return handler.handle_on_bool( as_copy( jv ) );
				} else if constexpr( hnd_checks::has_on_bool_handler_bl_v<Handler> ) {
					return handler.handle_on_bool( from_json<bool>( jv ) );
				} else {
					(void)jv;
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_string( Handler &&handler, basic_json_value<ParseState> &jv ) {
				if constexpr( hnd_checks::has_on_string_handler_jv_v<Handler,
				                                                     ParseState> ) {
					return handler.handle_on_string( as_copy( jv ) );
				} else if constexpr( hnd_checks::has_on_string_handler_str_v<
				                       Handler> ) {
					return handler.handle_on_string( jv.get_string( ) );
				} else {
					(void)jv;
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_null( Handler &&handler, basic_json_value<ParseState> &jv ) {
				if constexpr( hnd_checks::has_on_null_handler_jv_v<Handler,
				                                                   ParseState> ) {
					return handler.handle_on_null( as_copy( jv ) );
				} else if constexpr( hnd_checks::has_on_null_handler_v<Handler> ) {
					return handler.handle_on_null( );
				} else {
					return handler_result_holder{ };
				}
			}

			template<typename Handler, typename ParseState>
			inline constexpr handler_result_holder
			handle_on_error( Handler &&handler, basic_json_value<ParseState> jv ) {
				if constexpr( hnd_checks::has_on_error_handler_v<Handler,
				                                                 ParseState> ) {
					return handler.handle_on_error( DAW_MOVE( jv ) );
				} else {
					(void)jv;
					return handler_result_holder{ };
				}
			}

		} // namespace json_details
		enum class StackParseStateType { Class, Array };

		template<typename ParsePolicy>
		struct JsonEventParserStackValue {
			using iterator = basic_json_value_iterator<ParsePolicy>;
			StackParseStateType type;
			std::pair<iterator, iterator> value;
		};

		template<typename StackValue>
		class DefaultJsonEventParserStackPolicy {
			std::vector<StackValue> m_stack{ };

		public:
			using value_type = StackValue;
			using reference = StackValue &;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			CPP20CONSTEXPR DefaultJsonEventParserStackPolicy( ) = default;

			CPP20CONSTEXPR inline void push_back( value_type &&v ) {
				m_stack.push_back( DAW_MOVE( v ) );
			}

			[[nodiscard]] CPP20CONSTEXPR inline reference back( ) {
				return m_stack.back( );
			}

			CPP20CONSTEXPR inline void clear( ) {
				m_stack.clear( );
			}

			CPP20CONSTEXPR inline void pop_back( ) {
				m_stack.pop_back( );
			}

			[[nodiscard]] CPP20CONSTEXPR inline bool empty( ) const {
				return m_stack.empty( );
			}
		};

		template<typename ParsePolicy = NoCommentSkippingPolicyChecked,
		         typename StackContainerPolicy = DefaultJsonEventParserStackPolicy<
		           JsonEventParserStackValue<ParsePolicy>>,
		         typename Handler>
		inline constexpr void
		json_event_parser( basic_json_value<ParsePolicy> jvalue,
		                   Handler &&handler ) {
			using iterator = basic_json_value_iterator<ParsePolicy>;
			using json_value_t = basic_json_pair<ParsePolicy>;
			using stack_value_t = JsonEventParserStackValue<ParsePolicy>;

			auto parent_stack = StackContainerPolicy( );

			auto const move_to_last = [&]( ) {
				parent_stack.back( ).value.first = parent_stack.back( ).value.second;
			};

			auto const process_value = [&]( json_value_t p ) {
				{
					auto result = json_details::handle_on_value( handler, p );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
				}

				auto &jv = p.value;
				switch( jv.type( ) ) {
				case JsonBaseParseTypes::Array: {
					auto result = json_details::handle_on_array_start( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
					parent_stack.push_back(
					  { StackParseStateType::Array,
					    std::pair<iterator, iterator>( jv.begin( ), jv.end( ) ) } );
				} break;
				case JsonBaseParseTypes::Class: {
					auto result = json_details::handle_on_class_start( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
					parent_stack.push_back(
					  { StackParseStateType::Class,
					    std::pair<iterator, iterator>( jv.begin( ), jv.end( ) ) } );
				} break;
				case JsonBaseParseTypes::Number: {
					auto result = json_details::handle_on_number( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
				} break;
				case JsonBaseParseTypes::Bool: {
					auto result = json_details::handle_on_bool( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
				} break;
				case JsonBaseParseTypes::String: {
					auto result = json_details::handle_on_string( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
				} break;
				case JsonBaseParseTypes::Null: {
					auto result = json_details::handle_on_null( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
				} break;
				case JsonBaseParseTypes::None:
				default: {
					auto result = json_details::handle_on_error( handler, jv );
					switch( result.value ) {
					case json_parse_handler_result::Complete:
						parent_stack.clear( );
						return;
					case json_parse_handler_result::SkipClassArray:
						move_to_last( );
						return;
					case json_parse_handler_result::Continue:
						break;
					}
				} break;
				}
			};

			auto const process_range = [&]( stack_value_t v ) {
				if( v.value.first != v.value.second ) {
					auto jv = *v.value.first;
					v.value.first++;
					parent_stack.push_back( DAW_MOVE( v ) );
					process_value( DAW_MOVE( jv ) );
				} else {
					switch( v.type ) {
					case StackParseStateType::Class: {
						auto result = json_details::handle_on_class_end( handler );
						switch( result.value ) {
						case json_parse_handler_result::Complete:
							parent_stack.clear( );
							return;
						case json_parse_handler_result::SkipClassArray:
						case json_parse_handler_result::Continue:
							break;
						}
					} break;
					case StackParseStateType::Array: {
						auto result = json_details::handle_on_array_end( handler );
						switch( result.value ) {
						case json_parse_handler_result::Complete:
							parent_stack.clear( );
							return;
						case json_parse_handler_result::SkipClassArray:
						case json_parse_handler_result::Continue:
							break;
						}
					} break;
					}
				}
			};

			process_value( { std::nullopt, DAW_MOVE( jvalue ) } );

			while( not parent_stack.empty( ) ) {
				auto v = DAW_MOVE( parent_stack.back( ) );
				parent_stack.pop_back( );
				process_range( v );
			}
		}

		template<typename ParsePolicy = NoCommentSkippingPolicyChecked,
		         typename Handler>
		inline void json_event_parser( std::string_view json_document,
		                               Handler &&handler ) {
			return json_event_parser( basic_json_value<ParsePolicy>( json_document ),
			                          DAW_FWD( handler ) );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
