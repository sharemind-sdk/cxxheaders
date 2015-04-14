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

#include <cassert>
#include <type_traits>


namespace sharemind {
namespace Detail {

template <typename, bool> class AtomicFlags;

template <typename T, bool = std::is_integral<T>::value>
class Flags {

    friend class AtomicFlags<T, true>;

public: /* Methods: */

    inline constexpr Flags() noexcept {}
    inline constexpr Flags(T const value) noexcept : m_flags{value} {}

    inline Flags<T> & operator=(T const flags) noexcept { return store(flags); }

    inline Flags<T> & operator&=(T const flags) noexcept
    { return ((m_flags &= flags), *this); }

    inline Flags<T> & operator|=(T const flags) noexcept
    { return setFlags(flags); }

    inline Flags<T> & operator^=(T const flags) noexcept
    { return toggleFlags(flags); }

    inline Flags<T> operator&(T const flags) noexcept
    { return m_flags & flags; }

    inline Flags<T> operator|(T const flags) noexcept
    { return m_flags | flags; }

    inline Flags<T> operator^(T const flags) noexcept
    { return m_flags ^ flags; }

    inline operator T () const noexcept { return load(); }

    inline T load() const noexcept { return m_flags; }

    inline Flags<T> & store(T const flags) const noexcept
    { return ((m_flags = flags), *this); }

    inline Flags<T> exchange(T const flags) noexcept {
        T const old = m_flags;
        m_flags = flags;
        return old;
    }

    inline Flags<T> & setFlags() noexcept
    { return ((m_flags = ~static_cast<T>(0)), *this); }

    inline Flags<T> & setFlags(T const flags) noexcept
    { return ((m_flags |= flags), *this); }

    inline Flags<T> & unsetFlags() noexcept
    { return ((m_flags = static_cast<T>(0)), *this); }

    inline Flags<T> & unsetFlags(T const flags) noexcept
    { return ((m_flags &= ~flags), *this); }

    inline Flags<T> & toggleFlags() noexcept
    { return ((m_flags = ~m_flags), *this); }

    inline Flags<T> & toggleFlags(T const flags) noexcept
    { return ((m_flags ^= flags), *this); }

    inline bool hasAnyOf(T const flags) const noexcept
    { return m_flags & flags; }

    inline bool hasAllOf(T const flags) const noexcept
    { return (m_flags & flags) == flags; }

    inline bool hasNoneOf(T const flags) const noexcept
    { return !(hasAnyOf(flags)); }

private: /* Fields: */

    T m_flags;

};

template <typename T>
class Flags<T, false>: public Flags<typename std::underlying_type<T>::type> {

public: /* Methods: */

    using Flags<typename std::underlying_type<T>::type>::Flags;
    using Flags<typename std::underlying_type<T>::type>::operator=;

};

} /* namespace Detail { */

template <typename EnumOrIntegral> using Flags = Detail::Flags<EnumOrIntegral>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_FLAGS_H */
