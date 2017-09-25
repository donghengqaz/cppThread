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

#ifndef _POSIX_HPP_
#define _POSIX_HPP_

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <string>

#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <pthread.h>
#include <mqueue.h>

#include <unistd.h>

namespace estd {
    /**
     * time
     */
    using time_t = double;
    const time_t max_time = static_cast<time_t>(UINT32_MAX); //1.79769313486231570E+308 ?
    const time_t min_time = 0;

    /**
     * message
     */
    static int queue_id = 0;
    const unsigned int default_msg_priority = 1;

    inline int _message_init(int *mid)
    {
        *mid = 1;
        return 0;
    }

    inline int _message_fd(int *mid)
    { return *mid; }

    inline int _message_create(int *mid, size_t deepth, size_t item_length)
    {
        int ret;
        mqd_t mqd;
        struct mq_attr attr;
        char buf[32];

        std::sprintf(buf, "/estd_queue%d", queue_id++);

        std::string code = "rm /dev/mqueue";
        code += buf;
        system(code.c_str());

        attr.mq_flags = 0;
        attr.mq_maxmsg = deepth;
        attr.mq_msgsize = item_length;
        attr.mq_curmsgs = 0;

        mqd = mq_open(buf, O_CREAT | O_RDWR, 0777, &attr);
        if (mqd < 0)
            return ret;
        
        *mid = mqd;

        return 0;
    }

    inline int _message_poll(int *mid, bool enter, int mode)
    { return 0; }

    inline int _message_recv(int *mid, char *pbuf, size_t len, time_t t)
    {
        int ret;
        unsigned int priority = default_msg_priority;

        if (t == max_time)
            ret = mq_receive(*mid, pbuf, len, &priority);
        else if (t >= min_time && t < max_time) {
            struct timespec _time;
            
            _time.tv_sec = static_cast<__time_t>(t);
            _time.tv_nsec = static_cast<__syscall_slong_t>(std::fmod(t * 10e8, 10e8));
            
            ret = mq_timedreceive(*mid, pbuf, len, &priority, &_time);            
        } else
            ret = -EINVAL;

        return ret > 0 ? 0 : ret == 0 ? -ENODATA : ret;
    }

    inline int _message_send(int *mid, const char *pbuf, size_t len)
    { return mq_send(*mid, pbuf, len, default_msg_priority); }

    inline bool _message_selected()
    { return true; }

    inline int _message_destroy(int *mid)
    {
        int ret = mq_close(*mid);
        if (!ret)
            _message_init(mid);
        return ret;
    }

    /**
     * thread
     */
    using _thread_t = pthread_t;
#ifdef CONFIG_THREAD_TMPL_DEFAULT_ARGS
    const std::size_t default_priority = 1;
    const std::size_t default_stk_size = 8192;
#endif
    static int _thread_ref;

    inline void _thread_ref_take(void)
    { _thread_ref++; }

    inline void _thread_ref_release(void)
    {
        _thread_ref--;
    }

    inline int _thread_init(_thread_t *tid)
    {
        *tid = 0;
        return 0;
    }

    inline int _thread_deinit(_thread_t *tid)
    {
        *tid = 0;
        return 0;
    }

    inline int _thread_create(_thread_t *tid, void *(*entry)(void *), void *arg, size_t stack_bytes, size_t priority)
    { return pthread_create(tid, NULL, entry, arg); }

    inline int _thread_destroy(_thread_t *id)
    {
        int ret = pthread_cancel(*id);
        if (ret)
            return ret;
        ret = pthread_join(*id, NULL);
        if (ret)
            return ret;
        _thread_init(id);
        return 0;
    }

    inline time_t _thread_sleep(time_t t)
    {
        if (t > max_time)
            return 0;

        unsigned int s = static_cast<unsigned int>(t);
        __useconds_t us = static_cast<__useconds_t>(std::fmod(t * 10e5, 10e5));

        sleep(s);
        usleep(us);

        return t;
    }

