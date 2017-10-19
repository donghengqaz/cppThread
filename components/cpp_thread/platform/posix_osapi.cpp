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

#ifdef CONFIG_USING_POSIX

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <string>

#include <pthread.h>
#include <mqueue.h>

#include <arch/arch.hpp>

static int thread_ref;
static int queue_id = 0;
const unsigned int default_msg_priority = 1;

namespace estd {

/**
 * thread
 */
void _thread_ref_take(void)
{
	thread_ref++;
}

void _thread_ref_release(void)
{
	thread_ref--;
}

int _thread_init(estd::_thread_t *tid)
{
	*tid = 0;
	return 0;
}

int _thread_deinit(estd::_thread_t *tid)
{
	*tid = 0;
	return 0;
}

int _thread_exit(void)
{
    return 0;
}

int _thread_create(estd::_thread_t *tid, void *(*entry)(void *), void *arg, size_t stack_bytes, size_t priority)
{
	return pthread_create((pthread_t *)tid, NULL, entry, arg);
}

int _thread_destroy(estd::_thread_t *id)
{
	int ret = pthread_cancel((pthread_t)*id);
	if (ret)
		return ret;
	ret = pthread_join((pthread_t)*id, NULL);
	if (ret)
		return ret;
	_thread_deinit(id);
	return 0;
}

estd::time_t _thread_sleep(estd::time_t t)
{
	if (t > estd::max_time)
		return 0;

	unsigned int s = static_cast<unsigned int>(t);
	__useconds_t us = static_cast<__useconds_t>(std::fmod(t * 10e5, 10e5));

	sleep(s);
	usleep(us);

	return t;
}

/**
 * message
 */
int _message_init(int *mid)
{
    *mid = 1;
    return 0;
}

int _message_fd(int *mid)
{
	return *mid;
}

int _message_create(int *mid, std::size_t deepth, std::size_t item_length)
{
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
        return -ENOMEM;

    *mid = mqd;

    return 0;
}

int _message_poll(int *mid, bool enter, int mode)
{
	return 0;
}

int _message_recv(int *mid, char *pbuf, std::size_t len, estd::time_t t)
{
    int ret;
    unsigned int priority = default_msg_priority;

    if (t == estd::max_time)
        ret = mq_receive(*mid, pbuf, len, &priority);
    else if (t >= estd::min_time && t < estd::max_time) {
        struct timespec _time;

        _time.tv_sec = static_cast<__time_t>(t);
        _time.tv_nsec = static_cast<__syscall_slong_t>(std::fmod(t * 10e8, 10e8));

        ret = mq_timedreceive(*mid, pbuf, len, &priority, &_time);
    } else
        ret = -EINVAL;

    return ret > 0 ? 0 : ret == 0 ? -ENODATA : ret;
}

int _message_send(int *mid, const char *pbuf, std::size_t len)
{
	return mq_send(*mid, pbuf, len, default_msg_priority);
}

bool _message_selected()
{
	return true;
}

int _message_destroy(int *mid)
{
    int ret = mq_close(*mid);
    if (!ret)
        _message_init(mid);
    return ret;
}

/**
 * mutex
 */
int _mutex_init(estd::_mutex_t *mid)
{
    std::memset(mid, 0, sizeof(estd::_mutex_t));
    return 0;
}

int _mutex_create(estd::_mutex_t *mid, bool recursived)
{
    int ret;
    pthread_mutexattr_t attr;
    pthread_mutex_t *mutex;

    mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (!mutex)
        return -ENOMEM;

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
        goto fail1;

    ret = pthread_mutex_init(mutex, &attr);
    if (ret)
        goto fail2;

    *mid = (estd::_mutex_t)mutex;

    ret = pthread_mutexattr_destroy(&attr);

    return ret;

fail2:
    ret = pthread_mutexattr_destroy(&attr);
fail1:
    free(mutex);
    return ret;
}

int _mutex_destroy(estd::_mutex_t *mid)
{
	return pthread_mutex_destroy((pthread_mutex_t *)mid);
}

int _mutex_lock(estd::_mutex_t *mid, estd::time_t t, bool recursived)
{
    int ret;

    if (t == estd::max_time)
        ret = pthread_mutex_lock((pthread_mutex_t *)mid);
    else if (t >= estd::min_time && t < estd::max_time) {
        struct timespec _time;

        _time.tv_sec = static_cast<__time_t>(t);
        _time.tv_nsec = static_cast<__syscall_slong_t>(std::fmod(t * 10e8, 10e8));

        ret = pthread_mutex_timedlock((pthread_mutex_t *)mid, &_time);
    } else
        ret = -EINVAL;

    return ret;
}

int _mutex_unlock(estd::_mutex_t *mid, bool recursived)
{
	return pthread_mutex_unlock((pthread_mutex_t *)mid);
}

/**
 * socket
 */
int _socket_create(int *fd, int d, int t, int f)
{
	*fd = socket(d, t, f);
	return *fd >= 0 ? 0 : -ENOMEM;
}

int _socket_destroy(int *fd)
{
	int ret = close(*fd);
	if (!ret)
		*fd = -1;
	return ret;
}

int _socket_bind(int *fd, in_addr_t addr, std::uint16_t port)
{
	struct sockaddr_in _addr;

	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = addr;
	_addr.sin_port = htons(port);
	return bind(*fd, (struct sockaddr *)&_addr, sizeof(_addr));
}

int _socket_connect(int *_fd, in_addr_t addr, std::uint16_t port)
{
	struct sockaddr_in sockaddr;

	std::memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = addr;
	sockaddr.sin_port = htons(port);
	return connect(*_fd, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
}

int _socket_listen(int *fd, int max)
{
	return listen(*fd, max);
}

int _socket_accept(int *fd)
{
	struct sockaddr_in _addr;
	socklen_t len;

	return accept(*fd, (struct sockaddr *)&_addr, &len);
}

int _socket_recv(int *fd, void *pbuf, int n, int flag)
{
	return recv(*fd, pbuf, n, flag);
}

int _socket_send(int *fd, const void *pbuf, int n, int flag)
{
	return send(*fd, pbuf, n, flag);
}

int _socket_setsockopt(int *fd, int level, int optname, const void *optval, socklen_t optlen)
{
	return setsockopt(*fd, level, optname, optval, optlen);
}

int _socket_fcntl(int *fd, int opt, int val)
{
	return fcntl(*fd, opt, val);
}

int _raw_select(int fd, fd_set *rfd, fd_set *wfd, fd_set *efd, estd::time_t t)
{
	struct timeval timeout;

	timeout.tv_sec = static_cast<__time_t>(t);
	timeout.tv_usec = static_cast<__useconds_t>(std::fmod(t * 10e5, 10e5));

	return select(fd, rfd, wfd, efd, &timeout);
}

bool _socket_selected()
{
	return true;
}

}; // namespace estd

#endif // CONFIG_USING_POSIX
