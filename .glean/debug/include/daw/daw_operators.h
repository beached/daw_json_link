// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

/// Create comparison operators for a class that defines compare( rhs )
/// and returns an int where 0 means equal <0 means less than...etc
#define create_comparison_operators( cls_name )                                \
	bool operator==( cls_name const &lhs, cls_name const &rhs ) {                \
		return lhs.compare( rhs ) == 0;                                            \
	}                                                                            \
                                                                               \
	bool operator!=( cls_name const &lhs, cls_name const &rhs ) {                \
		return lhs.compare( rhs ) != 0;                                            \
	}                                                                            \
                                                                               \
	bool operator<( cls_name const &lhs, cls_name const &rhs ) {                 \
		return lhs.compare( rhs ) < 0;                                             \
	}                                                                            \
                                                                               \
	bool operator>( cls_name const &lhs, cls_name const &rhs ) {                 \
		return lhs.compare( rhs ) > 0;                                             \
	}                                                                            \
                                                                               \
	bool operator<=( cls_name const &lhs, cls_name const &rhs ) {                \
		return lhs.compare( rhs ) <= 0;                                            \
	}                                                                            \
                                                                               \
	bool operator>=( cls_name const &lhs, cls_name const &rhs ) {                \
		return lhs.compare( rhs ) >= 0;                                            \
	}

#define create_friend_comparison_operators( cls_name )                         \
	friend bool operator==( cls_name const &lhs, cls_name const &rhs ) {         \
		return lhs.compare( rhs ) == 0;                                            \
	}                                                                            \
                                                                               \
	friend bool operator!=( cls_name const &lhs, cls_name const &rhs ) {         \
		return lhs.compare( rhs ) != 0;                                            \
	}                                                                            \
                                                                               \
	friend bool operator<( cls_name const &lhs, cls_name const &rhs ) {          \
		return lhs.compare( rhs ) < 0;                                             \
	}                                                                            \
                                                                               \
	friend bool operator>( cls_name const &lhs, cls_name const &rhs ) {          \
		return lhs.compare( rhs ) > 0;                                             \
	}                                                                            \
                                                                               \
	friend bool operator<=( cls_name const &lhs, cls_name const &rhs ) {         \
		return lhs.compare( rhs ) <= 0;                                            \
	}                                                                            \
                                                                               \
	friend bool operator>=( cls_name const &lhs, cls_name const &rhs ) {         \
		return lhs.compare( rhs ) >= 0;                                            \
	}
