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

#ifndef SHAREMIND_ATOMICFLAGS_H
#define SHAREMIND_ATOMICFLAGS_H

#include <atomic>
#include <type_traits>
#include "compiler-support/GccInheritConstructor.h"
#include "Flags.h"


namespace sharemind {
namespace Detail {

template <typename T, bool = std::is_integral<T>::value>
class AtomicFlags {

public: /* Methods: */

    AtomicFlags() noexcept {}
    constexpr explicit AtomicFlags(T const value) noexcept : m_flags{value} {}

    Flags<T> load(std::memory_order const memoryOrder =
                          std::memory_order_seq_cst) noexcept
    { return m_flags.load(memoryOrder); }

    void store(T const flags,
               std::memory_order const memoryOrder =
                       std::memory_order_seq_cst) noexcept
    { return m_flags.store(flags, memoryOrder); }

    inline Flags<T> exchange(T const flags,
                             std::memory_order const memoryOrder =
                                     std::memory_order_seq_cst) noexcept
    { return m_flags.exchange(flags, memoryOrder); }

    inline bool compareExchangeStrong(T & expected,
                                      T const flags,
                                      std::memory_order const memoryOrder =
                                              std::memory_order_seq_cst)
            noexcept
    { return m_flags.compare_exchange_strong(expected, flags, memoryOrder); }

    inline bool compareExchangeStrong(T & expected,
                                      T const flags,
                                      std::memory_order const success,
                                      std::memory_order const failure)
            noexcept
    {
        return m_flags.compare_exchange_strong(expected,
                                               flags,
                                               success,
                                               failure);
    }

    inline bool compareExchangeStrong(Flags<T> & expected,
                                      T const flags,
                                      std::memory_order const memoryOrder =
                                              std::memory_order_seq_cst)
            noexcept
    {
        return m_flags.compare_exchange_strong(expected.m_flags,
                                               flags,
                                               memoryOrder);
    }

    inline bool compareExchangeStrong(Flags<T> & expected,
                                      T const flags,
                                      std::memory_order const success,
                                      std::memory_order const failure)
            noexcept
    {
        return m_flags.compare_exchange_strong(expected.m_flags,
                                               flags,
                                               success,
                                               failure);
    }

    inline bool compareExchangeWeak(T & expected,
                                    T const flags,
                                    std::memory_order const memoryOrder =
                                            std::memory_order_seq_cst)
            noexcept
    { return m_flags.compare_exchange_weak(expected, flags, memoryOrder); }

    inline bool compareExchangeWeak(T & expected,
                                    T const flags,
                                    std::memory_order const success,
                                    std::memory_order const failure)
            noexcept
    { return m_flags.compare_exchange_weak(expected, flags, success, failure); }

    inline bool compareExchangeWeak(Flags<T> & expected,
                                    T const flags,
                                    std::memory_order const memoryOrder =
                                            std::memory_order_seq_cst)
            noexcept
    {
        return m_flags.compare_exchange_weak(expected.m_flags,
                                             flags,
                                             memoryOrder);
    }

    inline bool compareExchangeWeak(Flags<T> & expected,
                                    T const flags,
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
    { return m_flags.store(~static_cast<T>(0), memoryOrder); }

    Flags<T> fetchSetFlags(std::memory_order const memoryOrder =
                                   std::memory_order_seq_cst) noexcept
    { return exchange(~static_cast<T>(0), memoryOrder); }

    Flags<T> fetchSetFlags(T const flags,
                           std::memory_order const memoryOrder =
                                   std::memory_order_seq_cst) noexcept
    { return m_flags.fetch_or(flags, memoryOrder); }

    void unsetFlags(std::memory_order const memoryOrder =
                            std::memory_order_seq_cst) noexcept
    { return m_flags.store(static_cast<T>(0), memoryOrder); }

    Flags<T> fetchUnsetFlags(std::memory_order const memoryOrder =
                                     std::memory_order_seq_cst) noexcept
    { return exchange(static_cast<T>(0), memoryOrder); }

    Flags<T> fetchUnsetFlags(T const flags,
                             std::memory_order const memoryOrder =
                                     std::memory_order_seq_cst) noexcept
    { return m_flags.fetch_and(~flags, memoryOrder); }

    Flags<T> fetchToggleFlags(T const flags,
                              std::memory_order const memoryOrder =
                                      std::memory_order_seq_cst) noexcept
    { return m_flags.fetch_xor(flags, memoryOrder); }

    bool has(T const flags,
             T const of,
             std::memory_order const memoryOrder =
                     std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).has(flags, of); }

    inline bool hasAnyOf(T const flags,
                         std::memory_order const memoryOrder =
                                 std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).hasAnyOf(flags); }

    inline bool hasAllOf(T const flags,
                         std::memory_order const memoryOrder =
                                 std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).hasAllOf(flags); }

    inline bool hasNoneOf(T const flags,
                          std::memory_order const memoryOrder =
                                  std::memory_order_seq_cst) const noexcept
    { return load(memoryOrder).hasNoneOf(flags); }

private: /* Fields: */

    std::atomic<T> m_flags;

};

template <typename T>
class AtomicFlags<T, false>:
        public AtomicFlags<typename std::underlying_type<T>::type>
{

public: /* Methods: */

    SHAREMIND_GCC_INHERITED_CONSTRUCTOR(
            AtomicFlags,
            AtomicFlags<typename std::underlying_type<T>::type>,
            AtomicFlags)
    using AtomicFlags<typename std::underlying_type<T>::type>::operator=;

};

} /* namespace Detail { */

template <typename EnumOrIntegral>
using AtomicFlags = Detail::AtomicFlags<EnumOrIntegral>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_ATOMICFLAGS_H */
