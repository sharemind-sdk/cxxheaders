/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_NOSTARTTHREAD_H
#define SHAREMIND_NOSTARTTHREAD_H

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>


namespace sharemind {

class NoStartThread {

private: /* Types: */

    struct Inner {

    /* Types: */

        enum State { WAITING_SIGNAL, DESTROY, START };

    /* Methods: */

        inline Inner() {}

        template <typename F, typename ... Args>
        inline Inner(F && f, Args && ... args)
            : m_func(std::forward<F>(f), std::forward<Args>(args)...) {}

        inline void run() noexcept {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_state == WAITING_SIGNAL)
                m_cond.wait(lock);
            assert(m_state == DESTROY || m_state == START);
            if (m_state == START) {
                lock.unlock();
                m_func();
            }
        }

        inline void startThread() noexcept {
            std::lock_guard<std::mutex> guard(m_mutex);
            assert(m_state == WAITING_SIGNAL);
            m_state = START;
            m_cond.notify_one();
        }

        inline void destroyPrivate() noexcept {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_state = DESTROY;
            m_cond.notify_one();
        }

    /* Fields: */

        std::mutex m_mutex;
        std::condition_variable m_cond;
        std::function<void () noexcept> m_func;
        State m_state = WAITING_SIGNAL;

    }; /* struct Inner { */

public: /* Methods: */

    inline NoStartThread() : m_thread(std::bind(&Inner::run, &m_inner)) {}

    template <typename F, typename ... Args>
    inline NoStartThread(F && f, Args && ... args)
        : m_inner(std::forward<F>(f), std::forward<Args>(args)...)
        , m_thread(std::bind(&Inner::run, &m_inner))
    {}

    inline ~NoStartThread() noexcept {
        m_inner.destroyPrivate();
        if (m_thread.joinable())
            m_thread.join();
    }

    template <typename F, typename ... Args>
    void setFunction(F && f, Args && ... args) {
        m_inner.m_func = std::function<void () noexcept>(
                    std::forward<F>(f),
                    std::forward<Args>(args)...);
    }

    inline void start() noexcept { m_inner.startThread(); }
    inline void join() noexcept { m_thread.join(); }

private: /* Fields: */

    Inner m_inner;
    std::thread m_thread;

}; /* class NoStartThread { */

} /* namespace sharemind { */

#endif /* SHAREMIND_NOSTARTTHREAD_H */
