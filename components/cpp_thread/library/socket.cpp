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
#include "socket.hpp"

namespace estd {

ipaddr::ipaddr()
{
    std::memset(&_ipv4_addr, 0, sizeof(_ipv4_addr));
    _port = 0;
}

ipaddr::ipaddr(std::string host, std::uint16_t port) : ipaddr()
{
    struct hostent *hp = gethostbyname(host.c_str());
    if (hp) {
        _ipv4_addr = *(struct in_addr *)hp->h_addr;
        _port = port;
    }
}

ipaddr::~ipaddr()
{

}

struct in_addr ipaddr::ipv4_addr() 
{
    return _ipv4_addr;
}

std::uint16_t ipaddr::port()
{
    return _port;
}

socket::socket()
{
    _fd = -1;
    _default_pdu_bytes = 1472;
}

socket::socket(int nfd)
{
    _fd = nfd;
}

socket::socket(std::uint8_t t) : socket()
{
    if (_socket_create(&_fd, AF_INET, t, 0) < 0)
        throw std::logic_error("socket low-level constructor(new) fails");
}

socket::socket(std::uint8_t t, std::uint16_t port, bool forced) : socket(t)
{
    if (forced && setsockopt(SOL_SOCKET, SO_REUSEADDR, 1) == false)
        throw std::logic_error("socket low-level constructor(setopt SO_REUSEADDR) fails");

    if (_socket_bind(&_fd, INADDR_ANY, port))
        throw std::logic_error("socket low-level constructor(bind) fails");
}

socket::socket(std::uint8_t t, class ipaddr&& val, bool forced) : socket(t)
{
    if (forced && setsockopt(SOL_SOCKET, SO_REUSEADDR, 1) == false)
        throw std::logic_error("socket low-level constructor(setopt SO_REUSEADDR) fails");

    class ipaddr ipaddr = std::forward<class ipaddr>(val);
    if (_socket_bind(&_fd, ipaddr.ipv4_addr().s_addr, ipaddr.port()))
        throw std::logic_error("socket low-level constructor(bind) fails");
}

socket::~socket()
{
    _socket_destroy(&_fd);
}

template<typename Type>
bool socket::setsockopt(std::int32_t level, std::int32_t optname, Type &val)
{
    return _socket_setsockopt(&_fd, level, optname, &val, sizeof(val)) == 0 ? true : false;
}

template<typename Type>
bool socket::setsockopt(std::int32_t level, std::int32_t optname, Type &&val)
{
    Type tmp = std::forward<Type>(val);
    return _socket_setsockopt(&_fd, level, optname, &tmp, sizeof(tmp)) == 0 ? true : false;
}

bool socket::connect(class ipaddr&& val)
{
    class ipaddr ipaddr = std::forward<class ipaddr>(val);
    return _socket_connect(&_fd, ipaddr.ipv4_addr().s_addr, ipaddr.port()) == 0 ? true : false;
}

bool socket::listen(int backlog)
{
    return _socket_listen(&_fd, backlog) == 0 ? true : false;
}

class socket::socket socket::accept()
{
    return socket(_socket_accept(&_fd));
}

/*
 * io base function
 */
bool socket::open(std::string s) 
{
    return false;
}

bool socket::fcntl(bool add, int& flag)
{
    flag = _socket_fcntl(&_fd, F_GETFL, 0);
    return true;
}

bool socket::fcntl(bool add, const int& val)
{
    int flags = _socket_fcntl(&_fd, F_GETFL, 0);
    if (add)
        flags |= val;
    else
        flags &= val;
    return _socket_fcntl(&_fd, F_SETFL, flags);
}

ssize_t socket::read(void *pbuf, size_t n)
{
    return _socket_recv(&_fd, pbuf, n, 0);
}

class iobytes socket::read(size_t n)
{
    n = _default_pdu_bytes;
    std::unique_ptr<char> p(new char[n]);
    int ret = read(p.get(), n);
    if (ret > 0) {
        std::unique_ptr<char> p2(new char[ret]);
        std::memcpy(p2.get(), p.get(), ret);
        return iobytes(p2, ret);
    }

    return iobytes(p, ret);
}

ssize_t socket::write(const void *pbuf, size_t n)
{
    return _socket_send(&_fd, pbuf, n, 0);
}

ssize_t socket::write(std::string s)
{
    return write(s.c_str(), s.size() + 1);
}

/*
 * close the closed socket will return error
 */
bool socket::close() 
{
    return _socket_destroy(&_fd) == 0 ? true : false;
}

int socket::poll(bool enter, int mode)
{
    return 0;
}

int socket::type()
{
    return IO_SOCKET;
}

bool socket::selected()
{
    return _socket_selected();
}

}; // namespace estd
