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

#include "sys/types.hpp"

namespace estd {

iobytes::iobytes()
{
    _ret = 0;
}

iobytes::iobytes(char *p, int ret)
{
    _pbuf = std::unique_ptr<char>(p);
    _ret = ret;
}

iobytes::iobytes(std::unique_ptr<char> &p, int ret)
{
    _pbuf = std::move(p);
    _ret = ret;
}

iobytes::~iobytes()
{

}

int iobytes::size()
{
    return _ret;
}

char *iobytes::data()
{
    return _pbuf.get();
}

void _lock_base::set_recursived(bool recursived)
{
    _recursived = recursived;
}

bool _lock_base::recursived()
{
    return _recursived;
}

bool _io_base::fcntl(bool add, int &&flag)
{
    int _ff = std::forward<int>(flag);
    return fcntl(add, _ff);
}

class iobytes _io_base::read(size_t n)
{
    n = default_pdu_size(n);
    std::unique_ptr<char> p(new char[n + 1]);
    int ret = read(p.get(), n);
    return iobytes(p, ret);
}

int _io_base::fd()
{
    return _fd;
}

int _io_base::move()
{
    int tmp = _fd;
    _fd = -1;
    return tmp;
}

bool _io_base::operator ==(class _io_base &other)
{
    return this->fd() == other.fd() ? true : false;
}

void _io_base::operator =(class _io_base &other)
{
    if (this->_fd >= 0)
        close();
    this->_fd = other.move();
}

size_t _io_base::default_pdu_size(size_t n) 
{
    if (!n)
        n = _default_pdu_bytes;
    return n;
}

}; // namespace estd
