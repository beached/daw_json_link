// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdio>
#ifndef WIN32
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <type_traits>
#include <unistd.h>
#else
#include <fstream>
#include <streambuf>
#include <string>
#endif
#include <utility>

namespace daw {
	enum class open_mode : bool { read, read_write };

#ifndef WIN32
	template<typename T = char>
	struct memory_mapped_file {
		using value_type = T;
		using reference = T &;
		using const_reference =
		  std::conditional_t<std::is_const_v<T>, T, T const> &;
		using pointer = T *;
		using const_pointer = std::conditional_t<std::is_const_v<T>, T, T const> *;
		using size_type = size_t;

	private:
		int m_file = -1;
		pointer m_ptr = nullptr;
		size_type m_size = 0;

		void cleanup( ) noexcept {
			if( m_ptr != nullptr ) {
				munmap( m_ptr, m_size );
				m_ptr = nullptr;
			}
			m_size = 0;
			if( m_file >= 0 ) {
				close( m_file );
				m_file = -1;
			}
		}

	public:
		constexpr memory_mapped_file( ) noexcept = default;

		explicit memory_mapped_file( std::string_view file,
		                    open_mode mode = open_mode::read ) noexcept {

			(void)open( file, mode );
		}

		[[nodiscard]] bool open( std::string_view file,
		           open_mode mode = open_mode::read ) noexcept {

			m_file =
			  ::open( file.data( ), mode == open_mode::read ? O_RDONLY : O_RDWR );
			if( m_file < 0 ) {
				return false;
			}
			{
				auto const fsz = lseek( m_file, 0, SEEK_END );
				lseek( m_file, 0, SEEK_SET );
				if( fsz <= 0 ) {
					cleanup( );
					return false;
				}
				m_size = static_cast<size_type>( fsz );
			}
			m_ptr = static_cast<pointer>(
			  mmap( nullptr, m_size,
			        mode == open_mode::read ? PROT_READ : PROT_READ | PROT_WRITE,
			        MAP_SHARED, m_file, 0 ) );

			if( m_ptr == MAP_FAILED ) {
				m_ptr = nullptr;
				cleanup( );
				return false;
			}
			return true;
		}

		[[nodiscard]] reference operator[]( size_type pos ) noexcept {
			return m_ptr[pos];
		}

		[[nodiscard]] const_reference operator[]( size_t pos ) const noexcept {
			return m_ptr[pos];
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_ptr;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_ptr;
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_size;
		}

		constexpr explicit operator bool( ) const noexcept {
			return m_file >= 0 and m_ptr != nullptr;
		}

		memory_mapped_file( memory_mapped_file const & ) = delete;
		memory_mapped_file &operator=( memory_mapped_file const & ) = delete;

		memory_mapped_file( memory_mapped_file &&other ) noexcept
		  : m_file( std::exchange( other.m_file, -1 ) )
		  , m_ptr( std::exchange( other.m_ptr, nullptr ) )
		  , m_size( std::exchange( other.m_size, 0 ) ) {}

		memory_mapped_file &operator=( memory_mapped_file &&rhs ) noexcept {
			if( this != &rhs ) {
				m_file = std::exchange( rhs.m_file, -1 );
				m_ptr = std::exchange( rhs.m_ptr, nullptr );
				m_size = std::exchange( rhs.m_size, 0 );
			}
			return *this;
		}

		~memory_mapped_file( ) noexcept {
			cleanup( );
		}
	};
#else
	template<typename T = char>
	struct memory_mapped_file {
		using value_type = T;
		using reference = T &;
		using const_reference =
		  std::conditional_t<std::is_const_v<T>, T, T const> &;
		using pointer = T *;
		using const_pointer = std::conditional_t<std::is_const_v<T>, T, T const> *;
		using size_type = size_t;

	private:
		std::string m_value{};

		void cleanup( ) noexcept {
			m_value.clear( );
		}

	public:
		constexpr memory_mapped_file( ) noexcept = default;

		memory_mapped_file( std::string_view file,
		                    open_mode mode = open_mode::read ) noexcept {

			(void)open( file, mode );
		}

		[[nodiscard]] bool open( std::string_view file,
		           open_mode mode = open_mode::read ) noexcept {

			std::ifstream in_file( file.data( ) );
			if( not in_file ) {
				return false;
			}
			m_value = std::string( std::istreambuf_iterator<char>( in_file ),
			                       std::istreambuf_iterator<char>( ) );
			return not m_value.empty( );
		}

		[[nodiscard]] reference operator[]( size_type pos ) noexcept {
			return m_value[pos];
		}

		[[nodiscard]] const_reference operator[]( size_t pos ) const noexcept {
			return m_value[pos];
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_value.data( );
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_value.data( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_value.size( );
		}

		constexpr explicit operator bool( ) const noexcept {
			return not m_value.empty( );
		}

		memory_mapped_file( memory_mapped_file const & ) = delete;
		memory_mapped_file &operator=( memory_mapped_file const & ) = delete;

		memory_mapped_file( memory_mapped_file && ) noexcept = default;
		memory_mapped_file &operator=( memory_mapped_file && ) noexcept = default;
		~memory_mapped_file( ) noexcept = default;
	};
#endif
} // namespace daw
