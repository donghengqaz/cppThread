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

#include "sys/sys.hpp"
#include "io.hpp"

namespace estd
{
    class ipaddr
    {
    public:
        ipaddr();

        ipaddr(std::string host, std::uint16_t port);

        ~ipaddr();

        struct in_addr ipv4_addr();

        std::uint16_t port();

    private:
        struct in_addr _ipv4_addr;
        std::uint16_t  _port;
    };

    class socket : public estd::_io_base 
    {
    public:
        socket();

        socket(int nfd);

        socket(std::uint8_t t);
        
        socket(std::uint8_t t, std::uint16_t port, bool forced = false);
    
        socket(std::uint8_t t, class ipaddr&& val, bool forced = false);

        ~socket();

        template<typename Type>
        bool setsockopt(std::int32_t level, std::int32_t optname, Type &val);

        template<typename Type>
        bool setsockopt(std::int32_t level, std::int32_t optname, Type &&val);

        bool connect(class ipaddr&& val);

        bool listen(int backlog);

        class socket accept();

        /*
         * io base function
         */
        bool open(std::string s);

        bool fcntl(bool add, int& flag);

        bool fcntl(bool add, const int& val);
        
        ssize_t read(void *pbuf, size_t n);

        class iobytes read(size_t n = 0);
        
        ssize_t write(const void *pbuf, size_t n);

        ssize_t write(std::string s);

        /*
         * close the closed socket will return error
         */
        bool close();

        int poll(bool enter, int mode);

        int type();

        bool selected();
    };
};

#endif /* c++11 */

#endif /* _SOCKET_HPP_ */
