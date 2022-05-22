// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/daw_utility.h>

#include "daw_json_link_types.h"
#include "impl/daw_json_serialize_policy.h"
#include "impl/to_daw_json_string.h"

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace utils {
			template<typename OutputIterator>
			inline constexpr OutputIterator output_kv( OutputIterator it,
			                                           std::string_view key,
			                                           std::string_view value ) {
				it = copy_to_iterator( it, key );
				*it++ = ':';
				it.output_space( );
				it = copy_to_iterator( it, value );
				return it;
			}
		} // namespace utils
		namespace json_details {
			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Bool>,
			                                         OutputIterator out_it ) {

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("boolean")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Custom>,
			                                         OutputIterator out_it ) {
				// TODO allow a trait to describe the valid literal types or if it
				// matches one of the other predefined types
				static_assert( JsonMember::custom_json_type ==
				               JsonCustomTypes::String );
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("string")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Date>,
			                                         OutputIterator out_it ) {

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("string")" );
				*out_it++ = ',';
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("format")", R"("date-time")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Real>,
			                                         OutputIterator out_it ) {

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("number")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Signed>,
			                                         OutputIterator out_it ) {

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("integer")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::StringEscaped>,
			                OutputIterator out_it ) {

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("string")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::StringRaw>,
			                OutputIterator out_it ) {

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("string")" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::Unsigned>,
			                OutputIterator out_it ) {
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
					out_it.next_member( );
				}
				out_it = utils::output_kv( out_it, R"("type")", R"("integer")" );
				*out_it++ = ',';
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("minimum")", "0" );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			/***
			 * Output the schema of a json_class and it's members
			 * @tparam JsonMember A json_class type
			 * @tparam is_root Is this the root item in the schema.
			 * @tparam OutputIterator An iterator type that allows for assigning to
			 * the result of operator* and pre/post-fix incrementing
			 * @param out_it Current OutputIterator
			 * @return the last value of out_it
			 */
			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Class>,
			                                         OutputIterator out_it );

			/***
			 * Output the schema of a json_array and it's element type
			 * @tparam JsonMember A json_array type
			 * @tparam is_root Is this the root item in the schema.
			 * @tparam OutputIterator An iterator type that allows for assigning to
			 * the result of operator and pre/post-fix incrementing
			 * @param out_it Current OutputIterator
			 * @return the last value of out_it
			 */
			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Array>,
			                                         OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Tuple>,
			                                         OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::SizedArray>,
			                OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::KeyValue>,
			                OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::KeyValueArray>,
			                OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::Variant>,
			                OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::VariantTagged>,
			                OutputIterator out_it );

			template<typename JsonMember, bool is_root = false,
			         typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::VariantIntrusive>,
			                OutputIterator out_it );

			template<typename, typename>
			struct json_class_processor;

			/// Specialization for processing json_class types that have a
			/// json_member_list of members.  This is for regular JSON objects
			/// \tparam OutputIterator Type that fulfills OutputIterator concept
			/// \tparam JsonMembers The member mappings for this class
			template<typename OutputIterator, typename... JsonMembers>
			struct json_class_processor<OutputIterator,
			                            json_member_list<JsonMembers...>> {

				static constexpr std::size_t size = sizeof...( JsonMembers );

				/// Output the schema for each member mapped in JsonMembers
				/// \param out_it The output iterator to write string data to
				/// \result The output iterator object after processing
				/// \pre out_it != nullptr if it is a pointer
				static constexpr OutputIterator process( OutputIterator out_it ) {
					out_it = utils::output_kv( out_it, R"("type")", R"("object")" );
					*out_it++ = ',';
					out_it.next_member( );
					out_it = utils::output_kv( out_it, R"("properties")", "{" );
					if constexpr( sizeof...( JsonMembers ) > 0 ) {
						out_it.add_indent( );
						out_it.next_member( );
						out_it = output_member_types(
						  out_it, std::index_sequence_for<JsonMembers...>{ } );
						out_it.del_indent( );
						out_it.next_member( );
					}
					*out_it++ = '}';
					*out_it++ = ',';
					out_it.next_member( );
					out_it = utils::output_kv( out_it, R"("required")", "[" );
					if constexpr( not is_empty_pack_v<JsonMembers...> ) {
						out_it.add_indent( );
						out_it = output_required_members( out_it );
						out_it.del_indent( );
						out_it.next_member( );
					}
					*out_it++ = ']';
					if constexpr( ( has_dependent_member_v<JsonMembers> or ... ) ) {
						*out_it++ = ',';
						out_it.next_member( );
						out_it = utils::output_kv( out_it, R"("dependencies")", "{" );
						out_it.add_indent( );
						bool is_first = true;
						out_it = static_cast<OutputIterator>(
						  ( output_dependency<
						      json_link_no_name<json_link_no_name<JsonMembers>>>(
						      out_it, is_first ),
						    ... ) );
						out_it.del_indent( );
						if( not is_first ) {
							// If we have at least 1 dependent member, is_first will be false
							out_it.next_member( );
						}
						*out_it++ = '}';
					}
					return out_it;
				}

			private:
				static constexpr auto indexer =
				  std::index_sequence_for<JsonMembers...>{ };

				///
				/// \tparam JsonMember
				/// \tparam Idx
				/// \param out_it
				/// \param seen
				/// \return
				template<typename JsonMember, std::size_t Idx>
				static constexpr OutputIterator
				output_member_type( OutputIterator &out_it, bool *seen ) {
					if( seen[Idx] ) {
						return out_it;
					} else {
						seen[Idx] = true;
					}

					*out_it++ = '"';
					out_it = utils::copy_to_iterator( out_it, JsonMember::name );
					out_it = utils::copy_to_iterator( out_it, R"(":)" );
					out_it.output_space( );
					out_it = to_json_schema<JsonMember>(
					  ParseTag<JsonMember::expected_type>{ }, out_it );
					if constexpr( Idx + 1 < sizeof...( JsonMembers ) ) {
						*out_it++ = ',';
						out_it.next_member( );
					}
					return out_it;
				}

				template<std::size_t... Is>
				static constexpr OutputIterator
				output_member_types( OutputIterator &out_it,
				                     std::index_sequence<Is...> ) {
					bool seen[sizeof...( JsonMembers )]{ };
					return static_cast<OutputIterator>(
					  ( output_member_type<JsonMembers, Is>( out_it, seen ), ... ) );
				}

				template<typename JsonMember>
				static constexpr OutputIterator
				output_required_member( OutputIterator &out_it, bool &is_first ) {
					if constexpr( JsonMember::nullable == JsonNullable::MustExist ) {
						if( not is_first ) {
							*out_it++ = ',';
						} else {
							is_first = false;
						}
						out_it.next_member( );
						*out_it++ = '"';
						out_it = utils::copy_to_iterator( out_it, JsonMember::name );
						*out_it++ = '"';
					}
					return out_it;
				}

				template<typename JsonMember>
				static constexpr OutputIterator
				output_dependency( OutputIterator &out_it, bool &is_first ) {
					if constexpr( has_dependent_member_v<JsonMember> ) {
						if( not is_first ) {
							*out_it++ = ',';
						} else {
							is_first = false;
						}
						out_it.next_member( );
						*out_it++ = '"';
						out_it = utils::copy_to_iterator( out_it, JsonMember::name );
						out_it = utils::copy_to_iterator( out_it, R"(":)" );
						out_it.output_space( );
						*out_it++ = '[';
						out_it.add_indent( );
						out_it.next_member( );
						*out_it++ = '"';
						out_it = utils::copy_to_iterator(
						  out_it, dependent_member_t<JsonMember>::name );
						*out_it++ = '"';
						out_it.del_indent( );
						out_it.next_member( );
						*out_it++ = ']';
					}
					return out_it;
				}

				static constexpr OutputIterator
				output_required_members( OutputIterator &out_it ) {
					bool is_first = true;
					return ( output_required_member<json_link_no_name<JsonMembers>>(
					           out_it, is_first ),
					         ... );
				}
			};

			template<typename OutputIterator, typename... JsonMembers>
			struct json_class_processor<OutputIterator,
			                            json_tuple_member_list<JsonMembers...>> {

				static constexpr std::size_t size = sizeof...( JsonMembers );
				static constexpr OutputIterator process( OutputIterator out_it ) {

					out_it = utils::output_kv( out_it, R"("type")", R"("array",)" );
					out_it.next_member( );
					out_it = utils::output_kv( out_it, R"("items")", "[" );
					if constexpr( sizeof...( JsonMembers ) > 0 ) {
						out_it.add_indent( );
						out_it = output_member_types( out_it );
						out_it.del_indent( );
						out_it.next_member( );
					}
					*out_it++ = ']';

					static_assert(
					  not( ( json_link_no_name<JsonMembers>::expected_type ==
					         JsonParseTypes::VariantTagged ) or
					       ... ),
					  "A tagged variant is not supported in a tuple/ordered json "
					  "class" );
					return out_it;
				}

				static constexpr OutputIterator
				output_member_types( OutputIterator &out_it ) {
					bool is_first = true;
					return static_cast<OutputIterator>(
					  ( output_member_type<json_link_no_name<JsonMembers>>( out_it,
					                                                        is_first ),
					    ... ) );
				}

				template<typename JsonMember>
				static constexpr OutputIterator
				output_member_type( OutputIterator &out_it, bool &is_first ) {
					if( not is_first ) {
						*out_it++ = ',';
					} else {
						is_first = false;
					}
					out_it.next_member( );
					out_it = to_json_schema<JsonMember>(
					  ParseTag<JsonMember::expected_type>{ }, out_it );
					return out_it;
				}
			};

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Class>,
			                                         OutputIterator out_it ) {
				using json_class_processor_t = json_class_processor<
				  OutputIterator,
				  json_data_contract_trait_t<typename JsonMember::base_type>>;

				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
				}
				if constexpr( json_class_processor_t::size > 0 ) {
					out_it.next_member( );
					out_it = json_class_processor_t::process( out_it );
				}
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				if constexpr( not is_root ) {
					out_it.next_member( );
					*out_it++ = '}';
				}
				return out_it;
			}

			namespace json_details {
				template<typename Tuple, bool is_root, typename OutputIterator,
				         std::size_t... Is>
				constexpr OutputIterator
				to_json_tuple_schema( OutputIterator out_it,
				                      std::index_sequence<Is...> ) {
					if constexpr( not is_root ) {
						*out_it++ = '{';
						out_it.add_indent( );
					}
					out_it.next_member( );
					out_it = utils::output_kv( out_it, R"("type")", R"("array",)" );
					out_it.next_member( );
					out_it = utils::output_kv( out_it, R"("items")", "[" );
					if constexpr( sizeof...( Is ) > 0 ) {
						out_it.add_indent( );
						bool is_first = true;
						auto const process_member = [&]( auto Idx ) {
							if( not is_first ) {
								*out_it++ = ',';
							} else {
								is_first = false;
							}
							out_it.next_member( );
							static constexpr std::size_t index = decltype( Idx )::value;
							using pack_element = tuple_elements_pack<Tuple>;
							using JsonMember = json_deduced_type<
							  typename pack_element::template element_t<index>>;

							out_it = to_json_schema<JsonMember>(
							  ParseTag<JsonMember::expected_type>{ }, out_it );
						};

						daw::Empty expander[] = {
						  ( process_member( daw::constant<Is>{ } ), daw::Empty{ } )...,
						  daw::Empty{} };
						(void)expander;
						out_it.del_indent( );
						out_it.next_member( );
					}
					*out_it++ = ']';
					if constexpr( not is_root ) {
						out_it.del_indent( );
					}
					out_it.next_member( );
					if constexpr( not is_root ) {
						*out_it++ = '}';
					}
					return out_it;
				}
			} // namespace json_details

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Tuple>,
			                                         OutputIterator out_it ) {
				using tuple_t = typename JsonMember::sub_member_list;
				return json_details::to_json_tuple_schema<tuple_t, is_root>(
				  out_it,
				  std::make_index_sequence<tuple_elements_pack<tuple_t>::size>{ } );
			}

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator to_json_schema( ParseTag<JsonParseTypes::Array>,
			                                         OutputIterator out_it ) {
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
				}
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("type")", R"("array",)" );
				out_it.next_member( );
				out_it = utils::copy_to_iterator( out_it, R"("items":)" );
				out_it.output_space( );
				using element_t = typename JsonMember::json_element_t;
				out_it = to_json_schema<element_t>(
				  ParseTag<element_t::expected_type>{ }, out_it );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::SizedArray>,
			                OutputIterator out_it ) {
				return to_json_schema<JsonMember, is_root>(
				  ParseTag<JsonParseTypes::Array>{ }, out_it );
			}

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::KeyValue>,
			                OutputIterator out_it ) {
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
				}
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("type")", R"("object")" );
				*out_it++ = ',';
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("additionalProperties")", "" );
				using element_t = typename JsonMember::json_element_t;
				out_it = to_json_schema<element_t>(
				  ParseTag<element_t::expected_type>{ }, out_it );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::KeyValueArray>,
			                OutputIterator out_it ) {
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
				}
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("type")", R"("array",)" );
				out_it.next_member( );
				out_it = utils::copy_to_iterator( out_it, R"("items":)" );
				out_it.output_space( );
				using element_t = typename JsonMember::json_class_t;
				out_it = to_json_schema<element_t>(
				  ParseTag<element_t::expected_type>{ }, out_it );
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename...>
			struct variant_element_types;

			template<typename... JsonElements>
			struct variant_element_types<json_variant_type_list<JsonElements...>> {

				template<typename JsonElement, typename OutputIterator>
				static constexpr OutputIterator output_element( OutputIterator out_it,
				                                                bool &is_first ) {
					if( not is_first ) {
						*out_it++ = ',';
					} else {
						is_first = false;
					}
					out_it.next_member( );
					return to_json_schema<JsonElement>(
					  ParseTag<JsonElement::expected_type>{ }, out_it );
				}

				template<typename OutputIterator>
				static constexpr OutputIterator
				output_elements( OutputIterator out_it ) {
					bool is_first = true;
					return static_cast<OutputIterator>(
					  ( output_element<JsonElements>( out_it, is_first ), ... ) );
				}
			};

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::Variant>,
			                OutputIterator out_it ) {
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
				}
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("oneOf")", "[" );
				using elements_t = typename JsonMember::json_elements;
				if constexpr( daw::pack_size_v<elements_t> != 0 ) {
					out_it.add_indent( );
					out_it = variant_element_types<elements_t>::output_elements( out_it );
					out_it.del_indent( );
					out_it.next_member( );
				}
				*out_it++ = ']';
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::VariantTagged>,
			                OutputIterator out_it ) {
				static_assert( not is_root,
				               "Attempt to have a tagged variant as root object.  This "
				               "is unsupported" );
				*out_it++ = '{';
				out_it.add_indent( );
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("oneOf")", "[" );
				using elements_t = typename JsonMember::json_elements;
				if constexpr( daw::pack_size_v<elements_t> != 0 ) {
					out_it.add_indent( );
					out_it = variant_element_types<elements_t>::output_elements( out_it );
					out_it.del_indent( );
					out_it.next_member( );
				}
				*out_it++ = ']';
				out_it.del_indent( );
				out_it.next_member( );
				*out_it++ = '}';
				return out_it;
			}

			template<typename JsonMember, bool is_root, typename OutputIterator>
			constexpr OutputIterator
			to_json_schema( ParseTag<JsonParseTypes::VariantIntrusive>,
			                OutputIterator out_it ) {
				if constexpr( not is_root ) {
					*out_it++ = '{';
					out_it.add_indent( );
				}
				out_it.next_member( );
				out_it = utils::output_kv( out_it, R"("oneOf")", "[" );
				using elements_t = typename JsonMember::json_elements;
				if constexpr( daw::pack_size_v<elements_t> != 0 ) {
					out_it.add_indent( );
					out_it = variant_element_types<elements_t>::output_elements( out_it );
					out_it.del_indent( );
					out_it.next_member( );
				}
				*out_it++ = ']';
				if constexpr( not is_root ) {
					out_it.del_indent( );
				}
				out_it.next_member( );
				if constexpr( not is_root ) {
					*out_it++ = '}';
				}
				return out_it;
			}
		} // namespace json_details

		template<typename T, typename OutputIterator>
		constexpr OutputIterator to_json_schema( OutputIterator it,
		                                         std::string_view id,
		                                         std::string_view title ) {

			using iter_t =
			  std::conditional_t<is_serialization_policy_v<OutputIterator>,
			                     OutputIterator,
			                     serialization_policy<OutputIterator>>;
			auto out_it = iter_t( it );
			*out_it++ = '{';
			out_it.add_indent( );
			out_it.next_member( );
			out_it = utils::output_kv(
			  out_it, R"("$schema")",
			  R"("https://json-schema.org/draft/2020-12/schema",)" );
			out_it.next_member( );
			out_it = utils::output_kv( out_it, R"("$id")", "\"" );
			out_it = utils::copy_to_iterator( out_it, id );
			out_it = utils::copy_to_iterator( out_it, R"(",)" );
			out_it.next_member( );
			out_it = utils::output_kv( out_it, R"("title")", "\"" );
			out_it = utils::copy_to_iterator( out_it, title );
			out_it = utils::copy_to_iterator( out_it, R"(",)" );
			using json_type = json_link_no_name<T>;
			out_it = json_details::to_json_schema<json_type, true>(
			  ParseTag<json_type::expected_type>{ }, out_it );
			out_it.del_indent( );
			out_it.next_member( );
			*out_it++ = '}';
			return out_it;
		}

		template<typename T,
		         typename SerializationPolicy = use_default_serialization_policy,
		         typename Result = std::string>
		Result to_json_schema( std::string_view id, std::string_view title ) {
			auto result = Result( );
			using iter_t = std::back_insert_iterator<Result>;
			using policy = std::conditional_t<
			  std::is_same_v<SerializationPolicy, use_default_serialization_policy>,
			  serialization_policy<iter_t>, SerializationPolicy>;

			(void)to_json_schema<T>( policy( iter_t( result ) ), id, title );
			return result;
		}
	} // namespace DAW_JSON_VER
} // namespace daw::json
