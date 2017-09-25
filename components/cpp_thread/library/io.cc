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

#include <iostream>

#include "io.hpp"
#include "sys/osapi.hpp"

namespace estd {

select_t select(class iolist &rlist, class iolist &wlist, class iolist &elist, estd::time_t t)
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

    int ret = _raw_select(std::max({rlist.max_fd(), wlist.max_fd(), elist.max_fd()}) + 1, &rfd, &wfd, &efd, t);
    if (ret == 0 && _poll_all(true) == false)
        ret = 1;

    return std::make_tuple(ret, rlist & rfd, wlist & wfd, elist & efd);
}

};

