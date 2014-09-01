/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_NOSTARTLOOPTHREAD_H
#define SHAREMIND_NOSTARTLOOPTHREAD_H

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>


namespace sharemind {

class NoStartLoopThread {

private: /* Types: */

    struct Inner {

    /* Types: */

        enum State { WAITING_SIGNAL, DESTROY, START, STARTED, FINISHED };

    /* Methods: */

        inline Inner() {}

        template <typename F, typename ... Args>
        inline Inner(F && f, Args && ... args)
            : m_func(std::forward<F>(f), std::forward<Args>(args)...) {}

        inline void run() noexcept {
            std::unique_lock<std::mutex> lock(m_mutex);
            assert(m_state != FINISHED);
            for (;;) {
                while (m_state == WAITING_SIGNAL)
                    m_userToThreadCond.wait(lock);

                assert(m_state == DESTROY || m_state == START);
                if (m_state == DESTROY) {
                    m_state = FINISHED;
                    return;
                }

                m_state = STARTED;
                lock.unlock();
                m_func();
                lock.lock();
                m_state = WAITING_SIGNAL;
                m_threadToUserCond.notify_all();
                m_userToThreadCond.wait(lock);
            }
        }

        inline void startThread() noexcept {
            std::lock_guard<std::mutex> guard(m_mutex);
            assert(m_state == WAITING_SIGNAL);
            m_state = START;
            m_userToThreadCond.notify_one();
        }

        inline void stopLoop() noexcept {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_state = DESTROY;
            m_userToThreadCond.notify_one();
        }

        inline void joinPrivate() noexcept {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_state != WAITING_SIGNAL)
                m_threadToUserCond.wait(lock);
        }

    /* Fields: */

        std::mutex m_mutex;
        std::condition_variable m_userToThreadCond;
        std::condition_variable m_threadToUserCond;
        std::function<void () noexcept> m_func;
        State m_state = WAITING_SIGNAL;

    }; /* struct Inner { */

public: /* Methods: */

    template <typename F, typename ... Args>
    inline NoStartLoopThread(F && f, Args && ... args)
        : m_inner(std::forward<F>(f), std::forward<Args>(args)...)
        , m_thread(std::bind(&Inner::run, &m_inner))
    {}

    inline NoStartLoopThread() : m_thread(std::bind(&Inner::run, &m_inner)) {}

    inline ~NoStartLoopThread() noexcept {
        m_inner.stopLoop();
        m_thread.join();
    }

    template <typename F, typename ... Args>
    void setFunction(F && f, Args && ... args) {
        m_inner.m_func = std::function<void () noexcept>(
                    std::forward<F>(f),
                    std::forward<Args>(args)...);
    }

    inline void start() noexcept { m_inner.startThread(); }
    inline void join() noexcept { m_inner.joinPrivate(); }

private: /* Fields: */

    Inner m_inner;
    std::thread m_thread;

}; /* class NoStartLoopThread { */

} /* namespace sharemind { */

#endif /* SHAREMIND_NOSTARTLOOPTHREAD_H */
