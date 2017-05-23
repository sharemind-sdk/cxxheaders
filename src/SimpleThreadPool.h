/*
 * Copyright (C) 2015-2017 Cybernetica
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

#ifndef SHAREMIND_SIMPLETHREADPOOL_H
#define SHAREMIND_SIMPLETHREADPOOL_H

#include "ThreadPool.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>


namespace sharemind {

class SimpleThreadPool: public ThreadPool {

public: /* Methods: */

    SimpleThreadPool(SimpleThreadPool &&) = delete;
    SimpleThreadPool(SimpleThreadPool const &) = delete;
    SimpleThreadPool & operator=(SimpleThreadPool &&) = delete;
    SimpleThreadPool & operator=(SimpleThreadPool const &) = delete;

    SimpleThreadPool(std::size_t const numThreads) {
        m_threads.reserve(numThreads);
        try {
            for (unsigned i = 0u; i < numThreads; i++)
                m_threads.emplace_back(&SimpleThreadPool::workerThread, this);
        } catch (...) {
            stopAndJoin();
            throw;
        }
    }

    ~SimpleThreadPool() noexcept override {
        assert((std::find_if(
                    m_threads.begin(),
                    m_threads.end(),
                    [](std::thread const & t)
                    { return t.get_id() == std::this_thread::get_id(); })
                == m_threads.end()) && "Can't destroy pool from pool thread!");
        stopAndJoin();
    }

    void join() noexcept {
        #ifndef NDEBUG
        std::thread::id const myId(std::this_thread::get_id());
        #endif
        std::lock_guard<std::mutex> const guard(m_threadsMutex);
        for (std::thread & thread : m_threads)
            if (((void) assert(thread.get_id() != myId), thread.joinable()))
                thread.join();
    }

    void stopAndJoin() noexcept {
        notifyStop();
        join();
    }

private: /* Fields: */

    std::mutex m_threadsMutex;
    std::vector<std::thread> m_threads;

}; /* class SimpleThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_SIMPLETHREADPOOL_H */
