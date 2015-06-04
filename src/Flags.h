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

#ifndef SHAREMIND_FLAGS_H
#define SHAREMIND_FLAGS_H

#include <type_traits>


namespace sharemind {
namespace Detail {

template <typename, typename> class AtomicFlags;

template <typename T, typename U>
class Flags {

    friend class AtomicFlags<T, U>;

public: /* Methods: */

    inline constexpr Flags() noexcept {}
    inline constexpr Flags(U const value) noexcept : m_flags{value} {}

    inline Flags<T, U> & operator=(U const flags) noexcept
    { return store(flags); }

    inline Flags<T, U> & operator&=(U const flags) noexcept
    { return ((m_flags &= flags), *this); }

    inline Flags<T, U> & operator|=(U const flags) noexcept
    { return setFlags(flags); }

    inline Flags<T, U> & operator^=(U const flags) noexcept
    { return toggleFlags(flags); }

    inline Flags<T, U> operator&(U const flags) noexcept
    { return m_flags & flags; }

    inline Flags<T, U> operator|(U const flags) noexcept
    { return m_flags | flags; }

    inline Flags<T, U> operator^(U const flags) noexcept
    { return m_flags ^ flags; }

    inline operator U () const noexcept { return load(); }

    inline U load() const noexcept { return m_flags; }

    inline Flags<T, U> & store(U const flags) noexcept
    { return ((m_flags = flags), *this); }

    inline Flags<T, U> exchange(U const flags) noexcept {
        U const old = m_flags;
        m_flags = flags;
        return old;
    }

    inline Flags<T, U> & setFlags() noexcept
    { return ((m_flags = ~static_cast<U>(0)), *this); }

    inline Flags<T, U> & setFlags(U const flags) noexcept
    { return ((m_flags |= flags), *this); }

    inline Flags<T, U> & unsetFlags() noexcept
    { return ((m_flags = static_cast<U>(0)), *this); }

    inline Flags<T, U> & unsetFlags(U const flags) noexcept
    { return ((m_flags &= ~flags), *this); }

    inline Flags<T, U> & toggleFlags() noexcept
    { return ((m_flags = ~m_flags), *this); }

    inline Flags<T, U> & toggleFlags(U const flags) noexcept
    { return ((m_flags ^= flags), *this); }

    inline bool hasAnyOf(U const flags) const noexcept
    { return m_flags & flags; }

    inline bool hasAllOf(U const flags) const noexcept
    { return (m_flags & flags) == flags; }

    inline bool hasNoneOf(U const flags) const noexcept
    { return !(hasAnyOf(flags)); }

private: /* Fields: */

    U m_flags;

};

} /* namespace Detail { */

template <typename EnumOrIntegral>
using Flags =
        Detail::Flags<
            EnumOrIntegral,
            typename std::conditional<
                std::is_integral<EnumOrIntegral>::value,
                EnumOrIntegral,
                typename std::underlying_type<EnumOrIntegral>::type>::type>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_FLAGS_H */
