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

#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#if __cplusplus < 201103L
#error "need c++11 or higher"
#else

#include <functional>
#include <stdexcept>
#include <memory>

#include "arch/arch.hpp"
#include "sys/osapi.hpp"

namespace estd 
{

    template<typename Type, size_t Deepth>
    class message : public _io_base 
    {
    public:
        message()
        {
            estd::_message_init(&_fd);
            if (_message_create(&_fd, _deepth, _length))
                throw std::logic_error("message low-level constructor fails");
            _default_pdu_bytes = _length + 1;
        }

        ~message() 
        {
            _message_destroy(&_fd);
        }

        bool recv(Type &p, estd::time_t t = estd::max_time) 
        {
            return _message_recv(&_fd, (char *)&p, _length, t) == 0 ? true : false;
        }

        bool send(Type &p) 
        {
            return _message_send(&_fd, (const char *)&p, _length) == 0 ? true : false;
        }

        bool send(Type &&p) 
        {
            Type pp = std::forward<Type>(p);
            return _message_send(&_fd, (const char *)&pp, _length) == 0 ? true : false;
        }

        /*
         * IO base
         **/
        bool open(std::string s) 
        {
            return false;
        }
        
        bool fcntl(bool add, int& flag)
        {
            return false;
        }
        
        bool fcntl(bool add, const int& val)
        {
            return false;
        }

        ssize_t read(void *pbuf, std::size_t n = 0)
        {
            return _message_recv(&_fd, (char *)pbuf, _length, estd::max_time) == 0 ? _length : -1;
        }

        ssize_t write(const void *pbuf, std::size_t n)
        {
            return _message_send(&_fd, (const char *)pbuf, _length) == 0 ? _length : -1;
        }

        virtual ssize_t write(std::string s)
        {
            return write(s.c_str(), s.size() + 1);
        }

        bool close()
        {
            return false;
        }

        int poll(bool enter, int mode)
        {
            return _message_poll(&_fd, enter, mode);
        }

        int type()
        {
            return IO_MESSAGE;
        }

        bool selected()
        {
            return _message_selected();
        }

    private:
        const int default_priority = 1;
        static const std::size_t _deepth = Deepth;
        static const std::size_t _length = sizeof(Type);
    };

}; // estd

#endif /* c++11 */

#endif /* _MESSAGE_HPP_ */
