/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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
              typename LoopDuration__ = StaticLoopDuration<5u> >
    inline void waitOrStop(StopTest && stopTest,
                           LoopDuration__ && loopDuration = LoopDuration__())
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
