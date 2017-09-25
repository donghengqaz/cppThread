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

#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#if __cplusplus < 201103L
#error "need c++11 or higher"
#else

#include <functional>
#include <memory>
#include <stdexcept>

#include "types.hpp"

namespace estd
{

    template<std::size_t Stk_size
#ifdef CONFIG_THREAD_TMPL_DEFAULT_ARGS
                                  =estd::default_stk_size
#endif
                                  ,
             std::size_t Priority
#ifdef CONFIG_THREAD_TMPL_DEFAULT_ARGS
                                  =estd::default_priority
#endif
                                  >
    class thread 
    {
    public:
        thread()
        { _thread_init(&_id); }

        template<typename Func, typename... Args>
        thread(Func&& f, Args&&... args) : thread() 
        {
            _start_thread(_make_routine(std::bind(std::forward<Func>(f),
                                                  std::forward<Args>(args)...),
                                        &_id));
        }

        ~thread()
        { if (_id) _thread_destroy(&_id); }

    private:
        const std::size_t _priority = Priority;
        const std::size_t _stk_size = Stk_size;
        estd::_thread_t _id;

        struct _base_tmpl
        {
        public:
            virtual ~_base_tmpl() { }
            virtual void start() = 0;
            virtual void exit() = 0;
        };

        template<typename Func>
        struct _func_tmpl : public _base_tmpl
        {
        private:
            Func _func;
            estd::_thread_t *_tid;
        public:
            _func_tmpl(Func &&f, estd::_thread_t *tid) : _func(std::forward<Func>(f)), _tid(tid) { }

            void start() {
                _func();
            }

            void exit() {
                estd::_thread_deinit(_tid);
            }
        };

        static void *_base_thread(void *p)
        {
            estd::_thread_ref_take();
            std::shared_ptr<struct _base_tmpl> tp((struct _base_tmpl *)p);

            try {
                tp.get()->start();
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
            }

            tp.get()->exit();
            estd::_thread_ref_release();

            return NULL;
        }

        template<typename Func>
        _func_tmpl<Func> *_make_routine(Func &&f, estd::_thread_t *tid) 
        { return new _func_tmpl<Func>(std::forward<Func>(f), tid); }

        void _start_thread(struct _base_tmpl *p)
        {
            if (!p || estd::_thread_create(&_id, _base_thread, p, _stk_size, _priority)) {
                delete p;
                throw std::logic_error("thread low-level constructor fails");
            }
        }
    }; // thread

    inline estd::time_t sleep(estd::time_t &t)
    {
        return estd::_thread_sleep(t);
    }

    inline estd::time_t sleep(estd::time_t &&t)
    {
        estd::time_t tt = std::forward<estd::time_t>(t);
        return estd::sleep(tt);
    }
};

#endif /* c++11 */

#endif /* _THREAD_HPP_ */
