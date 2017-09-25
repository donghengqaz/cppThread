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

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#if __cplusplus < 201103L
#error "need c++11 or higher"
#else

#include <functional>
#include <stdexcept>
#include <memory>

#include "arch/arch.hpp"
#include "io.hpp"

namespace estd
{
    class ipaddr
    {
    public:
        ipaddr()
        {
            std::memset(&_ipv4_addr, 0, sizeof(_ipv4_addr));
            _port = 0;
        }

        ipaddr(std::string host, std::uint16_t port) : ipaddr()
        {
            struct hostent *hp = gethostbyname(host.c_str());
            if (hp) {
                _ipv4_addr = *(struct in_addr *)hp->h_addr;
                _port = port;
            }
        }

        ~ipaddr() { }

        struct in_addr ipv4_addr() 
        {
        	return _ipv4_addr;
        }

        std::uint16_t port()
        {
        	return _port;
        }

    private:
        struct in_addr _ipv4_addr;
        std::uint16_t  _port;
    };

    class socket : public estd::_io_base 
    {
    public:
        socket()
        {
            _fd = -1;
            _default_pdu_bytes = 1472;
        }

        socket(int nfd)
        {
            _fd = nfd;
        }

        socket(std::uint8_t t) : socket()
        {
            if (_socket_create(&_fd, AF_INET, t, 0) < 0)
                throw std::logic_error("socket low-level constructor(new) fails");
        }
        
        socket(std::uint8_t t, std::uint16_t port, bool forced = false) : socket(t)
        {
            if (forced && setsockopt(SOL_SOCKET, SO_REUSEADDR, 1) == false)
                throw std::logic_error("socket low-level constructor(setopt SO_REUSEADDR) fails");

            if (_socket_bind(&_fd, INADDR_ANY, port))
                throw std::logic_error("socket low-level constructor(bind) fails");
        }
    
        socket(std::uint8_t t, class ipaddr&& val, bool forced = false) : socket(t)
        {
            if (forced && setsockopt(SOL_SOCKET, SO_REUSEADDR, 1) == false)
                throw std::logic_error("socket low-level constructor(setopt SO_REUSEADDR) fails");

            class ipaddr ipaddr = std::forward<class ipaddr>(val);
            if (_socket_bind(&_fd, ipaddr.ipv4_addr().s_addr, ipaddr.port()))
                throw std::logic_error("socket low-level constructor(bind) fails");
        }

        ~socket()
        {
            _socket_destroy(&_fd);
        }

        template<typename Type>
        bool setsockopt(std::int32_t level, std::int32_t optname, Type &val)
        {
            return _socket_setsockopt(&_fd, level, optname, &val, sizeof(val)) == 0 ? true : false;
        }

        template<typename Type>
        bool setsockopt(std::int32_t level, std::int32_t optname, Type &&val)
        {
            Type tmp = std::forward<Type>(val);
            return _socket_setsockopt(&_fd, level, optname, &tmp, sizeof(tmp)) == 0 ? true : false;
        }

        bool connect(class ipaddr&& val)
        {
            class ipaddr ipaddr = std::forward<class ipaddr>(val);
            return _socket_connect(&_fd, ipaddr.ipv4_addr().s_addr, ipaddr.port()) == 0 ? true : false;
        }

        bool listen(int backlog)
        {
            return _socket_listen(&_fd, backlog) == 0 ? true : false;
        }

        class socket accept()
        {
            return socket(_socket_accept(&_fd));
        }

        /*
         * io base function
         */
        bool open(std::string s) 
        {
            return false;
        }

        bool fcntl(bool add, int& flag)
        {
            flag = _socket_fcntl(&_fd, F_GETFL, 0);
            return true;
        }

        bool fcntl(bool add, const int& val)
        {
            int flags = _socket_fcntl(&_fd, F_GETFL, 0);
            if (add)
                flags |= val;
            else
                flags &= val;
            return _socket_fcntl(&_fd, F_SETFL, flags);
        }
        
        ssize_t read(void *pbuf, size_t n)
        {
            return _socket_recv(&_fd, pbuf, n, 0);
        }
        
        ssize_t write(const void *pbuf, size_t n)
        {
        	return _socket_send(&_fd, pbuf, n, 0);
        }

        ssize_t write(std::string s)
        {
            return write(s.c_str(), s.size() + 1);
        }

        /*
         * close the closed socket will return error
         */
        bool close() 
        {
        	return _socket_destroy(&_fd) == 0 ? true : false;
        }

        int poll(bool enter, int mode)
        {
        	return 0;
        }

        int type()
        {
        	return IO_SOCKET;
        }

        bool selected()
        {
        	return _socket_selected();
        }
    };
};

#endif /* c++11 */

#endif /* _SOCKET_HPP_ */
