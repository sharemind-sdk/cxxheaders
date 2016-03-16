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

    constexpr static inline std::ptrdiff_t ptrDiff(T const * const a,
                                                   T const * const b) noexcept
    { return a - b; }

    template <typename Diff>
    constexpr static inline T * ptrAdd(T * const p, Diff const size) noexcept {
        static_assert(std::is_arithmetic<Diff>::value, "");
        return p + size;
    }

    template <typename Diff>
    constexpr static inline T * ptrSub(T * const p, Diff const size) noexcept {
        static_assert(std::is_arithmetic<Diff>::value, "");
        return p - size;
    }

};

template <typename T>
struct Arith<T, typename std::enable_if<std::is_void<T>::value>::type> {

    constexpr static inline std::ptrdiff_t ptrDiff(T const * const a,
                                                   T const * const b) noexcept
    { return static_cast<UChar const *>(a) - static_cast<UChar const *>(b); }

    template <typename Diff>
    constexpr static inline T * ptrAdd(T * const p, Diff const size) noexcept {
        static_assert(std::is_arithmetic<Diff>::value, "");
        return static_cast<CopyCv_t<UChar, T> *>(p) + size;
    }

    template <typename Diff>
    constexpr static inline T * ptrSub(T * const p, Diff const size) noexcept {
        static_assert(std::is_arithmetic<Diff>::value, "");
        return static_cast<CopyCv_t<UChar, T> *>(p) - size;
    }

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
struct Sizeof { constexpr static std::size_t const value = sizeof(T); };

template <typename T>
struct Sizeof<T, typename std::enable_if<std::is_void<T>::value>::type>
{ constexpr static std::size_t const value = sizeof(UChar); };

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
constexpr T * ptrAdd(T * const ptr, Diff const size) noexcept {
    static_assert(std::is_arithmetic<Diff>::value,
                  "The second argument to ptrAdd must be of arithmetic type!");
    return Detail::PotentiallyVoidTypeInfo::Arith<T>::ptrAdd(ptr, size);
}

template <typename T, typename Diff = std::size_t>
constexpr T * ptrSub(T * const ptr, Diff const size) noexcept {
    static_assert(std::is_arithmetic<Diff>::value,
                  "The second argument to ptrSub must be of arithmetic type!");
    return Detail::PotentiallyVoidTypeInfo::Arith<T>::ptrSub(ptr, size);
}

template <typename T>
constexpr std::ptrdiff_t ptrDiff(T * const ptr, T * const ptr2) noexcept
{ return Detail::PotentiallyVoidTypeInfo::Arith<T>::ptrDiff(ptr, ptr2); }

template <typename T>
constexpr std::size_t sizeOf() noexcept
{ return Detail::PotentiallyVoidTypeInfo::Sizeof<T>::value; }

} /* namespace sharemind { */

#endif /* SHAREMIND_POTENTIALLY_VOID_TYPE_INFO_H */
