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

#ifndef SHAREMIND_SIMPLE_WAIT_H
#define SHAREMIND_SIMPLE_WAIT_H

#include <condition_variable>
#include <mutex>
#include "Durations.h"


namespace sharemind {

class SimpleWait {

public: /* Methods: */

    inline SimpleWait() noexcept
        : m_isReady(false) {}

    SimpleWait(SimpleWait &&) = delete;
    SimpleWait(const SimpleWait &) = delete;
    SimpleWait & operator=(SimpleWait &&) = delete;
    SimpleWait & operator=(const SimpleWait &) = delete;

    inline void wait() noexcept {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_isReady)
            m_cond.wait(lock);
    }

    template <typename StopTest,
              typename LoopDuration_ = StaticLoopDuration<5u> >
    inline void waitOrStop(StopTest && stopTest,
                           LoopDuration_ && loopDuration = LoopDuration_())
            noexcept(false)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_isReady) {
            if (m_cond.wait_for(lock, loopDuration)
                != std::cv_status::no_timeout)
            {
                stopTest();
            }
        }
    }

    inline void notifyReady() noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isReady = true;
        m_cond.notify_all();
    }

private: /* Fields: */

    std::condition_variable m_cond;
    mutable std::mutex m_mutex;
    bool m_isReady;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_SIMPLE_WAIT_H */
