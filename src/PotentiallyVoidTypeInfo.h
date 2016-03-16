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

#ifndef SHAREMIND_POTENTIALLY_VOID_TYPE_INFO_H
#define SHAREMIND_POTENTIALLY_VOID_TYPE_INFO_H

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <type_traits>
#include "CopyCv.h"


namespace sharemind {
namespace Detail {
namespace PotentiallyVoidTypeInfo {

using UChar = unsigned char;
static_assert(sizeof(UChar) == 1u, "");

template <typename T, typename Enable = void>
struct Alloc { using type = typename std::remove_const<T>::type; };

template <typename T>
struct Alloc<T, typename std::enable_if<std::is_void<T>::value>::type>
{ using type = UChar; };

template <typename T, typename Enable = void>
struct Arith {

    constexpr static inline auto dist(T * const a, T * const b)
            noexcept(noexcept(std::distance(a, b)))
            -> decltype(std::distance(a, b))
    { return std::distance(a, b); }

    template <typename Diff>
    constexpr static inline T * add(T * const p, Diff const size) noexcept {
        static_assert(std::is_arithmetic<Diff>::value, "");
        return p + size;
    }

    template <typename Diff>
    constexpr static inline T * sub(T * const p, Diff const size) noexcept {
        static_assert(std::is_arithmetic<Diff>::value, "");
        return p - size;
    }

};

template <typename T>
struct Arith<T, typename std::enable_if<std::is_void<T>::value>::type> {

    using CT = CopyCv_t<UChar, T>;
    using ACT = Arith<CT>;

    constexpr static inline CT * toCT(T * const ptr) noexcept
    { return static_cast<CT *>(ptr); }

    constexpr static inline auto dist(T * const a, T * const b)
            noexcept(noexcept(ACT::dist(toCT(a), toCT(b))))
            -> decltype(ACT::dist(toCT(a), toCT(b)))
    { return ACT::dist(toCT(a), toCT(b)); }

    template <typename Diff>
    constexpr static inline T * add(T * const p, Diff const size)
            noexcept(noexcept(ACT::add(toCT(p), size)))
    { return ACT::add(toCT(p), size); }

    template <typename Diff>
    constexpr static inline T * sub(T * const p, Diff const size)
            noexcept(noexcept(ACT::sub(toCT(p), size)))
    { return ACT::sub(toCT(p), size); }

};

template <typename T, typename Enable = void>
struct Copy {
    static_assert(!std::is_const<T>::value, "Can't copy to a const T!");
    static inline void copy(T const * const from,
                            T * const to,
                            std::size_t const size)
            noexcept(noexcept(std::copy(from, from + size, to)))
    { std::copy(from, from + size, to); }
};

template <typename T>
struct Copy<T, typename std::enable_if<std::is_void<T>::value>::type> {
    static_assert(!std::is_const<T>::value, "Can't copy to a const T!");
    static inline void copy(void const * const from,
                            void * const to,
                            std::size_t const size) noexcept
    { std::memcpy(to, from, size); }
};

template <typename T, typename Enable = void>
struct Sizeof { constexpr static auto const value = sizeof(T); };

template <typename T>
struct Sizeof<T, typename std::enable_if<std::is_void<T>::value>::type>
{ constexpr static auto const value = sizeof(UChar); };

} // namespace PotentiallyVoidTypeInfo {
} // namespace Detail {

template <typename T>
using AllocType = typename Detail::PotentiallyVoidTypeInfo::Alloc<T>::type;

template <typename T>
void copy(T const * const from, T * const to, std::size_t const size)
        noexcept(noexcept(Detail::PotentiallyVoidTypeInfo::Copy<T>::copy(from,
                                                                         to,
                                                                         size)))
{ Detail::PotentiallyVoidTypeInfo::Copy<T>::copy(from, to, size); }

template <typename T, typename Diff = std::size_t>
constexpr T * ptrAdd(T * const ptr, Diff const size)
        noexcept(noexcept(Detail::PotentiallyVoidTypeInfo::Arith<T>::add(ptr,
                                                                         size)))
{
    static_assert(std::is_arithmetic<Diff>::value,
                  "The second argument to ptrAdd must be of arithmetic type!");
    return Detail::PotentiallyVoidTypeInfo::Arith<T>::add(ptr, size);
}

template <typename T, typename Diff = std::size_t>
constexpr T * ptrSub(T * const ptr, Diff const size)
        noexcept(noexcept(Detail::PotentiallyVoidTypeInfo::Arith<T>::sub(ptr,
                                                                         size)))
{
    static_assert(std::is_arithmetic<Diff>::value,
                  "The second argument to ptrSub must be of arithmetic type!");
    return Detail::PotentiallyVoidTypeInfo::Arith<T>::sub(ptr, size);
}

template <typename T>
constexpr auto ptrDist(T * const a, T * const b)
        noexcept(noexcept(Detail::PotentiallyVoidTypeInfo::Arith<T>::dist(a,b)))
        -> decltype(Detail::PotentiallyVoidTypeInfo::Arith<T>::dist(a, b))
{ return Detail::PotentiallyVoidTypeInfo::Arith<T>::dist(a, b); }

template <typename T>
constexpr std::size_t sizeOf() noexcept
{ return Detail::PotentiallyVoidTypeInfo::Sizeof<T>::value; }

} /* namespace sharemind { */

#endif /* SHAREMIND_POTENTIALLY_VOID_TYPE_INFO_H */
