// MIT License

// Copyright (c) 2017 dongheng

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _POSIX_HPP_
#define _POSIX_HPP_

#include "sys/types.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>

namespace estd {
    /**
     * time
     */
    const estd::time_t max_time = static_cast<time_t>(UINT32_MAX); //1.79769313486231570E+308 ?
    const estd::time_t min_time = 0;

    /**
     * thread
     */
#ifdef CONFIG_THREAD_TMPL_DEFAULT_ARGS
    const std::size_t default_priority = 1;
    const std::size_t default_stk_size = 8192;
#endif
}; // estd

#endif /* _POSIX_HPP_ */

// 2. ticks  max? min?
