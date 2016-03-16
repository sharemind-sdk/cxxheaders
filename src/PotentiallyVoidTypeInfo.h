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
struct AllocBase { using allocType = typename std::remove_const<T>::type; };

template <typename T>
struct AllocBase<T, typename std::enable_if<std::is_void<T>::value>::type>
{ using allocType = UChar; };

template <typename T, typename Enable = void>
struct ArithBase {

    constexpr static inline ptrdiff_t ptrDiff(T const * const a,
                                              T const * const b) noexcept
    { return a - b; }

    constexpr static inline T * ptrAdd(T * const p, std::size_t const size)
            noexcept
    { return p + size; }

    constexpr static inline T * ptrSub(T * const p, std::size_t const size)
            noexcept
    { return p - size; }

};

template <typename T>
struct ArithBase<T, typename std::enable_if<std::is_void<T>::value>::type> {

    constexpr static inline ptrdiff_t ptrDiff(T const * const a,
                                              T const * const b) noexcept
    { return static_cast<UChar const *>(a) - static_cast<UChar const *>(b); }

    constexpr static inline T * ptrAdd(T * const p, std::size_t const size)
            noexcept
    { return static_cast<CopyCv_t<UChar, T> *>(p) + size; }

    constexpr static inline T * ptrSub(T * const p, std::size_t const size)
            noexcept
    { return static_cast<CopyCv_t<UChar, T> *>(p) - size; }

};

template <typename T, typename Enable = void>
struct CopyBase {
    static inline void copy(T const * const from,
                            T * const to,
                            std::size_t const size)
    { std::copy(from, from + size, to); }
};

template <typename T>
struct CopyBase<T, typename std::enable_if<std::is_void<T>::value>::type> {
    static inline void copy(void const * const from,
                            void * const to,
                            std::size_t const size) noexcept
    { std::memcpy(to, from, size); }
};

template <typename T, typename Enable = void>
struct SizeofBase { constexpr static std::size_t const SIZEOF = sizeof(T); };

template <typename T>
struct SizeofBase<T, typename std::enable_if<std::is_void<T>::value>::type>
{ constexpr static std::size_t const SIZEOF = sizeof(UChar); };

template <typename T, typename Enable = void>
struct Base
        : public AllocBase<T>
        , public ArithBase<T>
        , public CopyBase<T>
        , public SizeofBase<T>
{};

template <typename T>
struct Base<T, typename std::enable_if<std::is_const<T>::value>::type>
        : public AllocBase<T>
        , public ArithBase<T>
        , public SizeofBase<T>
{};

} // namespace PotentiallyVoidTypeInfo {
} // namespace Detail {

template <typename T>
using PotentiallyVoidTypeInfo = Detail::PotentiallyVoidTypeInfo::Base<T>;

template <typename T>
using AllocType = typename PotentiallyVoidTypeInfo<T>::AllocType;

template <typename T>
void copy(T const * const from, T * const to, std::size_t const size)
        noexcept(noexcept(PotentiallyVoidTypeInfo<T>::copy(from, to, size)))
{ PotentiallyVoidTypeInfo<T>::copy(from, to, size); }

template <typename T>
constexpr T * ptrAdd(T * const ptr, std::size_t const size) noexcept
{ return PotentiallyVoidTypeInfo<T>::ptrAdd(ptr, size); }

template <typename T>
constexpr T * ptrSub(T * const ptr, std::size_t const size) noexcept
{ return PotentiallyVoidTypeInfo<T>::ptrSub(ptr, size); }


template <typename T>
constexpr ptrdiff_t ptrDiff(T * const ptr, T * const ptr2) noexcept
{ return PotentiallyVoidTypeInfo<T>::ptrDiff(ptr, ptr2); }

template <typename T>
constexpr std::size_t sizeOf() noexcept
{ return PotentiallyVoidTypeInfo<T>::SIZEOF; }

} /* namespace sharemind { */

#endif /* SHAREMIND_POTENTIALLY_VOID_TYPE_INFO_H */
