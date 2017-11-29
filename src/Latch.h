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

#ifndef SHAREMIND_LATCH_H
#define SHAREMIND_LATCH_H

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <type_traits>


namespace sharemind {

template <typename Counter = unsigned>
class Latch {

    static_assert(std::is_unsigned<Counter>::value, "");

public: /* Methods: */

    Latch(Latch &&) = delete;
    Latch(Latch const &) = delete;
    Latch & operator=(Latch &&) = delete;
    Latch & operator=(Latch const &) = delete;

    Latch(Counter n) : m_counter((assert(n > 0), std::move(n))) {}

    /** \returns whether this was counted to be the last countdown. */
    bool countDown() noexcept {
        std::lock_guard<std::mutex> const guard(m_mutex);
        assert(m_counter > 0u);
        bool const r = !--m_counter;
        if (r)
            m_cond.notify_all();
        return r;
    }

    void countDownAndWait() noexcept {
        std::unique_lock<std::mutex> lock(m_mutex);
        assert(m_counter > 0u);
        if (!--m_counter) {
            m_cond.notify_all();
        } else {
            m_cond.wait(lock, [this]() noexcept { return !m_counter; });
        }
    }

    bool isReady() const noexcept {
        std::unique_lock<std::mutex> lock(m_mutex);
        return !m_counter;
    }

    void wait() const noexcept {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() noexcept { return !m_counter; });
    }

private: /* Fields: */

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond;
    Counter m_counter;

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_LATCH_H */
