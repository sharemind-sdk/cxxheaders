/*
 * Copyright (C) 2015 Cybernetica
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

#ifndef SHAREMIND_READYORFAILEDWAITER_H
#define SHAREMIND_READYORFAILEDWAITER_H

#include <cassert>
#include <mutex>
#include <condition_variable>
#include <utility>


namespace sharemind {

template <bool MultipleWaiters = false>
class ReadyOrFailedWaiter {

private: /* Types: */

    enum State { WAITING = 0, READY = 1, FAILED = -1 };

public: /* Methods: */

    inline void reset() noexcept { m_state = WAITING; }

    inline void notifyReady() noexcept { notify_<READY>(); }

    inline void notifyFailure() noexcept { notify_<FAILED>(); }

    inline bool waitReady() const noexcept
    { return waitReady_() == READY; }

    template <typename Exception, typename ... Args>
    inline void waitReady(Args && ... args) const {
        if (waitReady_() != READY)
            throw Exception(std::forward<Args>(args)...);
    }

private: /* Methods: */

    inline void condNotify_() noexcept;

    template <State NEWSTATE>
    inline void notify_() noexcept;

    inline State waitReady_() const noexcept {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_state == WAITING)
            m_cond.wait(lock);
        assert(m_state == READY || m_state == FAILED);
        return m_state;
    }

private: /* Fields: */

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond;
    State m_state = WAITING;

}; /* class ReadyOrFailedWaiter */

template <>
inline void ReadyOrFailedWaiter<true>::condNotify_() noexcept
{ m_cond.notify_all(); }

template <>
inline void ReadyOrFailedWaiter<false>::condNotify_() noexcept
{ m_cond.notify_one(); }

template <bool MultipleWaiters>
template <typename ReadyOrFailedWaiter<MultipleWaiters>::State NEWSTATE>
inline void ReadyOrFailedWaiter<MultipleWaiters>::notify_() noexcept {
    std::lock_guard<std::mutex> const guard(m_mutex);
    assert(m_state == WAITING);
    m_state = NEWSTATE;
    condNotify_();
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_READYORFAILEDWAITER_H */
