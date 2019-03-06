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

#ifndef SHAREMIND_PARTICIPATORYTHREADPOOL_H
#define SHAREMIND_PARTICIPATORYTHREADPOOL_H

#include "ThreadPool.h"

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <limits>
#include <mutex>
#include <new>


namespace sharemind {

class ParticipatoryThreadPool: public ThreadPool {

private: /* Types: */

    struct ParticipatorContext {

        ParticipatorContext(ParticipatoryThreadPool & threadPool) noexcept
            : m_threadPool(threadPool)
        {
            using N = decltype(ParticipatoryThreadPool::m_numParticipants);
            static constexpr auto const max = std::numeric_limits<N>::max();
            std::unique_lock<std::mutex> lock(threadPool.m_mutex);
            threadPool.m_cond.wait(
                        lock,
                        [&threadPool]() noexcept
                        { return threadPool.m_numParticipants < max; });
            ++threadPool.m_numParticipants;
        }

        ~ParticipatorContext() noexcept {
            std::lock_guard<std::mutex> const guard(m_threadPool.m_mutex);
            assert(m_threadPool.m_numParticipants > 0u);
            --m_threadPool.m_numParticipants;
            m_threadPool.m_cond.notify_all();
        }

        ParticipatoryThreadPool & m_threadPool;

    };
    friend class ParticipatorContext;

public: /* Methods: */

    ParticipatoryThreadPool(ParticipatoryThreadPool &&) = delete;
    ParticipatoryThreadPool(ParticipatoryThreadPool const &) = delete;
    ParticipatoryThreadPool & operator=(ParticipatoryThreadPool &&) = delete;
    ParticipatoryThreadPool & operator=(ParticipatoryThreadPool const &)
            = delete;

    ParticipatoryThreadPool() = default;

    ~ParticipatoryThreadPool() noexcept override { stopAndJoin(); }

    void join() noexcept {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock,
                    [this]() noexcept { return m_numParticipants <= 0u; });
    }

    void stopAndJoin() noexcept {
        notifyStop();
        join();
    }

    void participate() {
        ParticipatorContext const ctx(*this);
        workerThread();
    }

    void participateOnce() {
        ParticipatorContext const ctx(*this);
        oneTaskWorkerThread();
    }

private: /* Fields: */

    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::size_t m_numParticipants = 0u;

}; /* class ParticipatoryThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_PARTICIPATORYTHREADPOOL_H */
