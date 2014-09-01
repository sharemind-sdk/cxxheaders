/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_STOPPABLE_H
#define SHAREMIND_STOPPABLE_H

#include <atomic>
#include <utility>


namespace sharemind {
class Stoppable {

public: /* Types: */

    template <typename Stoppable__, typename Exception__>
    class CustomTestActor {

    public: /* Types: */

        typedef Exception__ Exception;

    public: /* Methods: */

        inline CustomTestActor(Stoppable__ & stoppable) noexcept
            : m_stoppable(stoppable) {}

        inline void operator()() const noexcept(false)
        { m_stoppable.template throwIfStopRequested<Exception>(); }

    private: /* Fields: */

        Stoppable__ & m_stoppable;

    };

    template <typename Exception__>
    using TestActor = CustomTestActor<Stoppable, Exception__>;

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
