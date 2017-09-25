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


#include <pthread.h>
#include <unistd.h>

#include <arch.hpp>

static int thread_ref;

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

int _thread_create(estd::_thread_t *tid, void *(*entry)(void *), void *arg, size_t stack_bytes, size_t priority)
{
	return pthread_create(tid, NULL, entry, arg);
}

int _thread_destroy(estd::_thread_t *id)
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

estd::time_t _thread_sleep(estd::time_t t)
{
	if (t > estd::max_time)
		return 0;

	unsigned int s = static_cast<unsigned int>(t);
	__useconds_t us = 0;//static_cast<__useconds_t>(std::fmod(t * 10e5, 10e5));

	sleep(s);
	usleep(us);

	return t;
}
