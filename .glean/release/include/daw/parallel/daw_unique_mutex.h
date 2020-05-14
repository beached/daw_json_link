// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
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

#include <memory>
#include <mutex>

namespace daw {
	// A ref counted mutex
	template<typename Mutex>
	class alignas( alignof( Mutex ) ) basic_unique_mutex {
		struct member_t {
			alignas( alignof( Mutex ) ) Mutex data = Mutex( );
		};
		std::unique_ptr<member_t> m_mutex = std::make_unique<member_t>( );

	public:
		basic_unique_mutex( ) noexcept {}

		void lock( ) {
			m_mutex->data.lock( );
		}

		[[nodiscard]] bool try_lock( ) {
			return m_mutex->data.try_lock( );
		}

		void unlock( ) {
			m_mutex->data.unlock( );
		}

		Mutex &get( ) noexcept {
			return m_mutex->data;
		}

		Mutex const &get( ) const noexcept {
			return m_mutex->data;
		}
	};

	using unique_mutex = basic_unique_mutex<std::mutex>;
} // namespace daw
