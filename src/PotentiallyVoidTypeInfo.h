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


namespace sharemind {
namespace Detail {
namespace PotentiallyVoidTypeInfo {

template <typename From, typename To>
struct CopyCV {
    typedef typename std::conditional<
                std::is_volatile<From>::value,
                typename std::conditional<
                    std::is_const<From>::value,
                    const volatile To,
                    volatile To
                >::type,
                typename std::conditional<
                    std::is_const<From>::value,
                    const To,
                    To
                >::type
            >::type type;
};

template <typename T, typename Enable = void>
struct AllocBase { typedef typename std::remove_const<T>::type allocType; };

template <typename T>
struct AllocBase<T, typename std::enable_if<std::is_void<T>::value>::type>
{ typedef char allocType; };

template <typename T, typename Enable = void>
struct ArithBase {

    constexpr static inline ptrdiff_t ptrDiff(const T * const a,
                                              const T * const b) noexcept
    { return a - b; }

    constexpr static inline T * ptrAdd(T * const p, const size_t size) noexcept
    { return p + size; }

    constexpr static inline T * ptrSub(T * const p, const size_t size) noexcept
    { return p - size; }

};

template <typename T>
struct ArithBase<T, typename std::enable_if<std::is_void<T>::value>::type> {

    constexpr static inline ptrdiff_t ptrDiff(const T * const a,
                                              const T * const b) noexcept
    { return static_cast<const char *>(a) - static_cast<const char *>(b); }

    constexpr static inline T * ptrAdd(T * const p, const size_t size) noexcept
    { return static_cast<typename CopyCV<T, char>::type *>(p) + size; }

    constexpr static inline T * ptrSub(T * const p, const size_t size) noexcept
    { return static_cast<typename CopyCV<T, char>::type *>(p) - size; }

};

template <typename T, typename Enable = void>
struct CopyBase {
    static inline void copy(const T * const from,
                            T * const to,
                            const size_t size)
    { std::copy(from, from + size, to); }
};

template <typename T>
struct CopyBase<T, typename std::enable_if<std::is_void<T>::value>::type> {
    static inline void copy(const void * const from,
                            void * const to,
                            const size_t size) noexcept
    { std::memcpy(to, from, size); }
};

template <typename T, typename Enable = void>
struct SizeofBase { constexpr static const size_t SIZEOF = sizeof(T); };

template <typename T>
struct SizeofBase<T, typename std::enable_if<std::is_void<T>::value>::type>
{ constexpr static const size_t SIZEOF = sizeof(char); };

} // namespace PotentiallyVoidTypeInfo {
} // namespace Detail {

template <typename T, typename Enable = void>
struct PotentiallyVoidTypeInfo
        : public Detail::PotentiallyVoidTypeInfo::AllocBase<T>
        , public Detail::PotentiallyVoidTypeInfo::ArithBase<T>
        , public Detail::PotentiallyVoidTypeInfo::CopyBase<T>
        , public Detail::PotentiallyVoidTypeInfo::SizeofBase<T> {};


template <typename T>
struct PotentiallyVoidTypeInfo<T, typename std::enable_if<
                                               std::is_const<T>::value
                                             >::type>
        : public Detail::PotentiallyVoidTypeInfo::AllocBase<T>
        , public Detail::PotentiallyVoidTypeInfo::ArithBase<T>
        , public Detail::PotentiallyVoidTypeInfo::SizeofBase<T> {};

template <typename T>
void copy(const T * const from, T * const to, const size_t size)
        noexcept(noexcept(PotentiallyVoidTypeInfo<T>::copy(from, to, size)))
{ PotentiallyVoidTypeInfo<T>::copy(from, to, size); }

template <typename T>
constexpr T * ptrAdd(T * const ptr, const size_t size) noexcept
{ return PotentiallyVoidTypeInfo<T>::ptrAdd(ptr, size); }

template <typename T>
constexpr T * ptrSub(T * const ptr, const size_t size) noexcept
{ return PotentiallyVoidTypeInfo<T>::ptrSub(ptr, size); }


template <typename T>
constexpr ptrdiff_t ptrDiff(T * const ptr, T * const ptr2) noexcept
{ return PotentiallyVoidTypeInfo<T>::ptrDiff(ptr, ptr2); }

template <typename T>
constexpr size_t sizeOf() noexcept
{ return PotentiallyVoidTypeInfo<T>::SIZEOF; }

} /* namespace sharemind { */

#endif /* SHAREMIND_POTENTIALLY_VOID_TYPE_INFO_H */
