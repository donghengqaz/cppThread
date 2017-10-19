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

#ifndef _IO_HPP_
#define _IO_HPP_

#if __cplusplus < 201103L
#error "need c++11 or higher"
#else

#include <vector>
#include <algorithm>
#include <tuple>

#include "sys/sys.hpp"

namespace estd
{

    using poll_t = std::tuple<int, int>;
    using select_t = std::tuple<int, class iolist, class iolist, class iolist>;

    class iolist
    {
    public:
        using value_type = class _io_base;
        using pointer = value_type *;
        using const_pointer = const pointer;

        using reference = value_type&;
        using const_reference = const value_type&;

        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using iterator = value_type *;
        using const_iterator = const value_type *;

    public:
        iolist() { }
        ~iolist() { }

        size_type size();

        iterator begin();

        iterator end();

        fd_set poll(int mode);

        int max_fd();

        void clear();

        void copy(iolist& to);

        void operator +=(reference io);

        void operator -=(reference io);

        void operator +=(pointer io);

        void operator -=(pointer io);

        iolist operator &(fd_set &pfd);

        iolist& operator =(iolist& others);

    private:
        void add(reference io);

        void add(pointer io);

        void rm(reference io);

        void rm(pointer io);

        std::vector<class _io_base *> list;
    };
    
    select_t select(class iolist &rlist, class iolist &wlist, class iolist &elist, estd::time_t t);

}; /* estd */

#endif /* c++11 */

#endif /* _IO_HPP_ */
