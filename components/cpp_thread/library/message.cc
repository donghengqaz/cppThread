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


template<typename Type, size_t Deepth>
message<Type, Deepth>::message()
{
    _message_init(&_fd);
    if (_message_create(&_fd, _deepth, _length))
        throw std::logic_error("message low-level constructor fails");
    _default_pdu_bytes = _length + 1;
}

template<typename Type, size_t Deepth>
message<Type, Deepth>::~message() 
{
    _message_destroy(&_fd);
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::recv(Type &p, estd::time_t t) 
{
    return _message_recv(&_fd, (char *)&p, _length, t) == 0 ? true : false;
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::send(Type &p) 
{
    return _message_send(&_fd, (const char *)&p, _length) == 0 ? true : false;
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::send(Type &&p) 
{
    Type pp = std::forward<Type>(p);
    return _message_send(&_fd, (const char *)&pp, _length) == 0 ? true : false;
}

/*
 * IO base
 **/
template<typename Type, size_t Deepth>
bool message<Type, Deepth>::open(std::string s) 
{
    return false;
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::fcntl(bool add, int& flag)
{
    return false;
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::fcntl(bool add, const int& val)
{
    return false;
}

template<typename Type, size_t Deepth>
ssize_t message<Type, Deepth>::read(void *pbuf, std::size_t n)
{
    return _message_recv(&_fd, (char *)pbuf, _length, max_time) == 0 ? _length : -1;
}

template<typename Type, size_t Deepth>
ssize_t message<Type, Deepth>::write(const void *pbuf, std::size_t n)
{
    return _message_send(&_fd, (const char *)pbuf, _length) == 0 ? _length : -1;
}

template<typename Type, size_t Deepth>
ssize_t message<Type, Deepth>::write(std::string s)
{
    return write(s.c_str(), s.size() + 1);
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::close()
{
    return false;
}

template<typename Type, size_t Deepth>
int message<Type, Deepth>::poll(bool enter, int mode)
{
    return _message_poll(&_fd, enter, mode);
}

template<typename Type, size_t Deepth>
int message<Type, Deepth>::type()
{
    return IO_MESSAGE;
}

template<typename Type, size_t Deepth>
bool message<Type, Deepth>::selected()
{
    return _message_selected();
}
