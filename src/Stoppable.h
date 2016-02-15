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

#ifndef SHAREMIND_STOPPABLE_H
#define SHAREMIND_STOPPABLE_H

#include <atomic>
#include <utility>


namespace sharemind {
class Stoppable {

public: /* Types: */

    template <typename Stoppable_, typename Exception_>
    class CustomTestActor {

    public: /* Types: */

        using Exception = Exception_;

    public: /* Methods: */

        inline CustomTestActor(Stoppable_ & stoppable) noexcept
            : m_stoppable(stoppable) {}

        inline void operator()() const noexcept(false)
        { m_stoppable.template throwIfStopRequested<Exception>(); }

    private: /* Fields: */

        Stoppable_ & m_stoppable;

    };

    template <typename Exception_>
    using TestActor = CustomTestActor<Stoppable, Exception_>;

public: /* Methods: */

    Stoppable(const Stoppable &) = delete;
    Stoppable & operator=(const Stoppable &) = delete;

    inline Stoppable() noexcept : m_stopRequested(false) {}

    inline bool stopRequested() const noexcept
    { return m_stopRequested.load(std::memory_order_relaxed); }

    inline void stop() noexcept
    { m_stopRequested.store(true, std::memory_order_relaxed); }

    inline void reset() noexcept
    { m_stopRequested.store(false, std::memory_order_relaxed); }

    template <typename Exception>
    inline void throwIfStopRequested() const noexcept(false) {
        if (stopRequested())
            throw Exception();
    }

    template <typename Exception>
    inline void throwIfStopRequested(Exception && e) const noexcept(false) {
        if (stopRequested())
            throw std::forward<Exception>(e);
    }

private: /* Fields: */

    std::atomic<bool> m_stopRequested;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_STOPPABLE_H */
