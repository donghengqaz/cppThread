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

#ifndef _FREERTOS_HPP_
#define _FREERTOS_HPP_

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstring>

#include "sys/types.hpp"

#include <sys/errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

namespace estd {
    /**
     * time
     */
    const time_t max_time = static_cast<time_t>(portMAX_DELAY * portTICK_PERIOD_MS / 1000); //1.79769313486231570E+308 ?
    const time_t min_time = 0;

    /**
     * thread
     */
#ifdef CONFIG_THREAD_TMPL_DEFAULT_ARGS
    const std::size_t default_priority = 2;
    const std::size_t default_stk_size = 8192;
#endif

    using ssize_t = int;
}; // estd

#endif /* _FREERTOS_HPP_ */
