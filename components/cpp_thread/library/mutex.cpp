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

#include "sys/osapi.hpp"
#include "mutex.hpp"

namespace estd {

mutex::mutex(bool recursived)
{
    _mutex_init(&_id);
    if (_mutex_create(&_id, recursived))
        throw std::logic_error("mutex low-level constructor fails");
    set_recursived(recursived);
}

mutex::~mutex()
{
    _mutex_destroy(&_id);
}

bool mutex::lock(estd::time_t t)
{
    return _mutex_lock(&_id, t, recursived()) == 0 ? true : false;
}

void mutex::unlock() 
{
    _mutex_unlock(&_id, recursived());
}

}; // namespace estd
