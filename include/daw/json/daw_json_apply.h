// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_link.h"

#include <daw/daw_function_traits.h>

#include <string_view>
#include <tuple>
#include <type_traits>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename T, typename = void>
			inline constexpr bool has_call_operator = std::is_function_v<T>;

			template<typename T>
			inline constexpr bool has_call_operator<
			  T, std::void_t<decltype( &std::remove_reference_t<T>::operator( ) )>> =
			  true;

			template<typename Signature, typename Callable,
			         typename... FromJsonParams>
			DAW_ATTRIB_INLINE constexpr auto
			json_apply_impl( Callable &&callable, FromJsonParams &&...params ) {
				using func_t = typename std::conditional_t<
				  std::is_same_v<use_default, Signature>,
				  daw::traits::identity<daw::func::function_traits<Callable>>,
				  daw::traits::identity<daw::func::function_traits<Signature>>>::type;

				if constexpr( func_t::arity == 1 ) {
					return DAW_FWD( callable )(
					  daw::json::from_json<
					    typename func_t::template argument<0>::plain_type>(
					    DAW_FWD( params )... ) );
				} else {
					return std::apply(
					  DAW_FWD( callable ),
					  daw::json::from_json<typename func_t::plain_params_t>(
					    DAW_FWD( params )... ) );
				}
			}
		} // namespace json_details

		/// @brief Parse to parameters specified in signature and pass them to
		/// Callable for evaluation.  This is similar to std::apply but using JSON
		/// documents instead of tuples.
		/// @tparam Signature  The signature of the Callable.  Will try to auto
		/// deduce if not specified.  Does not work when there are overloads(e.g.
		/// auto params/template params/overloaded call operator or functions)
		/// @tparam String Type of json document
		/// @tparam Callable A callable to pass the parsed json document to
		/// @param json_doc The json document with serialized data to parse
		/// @param callable The callable used to evaluate the parsed values.
		/// @return The result of calling Callable
		template<typename Signature = use_default, typename String,
		         typename Callable,
		         std::enable_if_t<json_details::has_call_operator<Callable>,
		                          std::nullptr_t> = nullptr>
		constexpr auto json_apply( String &&json_doc, Callable &&callable ) {
			return json_details::json_apply_impl<Signature>( DAW_FWD( callable ),
			                                                 DAW_FWD( json_doc ) );
		}

		/// @brief Parse to parameters specified in signature and pass them to
		/// Callable for evaluation.  This is similar to std::apply but using JSON
		/// documents instead of tuples.
		/// @tparam Signature  The signature of the Callable.  Will try to auto
		/// deduce if not specified.  Does not work when there are overloads(e.g.
		/// auto params/template params/overloaded call operator or functions)
		/// @tparam String Type of json document
		/// @tparam Callable A callable to pass the parsed json document to
		/// @param json_doc The json document with serialized data to parse
		/// @param json_path The JSON Path to the element in the document to parse
		/// @param callable The callable used to evaluate the parsed values.
		/// @return The result of calling Callable
		template<typename Signature = use_default, typename String,
		         typename Callable,
		         std::enable_if_t<json_details::has_call_operator<Callable>,
		                          std::nullptr_t> = nullptr>
		constexpr auto json_apply( String &&json_doc, std::string_view json_path,
		                           Callable &&callable ) {
			return json_details::json_apply_impl<Signature>(
			  DAW_FWD( callable ), DAW_FWD( json_doc ), json_path );
		}

		/// @brief Parse to parameters specified in signature and pass them to
		/// Callable for evaluation.  This is similar to std::apply but using JSON
		/// documents instead of tuples.
		/// @tparam Signature  The signature of the Callable.  Will try to auto
		/// deduce if not specified.  Does not work when there are overloads(e.g.
		/// auto params/template params/overloaded call operator or functions)
		/// @tparam String Type of json document
		/// @tparam Callable A callable to pass the parsed json document to
		/// @tparam PolicyFlags Parser Policy flags.  See parser_policies.md
		/// @param json_doc The json document with serialized data to parse
		/// @param json_path The JSON Path to the element in the document to parse
		/// @param flags Parsing policy flags to change parser behaviour
		/// @param callable The callable used to evaluate the parsed values.
		/// @return The result of calling Callable
		template<typename Signature = use_default, typename String,
		         auto... PolicyFlags, typename Callable,
		         std::enable_if_t<json_details::has_call_operator<Callable>,
		                          std::nullptr_t> = nullptr>
		constexpr auto
		json_apply( String &&json_doc, std::string_view json_path,
		            daw::json::options::parse_flags_t<PolicyFlags...> flags,
		            Callable &&callable ) {
			return json_details::json_apply_impl<Signature>(
			  DAW_FWD( callable ), DAW_FWD( json_doc ), json_path, flags );
		}

		/// @brief Parse to parameters specified in signature and pass them to
		/// Callable for evaluation.  This is similar to std::apply but using JSON
		/// documents instead of tuples.
		/// @tparam Signature  The signature of the Callable.  Will try to auto
		/// deduce if not specified.  Does not work when there are overloads(e.g.
		/// auto params/template params/overloaded call operator or functions)
		/// @tparam String Type of json document
		/// @tparam Callable A callable to pass the parsed json document to
		/// @tparam PolicyFlags Parser Policy flags.  See parser_policies.md
		/// @param json_doc The json document with serialized data to parse
		/// @param flags Parsing policy flags to change parser behaviour
		/// @param callable The callable used to evaluate the parsed values.
		/// @return The result of calling Callable
		template<typename Signature = use_default, typename String,
		         auto... PolicyFlags, typename Callable,
		         std::enable_if_t<json_details::has_call_operator<Callable>,
		                          std::nullptr_t> = nullptr>
		constexpr auto
		json_apply( String &&json_doc,
		            daw::json::options::parse_flags_t<PolicyFlags...> flags,
		            Callable &&callable ) {
			return json_details::json_apply_impl<Signature>(
			  DAW_FWD( callable ), DAW_FWD( json_doc ), flags );
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
