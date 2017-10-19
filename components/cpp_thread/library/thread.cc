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


template<std::size_t Stk_size, std::size_t Priority>
template<typename Func>
void thread<Stk_size, Priority>::_func_tmpl<Func>::start()
{
    _func();
}

template<std::size_t Stk_size, std::size_t Priority>
template<typename Func>
void thread<Stk_size, Priority>::_func_tmpl<Func>::exit()
{
    _thread_deinit(_tid);
    _thread_exit();
}

template<std::size_t Stk_size, std::size_t Priority>
template<typename Func, typename... Args>
thread<Stk_size, Priority>::thread(Func&& f, Args&&... args)
{
    _thread_init(&_id);
    _start_thread(_make_routine(std::bind(std::forward<Func>(f),
                                          std::forward<Args>(args)...),
                                &_id));
}

template<std::size_t Stk_size, std::size_t Priority>
thread<Stk_size, Priority>::~thread()
{
    if (_id)
        _thread_destroy(&_id);
}

template<std::size_t Stk_size, std::size_t Priority>
void *thread<Stk_size, Priority>::_base_thread(void *p)
{
    _thread_ref_take();
    std::shared_ptr<struct _base_tmpl> tp((struct _base_tmpl *)p);

    try {
        tp.get()->start();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    tp.get()->exit();
    _thread_ref_release();

    return NULL;
}

template<std::size_t Stk_size, std::size_t Priority>
template<typename Func>
thread<Stk_size, Priority>::_func_tmpl<Func> *thread<Stk_size, Priority>::_make_routine(Func &&f, _thread_t *tid)
{
    return new _func_tmpl<Func>(std::forward<Func>(f), tid);
}

template<std::size_t Stk_size, std::size_t Priority>
void thread<Stk_size, Priority>::_start_thread(struct thread<Stk_size, Priority>::_base_tmpl *p)
{
    if (!p || _thread_create(&_id, _base_thread, p, _stk_size, _priority)) {
        delete p;
        throw std::logic_error("thread low-level constructor fails");
    }
}

estd::time_t sleep(estd::time_t t)
{
    return _thread_sleep(t);
}
