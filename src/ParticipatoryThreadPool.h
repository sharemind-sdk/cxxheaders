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
        {
            static constexpr auto const max =
                    std::numeric_limits<decltype(m_numParticipants)>::max();
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock,
                        [this]() noexcept { return m_numParticipants < max; });
            ++m_numParticipants;
        }
        workerThread();
        {
            std::lock_guard<std::mutex> const guard(m_mutex);
            assert(m_numParticipants > 0u);
            --m_numParticipants;
            m_cond.notify_all();
        }
    }

private: /* Fields: */

    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::size_t m_numParticipants = 0u;

}; /* class ParticipatoryThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_PARTICIPATORYTHREADPOOL_H */
