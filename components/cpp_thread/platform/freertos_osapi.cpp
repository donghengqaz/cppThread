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

#ifdef CONFIG_USING_FREERTOS

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <string>

#include <arch/arch.hpp>

#define MESSAGE_SEM_OFF 0

#define _message_fd_offset CONFIG_LWIP_MAX_SOCKETS

/**
 * message
 * 
 * If we want to use "select" for FreeRTOS queue of esp-idf, following steps are needed:
 * 1. before real IO "select", we should mark the current task handle to the target queues
 *    and the opeartion must be atomic, it means we should use esp-idf APIS:
 *      a) vTaskSuspendAll()
 *      b) xTaskResumeAll()
 * 2. mutex lock must be used to protect the task handle to be changed
 * 3. When send queue message, we should wake up the select task with target semaphore,
 *    using the APIs:
 *      a) SemaphoreHandle_t *sem = pvTaskGetThreadLocalStoragePointer(task, CONFIG_LWIP_THREAD_LOCAL_STORAGE_INDEX);
 *      b) xSemaphoreGive(*sem);
 */
struct _os_message
{
    QueueHandle_t     qd;
    TaskHandle_t      taskd;
};

typedef void (*_thread_pfn)(void *p);

static int thread_ref;
const unsigned int default_msg_priority = 1;
static struct _os_message msg_array[CONFIG_MAX_MESSAGE];

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
    vTaskDelete(NULL);
    return 0;
}

int _thread_create(estd::_thread_t *tid, void *(*entry)(void *), void *arg, size_t stack_bytes, size_t priority)
{
    return xTaskCreate((_thread_pfn)entry, "estd", stack_bytes, arg, priority, (TaskHandle_t *)tid) == pdTRUE ? 0 : -ENOMEM;
}

int _thread_destroy(estd::_thread_t *id)
{
    _thread_deinit(id);
    vTaskDelete((TaskHandle_t)*id);
	return 0;
}

estd::time_t _thread_sleep(estd::time_t t)
{
	if (t > estd::max_time)
		return 0;

    TickType_t ticks = static_cast<TickType_t>(t * 10E2) / portTICK_PERIOD_MS;

	vTaskDelay(ticks);

	return t;
}

/**
 * message
 */
static inline int _message_alloc()
{
    int p = -1;
    vTaskSuspendAll();
    for (int i = 0; i < CONFIG_MAX_MESSAGE; i++)
        if (!msg_array[i].qd) {
            p = i;
            break;
        }
    xTaskResumeAll();
    return p >= 0 ? p + _message_fd_offset : p;
}

static inline void _message_free(int fd)
{
    vTaskSuspendAll();
    int p = fd - _message_fd_offset;
    msg_array[p].taskd = NULL;
    xTaskResumeAll();
}

int _message_init(int *mid)
{
    *mid = -1;
    return 0;
}

int _message_fd(int *mid)
{
	return *mid;
}

int _message_create(int *mid, std::size_t deepth, std::size_t item_length)
{
    int p = _message_alloc();
    if (p < 0)
        return -ENOMEM;

    msg_array[p - _message_fd_offset].qd = xQueueCreate(deepth, item_length);
    if (!msg_array[p- _message_fd_offset].qd)
        return -ENOMEM;

    msg_array[p - _message_fd_offset].taskd = NULL;

    *mid = (int)p;

    return 0;
}

int _message_poll(int *mid, bool enter, int mode)
{
    vTaskSuspendAll();
    if (enter)
        msg_array[*mid - _message_fd_offset].taskd = xTaskGetCurrentTaskHandle();
    else
        msg_array[*mid - _message_fd_offset].taskd = NULL;
    xTaskResumeAll();
    if (uxQueueMessagesWaiting(msg_array[*mid - _message_fd_offset].qd))
        return POLL_IN;
    return 0;
}

int _message_recv(int *mid, char *pbuf, std::size_t len, estd::time_t t)
{
    TickType_t ticks = static_cast<TickType_t>(t * 10E2) / portTICK_PERIOD_MS;
    
    return xQueueReceive(msg_array[*mid - _message_fd_offset].qd, pbuf, ticks) == pdTRUE ? 0 : -ETIMEDOUT;
}

int _message_send(int *mid, const char *pbuf, std::size_t len)
{
    int ret = xQueueSendToBack(msg_array[*mid - _message_fd_offset].qd, pbuf, 0) == pdTRUE ? 0 : -ENOSPC;
    vTaskSuspendAll();
    if (msg_array[*mid - _message_fd_offset].taskd) {
        SemaphoreHandle_t *sem = (SemaphoreHandle_t *)pvTaskGetThreadLocalStoragePointer(msg_array[*mid - _message_fd_offset].taskd, MESSAGE_SEM_OFF);
        if (*sem)
            xSemaphoreGive(*sem);
    }
    xTaskResumeAll();
    return ret;
}

bool _message_selected()
{
	return false;
}

int _message_destroy(int *mid)
{
    vQueueDelete(msg_array[*mid - _message_fd_offset].qd);
    _message_free(*mid);
    return 0;
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
    if (recursived)
        *mid = (estd::_mutex_t)xSemaphoreCreateRecursiveMutex();
    else
        *mid = (estd::_mutex_t)xSemaphoreCreateMutex();
    return *mid != 0 ? 0 : -ENOMEM;
}

int _mutex_destroy(estd::_mutex_t *mid)
{
    vSemaphoreDelete((SemaphoreHandle_t)*mid);
    return 0;
}

int _mutex_lock(estd::_mutex_t *mid, estd::time_t t, bool recursived)
{
    int ret;
    TickType_t ticks = static_cast<TickType_t>(t * 10e2) / portTICK_PERIOD_MS;

    if (recursived)
        ret = xSemaphoreTakeRecursive((SemaphoreHandle_t)*mid, ticks);
    else
        ret = xSemaphoreTake((SemaphoreHandle_t)*mid, ticks);
    return ret == pdTRUE ? 0 : -ETIMEDOUT;
}

int _mutex_unlock(estd::_mutex_t *mid, bool recursived)
{
    int ret;
    
    if (recursived)
        ret = xSemaphoreGiveRecursive((SemaphoreHandle_t)*mid);
    else
        ret = xSemaphoreGive((SemaphoreHandle_t)*mid);        
    return ret == pdTRUE ? 0 : -ENOENT;
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
	return lwip_fcntl(*fd, opt, val);
}

int _raw_select(int fd, fd_set *rfd, fd_set *wfd, fd_set *efd, estd::time_t t)
{
	struct timeval timeout;

	timeout.tv_sec = static_cast<long>(t);
	timeout.tv_usec = static_cast<long>(std::fmod(t * 10e5, 10e5));

	return select(fd, rfd, wfd, efd, &timeout);
}

bool _socket_selected()
{
	return true;
}

}; // namespace estd

#endif // CONFIG_USING_POSIX
