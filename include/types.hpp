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

#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#if __cplusplus < 201103L
#error "need c++11 or higher"
#else

#include "config.h"

#include <string>

namespace estd 
{
    using time_t = double;

    /*
     * POLL mode
     */
    static const int POLL_NONE   = 0;
    static const int POLL_IN     = 1;
    static const int POLL_OUT    = 2;
    static const int POLL_ERR    = 3;

    /*
     * I/O type(socket, hardware device, message)
     */
    static const int IO_NONE     = 0;
    static const int IO_SOCKET   = 1;
    static const int IO_HWDEVICE = 2;
    static const int IO_MESSAFE  = 3;
    static const int IO_FILESYS  = 4;
    static const int IO_SSL      = 5;

    class iobytes 
    {
    public:
        iobytes() { _ret = 0; }

        iobytes(char *p, int ret)
        {
            _pbuf = std::unique_ptr<char>(p);
            _ret = ret;
        }

        iobytes(std::unique_ptr<char> &p, int ret)
        {
            _pbuf = std::move(p);
            _ret = ret;
        }

        ~iobytes() { }

        int size()
        { return _ret; }

        char *data()
        { return _pbuf.get(); }

    private:
        std::shared_ptr<char> _pbuf;
        int _ret;
    };

    /*
     * select:
     *     we will register the queue to 
     */
    class _lock_base 
    {
    public:
        virtual bool lock(estd::time_t t) = 0;
        virtual void unlock() = 0;
    }; // _lock_base

    class _io_base 
    {
    public:
        virtual bool open(std::string s) = 0;

        virtual bool fcntl(bool add, int&& flag) = 0;

        virtual bool fcntl(bool add, int& flag) = 0;

        virtual bool fcntl(bool add, const int& val) = 0;

        virtual ssize_t read(void *pbuf, size_t n) = 0;
        virtual class iobytes read(size_t n = 0) = 0;

        virtual ssize_t write(const void *pbuf, size_t n) = 0;
        virtual ssize_t write(std::string s) = 0;

        virtual int fd() = 0;

        virtual int type() = 0;

        virtual int poll(bool enter, int mode) = 0;

        virtual bool selected() = 0;

        virtual bool close() = 0;

        bool operator ==(class _io_base &other)
        { return this->fd() == other.fd() ? true : false; }

        void operator =(class _io_base &other)
        {
            if (this->_fd >= 0)
                close();
            this->_fd = other.move();
        }

        int move()
        {
            int tmp = _fd;
            _fd = -1;
            return tmp;
        }

    protected:
        int _fd;
    }; // _io_base
}

#endif /* c++11 */

#endif /* _TYPES_HPP_ */
