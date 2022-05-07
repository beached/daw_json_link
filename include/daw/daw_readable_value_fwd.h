// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

namespace daw {
	/***
	 * @brief Readable values models an option/maybe/nullable type
	 * @tparam T The option type
	 */
	template<typename T, typename...>
	struct readable_value_traits {
		/***
		 * @brief The type of the value stored in the readable type
		 */
		using value_type = T;
		/***
		 * @brief The type of the readable type
		 */
		using readable_type = T;
		/***
		 * @brief Can this type be read
		 */
		static constexpr bool is_readable = false;

		/***
		 * @brief read the value in the readable type
		 * @return A value_type with the current value
		 * @pre has_value( ) == true
		 */
		static value_type read( T const & );
		/***
		 * @brief Construct a value in readable type and return it
		 */
		static readable_type construct_value( );
		/***
		 * @brief Return an empty readable type
		 */
		static readable_type construct_emtpy( );
		/***
		 * @brief Check the state of the readable type for a value
		 */
		static bool has_value( T const & );
	};

	/***
	 * @brief Determines the type stored inside T
	 */
	template<typename T>
	using readable_value_type_t = typename readable_value_traits<T>::value_type;

	/***
	 * @brief Is T a readable type
	 */
	template<typename T>
	inline constexpr bool is_readable_value_v =
	  readable_value_traits<T>::is_readable;

	/***
	 * @brief Is T a readable type
	 */
	template<typename T>
	using is_readable_value = std::bool_constant<is_readable_value_v<T>>;

	/***
	 * @brief Check if readable value has a value
	 */
	template<typename T>
	constexpr bool readable_value_has_value( T const &opt ) {
		assert( is_readable_value_v<T> );
		return readable_value_traits<T>::has_value( opt );
	}

	/***
	 * @brief Read value from a non-empty readable value
	 * @pre readable_value_traits<T>::has_value( ) == true
	 */
	template<typename T>
	constexpr decltype( auto ) readable_value_read( T const &opt ) {
		assert( is_readable_value_v<T> );
		return readable_value_traits<T>::read( opt );
	}
} // namespace daw
