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

#ifndef _FREERTOS_HPP_
#define _FREERTOS_HPP_

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cstring>

#include <sys/errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

namespace estd {
    /**
     * time
     */
    const time_t max_time = static_cast<time_t>(portMAX_DELAY * portTICK_PERIOD_MS / 1000); //1.79769313486231570E+308 ?
    const time_t min_time = 0;

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
    const int _message_fd_offset = CONFIG_LWIP_MAX_SOCKETS;

    static struct _os_message msg_array[CONFIG_MAX_MESSAGE];

    inline int _message_alloc()
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

    inline void _message_free(int fd)
    {
        vTaskSuspendAll();
        int p = fd - _message_fd_offset;
        msg_array[p].taskd = NULL;
        xTaskResumeAll();
    }

    inline int _message_fd(int *mid)
    { return (int)*mid; }

    inline int _message_init(int *mid)
    {
        std::memset(mid, 0, sizeof(int));
        return 0;
    }

    inline int _message_poll(int *mid, bool enter, int mode)
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

    inline int _message_create(int *mid, size_t deepth, size_t item_length)
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

    inline int _message_recv(int *mid, char *pbuf, size_t len, time_t t)
    {
        TickType_t ticks = static_cast<TickType_t>(t * 10E2) / portTICK_PERIOD_MS;

        return xQueueReceive(msg_array[*mid - _message_fd_offset].qd, pbuf, ticks) == pdTRUE ? 0 : -ETIMEDOUT;
    }

    inline int _message_send(int *mid, const char *pbuf, size_t len)
    {
        int ret = xQueueSendToBack(msg_array[*mid - _message_fd_offset].qd, pbuf, 0) == pdTRUE ? 0 : -ENOSPC;
        vTaskSuspendAll();
        if (msg_array[*mid - _message_fd_offset].taskd) {
            SemaphoreHandle_t *sem = (SemaphoreHandle_t *)pvTaskGetThreadLocalStoragePointer(msg_array[*mid - _message_fd_offset].taskd, CONFIG_LWIP_THREAD_LOCAL_STORAGE_INDEX);
            if (*sem)
                xSemaphoreGive(*sem);
        }
        xTaskResumeAll();
        return ret;
    }

    inline bool _message_selected()
    { return false; }

    inline int _message_destroy(int *mid)
    {
        vQueueDelete(msg_array[*mid - _message_fd_offset].qd);
        _message_free(*mid);
        return 0;
    }

    /**
     * thread
     */
    using _thread_t = TaskHandle_t;
#ifdef CONFIG_THREAD_TMPL_DEFAULT_ARGS
    const std::size_t default_priority = 2;
    const std::size_t default_stk_size = 8192;
#endif
    typedef void (*_thread_pfn)(void *p);
    static int _thread_ref;

    inline void _thread_ref_take(void)
    { _thread_ref++; }

    inline void _thread_ref_release(void)
    {
        _thread_ref--;
        vTaskDelete(NULL);
    }

    inline int _thread_init(_thread_t *tid)
    {
        *tid = NULL;
        return 0;
    }

    inline int _thread_deinit(_thread_t *tid)
    {
        *tid = NULL;
        return 0;
    }

    inline int _thread_create(_thread_t *tid, void *(*entry)(void *), void *arg, size_t stack_bytes, size_t priority)
    {
        return xTaskCreate((_thread_pfn)entry, "estd", stack_bytes, arg, priority, tid) == pdTRUE ? 0 : -ENOMEM;
    }

    inline int _thread_destroy(_thread_t *id)
    {
        _thread_t tmp = *id;
        _thread_init(id);
        vTaskDelete(tmp);
        return 0;
    }

    time_t _thread_sleep(time_t t)
    {
        if (t > max_time)
            return 0;

        TickType_t ticks = static_cast<TickType_t>(t * 10E2) / portTICK_PERIOD_MS;

        vTaskDelay(ticks);
        return t;
    }

    /**
     * mutex lock
     */
    using _mutex_t  = SemaphoreHandle_t;

    inline int _mutex_init(_mutex_t *mid)
    {
        *mid = NULL;
        return 0;
    }    

    inline int _mutex_create(_mutex_t *mid, bool recursived)
    {
        if (recursived)
            *mid = xSemaphoreCreateRecursiveMutex();
        else
            *mid = xSemaphoreCreateMutex();
        return *mid != NULL ? 0 : -ENOMEM;
    }

    inline int _mutex_destroy(_mutex_t *mid)
    {
        vSemaphoreDelete(*mid);
        return 0;
    }

    inline int _mutex_lock(_mutex_t *mid, time_t t, bool recursived)
    {
        int ret;
        TickType_t ticks = static_cast<TickType_t>(t * 10e2) / portTICK_PERIOD_MS;

        if (recursived)
            ret = xSemaphoreTakeRecursive(*mid, ticks);
        else
            ret = xSemaphoreTake(*mid, ticks);
        return ret == pdTRUE ? 0 : -ETIMEDOUT;
    }

    inline int _mutex_unlock(_mutex_t *mid, bool recursived)
    {
        int ret;

        if (recursived)
            ret = xSemaphoreGiveRecursive(*mid);
        else
            ret = xSemaphoreGive(*mid);        
        return ret == pdTRUE ? 0 : -ENOENT;
    }

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
    { return lwip_fcntl(*fd, opt, val); }

    inline int _raw_select(int fd, fd_set *rfd, fd_set *wfd, fd_set *efd, time_t t)
    {
        struct timeval timeout;

        timeout.tv_sec = static_cast<long>(t);
        timeout.tv_usec = static_cast<long>(std::fmod(t * 10e5, 10e5));

        return select(fd, rfd, wfd, efd, &timeout);
    }

    inline bool _socket_selected()
    { return true; }

}; // estd

#endif /* _FREERTOS_HPP_ */
