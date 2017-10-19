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

#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#if __cplusplus < 201103L
#error "need c++11 or higher"
#else

#include <functional>
#include <stdexcept>

#include "sys/sys.hpp"

namespace estd 
{

    class mutex : private estd::_lock_base  
    {
    public:
        mutex(bool recursived = false);

        ~mutex();

        bool lock(estd::time_t t = estd::max_time);

        void unlock();

    private:
        estd::_mutex_t  _id;
    }; // mutex

}; // estd

#endif /* c++11 */

#endif /* _MUTEX_HPP_ */
