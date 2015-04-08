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


namespace sharemind {
namespace Detail {

template <typename FlagEnum,
          typename =
                typename std::enable_if<std::is_enum<FlagEnum>::value>::type>
class AtomicFlags:
        public std::atomic<typename std::underlying_type<FlagEnum>::type>
{

public: /* Types: */

    using Underlying = typename std::underlying_type<FlagEnum>::type;

private: /* Types: */

    using Base = std::atomic<Underlying>;

public: /* Methods: */

    AtomicFlags() noexcept {}
    constexpr AtomicFlags(Underlying const value) noexcept : Base{value} {}

    Underlying setFlags(Underlying const flags,
                        std::memory_order const memoryOrder =
                                std::memory_order_seq_cst) noexcept
    { return this->fetch_or(flags, memoryOrder); }

    Underlying unsetFlags(Underlying const flags,
                          std::memory_order const memoryOrder =
                                  std::memory_order_seq_cst) noexcept
    { return this->fetch_and(~flags, memoryOrder); }

};

} /* namespace Detail { */

template <typename FlagsEnum>
using AtomicFlags = Detail::AtomicFlags<FlagsEnum>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_ATOMICFLAGS_H */
