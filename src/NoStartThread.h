/*
 * Copyright (C) Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef SHAREMIND_NOSTARTTHREAD_H
#define SHAREMIND_NOSTARTTHREAD_H

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include "Future.h"


namespace sharemind {

class NoStartThread {

public: /* Methods: */

    NoStartThread() : m_thread(&NoStartThread::run_, this) {}

    template <typename F>
    NoStartThread(F && f)
        : m_func(std::forward<F>(f))
        , m_thread(&NoStartThread::run_, this)
    {
        static_assert(noexcept(std::declval<F &&>()()),
                      "NoStartThread function must be noexcept!");
    }

    ~NoStartThread() noexcept { stop(); }

    template <typename F>
    void setFunction(F && f) {
        static_assert(noexcept(std::declval<F &&>()()),
                      "NoStartThread function must be noexcept!");
        m_func = std::function<void()>(std::forward<F>(f));
    }

    void start() noexcept {
        assert(m_startPromise.isValid());
        m_startPromise.setValue(true);
    }

    inline void stop() noexcept {
        if (m_startPromise.isValid())
            m_startPromise.setValue(false);
        if (m_thread.joinable())
            m_thread.join();
    }

    inline void join() noexcept { m_thread.join(); }
    inline bool joinable() noexcept { return m_thread.joinable(); }

private: /* Methods: */

    void run_() noexcept {
        if (m_startPromise.takeFuture().takeValue() && m_func)
            m_func();
    }

private: /* Fields: */

    Promise<bool> m_startPromise;
    std::function<void()> m_func;
    std::thread m_thread;

}; /* class NoStartThread { */

} /* namespace sharemind { */

#endif /* SHAREMIND_NOSTARTTHREAD_H */
