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

#include "types.hpp"

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
        using difference_type = estd::ptrdiff_t;

        using iterator = value_type *;
        using const_iterator = const value_type *;

    public:
        iolist() { }
        ~iolist() { }

        size_type size()
        { return list.size(); }

        iterator begin()
        {
            if (!list.size()) return NULL;
            return list[0];
        }

        iterator end()
        {
            if (!list.size()) return NULL;
            return list[list.size() - 1] + 1;
        }

        fd_set poll(int mode)
        {
            fd_set pfd;
            FD_ZERO(&pfd);
            for (auto c : list) {
                if (c->selected())
                    FD_SET(c->fd(), &pfd);
                if (c->poll(true, mode))
                    throw std::make_tuple(mode, c->fd());
            }
            return pfd;
        }

        int max_fd()
        {
            int maxfd = 0;
            for (auto &c : list)
                maxfd = std::max(maxfd, c->fd());
            return maxfd;
        }

        void clear()
        { list.clear(); }

        void copy(iolist& to)
        { for (auto i : list) to.add(i); }

        void operator +=(reference io)
        { add(io); }

        void operator -=(reference io)
        { rm(&io); }

        void operator +=(pointer io)
        { add(io); }

        void operator -=(pointer io)
        { rm(io); }

        iolist operator &(fd_set &pfd)
        {
            iolist l;
            for (auto c : list) {
                if (FD_ISSET(c->fd(), &pfd)) {
                    l += c;
                }
                c->poll(false, estd::POLL_NONE);
            }
            return l;
        }

        iolist& operator =(iolist& others)
        {
            clear();
            others.copy(*this);
            return *this;
        }

    private:
        void add(reference io)
        { add(&io); }

        void add(pointer io)
        { list.push_back(io); }

        void rm(reference io)
        { rm(&io); }

        void rm(pointer io)
        { list.erase(std::find(list.begin(), list.end(), io)); }

        std::vector<class _io_base *> list;
    };
    
    inline select_t select(class iolist &rlist, class iolist &wlist, class iolist &elist, estd::time_t& t)
    {
        fd_set rfd;
        fd_set wfd;
        fd_set efd;

        auto _poll_all = [&](bool clear = false)
        {
            try {
                rfd = rlist.poll(estd::POLL_IN);
                wfd = wlist.poll(estd::POLL_OUT);
                efd = elist.poll(estd::POLL_ERR);
                if (clear) {
                    FD_ZERO(&rfd);
                    FD_ZERO(&wfd);
                    FD_ZERO(&efd);                
                }
                return true;
            } catch (poll_t &ep) {
                int mode = std::get<0>(ep);
                int nfd  = std::get<1>(ep);
                FD_ZERO(&rfd);
                FD_ZERO(&wfd);
                FD_ZERO(&efd);
                if (mode == estd::POLL_IN)
                    FD_SET(nfd, &rfd);
                else if (mode == estd::POLL_OUT)
                    FD_SET(nfd, &wfd);
                else if (mode == estd::POLL_ERR)
                    FD_SET(nfd, &efd);
                else
                    std::cout << "select never happen" << std::endl;
                return false;
            }
        };

        if (_poll_all() == false)
            return std::make_tuple(1, rlist & rfd, wlist & wfd, elist & efd);

        int ret = estd::_raw_select(std::max({rlist.max_fd(), wlist.max_fd(), elist.max_fd()}) + 1, &rfd, &wfd, &efd, t);
        if (ret == 0 && _poll_all(true) == false)
            ret = 1;

        return std::make_tuple(ret, rlist & rfd, wlist & wfd, elist & efd);
    }

    inline select_t select(class iolist &in, class iolist &out, class iolist &err, estd::time_t&& t)
    {
        estd::time_t tt = std::forward<estd::time_t>(t);
        return select(in, out, err, tt);
    }

}; /* estd */

#endif /* c++11 */

#endif /* _IO_HPP_ */