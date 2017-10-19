#ifndef _OSAPI_HPP_
#define _OSAPI_HPP_

#include "types.hpp"
#include "arch/arch.hpp"

namespace estd {

	/**
	 * thread
	 */
	void _thread_ref_take(void);

	void _thread_ref_release(void);

	int _thread_init(estd::_thread_t *tid);

	int _thread_deinit(estd::_thread_t *tid);

	int _thread_exit(void);

	int _thread_create(estd::_thread_t *tid, void *(*entry)(void *), void *arg, size_t stack_bytes, size_t priority);

	int _thread_destroy(estd::_thread_t *id);

	estd::time_t _thread_sleep(estd::time_t t);

	/**
	 * message
	 */
	int _message_init(int *mid);

	int _message_fd(int *mid);

	int _message_create(int *mid, std::size_t deepth, std::size_t item_length);

	int _message_poll(int *mid, bool enter, int mode);

	int _message_recv(int *mid, char *pbuf, std::size_t len, estd::time_t t);

	int _message_send(int *mid, const char *pbuf, std::size_t len);

	bool _message_selected();

	int _message_destroy(int *mid);

	/**
	 * mutex
	 */
	int _mutex_init(estd::_mutex_t *mid);

	int _mutex_create(estd::_mutex_t *mid, bool recursived);

	int _mutex_destroy(estd::_mutex_t *mid);

	int _mutex_lock(estd::_mutex_t *mid, estd::time_t t, bool recursived);

	int _mutex_unlock(estd::_mutex_t *mid, bool recursived);

	/**
	 * socket
	 */
	int _socket_create(int *fd, int d, int t, int f);

	int _socket_destroy(int *fd);

	int _socket_bind(int *fd, in_addr_t addr, std::uint16_t port);

	int  _socket_connect(int *_fd, in_addr_t addr, std::uint16_t port);

	int _socket_listen(int *fd, int max);

	int _socket_accept(int *fd);

	int _socket_recv(int *fd, void *pbuf, int n, int flag);

	int _socket_send(int *fd, const void *pbuf, int n, int flag);

	int _socket_setsockopt(int *fd, int level, int optname, const void *optval, socklen_t optlen);

	int _socket_fcntl(int *fd, int opt, int val);

	int _raw_select(int fd, fd_set *rfd, fd_set *wfd, fd_set *efd, estd::time_t t);

	bool _socket_selected();
};

#endif /* _OSAPI_HPP_ */
