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

#ifndef SHAREMIND_UNALIGNEDPOINTER_H
#define SHAREMIND_UNALIGNEDPOINTER_H

#include <cstddef>
#include <type_traits>
#include "CopyCv.h"
#include "PotentiallyVoidTypeInfo.h"
#include "UnalignedReference.h"


namespace sharemind {

template <typename T>
class UnalignedPointer;

template <typename T, typename U>
bool operator==(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept;

template <typename T, typename U>
bool operator!=(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept;

template <typename T, typename U>
bool operator<(UnalignedPointer<T> const & lhs,
               UnalignedPointer<U> const & rhs) noexcept;

template <typename T, typename U>
bool operator<=(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept;

template <typename T, typename U>
bool operator>=(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept;

template <typename T, typename U>
bool operator>(UnalignedPointer<T> const & lhs,
               UnalignedPointer<U> const & rhs) noexcept;

template <typename T, typename U>
typename std::enable_if<
    std::is_same<
        typename std::remove_cv<T>::type,
        typename std::remove_cv<U>::type
    >::value,
    std::ptrdiff_t
>::type
operator-(UnalignedPointer<T> const & lhs, UnalignedPointer<U> const & rhs)
        noexcept;

template <typename T>
UnalignedPointer<T> operator+(
        UnalignedPointer<T> const & p,
        typename UnalignedPointer<T>::difference_type const n) noexcept;

template <typename T>
UnalignedPointer<T> operator+(
        typename UnalignedPointer<T>::difference_type const n,
        UnalignedPointer<T> const & p) noexcept;

template <typename T>
class UnalignedPointer {

    template <typename T_, typename U_>
    friend bool operator==(UnalignedPointer<T_> const & lhs,
                           UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_, typename U_>
    friend bool operator!=(UnalignedPointer<T_> const & lhs,
                           UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_, typename U_>
    friend bool operator<(UnalignedPointer<T_> const & lhs,
                          UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_, typename U_>
    friend bool operator<=(UnalignedPointer<T_> const & lhs,
                           UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_, typename U_>
    friend bool operator>=(UnalignedPointer<T_> const & lhs,
                           UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_, typename U_>
    friend bool operator>(UnalignedPointer<T_> const & lhs,
                          UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_, typename U_>
    friend bool operator-(UnalignedPointer<T_> const & lhs,
                          UnalignedPointer<U_> const & rhs) noexcept;

    template <typename T_>
    friend UnalignedPointer<T_> operator+(
            UnalignedPointer<T_> const & p,
            typename UnalignedPointer<T_>::difference_type const n) noexcept;

    template <typename T_>
    friend UnalignedPointer<T_> operator+(
            typename UnalignedPointer<T_>::difference_type const n,
            UnalignedPointer<T_> const & p) noexcept;

public: /* Types: */

    using type = UnalignedPointer<T>;

    using difference_type = std::size_t;
    using value_type = T;
    using reference = UnalignedReference<T>;

public: /* Methods: */

    UnalignedPointer(CopyCv_t<void, T> * const ptr) noexcept : m_ptr(ptr) {}

    UnalignedPointer(type &&) noexcept = default;
    UnalignedPointer(type const &) noexcept = default;

    type & operator=(type &&) noexcept = default;
    type & operator=(type const &) noexcept = default;

    reference operator*() const noexcept { return m_ptr; }

    type & operator++() noexcept {
        m_ptr = ptrAdd(m_ptr, sizeof(T));
        return *this;
    }

    type & operator--() noexcept {
        m_ptr = ptrSub(m_ptr, sizeof(T));
        return *this;
    }

    type operator++(int) noexcept {
        type tmp = *this;
        operator++();
        return tmp;
    }

    type operator--(int) noexcept {
        type tmp = *this;
        operator--();
        return tmp;
    }

    type & operator+=(difference_type const n) noexcept {
        m_ptr = ptrAdd(m_ptr, sizeof(T) * n);
        return *this;
    }

    type & operator-=(difference_type const n) noexcept {
        m_ptr = ptrSub(m_ptr, sizeof(T) * n);
        return *this;
    }

    type operator-(difference_type const n) const noexcept
    { return ptrSub(m_ptr, sizeof(T) * n); }

    reference operator[](difference_type const n) const noexcept
    { return ptrAdd(m_ptr, sizeof(T) * n); }

    explicit operator bool() const noexcept { return m_ptr; }

    operator CopyCv_t<void, T> *() const noexcept { return m_ptr; }

private: /* Fields: */

    CopyCv_t<void, T> * m_ptr;

}; /* class UnalignedPointer */

template <typename T, typename U>
bool operator==(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept
{ return lhs.m_ptr == rhs.m_ptr; }

template <typename T, typename U>
bool operator!=(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept
{ return lhs.m_ptr != rhs.m_ptr; }

template <typename T, typename U>
bool operator<(UnalignedPointer<T> const & lhs,
               UnalignedPointer<U> const & rhs) noexcept
{ return lhs.m_ptr < rhs.m_ptr; }

template <typename T, typename U>
bool operator<=(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept
{ return lhs.m_ptr <= rhs.m_ptr; }

template <typename T, typename U>
bool operator>=(UnalignedPointer<T> const & lhs,
                UnalignedPointer<U> const & rhs) noexcept
{ return lhs.m_ptr >= rhs.m_ptr; }

template <typename T, typename U>
bool operator>(UnalignedPointer<T> const & lhs,
               UnalignedPointer<U> const & rhs) noexcept
{ return lhs.m_ptr > rhs.m_ptr; }

template <typename T, typename U>
typename std::enable_if<
    std::is_same<
        typename std::remove_cv<T>::type,
        typename std::remove_cv<U>::type
    >::value,
    std::ptrdiff_t
>::type
operator-(UnalignedPointer<T> const & lhs, UnalignedPointer<U> const & rhs)
        noexcept
{ return ptrDist(rhs.m_ptr, lhs.m_ptr) / sizeof(T); }

template <typename T>
UnalignedPointer<T> operator+(
        UnalignedPointer<T> const & p,
        typename UnalignedPointer<T>::difference_type const n) noexcept
{ return ptrAdd(p.m_ptr, sizeof(T) * n); }

template <typename T>
UnalignedPointer<T> operator+(
        typename UnalignedPointer<T>::difference_type const n,
        UnalignedPointer<T> const & p) noexcept
{ return ptrAdd(p.m_ptr, sizeof(T) * n); }

} /* namespace sharemind { */

#endif /* SHAREMIND_UNALIGNEDPOINTER_H */