    /**
     * mutex lock
     */
    using _mutex_t  = pthread_mutex_t;

    inline int _mutex_init(_mutex_t *mid)
    {
        std::memset(mid, 0, sizeof(_mutex_t));
        return 0;
    } 

    inline int _mutex_create(_mutex_t *mid, bool recursived)
    {
        int ret;
        pthread_mutexattr_t attr;

        ret = pthread_mutexattr_init(&attr);
        if (ret)
            return ret;

        int type;
        if (recursived)
            type = PTHREAD_MUTEX_RECURSIVE;
        else
            type = PTHREAD_MUTEX_NORMAL;
        ret = pthread_mutexattr_settype(&attr, type);
        if (ret)
            goto fail;

        ret = pthread_mutex_init(mid, &attr);
        if (ret)
            goto fail;

    fail:
        ret = pthread_mutexattr_destroy(&attr);
        return ret;
    }

    inline int _mutex_destroy(_mutex_t *mid)
    { return pthread_mutex_destroy(mid); }

    inline int _mutex_lock(_mutex_t *mid, time_t t, bool recursived)
    {
        int ret;

        if (t == max_time)
            ret = pthread_mutex_lock(mid);
        else if (t >= min_time && t < max_time) {
            struct timespec _time;

            _time.tv_sec = static_cast<__time_t>(t);
            _time.tv_nsec = static_cast<__syscall_slong_t>(std::fmod(t * 10e8, 10e8));

            ret = pthread_mutex_timedlock(mid, &_time);
        } else
            ret = -EINVAL;

        return ret;
    }

    inline int _mutex_unlock(_mutex_t *mid, bool recursived)
    { return pthread_mutex_unlock(mid); }

    /**
     * socket
     */
    inline int _socket_create(int *fd, int d, int t, int f)
    {
        *fd = socket(d, t, f);
        return *fd >= 0 ? 0 : -ENOMEM;
    }

    inline int _socket_destroy(int *fd)
    {
        int ret = close(*fd);
        if (!ret)
            *fd = -1;
        return ret;
    }

    inline int _socket_bind(int *fd, in_addr_t addr, std::uint16_t port)
    {
        int ret;
        struct sockaddr_in _addr;

        std::memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = addr;
        _addr.sin_port = htons(port);
        return bind(*fd, (struct sockaddr *)&_addr, sizeof(_addr));
    }

    inline int  _socket_connect(int *_fd, in_addr_t addr, std::uint16_t port)
    {
        struct sockaddr_in sockaddr;

        std::memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = addr;
        sockaddr.sin_port = htons(port);
        return connect(*_fd, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
    }

    inline int _socket_listen(int *fd, int max)
    { return listen(*fd, max); }

    inline int _socket_accept(int *fd)
    {
        struct sockaddr_in _addr;
        socklen_t len;

        return accept(*fd, (struct sockaddr *)&_addr, &len);
    }

    inline int _socket_recv(int *fd, void *pbuf, int n, int flag)
    { return recv(*fd, pbuf, n, flag); }

    inline int _socket_send(int *fd, const void *pbuf, int n, int flag)
    { return send(*fd, pbuf, n, flag); }

    inline int _socket_setsockopt(int *fd, int level, int optname, const void *optval, socklen_t optlen)
    { return setsockopt(*fd, level, optname, optval, optlen); }

    inline int _socket_fcntl(int *fd, int opt, int val)
    { return fcntl(*fd, opt, val); }

    inline int _raw_select(int fd, fd_set *rfd, fd_set *wfd, fd_set *efd, time_t t)
    {
        struct timeval timeout;

        timeout.tv_sec = static_cast<__time_t>(t);
        timeout.tv_usec = static_cast<__useconds_t>(std::fmod(t * 10e5, 10e5));

        return select(fd, rfd, wfd, efd, &timeout);
    }

    inline bool _socket_selected() 
    { return true; }

}; // estd

#endif /* _POSIX_HPP_ */

// 2. ticks  max? min?