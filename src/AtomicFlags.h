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

#ifndef SHAREMIND_ATOMICFLAGS_H
#define SHAREMIND_ATOMICFLAGS_H

#include <atomic>
#include <type_traits>
#include "Flags.h"


namespace sharemind {
namespace Detail {

template <typename T, typename U>
class AtomicFlags {

public: /* Methods: */

    AtomicFlags() noexcept {}
    constexpr explicit AtomicFlags(U const value) noexcept : m_flags{value} {}

    Flags<T, U> load(std::memory_order const memoryOrder =
                             std::memory_order_seq_cst) noexcept
    { return m_flags.load(memoryOrder); }

    void store(U const flags,
               std::memory_order const memoryOrder =
                       std::memory_order_seq_cst) noexcept
    { return m_flags.store(flags, memoryOrder); }

    inline Flags<T, U> exchange(U const flags,
                                std::memory_order const memoryOrder =
                                        std::memory_order_seq_cst) noexcept
    { return m_flags.exchange(flags, memoryOrder); }

    inline bool compareExchangeStrong(U & expected,
                                      U const flags,
                                      std::memory_order const memoryOrder =
                                              std::memory_order_seq_cst)
            noexcept
    { return m_flags.compare_exchange_strong(expected, flags, memoryOrder); }

    inline bool compareExchangeStrong(U & expected,
                                      U const flags,
                                      std::memory_order const success,
                                      std::memory_order const failure)
            noexcept
    {
        return m_flags.compare_exchange_strong(expected,
                                               flags,
                                               success,
                                               failure);
    }

    inline bool compareExchangeStrong(Flags<T, U> & expected,
                                      U const flags,
                                      std::memory_order const memoryOrder =
                                              std::memory_order_seq_cst)
            noexcept
    {
        return m_flags.compare_exchange_strong(expected.m_flags,
                                               flags,
                                               memoryOrder);
    }

    inline bool compareExchangeStrong(Flags<T, U> & expected,
                                      U const flags,
                                      std::memory_order const success,
                                      std::memory_order const failure)
            noexcept
    {
        return m_flags.compare_exchange_strong(expected.m_flags,
                                               flags,
                                               success,
                                               failure);
    }

    inline bool compareExchangeWeak(U & expected,
                                    U const flags,
                                    std::memory_order const memoryOrder =
                                            std::memory_order_seq_cst)
            noexcept
    { return m_flags.compare_exchange_weak(expected, flags, memoryOrder); }

    inline bool compareExchangeWeak(U & expected,
                                    U const flags,
                                    std::memory_order const success,
                                    std::memory_order const failure)
            noexcept
    { return m_flags.compare_exchange_weak(expected, flags, success, failure); }

    inline bool compareExchangeWeak(Flags<T, U> & expected,
                                    U const flags,
                                    std::memory_order const memoryOrder =
                                            std::memory_order_seq_cst)
            noexcept
    {
        return m_flags.compare_exchange_weak(expected.m_flags,
                                             flags,
                                             memoryOrder);
    }

    inline bool compareExchangeWeak(Flags<T, U> & expected,
                                    U const flags,
                                    std::memory_order const success,
                                    std::memory_order const failure)
            noexcept
    {
        return m_flags.compare_exchange_weak(expected.m_flags,
                                             flags,
                                             success,
                                             failure);
    }

    void setFlags(std::memory_order const memoryOrder =
                          std::memory_order_seq_cst) noexcept
    { return m_flags.store(~static_cast<U>(0), memoryOrder); }

    Flags<T, U> fetchSetFlags(std::memory_order const memoryOrder =
                                      std::memory_order_seq_cst) noexcept
    { return exchange(~static_cast<U>(0), memoryOrder); }

    Flags<T, U> fetchSetFlags(U const flags,
                              std::memory_order const memoryOrder =
                                      std::memory_order_seq_cst) noexcept
    { return m_flags.fetch_or(flags, memoryOrder); }

    void unsetFlags(std::memory_order const memoryOrder =
                            std::memory_order_seq_cst) noexcept
    { return m_flags.store(static_cast<U>(0), memoryOrder); }

    Flags<T, U> fetchUnsetFlags(std::memory_order const memoryOrder =
                                        std::memory_order_seq_cst) noexcept
    { return exchange(static_cast<U>(0), memoryOrder); }

    Flags<T, U> fetchUnsetFlags(U const flags,
                                std::memory_order const memoryOrder =
                                        std::memory_order_seq_cst) noexcept
    { return m_flags.fetch_and(~flags, memoryOrder); }

    Flags<T, U> fetchToggleFlags(U const flags,
                                 std::memory_order const memoryOrder =
                                         std::memory_order_seq_cst) noexcept
    { return m_flags.fetch_xor(flags, memoryOrder); }

    bool has(U const flags,
             U const of,
             std::memory_order const memoryOrder =
                     std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).has(flags, of); }

    inline bool hasAnyOf(U const flags,
                         std::memory_order const memoryOrder =
                                 std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).hasAnyOf(flags); }

    inline bool hasAllOf(U const flags,
                         std::memory_order const memoryOrder =
                                 std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).hasAllOf(flags); }

    inline bool hasNoneOf(U const flags,
                          std::memory_order const memoryOrder =
                                  std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).hasNoneOf(flags); }

private: /* Fields: */

    std::atomic<U> m_flags;

};

} /* namespace Detail { */

template <typename EnumOrIntegral>
using AtomicFlags =
        Detail::AtomicFlags<
            EnumOrIntegral,
                typename std::conditional<
                    std::is_integral<EnumOrIntegral>::value,
                    EnumOrIntegral,
                    typename std::underlying_type<EnumOrIntegral>::type>::type>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_ATOMICFLAGS_H */
