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

#ifndef SHAREMIND_TICKETSPINLOCK_H
#define SHAREMIND_TICKETSPINLOCK_H

#include <atomic>
#include <cassert>
#include <cstdint>
#include <sharemind/CacheLineSize.h>
#include "Spinwait.h"


namespace sharemind {

class TicketSpinLock {

public: /* Methods: */

    #ifndef NDEBUG
    TicketSpinLock() {
        assert(m_active.is_lock_free());
        assert(m_next.is_lock_free());
    }
    #endif

    inline void lock() noexcept {
        auto const ticket = m_next.fetch_add(1u, std::memory_order_relaxed);
        while (ticket != m_active.load(std::memory_order_acquire))
            spinWait();
    }

    inline void unlock() noexcept {
        m_active.store(m_active.load(std::memory_order_relaxed) + 1u,
                       std::memory_order_release);
    }

private: /* Fields: */

    alignas(SHAREMIND_CACHE_LINE_SIZE) std::atomic_size_t m_active{0u};
    alignas(SHAREMIND_CACHE_LINE_SIZE) std::atomic_size_t m_next{0u};

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TICKETSPINLOCK_H */
