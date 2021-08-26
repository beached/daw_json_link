// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "daw_json_option_bits.h"
#include "daw_json_parse_options.h"
#include "daw_json_serialize_policy_options.h"
#include "version.h"

#include <cstddef>
#include <iterator>

namespace daw::json {
	inline namespace DAW_JSON_VER {
		namespace json_details {
			template<typename OutputIterator>
			struct iterator_wrapper : OutputIterator {
				inline constexpr OutputIterator get( ) const {
					return *this;
				}

				static constexpr bool is_pointer = false;

				inline constexpr void set( OutputIterator it ) {
					*static_cast<OutputIterator *>( this ) = it;
				}

			protected:
				inline constexpr OutputIterator &raw_it( ) {
					return *this;
				}
			};

			template<typename CharT>
			struct iterator_wrapper<CharT *> {
				using difference_type = std::ptrdiff_t;
				using size_type = std::size_t;
				using value_type = CharT;
				using pointer = CharT *;
				using reference = CharT &;
				using iterator_category = std::random_access_iterator_tag;

				CharT *ptr;

				static constexpr bool is_pointer = true;

			protected:
				inline constexpr CharT *raw_it( ) {
					return ptr;
				}

			public:
				inline constexpr CharT *get( ) const {
					return ptr;
				}

				inline constexpr void set( CharT *p ) {
					ptr = p;
				}

				inline constexpr reference operator*( ) {
					return *ptr;
				}

				inline constexpr pointer operator->( ) {
					return ptr;
				}

				inline constexpr iterator_wrapper &operator++( ) {
					++ptr;
					return *this;
				}

				inline constexpr iterator_wrapper operator++( int ) {
					auto result = *this;
					++ptr;
					return result;
				}

				inline constexpr iterator_wrapper &operator--( ) {
					--ptr;
					return *this;
				}

				inline constexpr iterator_wrapper operator--( int ) {
					auto result = *this;
					--ptr;
					return result;
				}

				inline constexpr iterator_wrapper &operator+=( difference_type n ) {
					ptr += n;
					return *this;
				}

				inline constexpr iterator_wrapper &operator-=( difference_type n ) {
					ptr -= n;
					return *this;
				}

				inline constexpr iterator_wrapper
				operator+( difference_type n ) const noexcept {
					iterator_wrapper result = *this;
					ptr += n;
					return result;
				}

				inline constexpr iterator_wrapper
				operator-( difference_type n ) const noexcept {
					iterator_wrapper result = *this;
					ptr -= n;
					return result;
				}

				inline constexpr reference operator[]( size_type n ) noexcept {
					return *( ptr + static_cast<difference_type>( n ) );
				}

				explicit inline constexpr operator bool( ) const {
					return static_cast<bool>( ptr );
				}

				friend inline constexpr bool operator==( iterator_wrapper const &lhs,
				                                         iterator_wrapper const &rhs ) {
					return lhs.ptr == rhs.ptr;
				}

				friend inline constexpr bool operator!=( iterator_wrapper const &lhs,
				                                         iterator_wrapper const &rhs ) {
					return lhs.ptr != rhs.ptr;
				}

				friend inline constexpr bool operator<( iterator_wrapper const &lhs,
				                                        iterator_wrapper const &rhs ) {
					return lhs.ptr < rhs.ptr;
				}

				friend inline constexpr bool operator<=( iterator_wrapper const &lhs,
				                                         iterator_wrapper const &rhs ) {
					return lhs.ptr <= rhs.ptr;
				}

				friend inline constexpr bool operator>( iterator_wrapper const &lhs,
				                                        iterator_wrapper const &rhs ) {
					return lhs.ptr > rhs.ptr;
				}

				friend inline constexpr bool operator>=( iterator_wrapper const &lhs,
				                                         iterator_wrapper const &rhs ) {
					return lhs.ptr >= rhs.ptr;
				}
			};

		} // namespace json_details

		namespace json_details::serialization {
			using policy_list =
			  typename option_list_impl<SerializationFormat, IndentationType,
			                            RestrictedStringOutput, NewLineDelimiter,
			                            OutputTrailingComma>::type;

			template<typename Policy, typename Policies>
			inline constexpr unsigned basic_policy_bits_start =
			  option_bits_start_impl<Policy, Policies>::template calc(
			    std::make_index_sequence<pack_size_v<Policies>>{ } );

			template<typename Policy>
			inline constexpr unsigned policy_bits_start =
			  basic_policy_bits_start<Policy, policy_list>;

			template<typename Policy>
			constexpr void set_bits_in( json_options_t &value, Policy e ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<unsigned>( e );
				constexpr unsigned mask = (1U << json_option_bits_width<Policy>)-1U;
				new_bits &= mask;
				new_bits <<= policy_bits_start<Policy>;
				value &= ~mask;
				value |= new_bits;
			}

			template<typename Policy, typename... Policies>
			constexpr json_options_t set_bits( json_options_t value, Policy pol,
			                                   Policies... pols ) {
				static_assert( ( is_option_flag<Policies> and ... ),
				               "Only registered policy types are allowed" );

				auto new_bits = static_cast<unsigned>( pol );
				constexpr unsigned mask = ( (1U << json_option_bits_width<Policy>)-1U );
				new_bits &= mask;
				new_bits <<= policy_bits_start<Policy>;
				value &= ~( mask << policy_bits_start<Policy> );
				value |= new_bits;
				if constexpr( sizeof...( Policies ) > 0 ) {
					if constexpr( sizeof...( pols ) > 0 ) {
						return set_bits( value, pols... );
					} else {
						return value;
					}
				} else {
					return value;
				}
			}

			template<typename Policy>
			constexpr json_options_t set_bits_for( Policy e ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				auto new_bits = static_cast<json_options_t>( e );
				new_bits <<= policy_bits_start<Policy>;
				return new_bits;
			}

			template<typename>
			struct default_policy_flag_t;

			template<typename... Policies>
			struct default_policy_flag_t<pack_list<Policies...>> {
				static constexpr json_options_t value =
				  ( set_bits_for<Policies>( default_json_option_value<Policies> ) |
				    ... );
			};

			/***
			 * The defaults for all known policies encoded as a json_options_t
			 */
			inline static constexpr json_options_t default_policy_flag =
			  default_policy_flag_t<policy_list>::value;

			template<typename Policy, typename Result = Policy>
			constexpr Result get_bits_for( json_options_t value ) {
				static_assert( is_option_flag<Policy>,
				               "Only registered policy types are allowed" );
				constexpr unsigned mask = ( 1U << (policy_bits_start<Policy> +
				                                   json_option_bits_width<Policy>)) -
				                          1U;
				value &= mask;
				value >>= policy_bits_start<Policy>;
				return static_cast<Result>( Policy{ value } );
			}

			template<SerializationFormat, IndentationType>
			inline constexpr std::string_view generate_indent{ };

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Tab> =
			    "\t";

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Space1> =
			    " ";

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Space2> =
			    "  ";

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Space3> =
			    "   ";

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Space4> =
			    "    ";

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Space5> =
			    "     ";

			template<>
			inline constexpr std::string_view
			  generate_indent<SerializationFormat::Pretty, IndentationType::Space8> =
			    "        ";

		} // namespace json_details::serialization
	}   // namespace DAW_JSON_VER
} // namespace daw::json
